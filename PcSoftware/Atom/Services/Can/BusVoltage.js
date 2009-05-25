
function BusVoltage(type, name, id)
{
	this.CanService(type, name, id);
	
	this.myLastValue = new Array();
}

extend(BusVoltage, CanService);

BusVoltage.prototype.myReportInterval = null;
BusVoltage.prototype.myLastValue = null;

BusVoltage.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Voltage":
		//log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("Value") + "\n");
		this.myLastValue[canMessage.getData("SensorId")] = canMessage.getData("Value");
		this.callEvent("newValue", canMessage.getData("SensorId"));
		break;
		
		case "Report_Interval":
		//log(this.myName + ":" + this.myId + "> New report interval: " + canMessage.getData("Time") + "\n");
		this.myReportInterval = canMessage.getData("Time");
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

BusVoltage.prototype.getValue = function(sensorId)
{
	return this.myLastValue[sensorId];
}

BusVoltage.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}
