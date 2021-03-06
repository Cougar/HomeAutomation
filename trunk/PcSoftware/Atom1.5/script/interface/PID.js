
PID_ModuleNames    	= [ "PID" ];
PID_SensorModuleNames 	= [ "DS18x20", "FOST02", "DHT11", "NTC", "TC1047A", "LM335", "TCN75A" ];
PID_ActModuleNames 	= [ "hwPWM", "softPWM", "SlowPWM", "PCAPWM" ];
PID_Algos      	= function() { return [ "ZIEGLER_NICHOLS_PI", "ZIEGLER_NICHOLS_PID", "TYREUS_LUYBEN_PI", "TYREUS_LUYBEN_PID", "CIANCONE_MARLIN_PI", "CIANCONE_MARLIN_PID", "AMIGOF_PI", "PESSEN_INTEGRAL_PID", "SOME_OVERSHOOT_PID", "NO_OVERSHOOT_PID" , "STOP"  ]; };
PID_Intervals      	= function() { return [ 1, 5, 10, 15, 20 ]; };
PID_Outputs         = function() { return [ 0, 500, 1000, 5000, 10000 ]; };
PID_Temperatures	= function() { return [ 20, 25, 30, 35 ]; };
PID_Constants		= function() { return [ 0, 0.1, 0.5, 1, 10, 100, 1000 ]; };
PID_LookBackTimes	= function() { return [ 0, 1, 10, 100, 255 ]; };
PID_TimeUnits		= function() { return [ "s", "ms"]; };
PID_ControllerDirection = function() { return [ "direct", "reverse"]; };
PID_Times		= function() { return [ 1, 5, 10, 15, 20, 40, 60]; };
PID_Aliases        	= function() { return Module_GetAliasNames(PID_ModuleNames); };
PID_AvailableIds   	= function() { return Module_GetAvailableIds(PID_ModuleNames); };
PID_SensorAliases	= function() { return Module_GetAliasNames(PID_SensorModuleNames); };
PID_ActAliases	= function() { return Module_GetAliasNames(PID_ActModuleNames); };

function PID_SetReportInterval(alias_name, interval)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
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
Console_RegisterCommand(PID_SetReportInterval, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_Intervals()); });


function PID_setValue(alias_name, temperature)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;
	
	Log ("Called SetValue: "+temperature+"\n");

	for (var name in aliases_data)
	{
		var variables = {	"Value"     : temperature,
					"SensorId"     : 0 };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Temperature_Celsius", variables))
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
Console_RegisterCommand(PID_setValue, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_Temperatures());  });

function PID_setSensor(alias_name, alias_sensor)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;
	
	var sensor_aliases_data = Module_ResolveAlias(alias_sensor, PID_SensorModuleNames);
	var first = true;
	for (var sensor in sensor_aliases_data) 
	{
		if (first == false)
		{
			Log("\033[31mNo sensor by the name " + alias_sensor + " were applicable for this command, or alias was a group.\033[0m\n");
			return false;
		}
		Log ("Called Set Sensor:\n");
		Log ("sensorModuleType:   " + sensor_aliases_data[sensor]["module_name"]+"\n");
		Log ("sensorModuleId:     " + sensor_aliases_data[sensor]["module_id"]+"\n");
		Log ("SensorId:           " + sensor_aliases_data[sensor]["specific"]["SensorId"]+"\n");
		var id = 0;
		switch (sensor_aliases_data[sensor]["module_name"])
		{
		  case "DS18x20": id = 3; break;
		  case "FOST02": id = 5; break;
		  case "TC1047A": id = 17; break;
		  case "TCN75A": id = 46; break;
		  case "LM335": id = 51; break;
		  case "NTC": id = 55; break;
		  case "DHT11": id = 61; break;
		  default: 
		    Log("\033[31mCould not resolve sensor module type.\033[0m\n");
		    return; break;
		}
		
		
		for (var name in aliases_data)
		{
			var variables = {	"sensorModuleType"     : id,
						"sensorModuleId"       : sensor_aliases_data[sensor]["module_id"], 
						"sensorId"     : sensor_aliases_data[sensor]["specific"]["SensorId"] };

			if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_SENSOR", variables))
			{
				Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
			}
			else
			{
				Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
			}
			
			found = true;
		}
		first = false;
		if (!found)
		{
			Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
			return false;
		}
	}

	if (!found)
	{
		Log("\033[31mNo sensor aliases by the name " + alias_sensor + " were applicable for this command.\033[0m\n");
		return false;
	}

	return true;
}
Console_RegisterCommand(PID_setSensor, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_SensorAliases());  });

function PID_setActuator(alias_name, alias_actuator)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;
	
	var act_aliases_data = Module_ResolveAlias(alias_actuator, PID_ActModuleNames);
	var first = true;
	for (var act in act_aliases_data) 
	{
		if (first == false)
		{
			Log("\033[31mNo actuator by the name " + alias_actuator + " were applicable for this command, or alias was a group.\033[0m\n");
			return false;
		}
		Log ("Called Set Actuator:\n");
		Log ("actModuleType:   " + act_aliases_data[act]["module_name"]+"\n");
		Log ("actModuleId:     " + act_aliases_data[act]["module_id"]+"\n");
		Log ("actId:           " + act_aliases_data[act]["specific"]["Channel"]+"\n");
		var id = 0;
		switch (act_aliases_data[act]["module_name"])
		{
		  case "hwPWM": id = 13; break;
		  case "softPWM": id = 21; break;
		  case "SlowPWM": id = 19; break;
		  case "PCAPWM": id = 29; break;
		  default: 
		    Log("\033[31mCould not resolve actuator module type.\033[0m\n");
		    return; break;
		}
		
		
		for (var name in aliases_data)
		{
			var variables = {	"actuatorModuleType"     : id,
						"actuatorModuleId"       : act_aliases_data[act]["module_id"], 
						"actuatorId"             : act_aliases_data[act]["specific"]["Channel"] };

			if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_ACTUATOR", variables))
			{
				Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
			}
			else
			{
				Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
			}
			
			found = true;
		}
		first = false;
		if (!found)
		{
			Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
			return false;
		}
	}

	if (!found)
	{
		Log("\033[31mNo actuator aliases by the name " + alias_actuator + " were applicable for this command.\033[0m\n");
		return false;
	}

	return true;
}
Console_RegisterCommand(PID_setActuator, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_ActAliases());  });


function PID_setParameters(alias_name, KP,KI,KD,Time,Unit,ControllerDirection)
{
	if (arguments.length < 6)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;

	for (var name in aliases_data)
	{
		var variables = {	"K_P"     		: KP,
					"K_I"     		: KI,
					"K_D"     		: KD,
					"RegulatorTime"     	: Time,
					"TimeUnit"     		: Unit };
		var variables_P_I = {	"K_P"     		: KP,
					"K_I"     		: KI };
		var variables_D_T = {	"K_D"     		: KD,
					"ControllerDirection"   : ControllerDirection,
					"RegulatorTime"     	: Time,
					"TimeUnit"     		: Unit };
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_PARAMETER", variables))
		{
			Log("\033[32mCommand sent successfully to " + name + ".\033[0m");
			Log("\033[32mK_P = " + KP + "K_I = " + KI + "K_D = " + KD + "RegTime = " + Time + "Unit = " + Unit + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
		}
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_PARAMETER_P_I", variables_P_I))
		{
			Log("\033[32mK_P = " + KP + "K_I = " + KI + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
		}
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_PARAMETER_D_T", variables_D_T))
		{
			Log("\033[32mK_D = " + KD + " RegTime = " + Time + " Unit = " + Unit + " Direction = " + ControllerDirection +".\033[0m\n");
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
Console_RegisterCommand(PID_setParameters, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_Constants(), PID_Constants(), PID_Constants(), PID_Times(), PID_TimeUnits(), PID_ControllerDirection()); });


function PID_setOutputMaxMin(alias_name, Max,Min)
{
	if (arguments.length < 3)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;

	for (var name in aliases_data)
	{
		var variables = {	"Min"     : Min,
					"Max"     : Max };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "OutMinMax", variables))
		{
			Log("\033[32mMin = " + Min + "Max = " + Max + ".\033[0m\n");
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
Console_RegisterCommand(PID_setOutputMaxMin, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_Outputs(), PID_Outputs()); });

function PID_getConfiguration(alias_name)
{
	if (arguments.length < 1)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;

	for (var name in aliases_data)
	{
		var variables = {};
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_SENSOR", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 1 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_ACTUATOR", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 2 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_PARAMETER", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 3 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Report_Interval", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 4 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_PARAMETER_P_I", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 5 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_PARAMETER_D_T", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 6 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "OutMinMax", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 7 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "RUN_AUTOTUNE", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 8 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_AUTOTUNE_NOISE_STEP", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 9 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_PID_START_VALUE", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 9 to " + name + ".\033[0m\n");
		}
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_ONOFF_REGULATOR", variables))
		{
			//Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31mFailed to send command 9 to " + name + ".\033[0m\n");
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
Console_RegisterCommand(PID_getConfiguration, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases()); });



function PID_startAutoTune(alias_name, Algo ,lookbacktime)
{
	if (arguments.length < 3)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;

	for (var name in aliases_data)
	{
		var variables= {	"Method"     		: Algo,
					"LookBackTime"     		: lookbacktime };
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "RUN_AUTOTUNE", variables))
		{
			Log("\033[32mCommand sent successfully to " + name + ".\033[0m");
			Log("\033[32mMethod = " + Algo + "LookBackTime = " + lookbacktime + ".\033[0m\n");
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
Console_RegisterCommand(PID_startAutoTune, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_Algos(), PID_LookBackTimes()); });

function PID_AutoTuneNoiceStep(alias_name, NoiseBand ,OutputStep)
{
	if (arguments.length < 3)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;

	for (var name in aliases_data)
	{
		var variables= {	"NoiseBand"     		: NoiseBand,
					"OutputStep"     		: OutputStep };
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_AUTOTUNE_NOISE_STEP", variables))
		{
			Log("\033[32mCommand sent successfully to " + name + ".\033[0m");
			Log("\033[32mNoiseBand = " + NoiseBand + "OutputStep = " + OutputStep + ".\033[0m\n");
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
Console_RegisterCommand(PID_AutoTuneNoiceStep, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_Constants(), PID_Constants()); });


function PID_ConfigStartValue(alias_name, Factor, Offset)
{
	if (arguments.length < 3)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;

	for (var name in aliases_data)
	{
		var variables= {	"Factor"     		: Factor,
					"Offset"     		: Offset};
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_PID_START_VALUE", variables))
		{
			Log("\033[32mCommand sent successfully to " + name + ".\033[0m");
			Log("\033[32mFactor = " + Factor+ "Offset = " + Offset+ ".\033[0m\n");
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
Console_RegisterCommand(PID_ConfigStartValue, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_Constants(), PID_Constants()); });

function PID_ConfigOnOff(alias_name, OnValue, OffValue, OuterRange, InnerRange)
{
	if (arguments.length < 5)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	var aliases_data = Module_ResolveAlias(alias_name, PID_ModuleNames);
	var found = false;

	for (var name in aliases_data)
	{
		var variables= {	"OnValue"     		: OnValue,
					"OffValue"     		: OffValue,
					"OuterRange"     		: OuterRange,
					"InnerRange"     		: InnerRange};
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_ONOFF_REGULATOR", variables))
		{
			Log("\033[32mCommand sent successfully to " + name + ".\033[0m");
			Log("\033[32mOnValue = " + OnValue+ "OffValue= " + OffValue+ "OuterRange= " + OuterRange+ "InnerRange= " + InnerRange+ ".\033[0m\n");
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
Console_RegisterCommand(PID_ConfigOnOff, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, PID_Aliases(), PID_Temperatures(), PID_Temperatures(), PID_Outputs(), PID_Outputs()); });



function PID_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(PID_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "DEBUG":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Debug"] = { "value" : { "P_term" : variables["P_term"],"I_term" : variables["I_term"],"D_term" : variables["D_term"],"Sum" : variables["Sum"] }, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
					//Log("Got message DEBUG");
				}
			break;
			case "CONFIG_SENSOR":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Sensor"] = { "value" : { "sensorModuleType" : variables["sensorModuleType"],"sensorModuleId" : variables["sensorModuleId"],"sensorId" : variables["sensorId"] }, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
			break;
			case "CONFIG_ACTUATOR":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Actuator"] = { "value" : { "actuatorModuleType" : variables["actuatorModuleType"],"actuatorModuleId" : variables["actuatorModuleId"],"actuatorId" : variables["actuatorId"] }, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
			break;
			case "CONFIG_PARAMETER":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Parameters"] = { "value" : { "K_P" : variables["K_P"],"K_I" : variables["K_I"],"K_D" : variables["K_D"],"TimeUnit" : variables["TimeUnit"],"RegulatorTime" : variables["RegulatorTime"] }, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
			break;
			case "PID_STATUS":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["PID_status"] = { "value" : { "Measurment" : variables["Measurment"],"Reference" : variables["Reference"],"PWM" : variables["PWM"],"Sum" : variables["Sum"] }, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
	//Log("PID status: mea:" + this.Measurment + " ref:" + this.Reference + " pwm:" + this.PWM/100 + " sum:" + this.Sum + "\n");
				//this.callEvent("newValue", canMessage.getData("Id"));

	//var self = this;
	//	this.myHTTP.request(function(result, header, content) { self.httpCallback(result, header, content); }, "linuxz.mine.nu/canlogg/add.php?measurment="+ this.Measurment + "&reference="+this.Reference+"&pwm="+this.PWM/100+"&sum="+this.Sum+"&p="+this.K_P+"&i="+this.K_I+"&d="+this.K_D+"&timeunit="+this.TimeUnit+"&pidtime="+this.RegulatorTime+"");
			break;
			case "Temperature_Celsius":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Reference"] = { "value" : variables["Value"], "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}	
				break;
			case "Report_Interval":
				this.myReportInterval = canMessage.getData("Time");
				break;
				
			case "CONFIG_PARAMETER_P_I":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Parameters"] = { "value" : { 
						"K_P" : variables["K_P"],
						"K_I" : variables["K_I"], 
						"K_D" : last_value["Parameters"]["value"]["K_D"],
						"ControllerDirection" : last_value["Parameters"]["value"]["ControllerDirection"],
						"TimeUnit" : last_value["Parameters"]["value"]["TimeUnit"],
						"RegulatorTime" : last_value["Parameters"]["value"]["RegulatorTime"],
						"OnValue" : last_value["Parameters"]["value"]["OnValue"],
						"OffValue" : last_value["Parameters"]["value"]["OffValue"] ,
						"OuterRange" : last_value["Parameters"]["value"]["OuterRange"] ,
						"InnerRange" : last_value["Parameters"]["value"]["InnerRange"] ,
						"Factor" : last_value["Parameters"]["value"]["Factor"] ,
						"Offset" : last_value["Parameters"]["value"]["Offset"] 
						}, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
					//Log("Stored PI");
				}	
				break;
			case "CONFIG_PARAMETER_D_T":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					last_value["Parameters"] = { "value" : { 
						"K_P" : last_value["Parameters"]["value"]["K_P"],
						"K_I" : last_value["Parameters"]["value"]["K_I"],
						"K_D" : variables["K_D"],
						"ControllerDirection" : variables["ControllerDirection"],
						"TimeUnit" : variables["TimeUnit"],
						"RegulatorTime" : variables["RegulatorTime"],
						"OnValue" : last_value["Parameters"]["value"]["OnValue"],
						"OffValue" : last_value["Parameters"]["value"]["OffValue"] ,
						"OuterRange" : last_value["Parameters"]["value"]["OuterRange"] ,
						"InnerRange" : last_value["Parameters"]["value"]["InnerRange"] ,
						"Factor" : last_value["Parameters"]["value"]["Factor"] ,
						"Offset" : last_value["Parameters"]["value"]["Offset"]  
						}, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
					//Log("Stored DT for "+ alias_name);
				}	
				break;
			case "CONFIG_ONOFF_REGULATOR":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					last_value["Parameters"] = { "value" : { 
						"K_P" : last_value["Parameters"]["value"]["K_P"],
						"K_I" : last_value["Parameters"]["value"]["K_I"],
						"K_D" : last_value["Parameters"]["value"]["K_D"],
						"ControllerDirection" : last_value["Parameters"]["value"]["ControllerDirection"],
						"TimeUnit" : last_value["Parameters"]["value"]["TimeUnit"],
						"RegulatorTime" : last_value["Parameters"]["value"]["RegulatorTime"],
						"OnValue" : variables["OnValue"],
						"OffValue" : variables["OffValue"] ,
						"OuterRange" : variables["OuterRange"] ,
						"InnerRange" : variables["InnerRange"] ,
						"Factor" : last_value["Parameters"]["value"]["Factor"] ,
						"Offset" : last_value["Parameters"]["value"]["Offset"] 
						}, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
					//Log("Stored DT for "+ alias_name);
				}	
				break;
			case "CONFIG_PID_START_VALUE":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					last_value["Parameters"] = { "value" : { 
						"K_P" : last_value["Parameters"]["value"]["K_P"],
						"K_I" : last_value["Parameters"]["value"]["K_I"],
						"K_D" : last_value["Parameters"]["value"]["K_D"],
						"ControllerDirection" : last_value["Parameters"]["value"]["ControllerDirection"],
						"TimeUnit" : last_value["Parameters"]["value"]["TimeUnit"],
						"RegulatorTime" : last_value["Parameters"]["value"]["RegulatorTime"],
						"OnValue" : last_value["Parameters"]["value"]["OnValue"],
						"OffValue" : last_value["Parameters"]["value"]["OffValue"] ,
						"OuterRange" : last_value["Parameters"]["value"]["OuterRange"] ,
						"InnerRange" : last_value["Parameters"]["value"]["InnerRange"] ,
						"Factor" : variables["Factor"] ,
						"Offset" : variables["Offset"] 
						}, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
					//Log("Stored DT for "+ alias_name);
				}	
				break;
			case "P_I_term":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Debug"] = { "value" : { "P_term" : variables["P_term"],"I_term" : variables["I_term"],"D_term" : last_value["Debug"]["value"]["D_term"],"Output" : last_value["Debug"]["value"]["Output"],"Min" : last_value["Debug"]["value"]["Min"],"Max" : last_value["Debug"]["value"]["Max"] }, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
					//Log("Got message DEBUG");
				}
			break;	
			case "D_term_Out":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					//Log("Got message DEBUG");
					
					last_value["Debug"] = { "value" : { "P_term" : last_value["Debug"]["value"]["P_term"],"I_term" : last_value["Debug"]["value"]["I_term"],"D_term" : variables["D_term"],"Output" : variables["Output"],"Min" : last_value["Debug"]["value"]["Min"],"Max" : last_value["Debug"]["value"]["Max"] }, "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
					//Log("Got message DEBUG");
				}
			break;		
			case "OutMinMax":
				for (var alias_name in aliases_data)
				{
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Debug"] = { "value" : { 
						"P_term" : last_value["Debug"]["value"]["P_term"],
						"I_term" : last_value["Debug"]["value"]["I_term"],
						"D_term" : last_value["Debug"]["value"]["D_term"],
						"Output" : last_value["Debug"]["value"]["Output"], 
						"Min" : variables["Min"],
						"Max" : variables["Max"] 
						},"timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
					//Log("Got message DEBUG");
				}
			break;	
		}
	}
}
Module_RegisterToOnMessage("all", PID_OnMessage);
/*PID={"rrd":{"file":"/var/www/CANgraph/PID.rrd","Period_s":"10"}, "rrdNames":{"Pterm":{"Module":"PID","data":"Debug","value":"P_term"},"Iterm":{"Module":"PID","data":"Debug","value":"I_term"}."Dterm":{"Module":"PID","data":"Debug","value":"D_term"},"KP":{"Module":"PID","data":"Parameters","value":"K_P"},"KI":{"Module":"PID","data":"Parameters","value":"K_I"},"KD":{"Module":"PID","data":"Parameters","value":"K_D"},"out":{"Module":"PID","data":"Debug","value":"Output"},"ref":{"Module":"PID","data":"Reference","value":"value"},"in":{"Module":"PID","data":"PID_status","value":"Measurment"}}}*/