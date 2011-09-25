
Display_ModuleNames    = [ "HD44789" ];
Display_Height         = function() { return [ 0, 1, 2, 3  ]; };
Display_Width          = function() { return [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 ]; };
Display_Backlight      = function() { return [ 0, 50, 100, 150, 200, 255 ]; };
Display_Aliases        = function() { return Module_GetAliasNames(Display_ModuleNames); };
Display_AvailableIds   = function() { return Module_GetAvailableIds(Display_ModuleNames); };

function Display_Clear(alias_name)
{
	if (arguments.length < 1)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, Display_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {};
			
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "LCD_Clear", variables))
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
Console_RegisterCommand(Display_Clear, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Display_Aliases()); });

function Display_SetBacklight(alias_name, strength)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, Display_ModuleNames);
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
Console_RegisterCommand(Display_SetBacklight, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Display_Aliases(), Display_Backlight()); });


function Display_Print(alias_name, x, y, text)
{
	if (arguments.length < 4)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
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
	
	var aliases_data = Module_ResolveAlias(alias_name, Display_ModuleNames);
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
Console_RegisterCommand(Display_Print, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Display_Aliases(), Display_Width(), Display_Height()); });


function Display_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(Display_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "LCD_Size":
			{
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);
					
					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Width"] = { "value" : variables["Width"], "timestamp" : get_time() };
					last_value["Height"] = { "value" : variables["Height"], "timestamp" : get_time() };
					
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
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
Module_RegisterToOnMessage("all", Display_OnMessage);

function Display_OnChange(module_name, module_id, available)
{
	if (in_array(Display_ModuleNames, module_name) && available)
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
			Module_SendMessage(aliases_data[alias_name]["module_name"], aliases_data[alias_name]["module_id"], "LCD_Size", variables);
		}
	}
}
Module_RegisterToOnChange("all", Display_OnChange);
