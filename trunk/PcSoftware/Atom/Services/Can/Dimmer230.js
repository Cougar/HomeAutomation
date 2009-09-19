
function Dimmer230(type, name, id)
{
	this.CanService(type, name, id);
}


Dimmer230.prototype.currentValue = 0;
Dimmer230.prototype.connection = "Disconnected";
Dimmer230.prototype.frequency = "Error / Unknown";


extend(Dimmer230, CanService);

Dimmer230.prototype.canMessageHandler = function(canMessage)
{
var self = this;
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Netinfo":
		this.connection = canMessage.getData("Connection");
		this.frequency = canMessage.getData("Frequency");
		this.currentValue = canMessage.getData("DimmerValue");
		var connectString = "disconnected.";
		var frequencyString = "";
		if (this.connection == "Connected")
		{
			connectString = "connected";
			frequencyString = " with frequency "+this.frequency;
		}

var dataArray = new Array();
dataArray["service"] = self;
dataArray["channel"] = canMessage.getData("Channel");
dataArray["connection"] = canMessage.getData("Connection");
dataArray["currentValue"] = canMessage.getData("DimmerValue");
dataArray["moduleName"] = canMessage.getModuleName();
dataArray["moduleId"] = canMessage.getModuleId();
this.callEvent("newValue", dataArray);
		//log(this.myName + ":" + this.myId + "> Netstatus, " + connectString + frequencyString + "\n");
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

Dimmer230.prototype.startFade = function(channel, speed, direction)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Start_Fade");
	canMessage.setData("Channel", channel);
	canMessage.setData("Speed", speed);
	canMessage.setData("Direction", direction);
	sendMessage(canMessage);
}

Dimmer230.prototype.stopFade = function(channel)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Stop_Fade");
	canMessage.setData("Channel", channel);
	sendMessage(canMessage);
}

Dimmer230.prototype.absFade = function(channel, speed, endValue)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Abs_Fade");
	canMessage.setData("Channel", channel);
	canMessage.setData("Speed", speed);
	canMessage.setData("EndValue", endValue);
	sendMessage(canMessage);
}

Dimmer230.prototype.relFade = function(channel, speed, direction, steps)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Rel_Fade");
	canMessage.setData("Channel", channel);
	canMessage.setData("Speed", speed);
	canMessage.setData("Direction", direction);
	canMessage.setData("Steps", steps);
	sendMessage(canMessage);
}

Dimmer230.prototype.demo = function(channel, speed, steps)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Demo");
	canMessage.setData("Channel", channel);
	canMessage.setData("Speed", speed);
	canMessage.setData("Steps", steps);
	sendMessage(canMessage);
}

