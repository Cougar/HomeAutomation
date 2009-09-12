
function FOST02(type, name, id)
{
	this.CanService(type, name, id);
	
	this.myLastValueTemp = new Array();
	this.myLastValueHumi = new Array();
}

extend(FOST02, CanService);

FOST02.prototype.myReportInterval = null;
FOST02.prototype.myLastValue = null;

FOST02.prototype.canMessageHandler = function(canMessage)
{
var self = this;
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Temperature_Celsius":
//		log(this.myName + ":" + this.myId + "> New temperature value: " + canMessage.getData("Value") + "\n");
		this.myLastValueTemp[canMessage.getData("SensorId")] = canMessage.getData("Value");
		this.callEvent("newTempValue", canMessage.getData("SensorId"));
		var dataArray = new Array();
		dataArray["service"] = self;
		dataArray["sensor"] = canMessage.getData("SensorId");
		dataArray["value"] = canMessage.getData("Value");
		dataArray["moduleName"] = canMessage.getModuleName();
		dataArray["moduleId"] = canMessage.getModuleId();
		this.callEvent("newValue", dataArray);
		break;
		
		case "Humidity_Percent":
//		log(this.myName + ":" + this.myId + "> New humidity value: " + canMessage.getData("Value") + "\n");
		this.myLastValueHumi[canMessage.getData("SensorId")] = canMessage.getData("Value");

		this.callEvent("newHumidityValue", canMessage.getData("SensorId"));

		break;

		case "Sensor_Report_Interval":
		log(this.myName + ":" + this.myId + "> New report interval: " + canMessage.getData("Time") + "\n");
		this.myReportInterval = canMessage.getData("Time");
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

FOST02.prototype.getValueTemp = function(sensorId)
{
	return this.myLastValueTemp[sensorId];
}

FOST02.prototype.getValueHumi = function(sensorId)
{
	return this.myLastValueHumi[sensorId];
}

FOST02.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "Sensor_Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}
