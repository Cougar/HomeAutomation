
Node_ResetNodeId = null;
Node_ResetClientId = null;

Node_ProgramNodeId = null;
Node_ProgramClientId = null;

Node_WaitClientId = null;

function Node_OnChange(node_id, available)
{
	if (node_id == Node_ResetNodeId && available)
	{
		Console_LogToClient(Node_ResetClientId, "\033[32m" + Node_ResetNodeId + " started okay.\033[0m\n");
		Node_ResetNodeId = null;
		Node_ResetClientId = null;
		Console_SetDefaultPrompt();
	}
	
	if (node_id == Node_ProgramNodeId && available)
    {
        Console_LogToClient(Node_ProgramClientId, "\033[32m" + Node_ProgramNodeId + " started okay.\033[0m\n");
        Node_ProgramNodeId = null;
        Node_ProgramClientId = null;
        Console_SetDefaultPrompt();
    }
    
    if (Node_WaitClientId != null && available)
    {    
        var result = NodeExport_GetNodeInformation(node_id);
    
        if (!result)
        {
            Console_LogToClient(Node_WaitClientId, "\033[31mNo node with id " + node_id + " found.\033[0m\n");
            return false;
        }
        
        var date = new Date(result["LastActive"] * 1000);
        
        Console_LogToClient(Node_WaitClientId, "Id: " + result["Id"] + "\n");
        Console_LogToClient(Node_WaitClientId, "Valid: " + result["Valid"] + "\n");
        Console_LogToClient(Node_WaitClientId, "Bios Version: " + result["BiosVersion"] + "\n");
        Console_LogToClient(Node_WaitClientId, "Device Type: " + result["DeviceType"] + "\n");
        Console_LogToClient(Node_WaitClientId, "Has Application: " + result["HasApplication"] + "\n");
        Console_LogToClient(Node_WaitClientId, "Last Active: " + date.toString() + "\n");
        
        Node_WaitClientId = null;
        Console_SetDefaultPrompt();
    }
}

function Node_GetAvailableIds()
{
	var result_list = [];
	var available_nodes = NodeExport_GetAvailableNodes();
	
	for (var n in available_nodes)
	{
		var id_parts = available_nodes[n].split(",", 2);
		
		result_list.push(id_parts[0]);
	}
	
	return result_list;
}

function Node_ListAvailable()
{
	var available_nodes = NodeExport_GetAvailableNodes();
	
	var lines = [["Id", "Modules"]];
	
	for (var n in available_nodes)
	{
		var parts = available_nodes[n].split(",");
		var id = parts.shift();
		var modules = "<none>";
		
		if (parts.length > 0)
		{
			modules = parts.join(", ");
		}
		
		lines.push([id, modules]);
	}
	
	var table_lines = create_table(lines);
	
	Log("\033[29;1m" + table_lines[0] + "\033[0m\n");
	
	for (var n = 1; n < table_lines.length; n++)
	{
		Log(table_lines[n] + "\n");
	}
	
	return true;
}
Console_RegisterCommand(Node_ListAvailable);

function Node_Reset(node_id)
{
	if (arguments.length < 1)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	Log("Sending reset to " + node_id + "...\n");
	
	if (!NodeExport_ResetNode(node_id))
	{
		Log("\033[31mFailed to send reset.\033[0m\n");
		return false;
	}
	
	Node_ProgramClientId = Console_GetClientId();
	Node_ProgramNodeId = node_id;
	Console_PreventDefaultPrompt();

	return true;
}
Console_RegisterCommand(Node_Reset, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Node_GetAvailableIds()); });

function Node_Program(node_id, bios, filename)
{
    if (arguments.length < 3)
    {
        Log("\033[31mNot enough parameters given.\033[0m\n");
        return false;
    }
    
    Log("Initiating programming of " + node_id + "...\n");
    
    if (!NodeExport_ProgramNode(node_id, bios > 0, filename))
    {
        Log("\033[31mFailed to start programming.\033[0m\n");
        return false;
    }
    
    Node_ResetClientId = Console_GetClientId();
    Node_ResetNodeId = node_id;
    Console_PreventDefaultPrompt();
    
    return true;
}
Console_RegisterCommand(Node_Program, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Node_GetAvailableIds(), [0, 1]); });

function Node_GetInformation(node_id)
{
    if (arguments.length < 1)
    {
        Log("\033[31mNot enough parameters given.\033[0m\n");
        return false;
    }
    
    var result = NodeExport_GetNodeInformation(node_id);
    
    if (!result)
    {
        Log("\033[31mNo node with id " + node_id + " found.\033[0m\n");
        return false;
    }
    
    var date = new Date(result["LastActive"] * 1000);
 
    Log("\033[96mId: \033[0;1m" + result["Id"] + "\033[0m\n");
    Log("\033[96mValid: \033[0;1m" + result["Valid"] + "\033[0m\n");
    Log("\033[96mBios Version: \033[0;1m" + result["BiosVersion"] + "\033[0m\n");
    Log("\033[96mDevice Type: \033[0;1m" + result["DeviceType"] + "\033[0m\n");
    Log("\033[96mHas Application: \033[0;1m" + result["HasApplication"] + "\033[0m\n");
    Log("\033[96mLast Active: \033[0;1m" + date.toString() + "\033[0m\n");
    
    return true;
}
Console_RegisterCommand(Node_GetInformation, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Node_GetAvailableIds()); });

function Node_WaitForInformation()
{
    Node_WaitClientId = Console_GetClientId();
    Console_PreventDefaultPrompt();
    
    return true;
}
Console_RegisterCommand(Node_WaitForInformation);
