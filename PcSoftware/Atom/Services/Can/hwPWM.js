
function hwPWM(type, name, id)
{
	this.CanService(type, name, id);
	this.myLastValue = new Array();
}

extend(hwPWM, CanService);

hwPWM.prototype.currentValue = 0;
hwPWM.prototype.myReportInterval = null;
hwPWM.prototype.myLastValue = null;

hwPWM.prototype.canMessageHandler = function(canMessage)
{
	var self = this;
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{

		case "Pwm":
				this.currentValue = canMessage.getData("Value")/39;
				var dataArray = new Array();
				dataArray["service"] = self;
				dataArray["channel"] = canMessage.getData("Id");
				dataArray["currentValue"] = (canMessage.getData("Value")/39);
				dataArray["moduleName"] = canMessage.getModuleName();
				dataArray["moduleId"] = canMessage.getModuleId();
				this.callEvent("newValue", dataArray);
				//log(this.myName + ":" + this.myId + "> Netstatus, " + connectString + frequencyString + "\n");
			break;

		/*case "hwPWM":
		log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("Value") + "\n");
		this.myLastValue[canMessage.getData("Id")] = canMessage.getData("Value");
		this.callEvent("newValue", canMessage.getData("Id"));
		break;
		
		case "Report_Interval":
		log(this.myName + ":" + this.myId + "> New report interval: " + canMessage.getData("Time") + "\n");
		this.myReportInterval = canMessage.getData("Time");
		break;*/
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

hwPWM.prototype.getValue = function(sensorId)
{
	return this.myLastValue[sensorId];
}


/*hwPWM.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}*/

/**
pwmValue in % with 0.01 resolution (Example: 55.25%)
*/
hwPWM.prototype.setPWMValue = function(pwmvalue, sensorId)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Pwm");
	canMessage.setData("Id", sensorId);
	canMessage.setData("Value", pwmvalue*100);
	sendMessage(canMessage);
	
	this.myLastValue[sensorId] = pwmvalue;
}
/*hwPWM.prototype.configure = function(pwmPeriod, pwmDefaultValue, DefaultState, StartUpState)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG");
	canMessage.setData("pwmPeriod", pwmPeriod);
	canMessage.setData("pwmDefaultValue", pwmDefaultValue);
	canMessage.setData("DefaultState", DefaultState);
	canMessage.setData("StartUpState", StartUpState);
	sendMessage(canMessage);
}*/

hwPWM.prototype.startFade = function(channel, speed, direction)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Start_Fade");
	canMessage.setData("Channel", channel);
	canMessage.setData("Speed", speed);
	canMessage.setData("Direction", direction);
	sendMessage(canMessage);
}

hwPWM.prototype.stopFade = function(channel)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Stop_Fade");
	canMessage.setData("Channel", channel);
	sendMessage(canMessage);
}

hwPWM.prototype.absFade = function(channel, speed, endValue)
{
log("sending: "+channel+" "+speed+" "+endValue*39+"\n");	
var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Abs_Fade");
	canMessage.setData("Channel", channel);
	canMessage.setData("Speed", speed);
	canMessage.setData("EndValue", endValue*39);
	sendMessage(canMessage);
}

hwPWM.prototype.relFade = function(channel, speed, direction, steps)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Rel_Fade");
	canMessage.setData("Channel", channel);
	canMessage.setData("Speed", speed);
	canMessage.setData("Direction", direction);
	canMessage.setData("Steps", steps*39);
	sendMessage(canMessage);
}

hwPWM.prototype.demo = function(channel, speed, steps)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Demo");
	canMessage.setData("Channel", channel);
	canMessage.setData("Speed", speed);
	canMessage.setData("Steps", steps*39);
	sendMessage(canMessage);
}

