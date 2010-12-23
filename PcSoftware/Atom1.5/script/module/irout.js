
IROut_ModuleNames    = [ "irTransmit" ];
IROut_Channels       = function() { return [ 0 ]; };
IROut_Statuses       = function() { return [ "Pressed", "Released", "Burst" ]; };
IROut_Protocols      = function() { return [ "RC5", "RC6", "RCMM", "SIRC", "Sharp", "NEC", "Samsung", "Marantz", "Panasonic", "Sky", "Nexa2" ]; };
IROut_Aliases        = function() { return Module_GetAliasNames(IROut_ModuleNames); };
IROut_AvailableIds   = function() { return Module_GetAvailableIds(IROut_ModuleNames); };

function IROut_StopSend(alias_name)
{
	if (arguments.length < 1)
	{
		Log("\033[31;1mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, IROut_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
		"Channel"  : aliases_data[name]["specific"]["Channel"],
		"Status"   : "Released" };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "IR", variables))
		{
			Log("\033[32;1mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31;1mFailed to send command to " + name + ".\033[0m\n");
		}
		
		found = true;
	}
	
	if (!found)
	{
		Log("\033[31;1mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	
	return true;
}
Console_RegisterCommand(IROut_StopSend, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, IROut_Aliases()); });

function IROut_StartSendAutoComplete(arg_index, args)
{
	if (arg_index == 0)
	{
		return IROut_Aliases();
	}
	else if (arg_index == 1)
	{
		return get_keys(Storage_GetParameters("RemoteList"));
	}
	else if (arg_index == 2)
	{
		var remote_storage_name = Storage_GetParameter("RemoteList", args[arg_index - 1]);

		return get_keys(Storage_GetParameters(remote_storage_name));
	}
	
	return [];
}

function IROut_Send(alias_name, remote_name, button_name, status)
{
	if (arguments.length < 4)
	{
		Log("\033[31;1mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, IROut_ModuleNames);
	
	var remote_storage_name = Storage_GetParameter("RemoteList", remote_name);
	
	if (!remote_storage_name)
	{
		Log("\033[31;1mNo such remote name in list, " + remote_name + ".\033[0m\n");
		return false;
	}
	
	var button_string = Storage_GetParameter(remote_storage_name, button_name);
	
	if (!button_string)
	{
		Log("\033[31;1mNo such button " + button_name + " found on remote " + remote + ".\033[0m\n");
		return false;
	}
	
	var button_data = eval("(" + button_string + ")");
	
	if (typeof button_data["protocol"] == 'undefined' || typeof button_data["data"] == 'undefined')
	{
		Log("\033[31;1mButton data is corrupt.\033[0m\n");
		return false;
	}
	
	for (var name in aliases_data)
	{
		var variables = {
		"Channel"  : aliases_data[name]["specific"]["Channel"],
		"Protocol" : button_data["protocol"],
		"IRdata"   : button_data["data"],
		"Status"   : status };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "IR", variables))
		{
			Log("\033[32;1mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31;1mFailed to send command to " + name + ".\033[0m\n");
		}
		
		found = true;
	}
	
	if (!found)
	{
		Log("\033[31;1mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	
	return true;
}

function IROut_StartSend(alias_name, remote_name, button_name)
{
	return IROut_Send(alias_name, remote_name, button_name, "Pressed");
}
Console_RegisterCommand(IROut_StartSend, IROut_StartSendAutoComplete);

function IROut_SendBurst(alias_name, remote_name, button_name)
{
	return IROut_Send(alias_name, remote_name, button_name, "Burst");
}
Console_RegisterCommand(IROut_SendBurst, IROut_StartSendAutoComplete);

