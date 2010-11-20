
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
	Log("status change!");
}

ir_module.Bind("onStatusChange", test_status);
ir_module.Bind("onPressed", test_press);
ir_module.Bind("onReleased", test_release);


function dmin()
{
	Dimmer230_AbsoluteFade(2, 0, 135, 0);
	Dimmer230_AbsoluteFade(1, 0, 135, 0);
	return "Faded to 0\n";
}

function dmax()
{
	Dimmer230_AbsoluteFade(2, 0, 135, 255);
	Dimmer230_AbsoluteFade(1, 0, 135, 255);
	return "Faded to 255\n";
}

function dfade(level)
{
	Dimmer230_AbsoluteFade(2, 0, 135, level);
	Dimmer230_AbsoluteFade(1, 0, 135, level);
	return "Faded to " + level + "\n";
}

function dfade_complete(args)
{
	var result = new Array();
	
	var arg_index = args.length - 1;
	
	if (arg_index == 1)
	{
		result.push(0);
		result.push(100);
		result.push(200);
		result.push(255);
	}
	
	return result;
}

RegisterConsoleCommand("dmax");
RegisterConsoleCommand("dmin");
RegisterConsoleCommand("dfade", dfade_complete);
