
function CanNode(hardwareId, numberOfModules)
{
	this.myHardwareId = hardwareId;
	this.myNumberOfModules = numberOfModules;
	this.myModules = new Array();
	this.myHeartbeatTime = 0;
	this.myIsOnline = false;
	
	this.stopProgramming();
}

CanNode.prototype.myIsOnline = null;
CanNode.prototype.myHardwareId = null;
CanNode.prototype.myNumberOfModules = null;
CanNode.prototype.myModules = null;
CanNode.prototype.myHeartbeatTime = null;
CanNode.prototype.myProgrammingIsBios = null;
CanNode.prototype.myProgramming = null;
CanNode.prototype.myProgrammingHex = null;
CanNode.prototype.myProgrammingHexIndex = null;
CanNode.prototype.myProgrammingCallback = null;
CanNode.prototype.myProgrammingWantAck = null;

CanNode.prototype.isOnline = function()
{
	return this.myIsOnline;
}

CanNode.prototype.start = function()
{
	var canMessage = new CanNMTMessage("nmt", "Start_App");
	canMessage.setData("HardwareId", this.myHardwareId);
	canMessage.send();
}

CanNode.prototype.reset = function()
{
	var canMessage = new CanNMTMessage("nmt", "Reset");
	canMessage.setData("HardwareId", this.myHardwareId);
	canMessage.send();
}

CanNode.prototype.startApplication = function()
{
	var canMessage = new CanNMTMessage("nmt", "Start_App");
	canMessage.setData("HardwareId", this.myHardwareId);
	canMessage.send();
}

CanNode.prototype.handleHeartbeat = function(numberOfModules)
{
	if (numberOfModules)
	{
		this.myNumberOfModules = numberOfModules;
	}
	
	if (this.myNumberOfModules == null || this.myNumberOfModules > this.myModules.length)
	{
		var canMessage = new CanMessage("mnmt", "To_Owner", "", 0, "List");
		canMessage.setData("HardwareId", this.myHardwareId);
		canMessage.send();
	}
	
	this.onlineHandler();
}

CanNode.prototype.startProgramming = function(hexData, progressCallback, isBios)
{
	if (isBios)
	{
		this.myProgrammingIsBios = bios;
	}
	else
	{
		this.myProgrammingIsBios = false;
	}
	
	this.myProgramming = true;
	this.myProgrammingHex = hexData;
	this.myProgrammingCallback = progressCallback;
	
	this.reset();
}

CanNode.prototype.handleBiosStart = function(biosVersion, hasApplication)
{
	if (this.myProgramming)
	{
		var hexLine = this.myProgrammingHex[this.myProgrammingHexIndex];
		this.myProgrammingHexIndex++;
	
		var address = ""; ///TODO: Fill in something here
	
		this.myProgrammingWantAck = "";  ///TODO: Fill in the 16 lower bits of address
	
		var canMessage = new CanNMTMessage("nmt", "Pgm_Start");
		canMessage.setData("HardwareId", this.myHardwareId);
		canMessage.setData("Address", address);
		canMessage.send();
		
		this.myProgrammingCallback(true, "START", "Started programming of node");
	}
}

CanNode.prototype.handleAck = function(data)
{
	if (data == this.myProgrammingWantAck)
	{
		var hexLine = this.myProgrammingHex[this.myProgrammingHexIndex];
		this.myProgrammingHexIndex++;
		
		var type = ""; ///TODO: Fill in which type of data we have
		
		switch (type)
		{
		case "DATA":///TODO: Maybe it is called something else, maybe a number
			var offset = ""; ///TODO: Fill in something here
			var data = ""; ///TODO: Fill in something here
		
			this.myProgrammingWantAck = "";  ///TODO: Fill in something here
		
			var canMessage = new CanNMTMessage("nmt", "Pgm_Data");
			canMessage.setData("Offset", offset);
			canMessage.setData("Data", data);
			canMessage.send();
			
			this.myProgrammingCallback(true, "PROGRESS", this.myProgrammingHexIndex + ":" + this.myProgrammingHex.length);
			break;
		
		case "END":///TODO: Maybe it is called something else, maybe a number
			this.myProgrammingWantAck = "";  ///TODO: Fill in something here
		
			var canMessage = new CanNMTMessage("nmt", "Pgm_End");
			canMessage.send();
			
			this.myProgrammingCallback(true, "END", ""); ///TODO: Fill in something meaningfull text
			break;
		
		case "COPY":///TODO: Maybe it is called something else, maybe a number
			var source = ""; ///TODO: Fill in something here
			var destination = ""; ///TODO: Fill in something here
			var length = ""; ///TODO: Fill in something here
			
			this.myProgrammingWantAck = "";  ///TODO: Fill in something here
		
			var canMessage = new CanNMTMessage("nmt", "Pgm_Copy");
			canMessage.setData("Source", source);
			canMessage.setData("Destination", destination);
			canMessage.setData("Length", length);
			canMessage.send();
			
			this.myProgrammingCallback(true, "END", ""); ///TODO: Fill in something meaningfull text
			break;
			
		default:
		///TODO: Are we done here? I have no idea :)
		///TODO: But we should call this.stopProgramming(true, "All done"); we are done, maybe restart node?
		}
	}
	else
	{
		///TODO: We got something that was not correct... we should abort
		stopProgramming(false, ""); ///TODO: Fill in some kind of error text
	}
}

CanNode.prototype.stopProgramming = function(status, text)
{
	if (status && text)
	{
		this.myProgrammingCallback(status, "STOP", text);
	}
	
	this.myProgrammingIsBios = false;
	this.myProgramming = false;
	this.myProgrammingHex = new Array();
	this.myProgrammingHexIndex = 0;
	this.myProgrammingCallback = function() {};
	this.myProgrammingWantAck = null;
}



CanNode.prototype.addService = function(sequenceNumber, service)
{
	this.myModules[sequenceNumber] = service;
	service.setOnline();
}

CanNode.prototype.onlineHandler = function()
{
	var date = new Date();
	this.myHeartbeatTime = date.getTimestamp();

	if (!this.myIsOnline)
	{
		this.myIsOnline = true;

		log(uint2hex(this.myHardwareId, 32) + "> Node went online\n");
		
		for (var n in this.myModules)
		{
			this.myModules[n].setOnline();
		}
	}
}

CanNode.prototype.checkOffline = function()
{
	if (this.myIsOnline)
	{
		var date = new Date();
		if (this.myHeartbeatTime + 10 < date.getTimestamp())
		{
			this.setOffline();
		}
	}
}

CanNode.prototype.setOffline = function()
{
	if (this.myIsOnline)
	{
		this.myIsOnline = false;

		log(uint2hex(this.myHardwareId, 32) + "> Node went offline\n");
		
		for (var n in this.myModules)
		{
			this.myModules[n].setOffline();
		}
	}
}
