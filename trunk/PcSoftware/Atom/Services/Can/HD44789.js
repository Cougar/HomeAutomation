
function HD44789(type, name, id)
{
	this.CanService(type, name, id);
}

extend(HD44789, CanService);

HD44789.prototype.myWidth = null;
HD44789.prototype.myHeight = null;
HD44789.prototype.myBacklightStrength = null;

HD44789.prototype.canMessageHandler = function(canMessage)
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
		
		case "LCD_Size":
		this.myWidth = canMessage.getData("Width");
		this.myHeight = canMessage.getData("Height");
		log(this.myName + ":" + this.myId + "> Reported size: " + this.myWidth + "x" + this.myHeight + "\n");
		break;
		
		case "LCD_Backlight":
		this.myBacklight = canMessage.getData("Strength");
		log(this.myName + ":" + this.myId + "> Reported backlight strength: " + this.myBacklight + "\n");
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

HD44789.prototype.setOnline = function()
{
	this.CanService.prototype.setOnline.call(this);

	// Request size of LCD
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_Size");
	sendMessage(canMessage);
	
	// Request backlight strength
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_Backlight");
	sendMessage(canMessage);
}

HD44789.prototype.printText = function(x, y, text)
{
	text = text.replace(/Å/, String.fromCharCode(1));
	text = text.replace(/å/, String.fromCharCode(0));
	text = text.replace(/Ä/, String.fromCharCode(2));
	text = text.replace(/ä/, String.fromCharCode(225));
	text = text.replace(/Ö/, String.fromCharCode(3));
	text = text.replace(/ö/, String.fromCharCode(239));
	text = text.replace(/¤/, String.fromCharCode(223));

	while (text.length > 0 && y < this.myHeight)
	{
		var text6 = text.substr(0, 6);
		
		if (text6.length > this.myWidth - x)
		{
			text6 = text6.substr(0, this.myWidth - x);
		}

		var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_TextAt");
		canMessage.setData("X", x);
		canMessage.setData("Y", y);
		canMessage.setData("Text", text6);
		sendMessage(canMessage);
		
		x += text6.length;
		text = text.slice(text6.length);
		
		if (text6.length < 6)
		{
			y++;
			x = 0;
		}
	}
}

HD44789.prototype.clearScreen = function()
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_Clear");
	sendMessage(canMessage);
}

HD44789.prototype.setBacklight = function(strength)
{
	if (strength != this.myBacklight)
	{
		var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_Backlight");
		canMessage.setData("Strength", strength);
		sendMessage(canMessage);
	}
}

HD44789.prototype.getBacklight = function()
{
	return this.myBacklight;
}

HD44789.prototype.getWidth = function()
{
	return this.myWidth;
}

HD44789.prototype.getHeight = function()
{
	return this.myHeight;
}
