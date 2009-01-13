
function irReceive(type, name, id)
{
	this.CanService(type, name, id);
}

extend(irReceive, CanService);

irReceive.prototype.myLastIRdata = null;
irReceive.prototype.myLastProtocol = null;
irReceive.prototype.myLastStatus = null;
irReceive.prototype.myLastButton = null;
irReceive.prototype.myLastRemote = null;

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
		this.lookupName(this.getProtocolName(this.myLastProtocol), this.myLastIRdata);
		
		if (this.myLastButton == null)
		{
			log(this.myName + ":" + this.myId + "> New IR, protocol: " + this.getProtocolName(this.myLastProtocol) + ", data: " + this.myLastIRdata + ", button was " + this.myLastStatus + "\n");
		}
		else
		{
			log(this.myName + ":" + this.myId + "> New IR, remote: " + this.myLastRemote + ", button: " + this.myLastButton + ", button was " + this.myLastStatus + "\n");
		}
		
		this.callEvent("newIRdata", null);
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

irReceive.prototype.lookupName = function(protocol, data)
{
	var remotesStore = DataStore.getStore("IRRemotes");
	
	if (remotesStore)
	{
		for (var n = 0; n < remotesStore['remotes'].length; n++)
		{
			if (remotesStore['remotes'][n]['protocol'] == protocol)
			{
				if (remotesStore['remotes'][n]['codes'][data])
				{
					this.myLastButton = remotesStore['remotes'][n]['codes'][data];
					this.myLastRemote = remotesStore['remotes'][n]['name'];
					return;
				}
			}
		}
	}
	
	this.myLastButton = null;
	this.myLastRemote = null;
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
