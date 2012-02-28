

function rfTransceive(type, name, id)
{
	this.CanService(type, name, id);
}

extend(rfTransceive, CanService);

rfTransceive.prototype.canMessageHandler = function(canMessage)
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

rfTransceive.prototype.sendClick = function(channel, remoteName, buttonName, time)
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

rfTransceive.prototype.sendButton = function(channel, remoteName, buttonName, down)
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

rfTransceive.prototype.sendData = function(channel, protocol, data, status)
{
	if (status == "Pressed")
	{
		var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "DataStart");
		canMessage.setData("Channel", channel);
		canMessage.setData("Protocol", protocol);
		canMessage.setData("Data", data);
		sendMessage(canMessage);
	}
	else if (status == "Released")
	{
		var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "DataStop");
		canMessage.setData("Channel", channel);
		canMessage.setData("Protocol", protocol);
		canMessage.setData("Data", data);
		sendMessage(canMessage);
	}
	else if (status == "Burst")
	{
		var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "DataBurst");
		canMessage.setData("Channel", channel);
		canMessage.setData("Protocol", protocol);
		canMessage.setData("Data", data);
		sendMessage(canMessage);
	}
}

