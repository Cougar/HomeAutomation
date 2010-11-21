
function Dimmer230(name)
{
	this.Module(name);
	Dimmer230.instance_ = this;
}

Extend(Dimmer230, Module);

Dimmer230.instance_ = null;

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

function Dimmer230_complete(args)
{
	var result = new Array();
	
	var arg_index = args.length - 1;
	
	if (arg_index == 1) // id
	{
		result = Dimmer230.instance_.GetAvailableIds();
	}
	else if (arg_index == 2) // channel
	{
		result = [ 0 ];
	}
	else if (arg_index > 2)
	{
		if (args[0] == "Dimmer230_StartFade")
		{
			if (arg_index == 3) // speed
			{
				result = [ 50, 135, 200, 255 ];
			}
			else if (arg_index == 4) // direction
			{
				result = [ "Increase", "Decrease" ];
			}
		}
		else if (args[0] == "Dimmer230_AbsoluteFade")
		{
			if (arg_index == 3) // speed
			{
				result = [ 50, 135, 200, 255 ];
			}
			else if (arg_index == 4) // end_value
			{
				result = [ 0, 50, 100, 150, 200, 255 ];
			}
		}
		else if (args[0] == "Dimmer230_RelativeFade")
		{
			if (arg_index == 3) // speed
			{
				result = [ 50, 135, 200, 255 ];
			}
			else if (arg_index == 4) // direction
			{
				result = [ "Increase", "Decrease" ];
			}
			else if (arg_index == 5) // steps
			{
				result = [ 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ];
			}
		}
		else if (args[0] == "Dimmer230_Demo")
		{
			if (arg_index == 3) // speed
			{
				result = [ 50, 135, 200, 255 ];
			}
			else if (arg_index == 4) // steps
			{
				result = [ 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ];
			}
		}
	}
	
	return result;
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
RegisterConsoleCommand(Dimmer230_StartFade, Dimmer230_complete);


function Dimmer230_StopFade(id, channel)
{
	var variables = [
	{ "Channel"  : channel } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Stop_Fade", variables);
	return "OK";
}
RegisterConsoleCommand(Dimmer230_StopFade, Dimmer230_complete);


function Dimmer230_AbsoluteFade(id, channel, speed, end_value)
{
	var variables = [
	{ "Channel"  : channel },
	{ "Speed"    : speed },
	{ "EndValue" : end_value } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Abs_Fade", variables);
	return "OK";
}
RegisterConsoleCommand(Dimmer230_AbsoluteFade, Dimmer230_complete);


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
RegisterConsoleCommand(Dimmer230_RelativeFade, Dimmer230_complete);


function Dimmer230_Demo(id, channel, speed, steps)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Steps"     : steps } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Demo", variables);
	return "OK";
}
RegisterConsoleCommand(Dimmer230_Demo, Dimmer230_complete);
