
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
CanNode.prototype.myProgrammingBiosOffset = null;
CanNode.prototype.myProgramming = null;
CanNode.prototype.myProgrammingHex = null;
CanNode.prototype.myProgrammingHexAddress = null;
CanNode.prototype.myProgrammingOffset = null;
CanNode.prototype.myProgrammingCallback = null;
CanNode.prototype.myProgrammingWantAck = null;
CanNode.prototype.myProgrammingState = null;
CanNode.prototype.myProgrammingLastPercent = null;
CanNode.prototype.myProgrammingLastTenPercent = null;
CanNode.prototype.myProgrammingnrOfTicks = null;
CanNode.prototype.myProgrammingTimeout = null;
CanNode.prototype.myProgrammingResend = null;
CanNode.prototype.myProgrammingLastPacket = null;
CanNode.prototype.myProgrammingTimeStarted = null;
CanNode.prototype.myResetCallback = null;
CanNode.prototype.myResetTimer = null;

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

CanNode.prototype.reset = function(callback)
{
	var self = this;
	
	if (callback)
	{
		this.myResetCallback = callback;
		if (this.myResetTimer)
		{
			this.myResetTimer.stop();
		}
		
		this.myResetTimer = new Interval(	function()
							{
								self.myResetTimer.stop();
								self.myResetTimer = null;
								self.myResetCallback(false);
								self.myResetCallback = null;
							}, 5000);
		this.myResetTimer.start();
	}

	var canMessage = new CanNMTMessage("nmt", "Reset");
	canMessage.setData("HardwareId", this.myHardwareId);
	canMessage.send();
	
	this.setOffline();
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
	this.myProgrammingCallback = progressCallback;
	if (isBios)
	{
		this.myProgrammingIsBios = isBios;
		this.myProgrammingBiosOffset = hexData.getAddrLower();
		//this.myProgrammingCallback(true, "", "got IsBios, address: " + this.myProgrammingBiosOffset, false);
	}
	else
	{
		this.myProgrammingIsBios = false;
	}
	
	this.myProgramming = true;
	this.myProgrammingHex = hexData;
	
	CanProgrammingNode = this;
	
	var self = this;
	this.myProgrammingTimeout = new Interval(function() { self.programmingTimeout() }, 6000);
	this.myProgrammingTimeout.start();
	this.myProgrammingState = "RESET";

	this.reset();
}

CanNode.prototype.programmingTimeout = function()
{
	switch (this.myProgrammingState)
	{
	case "RESET":
		this.stopProgramming(true, "Timeout while waiting for node to reset");
		this.reset();
		break;
	case "DATA":
		this.stopProgramming(true, "Timeout while waiting for node to ack data packet");
		this.reset();
		break;
	case "END":
		this.stopProgramming(true, "Timeout while waiting for node to ack data packet*");
		this.reset();
		break;
	case "CRC":
		this.stopProgramming(true, "Timeout while waiting for node to send crc");
		this.reset();
		break;
	case "COPY":
		//this will never occur, bios does not send ack for copy command, it just moves data in flash then reset itself
		this.myProgrammingCallback(true, "ERROR", "Unexpected error 100", false);
		break;
	case "DONE":
		this.stopProgramming(true, "Timeout while waiting for node to start bios");
		this.reset();
		break;
	default:
		this.myProgrammingCallback(true, "ERROR", "Unexpected error 101, state: " + this.myProgrammingState, false);
		break;
	}
}

CanNode.prototype.programmingResend = function()
{
this.myProgrammingCallback(true, "", "Resend timeout, a packet was lost, either program data or ack", false);
	this.myProgrammingResend.reset();
	//if we did not got ack then try to resend the data
	this.myProgrammingLastPacket.send();
}

CanNode.prototype.handleBiosStart = function(biosVersion, hasApplication)
{
	if (this.myResetCallback)
	{
		this.myResetCallback(true);
		this.myResetCallback = null;
	}
	
	if (this.myResetTimer)
	{
		this.myResetTimer.stop();
		this.myResetTimer = null;
	}

//print("node was reset and bios just started, ver:" + biosVersion);
	if (this.myProgramming)
	{
		switch (this.myProgrammingState)
		{
		case "RESET":
			this.myProgrammingHexAddress = this.myProgrammingHex.getAddrLower(); 
			this.myProgrammingWantAck = (((this.myProgrammingHexAddress-this.myProgrammingBiosOffset)<<8)|((this.myProgrammingHexAddress-this.myProgrammingBiosOffset)>>8))&0xffff;

			var canMessage = new CanNMTMessage("nmt", "Pgm_Start");
			canMessage.setData("HardwareId", this.myHardwareId);
			canMessage.setData("Address0", (this.myProgrammingHexAddress-this.myProgrammingBiosOffset)&0xff);
			canMessage.setData("Address1", ((this.myProgrammingHexAddress-this.myProgrammingBiosOffset)>>8)&0xff);
			canMessage.setData("Address3", ((this.myProgrammingHexAddress-this.myProgrammingBiosOffset)>>16)&0xff);
			canMessage.setData("Address4", ((this.myProgrammingHexAddress-this.myProgrammingBiosOffset)>>24)&0xff);
			canMessage.send();
			this.myProgrammingState = "DATA";
		
			var self = this;
			this.myProgrammingTimeout.setTimeout(1000);
		
			this.myProgrammingResend = new Interval(function() { self.programmingResend() }, 1100);
			this.myProgrammingResend.start();

			var date = new Date();
			this.myProgrammingTimeStarted = date.getTime();

			this.myProgrammingCallback(true, "START", "Started programming of node", false);
			this.myProgrammingCallback(true, "", "  0% [", true);
			break;
		case "COPY":
			//bios was moved and now started (so it seems like it was successfully programmed and moved)
			//now we should send a new programming with one empty packet to erase application area
			this.myProgrammingWantAck = 0;
			var canMessage = new CanNMTMessage("nmt", "Pgm_Start");
			canMessage.setData("HardwareId", this.myHardwareId);
			canMessage.setData("Address0", 0);
			canMessage.setData("Address1", 0);
			canMessage.setData("Address3", 0);
			canMessage.setData("Address4", 0);
			canMessage.send();
			this.myProgrammingState = "APPCLEANDATA";
			break;
		case "DONE":
			this.stopProgramming(true, "Done, successful");
			break;
		default:
			this.myProgrammingCallback(true, "ERROR", "Unexpected error 105, state: " + this.myProgrammingState, false);
			break;
		}
	
	}
}

CanNode.prototype.handleNack = function(data)
{
	this.stopProgramming(true, "Failed, got NACK, sending reset");
	this.reset();
	if (false)	
	{
	}
}

CanNode.prototype.handleAck = function(data)
{
	this.myProgrammingTimeout.reset();

	if (data == this.myProgrammingWantAck)	
	{
		switch (this.myProgrammingState)
		{
		case "DATA":
			var offset = this.myProgrammingHexOffset;
			this.myProgrammingResend.setTimeout(100);
			this.myProgrammingResend.reset();
		
			this.myProgrammingWantAck = ((offset<<8)|(offset>>8))&0xffff;
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
					canMessage.setData("Data0", this.myProgrammingHex.getByte(this.myProgrammingHexAddress+offset));
					break;
				case 1:
					canMessage.setData("Data1", this.myProgrammingHex.getByte(this.myProgrammingHexAddress+offset));
					break;
				case 2:
					canMessage.setData("Data2", this.myProgrammingHex.getByte(this.myProgrammingHexAddress+offset));
					break;
				case 3:
					canMessage.setData("Data3", this.myProgrammingHex.getByte(this.myProgrammingHexAddress+offset));
					break;
				case 4:
					canMessage.setData("Data4", this.myProgrammingHex.getByte(this.myProgrammingHexAddress+offset));
					break;
				case 5:
					canMessage.setData("Data5", this.myProgrammingHex.getByte(this.myProgrammingHexAddress+offset));
					break;
				}
				offset++;
			}
			canMessage.send();
			this.myProgrammingHexOffset = offset;

			if (this.myProgrammingHexOffset >= this.myProgrammingHex.getLength()) {
				this.myProgrammingState = "END";
			}
			
			//this.myProgrammingCallback(true, "PROGRESS", this.myProgrammingHexOffset + ":" + this.myProgrammingHex.getLength(), false);
			var percent = 100*this.myProgrammingHexOffset / this.myProgrammingHex.getLength();
			if (percent >= this.myProgrammingLastPercent+2.5)
			{
				this.myProgrammingCallback(true, "", "=", true);
				this.myProgrammingLastPercent+=2.5;
				this.myProgrammingnrOfTicks++;
			}
			if (percent > this.myProgrammingLastTenPercent+10)
			{
				for (var i=this.myProgrammingnrOfTicks; i<40; i++) this.myProgrammingCallback(true, "", " ", true);
				this.myProgrammingCallback(true, "", "]", true);
				this.myProgrammingCallback(true, "", "\r", true);
				this.myProgrammingCallback(true, "", " "+Math.round(percent)+"% [", true);
				this.myProgrammingLastTenPercent+=10;
				for (var i=0; i<this.myProgrammingnrOfTicks; i++) this.myProgrammingCallback(true, "", "=", true);
			}
			
			this.myProgrammingLastPacket = canMessage;
			break;
		
		case "END":
			this.myProgrammingWantAck = ((this.myProgrammingHex.getCRC16()<<8)|(this.myProgrammingHex.getCRC16()>>8))&0xffff;
			
			var timeElapsed = 0;
			var printTime = "";
			if (this.myProgrammingTimeStarted>0)
			{
				var date = new Date();
				timeElapsed = date.getTime() - this.myProgrammingTimeStarted;
				printTime = " in " + Math.round(timeElapsed/1000) + "s, " + Math.round(this.myProgrammingHex.getLength()*1000/timeElapsed)+"bytes/s";
			}
			
			var canMessage = new CanNMTMessage("nmt", "Pgm_End");
			canMessage.send();

			this.myProgrammingState = "CRC";
			for (var i=this.myProgrammingnrOfTicks; i<40; i++) this.myProgrammingCallback(true, "", " ", true);
			this.myProgrammingCallback(true, "", "]", true);
			this.myProgrammingCallback(true, "", "\r", true);
			this.myProgrammingCallback(true, "", "100% [========================================]"+printTime, true);

			if (this.myProgrammingResend != null)
			{
				/* Stop the interval */
				this.myProgrammingResend.stop();
			}

			break;
			
		case "CRC":
			this.myProgrammingTimeout.setTimeout(6000);
			//is this a bios programming?
			if (this.myProgrammingIsBios)
			{
				this.myProgrammingState = "COPY";
				this.myProgrammingCallback(true, "\nBIOS", "Start to copy bios to new location", false);
				var canMessage = new CanNMTMessage("nmt", "Pgm_Copy");
				canMessage.setData("Source0", 0);
				canMessage.setData("Source1", 0);
				canMessage.setData("Destination0", this.myProgrammingBiosOffset&0xff);
				canMessage.setData("Destination1", (this.myProgrammingBiosOffset>>8)&0xff);
				canMessage.setData("Length0", (this.myProgrammingHex.getLength())&0xff);
				canMessage.setData("Length1", (this.myProgrammingHex.getLength()>>8)&0xff);
				canMessage.send();
				//this.stopProgramming();
			} 
			else 
			{
				this.myProgrammingState = "DONE";
				this.reset();
			}
			break;
		
		case "COPY":	//this will never occur, bios does not send ack for copy command, it just moves data in flash then reset itself
			break;
		case "APPCLEANDATA":
			var canMessage = new CanNMTMessage("nmt", "Pgm_Data_16");
			this.myProgrammingWantAck = 0;
			this.myProgrammingState = "APPCLEANEND";
			canMessage.setData("Data0", 0xff);
			canMessage.setData("Data1", 0xff);
			canMessage.send();
			break;
		case "APPCLEANEND":
			this.myProgrammingWantAck = 0x1b0;
			this.myProgrammingState = "APPCLEANCRC";
			var canMessage = new CanNMTMessage("nmt", "Pgm_End");
			canMessage.send();
			break;
		case "APPCLEANCRC":
			this.myProgrammingState = "DONE";
			this.reset();
			break;
		
		default:
			this.myProgrammingCallback(true, "ERROR", "Unexpected error 104, state: " + this.myProgrammingState, false);
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
			this.myProgrammingCallback(true, "ERROR", "Unexpected error 103", false);
			break;
		default:
			this.myProgrammingCallback(true, "ERROR", "Unexpected error 102, state: " + this.myProgrammingState, false);
			break;
		}
	}
}

CanNode.prototype.stopProgramming = function(status, text)
{
	if (status && text)
	{
		this.myProgrammingCallback(status, "\nSTOP", text, false);
	}

	this.myProgrammingIsBios = false;
	this.myProgramming = false;
	this.myProgrammingHex = new Array();
	this.myProgrammingHexAddress = 0;
	this.myProgrammingHexOffset = 0;
	this.myProgrammingCallback = function() {};
	this.myProgrammingWantAck = null;
	this.myProgrammingState = "";
	this.myProgrammingLastPercent = 0;
	this.myProgrammingLastTenPercent = 0;
	this.myProgrammingnrOfTicks = 0;
	this.myProgrammingTimeStarted = 0;
	this.myProgrammingBiosOffset = 0;

	if (this.myProgrammingTimeout != null)
	{
		/* Stop the interval */
		this.myProgrammingTimeout.stop();
	}
	if (this.myProgrammingResend != null)
	{
		/* Stop the interval */
		this.myProgrammingResend.stop();
	}

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
		if (this.myHeartbeatTime + 21 < date.getTimestamp())
		{
			log(uint2hex(this.myHardwareId, 32) + "> Offline time: "+(date.getTimestamp()-this.myHeartbeatTime)+"\n");
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
