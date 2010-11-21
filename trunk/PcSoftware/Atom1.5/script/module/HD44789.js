
function HD44789(name)
{
	this.Module(name);
	HD44789.instance_ = this;
}

Extend(HD44789, Module);

HD44789.instance_ = null;

HD44789.prototype.ReceiveMessage = function(id, command, variables)
{
	switch (command)
	{
		case "LCD_Cursor":
		{
			break;
		}
		case "LCD_Text":
		{
			break;
		}
		case "LCD_TextAt":
		{
			break;
		}
		case "LCD_Size":
		{
			this.EventBase.prototype.Trigger.call(this, "onSize", id, variables["Width"], variables["Height"]);
			break;
		}
		case "LCD_Backlight":
		{
			this.EventBase.prototype.Trigger.call(this, "onBacklight", id, variables["Strength"]);
			break;
		}
	}
	
	this.Module.prototype.ReceiveMessage.call(this, id, command, variables);
}

function HD44789_complete(args)
{
	var result = new Array();
	
	var arg_index = args.length - 1;
	
	if (arg_index == 1) // id
	{
		result = HD44789.instance_.GetAvailableIds();
	}
	
	return result;
}

function HD44789_PrintText(id, x, y, text)
{
	text = text.replace(/Å/, String.fromCharCode(1));
	text = text.replace(/å/, String.fromCharCode(0));
	text = text.replace(/Ä/, String.fromCharCode(2));
	text = text.replace(/ä/, String.fromCharCode(225));
	text = text.replace(/Ö/, String.fromCharCode(3));
	text = text.replace(/ö/, String.fromCharCode(239));
	text = text.replace(/¤/, String.fromCharCode(223));
	
	x = parseInt(x);
	y = parseInt(y);
	
	for (var n = 4;  n < arguments.length; n++)
	{
		text += " " + arguments[n];
	}
	
	while (text.length > 0)
	{
		var length = text.length < 6 ? text.length : 6;
		var text_part = text.substr(0, length);

		var variables = [
		{ "X"    : x },
		{ "Y"    : y },
		{ "Text" : text_part } ];

		HD44789.instance_.Module.prototype.SendMessage.call(HD44789.instance_, id, "LCD_TextAt", variables);
		
		text = text.substr(6);
		x += 6;
	}
	
	return "OK";
}
RegisterConsoleCommand(HD44789_PrintText, HD44789_complete);


function HD44789_ClearScreen(id)
{
	var variables = [ ];
	
	HD44789.instance_.Module.prototype.SendMessage.call(HD44789.instance_, id, "LCD_Clear", variables);
	return "OK";
}
RegisterConsoleCommand(HD44789_ClearScreen, HD44789_complete);


function HD44789_SetBacklight(id, strength)
{
	var variables = [
	{ "Strength"  : strength } ];
	
	HD44789.instance_.Module.prototype.SendMessage.call(HD44789.instance_, id, "LCD_Backlight", variables);
	return "OK";
}
RegisterConsoleCommand(HD44789_SetBacklight, HD44789_complete);

