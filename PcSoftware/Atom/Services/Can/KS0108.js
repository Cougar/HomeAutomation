
function KS0108(type, name, id)
{
	this.CanService(type, name, id);
}

extend(KS0108, CanService);

KS0108.prototype.myBacklightStrength = null;

KS0108.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "LCD_Clear":
		break;
		
		case "LCD_Cursor":
		break;
		
		case "LCD_Text":
		break;
		
		case "LCD_TextAt":
		break;
		
		case "LCD_Backlight":
		this.myBacklight = canMessage.getData("Strength");
		log(this.myName + ":" + this.myId + "> Reported backlight strength: " + this.myBacklight + "\n");
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

KS0108.prototype.onlineHandler = function()
{
	this.CanService.prototype.onlineHandler.call(this);
	
	// Request backlight strength
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_Backlight");
	sendMessage(canMessage);
}

KS0108.prototype.printText = function(x, y, text)
{
	while (text.length > 0)
	{
		var text6 = text.substr(0, 6);

		var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_TextAt");
		canMessage.setData("X", x);
		canMessage.setData("Y", y);
		canMessage.setData("Text", text6);
		sendMessage(canMessage);
		
		x += text6.length;
		text = text.slice(text6.length);
	}
}

KS0108.prototype.clearScreen = function()
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_Clear");
	sendMessage(canMessage);
}

KS0108.prototype.setBacklight = function(strength)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_Backlight");
	canMessage.setData("Strength", strength);
	sendMessage(canMessage);
}

KS0108.prototype.getBacklight = function()
{
	return this.myBacklight;
}
