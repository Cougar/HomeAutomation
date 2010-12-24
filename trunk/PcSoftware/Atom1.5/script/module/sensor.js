
Sensor_ModuleNames    = [ "DS18x20", "FOST02", "BusVoltage" ];
Sensor_Intervals      = function() { return [ 1, 5, 10, 15, 20 ]; };
Sensor_Aliases        = function() { return Module_GetAliasNames(Sensor_ModuleNames); };
Sensor_AvailableIds   = function() { return Module_GetAvailableIds(Sensor_ModuleNames); };

function Sensor_SetReportInterval(alias_name, interval)
{
	if (arguments.length < 2)
	{
		Log("\033[31;1mNot enough parameters given.\033[0m\n");
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
				Log("\033[32;1mCommand sent successfully to " + name + ".\033[0m\n");
			}
			else
			{
				Log("\033[31;1mFailed to send command to " + name + ".\033[0m\n");
			}
			
			found = true;
	}
	
	if (!found)
	{
		Log("\033[31;1mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	
	return true;
}
Console_RegisterCommand(Sensor_SetReportInterval, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Sensor_Aliases(), Sensor_Intervals()); });
