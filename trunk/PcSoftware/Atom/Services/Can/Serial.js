
function Serial(type, name, id)
{
	this.CanService(type, name, id);
}

extend(Serial, CanService);

Serial.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "BLIPPBLOPP":
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

Serial.prototype.printAscii = function(text)
{
	var canMessage = new CanMessage("sns", "From_Owner", this.myName, this.myId, "SerialData");
	canMessage.setData("Text", text);
	sendMessage(canMessage);
}

Serial.prototype.serialConfig = function(baudrate, phyFormat)
{
	var canMessage = new CanMessage("sns", "From_Owner", this.myName, this.myId, "SerialConfig");
	canMessage.setData("BaudRate", baudrate/10);
	canMessage.setData("PhysicalFormat", phyFormat);
	sendMessage(canMessage);
}

