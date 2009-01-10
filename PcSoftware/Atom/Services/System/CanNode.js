
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
CanNode.prototype.myProgrammingHexAddress = null;
CanNode.prototype.myProgrammingOffset = null;
CanNode.prototype.myProgrammingCallback = null;
CanNode.prototype.myProgrammingWantAck = null;
CanNode.prototype.myProgrammingState = null;

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
		this.myProgrammingIsBios = isBios;
	}
	else
	{
		this.myProgrammingIsBios = false;
	}
	
	this.myProgramming = true;
	this.myProgrammingHex = hexData;
	this.myProgrammingCallback = progressCallback;
	
	CanProgrammingNode = this;
	
	this.reset();
}

CanNode.prototype.handleBiosStart = function(biosVersion, hasApplication)
{
//print("node was reset and bios just started, ver:" + biosVersion);
	if (this.myProgramming)
	{
		this.myProgrammingHexAddress = this.myProgrammingHex.getAddrLower(); 
		this.myProgrammingWantAck = ((this.myProgrammingHexAddress<<8)|(this.myProgrammingHexAddress>>8))&0xffff;
	
		var canMessage = new CanNMTMessage("nmt", "Pgm_Start");
		canMessage.setData("HardwareId", this.myHardwareId);
		canMessage.setData("Address0", this.myProgrammingHexAddress&0xff);
		canMessage.setData("Address1", (this.myProgrammingHexAddress>>8)&0xff);
		canMessage.setData("Address3", (this.myProgrammingHexAddress>>16)&0xff);
		canMessage.setData("Address4", (this.myProgrammingHexAddress>>24)&0xff);
		canMessage.send();
		this.myProgrammingState = "DATA";
		
		this.myProgrammingCallback(true, "START", "Started programming of node");
	}
}

CanNode.prototype.handleNack = function(data)
{
	stopProgramming(true, "Failed, got NACK, sending reset");
	this.reset();
	if (false)	
	{
	}
}

CanNode.prototype.handleAck = function(data)
{
//this.myProgrammingCallback(true, "DBG", "data: "+data+", myProgrammingWantAck: "+this.myProgrammingWantAck+"");
	
	if (data == this.myProgrammingWantAck)	
	{

		switch (this.myProgrammingState)
		{
		case "DATA":
			var offset = this.myProgrammingHexOffset;
		
			this.myProgrammingWantAck = ((offset<<8)|(offset>>8))&0xffff;
			//var bytesLeft = this.myProgrammingHex.getAddrUpper() - (this.myProgrammingHexAddress+this.myProgrammingHexOffset);
			var bytesLeft = this.myProgrammingHex.getLength() - this.myProgrammingHexOffset;
			
			//here we must handle the possibility that data is less then 48 bits
			var canMessage = new CanNMTMessage("nmt", "Pgm_Data_48");
			switch (bytesLeft)
			{
			case 1:
				canMessage = new CanNMTMessage("nmt", "Pgm_Data_8");
				break;
			case 2:
				canMessage = new CanNMTMessage("nmt", "Pgm_Data_16");
				break;
			case 3:
				canMessage = new CanNMTMessage("nmt", "Pgm_Data_24");
				break;
			case 4:
				canMessage = new CanNMTMessage("nmt", "Pgm_Data_32");
				break;
			case 5:
				canMessage = new CanNMTMessage("nmt", "Pgm_Data_40");
				break;
			}
			
			canMessage.setData("Offset0", offset&0xff);
			canMessage.setData("Offset1", (offset>>8)&0xff);
			for (var i=0; i<bytesLeft && i<6; i++)
			{
				switch (i)
				{
				case 0:
					canMessage.setData("Data0", this.myProgrammingHex.getByte(offset));
					break;
				case 1:
					canMessage.setData("Data1", this.myProgrammingHex.getByte(offset));
					break;
				case 2:
					canMessage.setData("Data2", this.myProgrammingHex.getByte(offset));
					break;
				case 3:
					canMessage.setData("Data3", this.myProgrammingHex.getByte(offset));
					break;
				case 4:
					canMessage.setData("Data4", this.myProgrammingHex.getByte(offset));
					break;
				case 5:
					canMessage.setData("Data5", this.myProgrammingHex.getByte(offset));
					break;
				}
				offset++;
			}
			canMessage.send();
			this.myProgrammingHexOffset = offset;

			if (this.myProgrammingHexOffset >= this.myProgrammingHex.getLength()) {
				this.myProgrammingState = "END";
			}
			
			this.myProgrammingCallback(true, "PROGRESS", this.myProgrammingHexOffset + ":" + this.myProgrammingHex.getLength());
			break;
		
		case "END":
			this.myProgrammingWantAck = ((this.myProgrammingHex.getCRC16()<<8)|(this.myProgrammingHex.getCRC16()>>8))&0xffff;
		
			var canMessage = new CanNMTMessage("nmt", "Pgm_End");
			canMessage.send();

			this.myProgrammingState = "CRC";
			break;
			
		case "CRC":
			//is this a bios programming?
			if (false) 
			{
				this.myProgrammingState = "COPY";
				this.myProgrammingCallback(true, "PROGRESS", "Start to copy bios to new location");
				var canMessage = new CanNMTMessage("nmt", "Pgm_Copy");
				canMessage.setData("Source0", source);
				canMessage.setData("Source1", source);
				canMessage.setData("Destination0", destination);
				canMessage.setData("Destination1", destination);
				canMessage.setData("Length0", length);
				canMessage.setData("Length1", length);
				canMessage.send();
				this.stopProgramming();
			} 
			else 
			{
				this.stopProgramming(true, "Done, sucessful");
				this.reset();
			}
			break;
		
		case "COPY":	//this will never occur, bios does not send ack for copy command, it just moves data in flash then reset itself
			break;
			
		default:
			break;
		}
	}
	else
	{
		switch (this.myProgrammingState)
		{
		case "DATA":
			//Fall through
		case "END":
			///TODO: We got something that was not correct... we should abort
			this.stopProgramming(true, "Failed to program, ack had wrong offset, expected data: "+this.myProgrammingWantAck+" got: "+data);
			this.reset();
			break;
		case "CRC":
			this.stopProgramming(true, "Failed to program, CRC not matching, expected data: "+this.myProgrammingWantAck+" got: "+data);
			this.reset();
			break;
		case "COPY":	//this will never occur, bios does not send ack for copy command, it just moves data in flash then reset itself
			break;
		default:
			break;
		}
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
	this.myProgrammingHexAddress = 0;
	this.myProgrammingHexOffset = 0;
	this.myProgrammingCallback = function() {};
	this.myProgrammingWantAck = null;
	this.myProgrammingState = "";
	
	CanProgrammingNode = null;
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
