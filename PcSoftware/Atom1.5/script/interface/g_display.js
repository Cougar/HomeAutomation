
GDisplay_ModuleNames    = [ "KS0108" ];
GDisplay_Height         = function() { return [ 0, 1, 2, 3  ]; };
GDisplay_Radius         = function() { return [ 2, 5, 10, 20, 30, 50, 100  ]; };
GDisplay_Inverted       = function() { return [ "Standard", "Inverted"  ]; };
GDisplay_Fill           = function() { return [ "NoFill", "Fill"  ]; };
GDsplay_Transparent    = function() { return [ "Standard", "Transparent"  ]; };
GDisplay_Width          = function() { return [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 ]; };
GDisplay_Backlight      = function() { return [ 0, 50, 100, 150, 200, 255 ]; };
GDisplay_Aliases        = function() { return Module_GetAliasNames(GDisplay_ModuleNames); };
GDisplay_AvailableIds   = function() { return Module_GetAvailableIds(GDisplay_ModuleNames); };

function GDisplay_Clear(alias_name, inverted)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, GDisplay_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
			"Inverted" : inverted
		};			
		if (!Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "LCD_Clear", variables))
		{
			Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
		}
		
		found = true;
	}
	
	if (!found)
	{
		Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	
	return true;
}
Console_RegisterCommand(GDisplay_Clear, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, GDisplay_Aliases(), GDisplay_Inverted()); });

function GDisplay_SetBacklight(alias_name, strength)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, GDisplay_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
		"Strength" : strength,
		"AutoLight" : "OFF"
		};
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "LCD_Backlight", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
		}
		
		found = true;
	}
	
	if (!found)
	{
		Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	
	return true;
}
Console_RegisterCommand(GDisplay_SetBacklight, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, GDisplay_Aliases(), GDisplay_Backlight()); });


function GDisplay_Print(alias_name, x, y, inverted, transparent, text)
{
	if (arguments.length < 6)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	text = text.replace(/å/, String.fromCharCode(128));
	text = text.replace(/Å/, String.fromCharCode(129));
	text = text.replace(/ä/, String.fromCharCode(130));
	text = text.replace(/Ä/, String.fromCharCode(131));
	text = text.replace(/ö/, String.fromCharCode(132));
	text = text.replace(/Ö/, String.fromCharCode(133));
	text = text.replace(/¤/, String.fromCharCode(134));
	
	x = parseInt(x);
	y = parseInt(y);
	
	for (var n = 6;  n < arguments.length; n++)
	{
		text += " " + arguments[n];
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, GDisplay_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var org_text = text;
		
		while (text.length > 0)
		{
			var length = text.length < 6 ? text.length : 6;
			var text_part = text.substr(0, length);
			
			var variables = {
			"X"    : x,
			"Y"    : y,
			"Inverted" : inverted,
			"Transparent" : transparent,
			"Text" : text_part };
			
			if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "LCD_TextAt", variables))
			{
				//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
			}
			else
			{
				Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
			}
			
			text = text.substr(6);
			x += 6;
		}
		
		text = org_text;
		found = true;
	}
	
	if (!found)
	{
		Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	
	return true;
}
Console_RegisterCommand(GDisplay_Print, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, GDisplay_Aliases(), GDisplay_Width(), GDisplay_Height(), GDisplay_Inverted(), GDisplay_Transparent()); });

function GDisplay_DrawCircle(alias_name, xCentre, yCentre, radius, inverted)
{
	if (arguments.length < 5)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	xCentre = parseInt(xCentre);
	yCentre = parseInt(yCentre);
	radius = parseInt(radius);
	var aliases_data = Module_ResolveAlias(alias_name, GDisplay_ModuleNames);
	var found = false;
	for (var name in aliases_data)
	{
		var variables = {
			"Xcentre"    : xCentre,
			"Ycentre"    : yCentre,
			"Inverted" : inverted,
			"Radius" : radius};
			
		if (!Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "LCD_DrawCircle", variables))
		{
			Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
		}
		found = true;
	}
	if (!found)
	{
		Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	return true;
}
Console_RegisterCommand(GDisplay_DrawCircle, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, GDisplay_Aliases(), GDisplay_Width(), GDisplay_Height(), GDisplay_Radius(), GDisplay_Inverted()); });

function GDisplay_DrawLine(alias_name, xStart, yStart, xEnd, yEnd, inverted)
{
	if (arguments.length < 6)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	xStart = parseInt(xStart);
	yStart = parseInt(yStart);
	xEnd = parseInt(xEnd);
	yEnd = parseInt(yEnd);
	var aliases_data = Module_ResolveAlias(alias_name, GDisplay_ModuleNames);
	var found = false;
	for (var name in aliases_data)
	{
		var variables = {
			"X1"    : xStart,
			"Y1"    : yStart,
			"Inverted" : inverted,
			"X2"    : xEnd,
			"Y2"    : yEnd};
			
		if (!Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "LCD_DrawLine", variables))
		{
			Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
		}
		found = true;
	}
	if (!found)
	{
		Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	return true;
}
Console_RegisterCommand(GDisplay_DrawLine, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, GDisplay_Aliases(), GDisplay_Width(), GDisplay_Height(),GDisplay_Width(), GDisplay_Height(), GDisplay_Inverted()); });

function GDisplay_invertRect(alias_name, xStart, yStart, width, height)
{
	if (arguments.length < 5)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	xStart = parseInt(xStart);
	yStart = parseInt(yStart);
	width = parseInt(width);
	height = parseInt(height);
	var aliases_data = Module_ResolveAlias(alias_name, GDisplay_ModuleNames);
	var found = false;
	for (var name in aliases_data)
	{
		var variables = {
			"X"    : xStart,
			"Y"    : yStart,
			"Width"    : width,
			"Height"    : height};
			
		if (!Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "LCD_InvertRect", variables))
		{
			Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
		}
		found = true;
	}
	if (!found)
	{
		Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	return true;
}
Console_RegisterCommand(GDisplay_invertRect, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, GDisplay_Aliases(), GDisplay_Width(), GDisplay_Height(),GDisplay_Width(), GDisplay_Height()); });

function GDisplay_DrawRect(alias_name, xStart, yStart, width, height, inverted, fill, radius)
{
	if (arguments.length < 8)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	xStart = parseInt(xStart);
	yStart = parseInt(yStart);
	width = parseInt(width);
	height = parseInt(height);
	radius = parseInt(radius);
	var aliases_data = Module_ResolveAlias(alias_name, GDisplay_ModuleNames);
	var found = false;
	for (var name in aliases_data)
	{
		var variables = {
			"X"    : xStart,
			"Y"    : yStart,
			"Width"    : width,
			"Height"    : height,
			"Inverted" : inverted,
			"Fill" : fill,
			"Radius" : radius};
			
		if (!Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "LCD_DrawRect", variables))
		{
			Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
		}
		found = true;
	}
	if (!found)
	{
		Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	return true;
}
Console_RegisterCommand(GDisplay_DrawRect, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, GDisplay_Aliases(), GDisplay_Width(), GDisplay_Height(),GDisplay_Width(), GDisplay_Height(), GDisplay_Inverted(), GDisplay_Fill(), GDisplay_Radius()); });

function GDisplay_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(GDisplay_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "LCD_Backlight":
			{
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);
					
					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Strength"] = { "value" : variables["Strength"], "timestamp" : get_time() };
					last_value["AutoLight"] = { "value" : variables["AutoLight"], "timestamp" : get_time() };
					
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
		}
	}
}
Module_RegisterToOnMessage("all", GDisplay_OnMessage);

function GDisplay_OnChange(module_name, module_id, available)
{
	if (in_array(GDisplay_ModuleNames, module_name) && available)
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		for (var alias_name in aliases_data)
		{
			var variables = { };
			
			Module_SendMessage(aliases_data[alias_name]["module_name"], aliases_data[alias_name]["module_id"], "LCD_Backlight", variables);
		}
	}
}
Module_RegisterToOnChange("all", GDisplay_OnChange);
