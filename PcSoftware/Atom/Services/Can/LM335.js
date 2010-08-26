
function LM335(type, name, id)
{
	this.CanService(type, name, id);
	
	this.myLastValue = new Array();
}

extend(LM335, CanService);

LM335.prototype.myReportInterval = null;
LM335.prototype.myLastValue = null;

LM335.prototype.canMessageHandler = function(canMessage)
{
	var self = this;
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Temperature_Celsius":
		log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("Value") + "\n");
		this.myLastValue[canMessage.getData("SensorId")] = canMessage.getData("Value");
		//this.callEvent("newValue", canMessage.getData("SensorId"));
		var dataArray = new Array();
		dataArray["service"] = self;
		dataArray["sensor"] = canMessage.getData("SensorId");
		dataArray["value"] = canMessage.getData("Value");
		dataArray["moduleName"] = canMessage.getModuleName();
		dataArray["moduleId"] = canMessage.getModuleId();
		this.callEvent("newValue", dataArray);
		break;
		
		case "Sensor_Report_Interval":
		log(this.myName + ":" + this.myId + "> New report interval: " + canMessage.getData("Time") + "\n");
		this.myReportInterval = canMessage.getData("Time");
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

LM335.prototype.getValue = function(sensorId)
{
	return this.myLastValue[sensorId];
}

LM335.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "Sensor_Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}
