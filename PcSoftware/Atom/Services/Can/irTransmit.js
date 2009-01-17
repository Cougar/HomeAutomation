
function irTransmit(type, name, id)
{
	this.CanService(type, name, id);
}

extend(irTransmit, CanService);

irTransmit.prototype.canMessageHandler = function(canMessage)
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

irTransmit.prototype.sendClick = function(remoteName, buttonName, time)
{
	this.sendButton(remoteName, buttonName, true);
	
	if (time)
	{
		sleep(time);
	}
	else
	{
		sleep(50000);
	}
	
	this.sendButton(remoteName, buttonName, false);
}

irTransmit.prototype.sendButton = function(remoteName, buttonName, down)
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
						this.sendData(remotesStore['remotes'][n]['protocol'], code, down ? "Pressed" : "Released");
						return;
					}
				}
			}
		}
	}
}

irTransmit.prototype.sendData = function(protocol, data, status)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "IR");
	canMessage.setData("Status", status);
	canMessage.setData("Protocol", protocol);
	canMessage.setData("IRdata", data);
	sendMessage(canMessage);
}
