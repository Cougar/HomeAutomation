
//Require("plugin/module.js");

Water_ModuleNames    = [ "water" ];
Water_Intervals      = function() { return [ 1, 5, 10, 15, 20 ]; };
Water_Volume         = function() { return [ 0, 500, 1000, 5000, 10000 ]; };
Water_Aliases        = function() { return Module_GetAliasNames(Water_ModuleNames); };
Water_AvailableIds   = function() { return Module_GetAvailableIds(Water_ModuleNames); };

/*
function Water_SetVolume(alias_name, volume)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, Water_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {"Volume"     : volume };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "setVolume", variables))
		{
			Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
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
Console_RegisterCommand(Water_SetVolume, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Water_Aliases(), Water_Volume()); });
*/
/*
function Water_SetReportInterval(alias_name, interval)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, Water_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {"Time"     : interval };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Report_Interval", variables))
		{
			Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
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
Console_RegisterCommand(Water_SetReportInterval, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Water_Aliases(), Water_Intervals()); });
*/

function Water_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(Water_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "Flow":
			{
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Flow"] = { "value" : variables["Flow"], "timestamp" : get_time() };
					last_value["Volume"] = { "value" : variables["Volume"], "timestamp" : get_time() };
					
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
		}
	}
}
Module_RegisterToOnMessage("all", Water_OnMessage);
