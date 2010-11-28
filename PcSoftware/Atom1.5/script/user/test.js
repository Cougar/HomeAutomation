
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
