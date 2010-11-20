
var dimmer_module = GetModule("Dimmer230");
var ir_module = GetModule("irReceive");

function test_press(event, id, channel, data, protocol)
{
	if (protocol == "NEC" && data == 3877174275)
	{
		dimmer_module.StartFade(2, 0, 135, "Increase");
		dimmer_module.StartFade(1, 0, 135, "Increase");
	}
	else if (protocol == "NEC" && data == 3860462595)
	{
		dimmer_module.StartFade(2, 0, 135, "Decrease");
		dimmer_module.StartFade(1, 0, 135, "Decrease");
	}
}

function test_release(event, id, channel, data, protocol)
{
	if (protocol == "NEC" && (data == 3877174275 || data == 3860462595))
	{
		dimmer_module.StopFade(2, 0);
	}
}

function test_status(event, id, available)
{
	Log("status change!");
	dimmer_module.AbsoluteFade(2, 0, 135, 0);
}

ir_module.Bind("onStatusChange", test_status);
ir_module.Bind("onPressed", test_press);
ir_module.Bind("onReleased", test_release);


function dmin()
{
	dimmer_module.AbsoluteFade(2, 0, 135, 0);
	dimmer_module.AbsoluteFade(1, 0, 135, 0);
	return "Faded to 0\n";
}

function dmax()
{
	dimmer_module.AbsoluteFade(2, 0, 135, 255);
	dimmer_module.AbsoluteFade(1, 0, 135, 255);
	return "Faded to 255\n";
}

function dfade(level)
{
	dimmer_module.AbsoluteFade(2, 0, 135, level);
	dimmer_module.AbsoluteFade(1, 0, 135, level);
	return "Faded to " + level + "\n";
}

Console_RegisterConsoleCommand("dmax");
Console_RegisterConsoleCommand("dmin");
Console_RegisterConsoleCommand("dfade");
