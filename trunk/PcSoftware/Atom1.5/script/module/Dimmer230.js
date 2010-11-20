
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

Dimmer230_complete = function(args)
{
	var result = new Array();
	
	var arg_index = args.length - 1;
	
	if (arg_index == 1) // id
	{
		result = Dimmer230.instance_.GetAvailableIds();
	}
	else if (arg_index == 2) // channel
	{
		result.push(0);
	}
	else if (arg_index > 2)
	{
		if (args[0] == "Dimmer230_StartFade")
		{
			if (arg_index == 3) // speed
			{
				result.push(50);
				result.push(135);
				result.push(200);
				result.push(255);
			}
			else if (arg_index == 4) // direction
			{
				result.push("Increase");
				result.push("Decrease");
			}
		}
		else if (args[0] == "Dimmer230_AbsoluteFade")
		{
			if (arg_index == 3) // speed
			{
				result.push(50);
				result.push(135);
				result.push(200);
				result.push(255);
			}
			else if (arg_index == 4) // end_value
			{
				result.push(0);
				result.push(50);
				result.push(100);
				result.push(150);
				result.push(200);
				result.push(255);
			}
		}
		else if (args[0] == "Dimmer230_RelativeFade")
		{
			if (arg_index == 3) // speed
			{
				result.push(50);
				result.push(135);
				result.push(200);
				result.push(255);
			}
			else if (arg_index == 4) // direction
			{
				result.push("Increase");
				result.push("Decrease");
			}
			else if (arg_index == 5) // steps
			{
				result.push(0);
				result.push(10);
				result.push(20);
				result.push(30);
				result.push(40);
				result.push(50);
				result.push(60);
				result.push(70);
				result.push(80);
				result.push(90);
			}
		}
		else if (args[0] == "Dimmer230_Demo")
		{
			if (arg_index == 3) // speed
			{
				result.push(50);
				result.push(135);
				result.push(200);
				result.push(255);
			}
			else if (arg_index == 4) // steps
			{
				result.push(0);
				result.push(10);
				result.push(20);
				result.push(30);
				result.push(40);
				result.push(50);
				result.push(60);
				result.push(70);
				result.push(80);
				result.push(90);
			}
		}
	}
	
	return result;
}

Dimmer230_StartFade = function(id, channel, speed, direction)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Direction" : direction } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Start_Fade", variables);
	return "OK";
}
RegisterConsoleCommand("Dimmer230_StartFade", Dimmer230_complete);


Dimmer230_StopFade = function(id, channel)
{
	var variables = [
	{ "Channel"  : channel } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Stop_Fade", variables);
	return "OK";
}
RegisterConsoleCommand("Dimmer230_StopFade", Dimmer230_complete);


Dimmer230_AbsoluteFade = function(id, channel, speed, end_value)
{
	var variables = [
	{ "Channel"  : channel },
	{ "Speed"    : speed },
	{ "EndValue" : end_value } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Abs_Fade", variables);
	return "OK";
}
RegisterConsoleCommand("Dimmer230_AbsoluteFade", Dimmer230_complete);


Dimmer230_RelativeFade = function(id, channel, speed, direction, steps)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Direction" : direction },
	{ "Steps"     : steps } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Rel_Fade", variables);
	return "OK";
}
RegisterConsoleCommand("Dimmer230_RelativeFade", Dimmer230_complete);


Dimmer230_Demo = function(id, channel, speed, steps)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Steps"     : steps } ];
	
	Dimmer230.instance_.Module.prototype.SendMessage.call(Dimmer230.instance_, id, "Demo", variables);
	return "OK";
}
RegisterConsoleCommand("Dimmer230_Demo", Dimmer230_complete);
