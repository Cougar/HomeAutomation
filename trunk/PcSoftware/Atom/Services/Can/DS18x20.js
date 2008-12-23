
function DS18x20(name, id)
{
	this.CanService(name, id);
	
	this.myLastValue = new Array();
}

extend(DS18x20, CanService);

DS18x20.prototype.myReportInterval = null;
DS18x20.prototype.myLastValue = null;

DS18x20.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Temperature_Celsius":
		log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("Value") + "\n");
		this.myLastValue[canMessage.getData("SensorId")] = canMessage.getData("Value");
		this.callEvent("newValue", canMessage.getData("SensorId"));
		break;
		
		case "Sensor_Report_Interval":
		log(this.myName + ":" + this.myId + "> New report interval: " + canMessage.getData("Time") + "\n");
		this.myReportInterval = canMessage.getData("Time");
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

DS18x20.prototype.getValue = function(sensorId)
{
	return this.myLastValue[sensorId];
}

DS18x20.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "Sensor_Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}
