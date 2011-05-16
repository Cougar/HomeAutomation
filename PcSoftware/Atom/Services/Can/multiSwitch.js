
function multiSwitch(type, name, id)
{
	this.CanService(type, name, id);
	this.myLastValue = new Array();
}

extend(multiSwitch, CanService);

multiSwitch.prototype.myReportInterval = null;
multiSwitch.prototype.myLastValue = 0;

multiSwitch.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "SetPin":
		log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("PinId") + "\n");
		this.myLastValue = canMessage.getData("PinId");
		this.callEvent("newValue", canMessage.getData("PinId"));
		break;
		/*
		case "Report_Interval":
		log(this.myName + ":" + this.myId + "> New report interval: " + canMessage.getData("Time") + "\n");
		this.myReportInterval = canMessage.getData("Time");
		break;*/
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

multiSwitch.prototype.getValue = function()
{
	return this.myLastValue;
}

multiSwitch.prototype.setPin = function(pinId)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "SetPin");
	canMessage.setData("PinId", pinId);
	canMessage.setData("Status","High");
	sendMessage(canMessage);	
	this.myLastValue = pinId;
}



