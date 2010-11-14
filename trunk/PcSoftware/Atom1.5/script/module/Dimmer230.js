
function Dimmer230(name)
{
	this.Module(name);
}

Extend(Dimmer230, Module);

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

Dimmer230.prototype.StartFade = function(id, channel, speed, direction)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Direction" : direction } ];
	
	this.Module.prototype.SendMessage.call(this, id, "Start_Fade", variables);
}

Dimmer230.prototype.StopFade = function(id, channel)
{
	var variables = [
	{ "Channel"  : channel } ];
	
	this.Module.prototype.SendMessage.call(this, id, "Stop_Fade", variables);
}

Dimmer230.prototype.AbsoluteFade = function(id, channel, speed, end_value)
{
	var variables = [
	{ "Channel"  : channel },
	{ "Speed"    : speed },
	{ "EndValue" : end_value } ];
	
	this.Module.prototype.SendMessage.call(this, id, "Abs_Fade", variables);
}

Dimmer230.prototype.RelativeFade = function(id, channel, speed, direction, steps)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Direction" : direction },
	{ "Steps"     : steps } ];
	
	this.Module.prototype.SendMessage.call(this, id, "Rel_Fade", variables);
}

Dimmer230.prototype.Demo = function(id, channel, speed, steps)
{
	var variables = [
	{ "Channel"   : channel },
	{ "Speed"     : speed },
	{ "Steps"     : steps } ];
	
	this.Module.prototype.SendMessage.call(this, id, "Demo", variables);
}
