
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

irTransceive.prototype.sendConfig = function(channel, direction, power)
{
	var canMessage2 = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IrConfig");
	canMessage2.setData("Channel", channel);
	canMessage2.setData("Direction", direction);
	canMessage2.setData("Transmit power", power);
	sendMessage(canMessage2);
}

