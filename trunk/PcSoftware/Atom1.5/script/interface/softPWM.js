
softPWM_ModuleNames    = [ "softPWM" ];
softPWM_Intervals      = function() { return [ 1, 5, 10, 15, 20 ]; };
softPWM_PWMLevels      = function() { return [ 0, 25, 50, 75, 100 ]; };
softPWM_PWMPeriod      = function() { return [ 0, 128, 255, 5000, 10000, 32768, 65535 ]; };
softPWM_PWMResolution  = function() { return [ 0, 10, 100, 128, 255 ]; };
//softPWM_StrengthLevels = function() { return [ 0, 50, 100, 150, 200, 255 ]; };
//softPWM_StepsLevels    = function() { return [ 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ]; };
//softPWM_Directions     = function() { return [ "Increase", "Decrease" ]; };
//softPWM_Channels       = function() { return [ 0 ]; };
softPWM_Aliases        = function() { return Module_GetAliasNames(softPWM_ModuleNames); };
softPWM_AvailableIds   = function() { return Module_GetAvailableIds(softPWM_ModuleNames); };

function softPWM_Configure(alias_name, pwmPeriod, pwmResolution)
{
	if (arguments.length < 3)
	{	
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, softPWM_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
			"Period"   	: pwmPeriod,
			"Resolution"	: pwmResolution };
	
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG", variables))
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
Console_RegisterCommand(softPWM_Configure, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, softPWM_Aliases(), softPWM_PWMPeriod(), softPWM_PWMResolution()); });

function softPWM_SetPWMValue(alias_name, pwmvalue)
{
	if (arguments.length < 2)
	{	
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, softPWM_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		
		var value = parseInt(pwmvalue*100).toString();
		var variables = {
			"Id"   : aliases_data[name]["specific"]["Channel"],
			"Value"     : value };
	
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Pwm", variables))
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
Console_RegisterCommand(softPWM_SetPWMValue, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, softPWM_Aliases(), softPWM_PWMLevels()); });

function softPWM_SetReportInterval(alias_name, interval)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, Power_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {"Time"     : interval };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Report_Interval", variables))
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
Console_RegisterCommand(softPWM_SetReportInterval, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, softPWM_Aliases(), softPWM_Intervals()); });


function softPWM_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(softPWM_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "CONFIG":
			{
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					last_value["Period"] = { "value" : variables["Period"], "timestamp" : get_time() };
					last_value["Resolution"] = { "value" : variables["Resolution"], "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				break;
			}
			case "Pwm":
			{
				for (var alias_name in aliases_data)
				{
					if (aliases_data[alias_name]["specific"]["Channel"] != variables["Id"])
					{
						continue;
					}
					
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					var value = 0 + variables["Value"];
					value = value / 100;
					value = parseFloat(value).toString();
					last_value["Level"] = { "value" : value , "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
		}
	}
}
Module_RegisterToOnMessage("all", softPWM_OnMessage);
