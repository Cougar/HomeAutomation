
function irReceive(name, id)
{
	this.CanService(name, id);
}

extend(irReceive, CanService);

irReceive.prototype.myLastIRdata = null;
irReceive.prototype.myLastProtocol = null;
irReceive.prototype.myLastStatus = null;

irReceive.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "IR":

			this.myLastIRdata = canMessage.getData("IRdata");
			this.myLastProtocol = canMessage.getData("Protocol");
			this.myLastStatus = canMessage.getData("Status");
		
			log(this.myName + ":" + this.myId + "> New IR, protocol: " + this.getProtocolName(this.myLastProtocol) + ", data: " + this.myLastIRdata + ", button was " + this.getStatusName(this.myLastStatus).toLowerCase() + "\n");
			this.callEvent("newIRdata", null);
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

irReceive.prototype.getLastIRdata = function()
{
	return this.myLastIRdata;
}

irReceive.prototype.getLastProtocol = function()
{
	return this.myLastProtocol;
}

irReceive.prototype.getLastStatus = function()
{
	return this.myLastStatus;
}

irReceive.prototype.getStatusName = function(status)
{
	var returnData = "Released";
	if (status == 1) {
		returnData = "Pressed";
	}
	return returnData;
}
irReceive.prototype.getProtocolName = function(protocol)
{
	var returnData = "Unknown";
	switch (protocol)
	{
	case 0:
		returnData = "RC5";
	break;
	case 1:
		returnData = "RC6";
	break;
	case 2:
		returnData = "RCMM";
	break;
	case 3:
		returnData = "SIRC";
	break;
	case 4:
		returnData = "Sharp";
	break;
	case 5:
		returnData = "NEC";
	break;
	case 6:
		returnData = "Samsung";
	break;
	case 7:
		returnData = "Marantz";
	break;
	case 255:
		returnData = "Unknown";
	break;
	default:
		returnData = "New protocol, irReceive.js must be updated";
	break;
	}
	return returnData;
}
