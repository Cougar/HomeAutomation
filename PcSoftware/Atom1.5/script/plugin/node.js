
Node_ResetNodeId = null;
Node_ResetClientId = null;

function Node_OnChange(node_id, available)
{
	if (node_id == Node_ResetNodeId && available)
	{
		Console_LogToClient(Node_ResetClientId, "\033[32m" + Node_ResetNodeId + " started okay.\033[0m\n");
		Node_ResetNodeId = null;
		Node_ResetClientId = null;
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
	
	Node_ResetClientId = Console_GetClientId();
	Node_ResetNodeId = node_id;
	Console_PreventDefaultPrompt();

	return true;
}
Console_RegisterCommand(Node_Reset, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Node_GetAvailableIds()); });

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
    
    Log("Id: " + result["Id"] + "\n");
    Log("Valid: " + result["Valid"] + "\n");
    Log("BiosVersion: " + result["BiosVersion"] + "\n");
    Log("DeviceType: " + result["DeviceType"] + "\n");
    Log("HasApplication: " + result["HasApplication"] + "\n");
    Log("LastActive: " + date.toString() + "\n");
    
    return true;
}
Console_RegisterCommand(Node_GetInformation, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Node_GetAvailableIds()); });
