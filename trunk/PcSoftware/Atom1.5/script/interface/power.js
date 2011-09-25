
//Require("plugin/module.js");

Power_ModuleNames    = [ "power" ];
Power_Intervals      = function() { return [ 1, 5, 10, 15, 20 ]; };
Power_Energy         = function() { return [ 0, 500, 1000, 5000, 10000 ]; };
Power_Aliases        = function() { return Module_GetAliasNames(Power_ModuleNames); };
Power_AvailableIds   = function() { return Module_GetAvailableIds(Power_ModuleNames); };

function Power_SetEnergy(alias_name, energy)
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
		var variables = {"Energy"     : energy };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "setEnergy", variables))
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
Console_RegisterCommand(Power_SetEnergy, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Power_Aliases(), Power_Energy()); });

function Power_SetReportInterval(alias_name, interval)
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
Console_RegisterCommand(Power_SetReportInterval, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Power_Aliases(), Power_Intervals()); });

function Power_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(Power_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			/*
			case "avgPower":
			{
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Energy"] = { "value" : variables["Energy"], "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
			*/
			case "ElectricPower":
			{
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Power"] = { "value" : variables["Power"], "timestamp" : get_time() };
					last_value["EnergySum"] = { "value" : variables["EnergySum"], "timestamp" : get_time() };
					last_value["Power32"] = { "value" : variables["Power32"], "timestamp" : get_time() };
					
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
		}
	}
}
Module_RegisterToOnMessage("all", Power_OnMessage);
