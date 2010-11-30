
function Dimmer230(name)
{
	this.Module(name);
	Dimmer230.instance_ = this;
}

Extend(Dimmer230, Module);

Dimmer230.instance_ = null;
Dimmer230.standard_speeds_ = [ 50, 135, 200, 255 ];
Dimmer230.standard_levels_ = [ 0, 50, 100, 150, 200, 255 ];
Dimmer230.standard_steps_ = [ 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ];
Dimmer230.standard_direction_ = [ "Increase", "Decrease" ];

Dimmer230.prototype.ReceiveMessage = function(id, command, variables)
{
	switch (command)
	{
		case "Netinfo":
		{
			break;
		}
	}
	
	this.Module.prototype.ReceiveMessage.call(this, id, command, variables);
}

function Dimmer230_StartFade(id, channel, speed, direction)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Direction" : direction } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Start_Fade", variables);
	return "OK";
}
RegisterConsoleCommand(Dimmer230_StartFade, function(args) { return StandardAutocomplete(args, Dimmer230.instance_.GetAvailableIds(), [ 0 ], Dimmer230.standard_speeds_, Dimmer230.standard_direction_); });


function Dimmer230_StopFade(id, channel)
{
	var variables = [
	{ "Channel"  : channel } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Stop_Fade", variables);
	return "OK";
}
RegisterConsoleCommand(Dimmer230_StopFade, function(args) { return StandardAutocomplete(args, Dimmer230.instance_.GetAvailableIds(), [ 0 ]); });


function Dimmer230_AbsoluteFade(id, channel, speed, level)
{
	var variables = [
	{ "Channel"  : channel },
	{ "Speed"    : speed },
	{ "EndValue" : level } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Abs_Fade", variables);
	return "OK";
}
RegisterConsoleCommand(Dimmer230_AbsoluteFade, function(args) { return StandardAutocomplete(args, Dimmer230.instance_.GetAvailableIds(), [ 0 ], Dimmer230.standard_speeds_, Dimmer230.standard_levels_); });


function Dimmer230_RelativeFade(id, channel, speed, direction, steps)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Direction" : direction },
	{ "Steps"     : steps } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Rel_Fade", variables);
	return "OK";
}
RegisterConsoleCommand(Dimmer230_RelativeFade, function(args) { return StandardAutocomplete(args, Dimmer230.instance_.GetAvailableIds(), [ 0 ], Dimmer230.standard_speeds_, Dimmer230.standard_direction_, Dimmer230.standard_steps_); });


function Dimmer230_Demo(id, channel, speed, steps)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Steps"     : steps } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Demo", variables);
	return "OK";
}
RegisterConsoleCommand(Dimmer230_Demo, function(args) { return StandardAutocomplete(args, Dimmer230.instance_.GetAvailableIds(), [ 0 ], Dimmer230.standard_speeds_, Dimmer230.standard_steps_); });
