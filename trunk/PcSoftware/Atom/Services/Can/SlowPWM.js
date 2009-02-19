
function SlowPWM(type, name, id)
{
	this.CanService(type, name, id);
	this.myLastValue = new Array();
}

extend(SlowPWM, CanService);

SlowPWM.prototype.myReportInterval = null;
SlowPWM.prototype.myLastValue = null;

SlowPWM.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "SlowPWM":
		log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("Value") + "\n");
		this.myLastValue[canMessage.getData("Id")] = canMessage.getData("Value");
		this.callEvent("newValue", canMessage.getData("Id"));
		break;
		
		case "Report_Interval":
		log(this.myName + ":" + this.myId + "> New report interval: " + canMessage.getData("Time") + "\n");
		this.myReportInterval = canMessage.getData("Time");
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

SlowPWM.prototype.getValue = function(sensorId)
{
	return this.myLastValue[sensorId];
}


SlowPWM.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}
SlowPWM.prototype.setPWMValue = function(pwmvalue, sensorId)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "SlowPWM");
	canMessage.setData("ChannelId", sensorId);
	canMessage.setData("Value", pwmvalue);
	sendMessage(canMessage);
}
SlowPWM.prototype.configure = function(pwmPeriod, pwmDefaultValue, DefaultState, StartUpState)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG");
	canMessage.setData("pwmPeriod", pwmPeriod);
	canMessage.setData("pwmDefaultValue", pwmDefaultValue);
	canMessage.setData("DefaultState", DefaultState);
	canMessage.setData("StartUpState", StartUpState);
	sendMessage(canMessage);
}

