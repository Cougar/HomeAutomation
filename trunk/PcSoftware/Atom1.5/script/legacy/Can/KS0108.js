
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
		//log(this.myName + ":" + this.myId + "> Reported backlight strength: " + this.myBacklight + "\n");
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

KS0108.prototype.printText = function(x, y, text, inverted, transparent)
{
	text = text.replace(/å/, String.fromCharCode(128));
	text = text.replace(/Å/, String.fromCharCode(129));
	text = text.replace(/ä/, String.fromCharCode(130));
	text = text.replace(/Ä/, String.fromCharCode(131));
	text = text.replace(/ö/, String.fromCharCode(132));
	text = text.replace(/Ö/, String.fromCharCode(133));
	text = text.replace(/¤/, String.fromCharCode(134));
	while (text.length > 0)
	{
		var text6 = text.substr(0, 6);

		var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_TextAt");
		canMessage.setData("Inverted", inverted);
		canMessage.setData("Transparent", transparent);
		canMessage.setData("X", x);
		canMessage.setData("Y", y);
		canMessage.setData("Text", text6);
		sendMessage(canMessage);
		
		x += text6.length;
		text = text.slice(text6.length);
	}
}

KS0108.prototype.clearScreen = function(inverted)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_Clear");
	canMessage.setData("Inverted", inverted);
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

KS0108.prototype.drawLine = function(xStart, yStart , xEnd, yEnd, inverted)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_DrawLine");
	canMessage.setData("Inverted", inverted);
	canMessage.setData("X1", xStart);
	canMessage.setData("Y1", yStart);
	canMessage.setData("X2", xEnd);
	canMessage.setData("Y2", yEnd);
	sendMessage(canMessage);
}

KS0108.prototype.drawCircle = function(xCentre, yCentre , radius , inverted)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_DrawCircle");
	canMessage.setData("Inverted", inverted);
	canMessage.setData("Xcentre", xCentre);
	canMessage.setData("Ycentre", yCentre);
	canMessage.setData("Radius", radius);
	sendMessage(canMessage);
}

KS0108.prototype.invertRect = function(x, y , width, height)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_InvertRect")
	canMessage.setData("X", x);
	canMessage.setData("Y", y);
	canMessage.setData("Width", width);
	canMessage.setData("Height", height);
	sendMessage(canMessage);
}

KS0108.prototype.DrawRect = function(x, y , width, height, inverted, fill, radius)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "LCD_DrawRect")
	canMessage.setData("X", x);
	canMessage.setData("Y", y);
	canMessage.setData("Width", width);
	canMessage.setData("Height", height);
	canMessage.setData("Inverted", inverted);
	canMessage.setData("Fill", fill);
	canMessage.setData("Radius", radius);
	sendMessage(canMessage);
}
