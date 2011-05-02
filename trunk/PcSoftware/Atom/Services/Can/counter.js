
function counter(type, name, id)
{
	this.CanService(type, name, id);

	this.myLastValue = new Array();
}

extend(counter, CanService);

counter.prototype.myReportInterval = null;
counter.prototype.myLastValue = null;

counter.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Counter":
		//log(this.myName + ":" + this.myId + "> PinId : " + canMessage.getData("PinId") + " New value: " + canMessage.getData("Count") + "\n");
		this.myLastValue[canMessage.getData("PinId")] = canMessage.getData("Count");
		this.callEvent("newValue");
		break;

		case "Report_Interval":
		//log(this.myName + ":" + this.myId + "> PinId : " + canMessage.getData("PinId") + " New report interval: " + canMessage.getData("Time") + "\n");
		this.myReportInterval = canMessage.getData("Time");
		break;

		case "setCounter":
		//log(this.myName + ":" + this.myId + "> PinId : " + canMessage.getData("PinId") + " New count stored: " + canMessage.getData("Count") + "\n");
		break;
		}
	}

	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

counter.prototype.getCount = function()
{
	return this.myLastValueCount;
}
counter.prototype.setCount = function(id, count)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "setCounter");
	canMessage.setData("PinId", id);
	canMessage.setData("Count", count);
	sendMessage(canMessage);
}

counter.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}
