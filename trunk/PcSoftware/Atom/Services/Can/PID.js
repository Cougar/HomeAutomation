
function PID(type, name, id)
{
	this.CanService(type, name, id);
	this.myLastValue = new Array();
	this.registerEventCallback("online", function(args) { _PID_Online(args); });
}

function _PID_Online(data) {
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG_SENSOR");
	sendMessage(canMessage);
	canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG_ACTUATOR");
	sendMessage(canMessage);
	canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG_PARAMETER");
	sendMessage(canMessage);
	canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Report_Interval");
	sendMessage(canMessage);
}

extend(PID, CanService);

PID.prototype.myReportInterval = null;

PID.prototype.sensorModuleType = null;
PID.prototype.sensorModuleId = null;
PID.prototype.sensorId = null;

PID.prototype.actuatorModuleType = null;
PID.prototype.actuatorModuleId = null;
PID.prototype.actuatorId = null;

PID.prototype.K_P= null;
PID.prototype.K_I= null;
PID.prototype.K_D= null;
PID.prototype.TimeUnit= null;
PID.prototype.RegulatorTime = null;

PID.prototype.Measurment = null;
PID.prototype.Reference = null;
PID.prototype.PWM = null;
PID.prototype.Sum = null;

PID.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "DEBUG":
			//<variable name="P_term" type="uint" start_bit="0" bit_length="16" unit="" description=""/>
			//<variable name="I_term" type="uint" start_bit="16" bit_length="16" unit="" description=""/>
			//<variable name="D_term" type="uint" start_bit="32" bit_length="16" unit="" description=""/>
			//<variable name="Sum" type="uint" start_bit="48" bit_length="16" unit="" description=""/>
			//this.myLastValue[canMessage.getData("Id")] = canMessage.getData("Value");
			//this.callEvent("newValue", canMessage.getData("Id"));
		break;
		case "CONFIG_SENSOR":
			this.sensorModuleType= canMessage.getData("sensorModuleType");
			this.sensorModuleId= canMessage.getData("sensorModuleId");
			this.sensorId= canMessage.getData("sensorId");
			  log("conf_sensor: " + this.sensorModuleType + " " + this.sensorModuleId + " " + this.sensorId + "\n");
			//this.callEvent("newValue", canMessage.getData("Id"));
		break;
		case "CONFIG_ACTUATOR":
			this.actuatorModuleType= canMessage.getData("actuatorModuleType");
			this.actuatorModuleId= canMessage.getData("actuatorModuleId");
			this.actuatorId= canMessage.getData("actuatorId");
			//this.callEvent("newValue", canMessage.getData("Id"));
log("conf_act: " + this.actuatorModuleType + " " + this.actuatorModuleId + " " + this.actuatorId + "\n");

		break;
		case "CONFIG_PARAMETER":
			this.K_P= canMessage.getData("K_P");
			this.K_I= canMessage.getData("K_I");
			this.K_D= canMessage.getData("K_D");
			this.TimeUnit= canMessage.getData("TimeUnit");
			this.RegulatorTime= canMessage.getData("RegulatorTime");
log("conf_param: " + this.K_P + " " + this.K_I + " " + this.K_D + " " + this.RegulatorTime +  " " + this.TimeUnit + "\n");
			//this.callEvent("newValue", canMessage.getData("Id"));
		break;
		case "PID_STATUS":
			this.Measurment= canMessage.getData("Measurment");
			this.Reference= canMessage.getData("Reference");
			this.PWM= canMessage.getData("PWM");
			this.Sum= canMessage.getData("Sum");
log("PID status: mea:" + this.Measurment + " ref:" + this.Reference + " pwm:" + this.PWM/100 + " sum:" + this.Sum + "\n");
			//this.callEvent("newValue", canMessage.getData("Id"));
		break;
		case "Temperature_Celsius":
			this.Reference= canMessage.getData("Value");
log("PID status: ref:" + this.Reference + "\n");
			//this.callEvent("newValue", canMessage.getData("Id"));
		break;
		case "Report_Interval":
			this.myReportInterval = canMessage.getData("Time");
		break;
		}
	}
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

PID.prototype.setValue = function(temperature)
{
log ("Called SetValue: "+temperature+"\n");
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Temperature_Celsius");
	canMessage.setData("Value", ""+temperature);
	canMessage.setData("SensorId", 0);
log ("> "+canMessage.toString()+"\n");
	sendMessage(canMessage);


}

PID.prototype.getConfiguration = function()
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG_SENSOR");
	sendMessage(canMessage);
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG_ACTUATOR");
	sendMessage(canMessage);
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG_PARAMETER");
	sendMessage(canMessage);
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Report_Interval");
	sendMessage(canMessage);
}

PID.prototype.setParameters = function(KP,KI,KD,Time,Unit)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "CONFIG_PARAMETER");
	canMessage.setData("K_P", ""+KP);
	canMessage.setData("K_I", ""+KI);
	canMessage.setData("K_D", ""+KD);
	canMessage.setData("RegulatorTime", ""+Time);
	canMessage.setData("TimeUnit", ""+Unit);
//log ("> "+canMessage.toString()+"\n");
	sendMessage(canMessage);
}


PID.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}

PID.prototype.getReportInterval = function(time)
{
	
}
