
Node_ResetNodeId = null;
Node_ResetClientId = null;

Node_ProgramNodeId = null;
Node_ProgramClientId = null;

Node_WaitClientId = null;

Node_Native_Reset = false;
Node_Native_Program = false;
Node_Native_WaitNodeId = false;

Node_HexData = "";
Node_HexNumLines = 0;

function Node_OnChange(node_id, available)
{
//Log("Node_OnChange "+Node_ResetNodeId+" "+Node_ProgramNodeId+" "+node_id+" "+available+"\n");
	if (node_id == Node_ResetNodeId && available)
	{
//Log("ResetNodeId "+Node_Native_Reset+"\n");
		Console_LogToClient(Node_ResetClientId, "\033[32m" + Node_ResetNodeId + " started okay.\033[0m\n");

        Console_NewConnection(Node_ResetClientId); // This is a ugly hack for a bad design for multiusers
        Node_ResetNodeId = null;
        Node_ResetClientId = null;
        
        Node_Native_Reset = true;
	}

	if (node_id == Node_ProgramNodeId && available)
    {
//Log("ProgramNodeId "+Node_Native_Program+"\n");
        Console_LogToClient(Node_ProgramClientId, "\033[32m" + Node_ProgramNodeId + " started okay.\033[0m\n");

        Console_NewConnection(Node_ProgramClientId); // This is a ugly hack for a bad design for multiusers
        Node_ProgramNodeId = null;
        Node_ProgramClientId = null;
        
        Node_Native_Program = true;
    }

    if (Node_WaitClientId != null && available)
    {
        var result = NodeExport_GetNodeInformation(node_id);
        Node_Native_WaitNodeId = node_id;

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

        Console_NewConnection(Node_WaitClientId); // This is a ugly hack for a bad design for multiusers
        Node_WaitClientId = null;
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

function Node_WaitNodePollNative()
{
	if (Node_Native_WaitNodeId)
	{
		var result = NodeExport_GetNodeInformation(Node_Native_WaitNodeId);

		if (!result)
		{
			return "\033[31mError: No node with id " + Node_Native_WaitNodeId + " found.\033[0m";
		}

		var date = new Date(result["LastActive"] * 1000);

		return "Id: " + result["Id"] + "\t"+
		"Valid: " + result["Valid"] + "\t"+
		"Bios Version: " + result["BiosVersion"] + "\t"+
		"Device Type: " + result["DeviceType"] + "\t"+
		"Has Application: " + result["HasApplication"] + "\t"+
		"Last Active: " + date.toString();
	}
	else
	{
		return false;
	}
}

function Node_WaitForInformationNative()
{
    Node_WaitClientId = Console_GetClientId();
    Node_Native_WaitNodeId = false;
}

function Node_ResetPollNative()
{
	return Node_Native_Reset;
}

function Node_ResetNative(node_id)
{
	if (arguments.length < 1)
	{
		return "\033[31mError: Not enough parameters given.\033[0m";
	}

	Node_Native_Reset = false;
	if (!NodeExport_ResetNode(node_id))
	{
		return "\033[31mError: Failed to send reset, this can happen if node only contains bios and it was connected before atom was started/restarted.\033[0m";
	}

	Node_ResetClientId = Console_GetClientId();
	Node_ResetNodeId = node_id;

	return "Reset command sent successfully";
}

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
		Log("\033[31mFailed to send reset, this can happen if node only contains bios and it was connected before atom was started/restarted.\033[0m\n");
		return false;
	}

	Node_ResetClientId = Console_GetClientId();
	Node_ResetNodeId = node_id;
	Console_PreventDefaultPrompt();

	return true;
}
Console_RegisterCommand(Node_Reset, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Node_GetAvailableIds()); });

function Node_ProgramPollNative()
{
	return Node_Native_Program;
}

function Node_AppendHexNative(hexrow)
{
    if (arguments.length < 1)
    {
        return "\033[31mError: Not enough parameters given.\033[0m";
    }

    Node_HexData = Node_HexData + hexrow + "\n";
    Node_HexNumLines += 1;

    return true;
}

function Node_GetProgramProgress(node_id)
{
    if (arguments.length < 1)
    {
        return "\033[31mError: Not enough parameters given.\033[0m";
    }
    
    //if (!NodeExport_ProgramNodeHex(node_id, bios > 0, Node_HexData))
    //{
    //    return "\033[31mError: Failed to start programming, this can happen if node only contains bios and it was connected before atom was started/restarted.\033[0m";
    //}

    return NodeExport_GetProgramProgress(node_id);
}

function Node_ProgramNative(node_id, bios, hexnumlines)
{
    if (arguments.length < 3)
    {
        return "\033[31mError: Not enough parameters given.\033[0m";
    }
    if (Node_HexNumLines != hexnumlines)
    {
        return "\033[31mError: Number of hex lines does not match received (" + Node_HexNumLines + " received, should be " + hexnumlines + ").\033[0m";
    }
    
	Node_Native_Program = false;
    if (!NodeExport_ProgramNodeHex(node_id, bios > 0, Node_HexData))
    {
        return "\033[31mError: Failed to start programming, this can happen if node only contains bios and it was connected before atom was started/restarted.\033[0m";
    }

    Node_ProgramClientId = Console_GetClientId();
    Node_ProgramNodeId = node_id;
    Node_HexData = "";
    Node_HexNumLines = 0;

    return "Program command sent successfully";
}

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
        Log("\033[31mFailed to start programming, this can happen if node only contains bios and it was connected before atom was started/restarted.\033[0m\n");
        return false;
    }

    Node_ProgramClientId = Console_GetClientId();
    Node_ProgramNodeId = node_id;
    Console_PreventDefaultPrompt();

    return true;
}
Console_RegisterCommand(Node_Program, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Node_GetAvailableIds(), [0, 1]); });

function Node_ProgramHex(node_id, bios, hexnumlines)
{
    if (arguments.length < 2)
    {
        Log("\033[31mNot enough parameters given.\033[0m\n");
        return false;
    }
    if (Node_HexNumLines != hexnumlines)
    {
        Log("\033[31mNumber of hex lines does not match received (" + Node_HexNumLines + " received, should be " + hexnumlines + ").\033[0m\n");
        return false;
    }

    Log("Initiating programming of " + node_id + "...\n");

    if (!NodeExport_ProgramNodeHex(node_id, bios > 0, Node_HexData))
    {
        Log("\033[31mFailed to start programming, this can happen if node only contains bios and it was connected before atom was started/restarted.\033[0m\n");
        return false;
    }

    Node_ProgramClientId = Console_GetClientId();
    Node_ProgramNodeId = node_id;
    Console_PreventDefaultPrompt();

    Node_HexData = "";
    Node_HexNumLines = 0;


    return true;
}
Console_RegisterCommand(Node_ProgramHex, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Node_GetAvailableIds(), [0, 1]); });

function Node_ClearHex()
{
//    Log("Clearing hexdata...\n");

    Node_HexData = "";
    Node_HexNumLines = 0;

    return true;
}
Console_RegisterCommand(Node_ClearHex);

function Node_AppendHex(hexrow)
{
    if (arguments.length < 1)
    {
        Log("\033[31mNot enough parameters given.\033[0m\n");
        return false;
    }

//    Log("Appending hexrow...\n");

    Node_HexData = Node_HexData + hexrow + "\n";
    Node_HexNumLines += 1;

    return true;
}
Console_RegisterCommand(Node_AppendHex);

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
