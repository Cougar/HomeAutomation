
Sensor_ModuleNames    = [ "DS18x20", "FOST02", "BusVoltage", "SimpleDTMF" ];
Sensor_Intervals      = function() { return [ 1, 5, 10, 15, 20 ]; };
Sensor_Aliases        = function() { return Module_GetAliasNames(Sensor_ModuleNames); };
Sensor_AvailableIds   = function() { return Module_GetAvailableIds(Sensor_ModuleNames); };

function Sensor_SetReportInterval(alias_name, interval)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, Sensor_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
		"SensorId" : aliases_data[name]["specific"]["SensorId"],
		"time"     : interval };
		
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
Console_RegisterCommand(Sensor_SetReportInterval, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Sensor_Aliases(), Sensor_Intervals()); });

function Sensor_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(Sensor_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "Phonenumber":
			{
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);
				
					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value[command] = { "value" : variables["Number"], "timestamp" : get_time() };
					
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
			case "Voltage":
			case "Temperature_Celsius":
			case "Humidity_Percent":
			{
				for (var alias_name in aliases_data)
				{
					if (aliases_data[alias_name]["specific"]["SensorId"] != variables["SensorId"])
					{
						continue;
					}
					
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value[command] = { "value" : variables["Value"], "timestamp" : get_time() };
					
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
		}
	}
}
Module_RegisterToOnMessage("all", Sensor_OnMessage);
