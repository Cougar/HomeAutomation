
function input(type, name, id)
{
	this.CanService(type, name, id);
	this.myLastValue = new Array();
}

extend(input, CanService);

input.prototype.myReportInterval = null;
input.prototype.myLastValue = null;

input.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "PinStatus":
		//log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("Value") + "\n");
		this.myLastValue[canMessage.getData("PinId")] = canMessage.getData("Status");
		this.callEvent("newValue", canMessage.getData("PinId"));
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

input.prototype.getValue = function(chnId)
{
	return this.myLastValue[chnId];
}




