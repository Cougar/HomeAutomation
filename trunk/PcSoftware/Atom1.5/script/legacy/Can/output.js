
function output(type, name, id)
{
	this.CanService(type, name, id);
	this.myLastValue = new Array();
}

extend(output, CanService);

output.prototype.myReportInterval = null;
output.prototype.myLastValue = null;

output.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "SetPin":
		//log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("Value") + "\n");
		this.myLastValue[canMessage.getData("PinId")] = canMessage.getData("Status");
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

output.prototype.getValue = function(chnId)
{
	return this.myLastValue[chnId];
}

output.prototype.setPin = function(status, chnId)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "SetPin");
	canMessage.setData("PinId", chnId);
	canMessage.setData("Status", status);
	sendMessage(canMessage);	
	this.myLastValue[chnId] = status;
}



