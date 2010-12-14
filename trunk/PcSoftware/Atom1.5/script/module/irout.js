
IROut_ModuleName     = "irTransmit";
IROut_Channels       = function() { return [ 0 ]; };
IROut_Statuses       = function() { return [ "Pressed", "Released", "Burst" ]; };
IROut_Protocols      = function() { return [ "RC5", "RC6", "RCMM", "SIRC", "Sharp", "NEC", "Samsung", "Marantz", "Panasonic", "Sky", "Nexa2" ]; };
IROut_Aliases        = function() { return Module_GetAliasNames([ IROut_ModuleName ]); };
IROut_AvailableIds   = function() { return Module_GetAvailableIds([ IROut_ModuleName ]); };

function IROut_MessageAliasLookup(module_id, command, variables)
{
	var aliases_data = {};
	
	aliases_data = Module_LookupAliases({
		"module_name" : IROut_ModuleName,
		"module_id"   : module_id,
		"channel"     : channel
	});
	
	return aliases_data;
}
Module_RegisterMessageAliasLookup(IROut_ModuleName, IROut_MessageAliasLookup);

function IROut_CreateAlias(alias_name, module_id, channel)
{
	var alias_data = {
		"group"       : false,
		"name"        : alias_name,
		"module_name" : IROut_ModuleName,
		"module_id"   : module_id,
		"channel"     : channel
	};
	
	return Storage_SetParameter("alias", alias_name, JSON.stringify(alias_data));
}
Console_RegisterCommand(IROut_CreateAlias, function(args) { return Console_StandardAutocomplete(args, IROut_Aliases(), IROut_AvailableIds(), IROut_Channels()); });

function IROut_StopSend(alias_name)
{
	if (arguments.length < 1)
	{
		throw("Not enought parameters given");
	}
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, [ IROut_ModuleName ]);
	
	
	for (var name in aliases_data)
	{
		var variables = {
			"Channel"  : aliases_data[name]["channel"],
			"Status"   : "Released" };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "IR", variables))
		{
			result_text += "StopSend sent successfully to " + name + "\n";
		}
		else
		{
			result_text += "StopSend failed to send to " + name + "\n";
		}
	}
	
	if (result_text.length == 0)
	{
		result_text = "No aliases by the name " + alias_name + " were applicable to StopSend.";
	}
	
	return result_text;
}
Console_RegisterCommand(IROut_StopSend, function(args) { return Console_StandardAutocomplete(args, IROut_Aliases()); });

function IROut_StartSendAutoComplete(args)
{
	// args[0] == command name
	var arg_index = args.length - 2;
	
	if (arg_index == 0)
	{
		return IROut_Aliases();
	}
	else if (arg_index == 1)
	{
		var remotes = Storage_GetParameters("RemoteList");
		
		var result = [];
		
		for (var name in remotes)
		{
			result.push(name);
		}
		
		return result;
	}
	else if (arg_index == 2)
	{
		var remote_storage_name = Storage_GetParameter("RemoteList", args[arg_index]);
		
		var buttons = Storage_GetParameters(remote_storage_name);
		
		var result = [];
		
		for (var name in buttons)
		{
			result.push(name);
		}
		
		return result;
	}
	
	return [];
}

function IROut_Send(alias_name, remote_name, button_name, status)
{
	if (arguments.length < 4)
	{
		throw("Not enought parameters given");
	}
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, [ IROut_ModuleName ]);
	
	var remote_storage_name = Storage_GetParameter("RemoteList", remote_name);
	
	if (!remote_storage_name)
	{
		return "No such remote name in list, " + remote_name;
	}
	
	var button_string = Storage_GetParameter(remote_storage_name, button_name);
	
	if (!button_string)
	{
		return "No such button " + button_name + " found on remote " + remote;
	}
	
	var button_data = eval("(" + button_string + ")");
	
	if (typeof button_data["protocol"] == 'undefined' || typeof button_data["data"] == 'undefined')
	{
		return "Button data is corrupt";
	}
	
	
	for (var name in aliases_data)
	{
		var variables = {
			"Channel"  : aliases_data[name]["channel"],
			"Protocol" : button_data["protocol"],
			"IRdata"   : button_data["data"],
			"Status"   : status };
			
			if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "IR", variables))
			{
				result_text += "StartSend sent successfully to " + name + "\n";
			}
			else
			{
				result_text += "StartSend failed to send to " + name + "\n";
			}
	}
	
	if (result_text.length == 0)
	{
		result_text = "No aliases by the name " + alias_name + " were applicable to StartSend.";
	}
	
	return result_text;
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

