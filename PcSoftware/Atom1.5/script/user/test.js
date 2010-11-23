
var dimmer_module = GetModule("Dimmer230");
var ir_module = GetModule("irReceive");

function test_press(event, id, channel, data, protocol)
{
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
}

function test_release(event, id, channel, data, protocol)
{
	if (protocol == "NEC" && (data == 3877174275 || data == 3860462595))
	{
		Dimmer230_StopFade(2, 0);
	}
}

function test_status(event, id, available)
{
	//Log("status change!");
}

ir_module.Bind("onStatusChange", test_status);
ir_module.Bind("onPressed", test_press);
ir_module.Bind("onReleased", test_release);


CreateModuleAlias("bardisk", "Dimmer230", 2, { "channel" : 0 });
CreateModuleAlias("bokhylla", "Dimmer230", 1, { "channel" : 0 });
CreateModuleAliasGroup("vardagsrum", [ "bokhylla", "bardisk" ]);


function fadeto(alias, level, speed)
{
	if (typeof module_aliases[alias] == 'undefined')
	{
		return "No such module name found";
	}
	
	if (module_aliases[alias]["is_group"])
	{
		var result = "";
		
		for (var n = 0; n < module_aliases[alias]["aliases"].length; n++)
		{
			result += fadeto(module_aliases[alias]["aliases"][n], level, speed) + "\n";
		}
		
		return result;
	}
	
	if (module_aliases[alias]["module_name"] != "Dimmer230")
	{
		return "Module type " + module_aliases[alias]["module_name"] + " is not valid for this action, valid are Dimmer230";
	}
	
	if (!speed)
	{
		speed = 135;
	}

	return Dimmer230_AbsoluteFade(module_aliases[alias]["module_id"], module_aliases[alias]["extra"]["channel"], speed, level);
}

RegisterConsoleCommand(fadeto, function(args) { return StandardAutocomplete(args, Dimmer230.instance_.GetAvailableNames(), [ 0, 50, 100, 150, 200, 255 ], [ 50, 135, 200, 255 ]); });
