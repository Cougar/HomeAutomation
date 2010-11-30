
function fadeto(alias, level, speed)
{
	var alias_data = resolvealias(alias);
	
	if (alias_data == null)
	{
		return "No such module name found";
	}
	
	if (alias_data["is_group"])
	{
		var result = "";
		
		for (var n = 0; n < alias_data["aliases"].length; n++)
		{
			result += fadeto(alias_data["aliases"][n], level, speed) + "\n";
		}
		
		return result;
	}
	
	if (alias_data["module_name"] != "Dimmer230")
	{
		return "Module type " + alias_data["module_name"] + " is not valid for this action, valid are Dimmer230";
	}
	
	if (typeof alias_data["module_id"] == 'undefined' || typeof alias_data["extra"] == 'undefined')
	{
		return "Alias is corrupt";
	}
	
	if (!speed)
	{
		speed = 135;
	}
	
	return Dimmer230_AbsoluteFade(alias_data["module_id"], alias_data["extra"], speed, level);
}
RegisterConsoleCommand(fadeto, function(args) { return StandardAutocomplete(args, Dimmer230.instance_.GetAvailableNames(), Dimmer230.standard_levels_ , Dimmer230.standard_speeds_ ); });

function irsend_autocomplete(args)
{
	// args[0] == command name
	var arg_index = args.length - 2;
	
	if (arg_index == 0)
	{
		return irTransmit.instance_.GetAvailableNames();
	}
	else if (arg_index == 1)
	{
		var remotes = Storage_GetParameters("RemoteList");
		
		var result = new Array();
		
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
		
		var result = new Array();
		
		for (var name in buttons)
		{
			result.push(name);
		}
		
		return result;
	}
	
	return new Array();
}

function irsend(alias, remote, button)
{
	var alias_data = resolvealias(alias);
	
	if (alias_data == null)
	{
		return "No such module name found";
	}
	
	if (alias_data["is_group"])
	{
		var result = "";
		
		for (var n = 0; n < alias_data["aliases"].length; n++)
		{
			result += irsend(alias_data["aliases"][n], remote, button) + "\n";
		}
		
		return result;
	}
	
	if (alias_data["module_name"] != "irTransmit")
	{
		return "Module type " + alias_data["module_name"] + " is not valid for this action, valid are irTransmit";
	}
	
	if (typeof alias_data["module_id"] == 'undefined' || typeof alias_data["extra"] == 'undefined')
	{
		return "Alias is corrupt";
	}
	
	var remote_storage_name = Storage_GetParameter("RemoteList", remote);
	
	if (!remote_storage_name)
	{
		return "No such remote name in list, " + remote;
	}
	
	var button_string = Storage_GetParameter(remote_storage_name, button);
	
	if (!button_string)
	{
		return "No such button " + button + " found on remote " + remote;
	}
	
	var button = eval("(" + button_string + ")");
	
	if (typeof button["protocol"] == 'undefined' || typeof button["data"] == 'undefined')
	{
		return "Button is corrupt";
	}
	
	return irTransmit_SendCode(alias_data["module_id"], alias_data["extra"], "Burst", button["protocol"], button["data"]);
}
RegisterConsoleCommand(irsend, irsend_autocomplete);
