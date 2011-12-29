

function irTransceive(type, name, id)
{
	this.CanService(type, name, id);
}

extend(irTransceive, CanService);

irTransceive.prototype.canMessageHandler = function(canMessage)
{
	/*if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "IR":
		break;
		}
	}*/
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

irTransceive.prototype.sendClick = function(channel, remoteName, buttonName, time)
{
	this.sendButton(channel, remoteName, buttonName, true);
	
	if (time)
	{
		sleep(time);
	}
	else
	{
		sleep(50000);
	}
	
	this.sendButton(channel, remoteName, buttonName, false);
}

irTransceive.prototype.sendButton = function(channel, remoteName, buttonName, down)
{
	var remotesStore = DataStore.getStore("IRRemotes");
	
	if (remotesStore)
	{
		for (var n = 0; n < remotesStore['remotes'].length; n++)
		{
			if (remotesStore['remotes'][n]['name'] == remoteName)
			{
				for (var code in remotesStore['remotes'][n]['codes'])
				{
					if (remotesStore['remotes'][n]['codes'][code] == buttonName)
					{
						this.sendData(channel, remotesStore['remotes'][n]['protocol'], code, down ? "Pressed" : "Released");
						return;
					}
				}
			}
		}
	}
}

irTransceive.prototype.sendData = function(channel, protocol, data, status)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IR");
	canMessage.setData("Channel", channel);
	canMessage.setData("Status", status);
	canMessage.setData("Protocol", protocol);
	canMessage.setData("IRdata", data);
	sendMessage(canMessage);
}

irTransceive.prototype.sendConfig = function(channel, direction, power, modfreq)
{
	var canMessage2 = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrConfig");
	canMessage2.setData("Channel", channel);
	canMessage2.setData("Direction", direction);
	canMessage2.setData("Transmit power", power);
	canMessage2.setData("Modulation divider", modfreq);
	sendMessage(canMessage2);
}

function hexString2NumberArray(hexString)
{
	var tokens = hexString.split(' ');
	var numbers = new Array();
	for (var i=0; i<tokens.length; i++) {
		numbers[i] = parseInt(tokens[i], 16)
	}
	return numbers;
}

// prontoHexString is a space-separated hex string, including both the pronto header and the data
irTransceive.prototype.sendProntoHex = function(channel, repeatCount, prontoHexString)
{
	var prontoNumbers = hexString2NumberArray(prontoHexString);
	
	// header
	var format = prontoNumbers[0];
	var frqDiv = prontoNumbers[1];
	var onceSeqLen = prontoNumbers[2];
	var repSeqLen = prontoNumbers[3];
	
	print("Sending START. CH=" + channel + ", fmt=" + format + ", wFrqDiv=" + frqDiv + ", onceLen=" + onceSeqLen + ", repLen=" + repSeqLen + "\n");
	var msg = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrProntoStart");
	msg.setData("Channel", channel);
	msg.setData("Format", format);
	msg.setData("wFrqDiv", frqDiv);
	msg.setData("OnceSeqLen", onceSeqLen);
	msg.setData("RepSeqLen",  repSeqLen);
	sendMessage(msg);
	sleep(10);
	
	// data
	var nrBytes = 0;
	var bytes = new Array();
	var seqNr = 0;
	for (var i=4; i<prontoNumbers.length; i++) {
		// 8bit value?
		if (prontoNumbers[i] <= 255) {
			// compress into single byte
			bytes[nrBytes] = prontoNumbers[i];
			nrBytes++;
		}
		// 16bit value?
		else {
			// occupy 3 bytes to describe this value
			bytes[nrBytes] = (prontoNumbers[i] & 0xFF00) >> 8;
			bytes[nrBytes+1] = 0x00;
			bytes[nrBytes+2] = (prontoNumbers[i] & 0x00FF) >> 0;
			nrBytes += 3;
		}
		// time to send?
		if (nrBytes >= 8) {
			print("Sending DATA. " + nrBytes + " bytes: " + bytes + " (SEQ NR:" + (seqNr+1) + ")\n");
			msg = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrProntoData" + (seqNr+1));
			msg.setData("Act1", bytes[0]);
			msg.setData("Pas1", bytes[1]);
			msg.setData("Act2", bytes[2]);
			msg.setData("Pas2", bytes[3]);
			msg.setData("Act3", bytes[4]);
			msg.setData("Pas3", bytes[5]);
			msg.setData("Act4", bytes[6]);
			msg.setData("Pas4", bytes[7]);
			sendMessage(msg);
			sleep(10);
			
			bytes = new Array();
			nrBytes = 0;
			seqNr = (seqNr + 1) % 16;
		}
	}
	
	print("Sending END. " + nrBytes + " bytes: " + bytes + " (SEQ NR:" + (seqNr+1) + ")\n");
	msg = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrProntoEnd" + (seqNr+1));
	if (nrBytes > 1) msg.setData("Act1", bytes[0]);
	if (nrBytes > 2) msg.setData("Pas1", bytes[1]);
	if (nrBytes > 3) msg.setData("Act2", bytes[2]);
	if (nrBytes > 4) msg.setData("Pas2", bytes[3]);
	if (nrBytes > 5) msg.setData("Act3", bytes[4]);
	if (nrBytes > 6) msg.setData("Pas3", bytes[5]);
	msg.setData("Repeat", repeatCount);
	sendMessage(msg);
	sleep(10);
}

irTransceive.prototype.sendProntoStop = function(channel)
{
	var msg = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrProntoStop");
	msg.setData("Channel", channel);
	sendMessage(msg);
}

irTransceive.prototype.sendProntoContinue = function(channel)
{
	var msg = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrProntoContinue");
	msg.setData("Channel", channel);
	sendMessage(msg);
}

function prontoStart(repeats)
{
	var ir = ServiceManager.getService("Can","irTransceive",1);
	ir.sendProntoHex(1, repeats, "0000 006d 0022 0002 0156 00ad 0016 0016 0016 0041 0016 0041 0016 0041 0016 0016 0016 0041 0016 0041 0016 0041 0016 0041 0016 0041 0016 0041 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0016 0016 0041 0016 0016 0016 0041 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0041 0016 0016 0016 0016 0016 0041 0016 05ed 0156 0056 0016 0e62");
}

function prontoStop()
{
	var ir = ServiceManager.getService("Can","irTransceive",1);
	ir.sendProntoStop(1);
}

function prontoContinue()
{
	var ir = ServiceManager.getService("Can","irTransceive",1);
	ir.sendProntoContinue(1);
}
