

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
	canMessage2.setData("Modulation frequency", modfreq);
	sendMessage(canMessage2);
}

irTransceive.prototype.sendProntoTest = function(channel)
{
	// pronto start
	var msg = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrProntoStart");
	msg.setData("Channel", channel);
	msg.setData("Format", 0);
	msg.setData("wFrqDiv", 109); // 4.145146MHz / 38kHz
	msg.setData("OnceSeqLen", 8);
	msg.setData("RepSeqLen", 0);
	sendMessage(msg);
	sleep(1);
	
	// pronto data
	msg = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrProntoData1");
	msg.setData("Act1", 20);
	msg.setData("Pas1", 100);
	msg.setData("Act2", 12);
	msg.setData("Pas2", 25);
	msg.setData("Act3", 1);
	msg.setData("Pas3", 200);
	msg.setData("Act4", 65);
	msg.setData("Pas4", 13);
	sendMessage(msg);
	sleep(1);
	
	// pronto data/end
	msg = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrProntoEnd2");
	msg.setData("Act1", 220);
	msg.setData("Pas1", 20);
	msg.setData("Act2", 2);
	msg.setData("Pas2", 75);
	msg.setData("Act3", 19);
	msg.setData("Pas3", 2);
	msg.setData("Act4", 73);
	msg.setData("Pas4", 37);
	sendMessage(msg);
	sleep(1);
}
