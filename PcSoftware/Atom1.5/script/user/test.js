
RequireModule("irReceive");
RequireModule("irTransmit");
RequireModule("Dimmer230");

var dimmer_module = GetModule("Dimmer230");
var ir_module = GetModule("irReceive");

function test_press(event, id, channel, data, protocol)
{
	Log("Pressed: protocol=" + protocol + ", channel=" + channel + ", data=" + data);
	
	if (protocol == "NEC" && data == 3877174275)
	{
		Dimmer230_StartFade(2, 0, 135, "Increase");
		Dimmer230_StartFade(1, 0, 135, "Increase");
	}
	else if (protocol == "NEC" && data == 3860462595)
	{
		Dimmer230_StartFade(2, 0, 135, "Decrease");
		Dimmer230_StartFade(1, 0, 135, "Decrease");
	}
	else if (protocol == "NEC" && data == 4144561155)
	{
		irTransmit_SendCode(1, 0, "Burst", "NEC", 3843720570);
	}
	else if (protocol == "NEC" && data == 4211407875)
	{
		irTransmit_SendCode(1, 0, "Burst", "NEC", 3827008890);
	}
}

function test_release(event, id, channel, data, protocol)
{
	Log("Released: protocol=" + protocol + ", channel=" + channel + ", data=" + data);
	
	if (protocol == "NEC" && (data == 3877174275 || data == 3860462595))
	{
		Dimmer230_StopFade(2, 0);
		Dimmer230_StopFade(1, 0);
	}
	else if (protocol == "NEC" && (data == 4144561155 || data == 4211407875))
	{
		//irTransmit_SendCode(1, 0, "Released", "NEC", data);
	}
}

function test_status(event, id, available)
{
	//Log("status change!");
}

ir_module.Bind("onStatusChange", test_status);
ir_module.Bind("onPressed", test_press);
ir_module.Bind("onReleased", test_release);


/*CreateModuleAlias("bardisk", "Dimmer230", 2, { "channel" : 0 });
CreateModuleAlias("bokhylla", "Dimmer230", 1, { "channel" : 0 });
CreateModuleAliasGroup("vardagsrum", [ "bokhylla", "bardisk" ]);
*/

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
	
	if (!speed)
	{
		speed = 135;
	}

	return Dimmer230_AbsoluteFade(alias_data["module_id"], alias_data["extra"], speed, level);
}
RegisterConsoleCommand(fadeto, function(args) { return StandardAutocomplete(args, Dimmer230.instance_.GetAvailableNames(), [ 0, 50, 100, 150, 200, 255 ], [ 50, 135, 200, 255 ]); });

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
	
	Log(JSON.stringify(button));
	Log(alias_data["module_id"]);
	Log(alias_data["extra"]);
	Log(button["protocol"]);
	Log(button["data"]);
	
	return irTransmit_SendCode(alias_data["module_id"], alias_data["extra"], "Burst", button["protocol"], button["data"]);
}
RegisterConsoleCommand(irsend, irsend_autocomplete);


function media(mode)
{
	if (!mode)
	{
		return "You must specify a mode";
	}
	
	if (mode == "movie")
	{
		irsend("tvbankut", "Yamaha_RAV34", "POWER");
		
		fadeto("bokhylla", 100, 135);
		fadeto("bardisk", 0, 135);

		sleep(1000);
		irsend("tvbankut", "Yamaha_RAV34", "CD");
	}
	else if (mode == "tv")
	{
		irsend("tvbankut", "Yamaha_RAV34", "POWER");
		
		fadeto("bokhylla", 150, 135);
		fadeto("bardisk", 150, 135);

		sleep(1000);
		irsend("tvbankut", "Yamaha_RAV34", "DTV_CBL");
	}
	else if (mode == "off")
	{
		irsend("tvbankut", "Yamaha_RAV34", "STANDBY");
		
		fadeto("bokhylla", 0, 135);
		fadeto("bardisk", 0, 135);
	}
	
	return "OK";
}
RegisterConsoleCommand(media, function(args) { return StandardAutocomplete(args, [ "off", "tv", "movie" ]); });
