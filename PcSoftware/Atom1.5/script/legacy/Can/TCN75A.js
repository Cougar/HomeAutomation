
function TCN75A(type, name, id)
{
	this.CanService(type, name, id);
	
	this.myLastValue = new Array();
}

extend(TCN75A, CanService);

TCN75A.prototype.myReportInterval = null;
TCN75A.prototype.myLastValue = null;

TCN75A.prototype.canMessageHandler = function(canMessage)
{
	var self = this;
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Temperature_Celsius":
		//log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("Value") + "\n");
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
		
		//case "Sensor_Report_Interval":
		//log(this.myName + ":" + this.myId + "> New report interval: " + canMessage.getData("Time") + "\n");
		//this.myReportInterval = canMessage.getData("Time");
		//break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

TCN75A.prototype.getValue = function(sensorId)
{
	return this.myLastValue[sensorId];
}


