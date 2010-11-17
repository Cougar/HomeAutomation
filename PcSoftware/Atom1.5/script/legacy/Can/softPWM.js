
function softPWM(type, name, id)
{
	this.CanService(type, name, id);
	this.myLastValue = new Array();
}

extend(softPWM, CanService);

softPWM.prototype.myReportInterval = null;
softPWM.prototype.myLastValue = null;

softPWM.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Pwm":
		this.myLastValue[canMessage.getData("Id")] = canMessage.getData("Value");
		this.callEvent("newValue", canMessage.getData("Id"));
//log("PWM settings "+canMessage.getData("Id") + " : "+this.myLastValue[canMessage.getData("Id")] + "\n");
		break;
		
		case "Report_Interval":
		this.myReportInterval = canMessage.getData("Time");
		break;
		}
	}
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

softPWM.prototype.getValue = function(sensorId)
{
	return this.myLastValue[sensorId];
}


softPWM.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}
softPWM.prototype.setPWMValue = function(pwmvalue, sensorId)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Pwm");
	canMessage.setData("Id", sensorId);
	canMessage.setData("Value", pwmvalue);
	sendMessage(canMessage);
}
softPWM.prototype.configure = function(pwmPeriod, pwmResolution)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG");
	canMessage.setData("Period", pwmPeriod);
	canMessage.setData("Resolution", pwmResolution);
	sendMessage(canMessage);
}

