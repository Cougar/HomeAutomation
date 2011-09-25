
PID_ModuleNames    = [ "PID" ];
PID_Intervals      = function() { return [ 1, 5, 10, 15, 20 ]; };
//PID_Energy         = function() { return [ 0, 500, 1000, 5000, 10000 ]; };
PID_Temperatures	= function() { return [ 20, 25, 30, 35 ]; };
PID_Aliases        = function() { return Module_GetAliasNames(PID_ModuleNames); };
PID_AvailableIds   = function() { return Module_GetAvailableIds(PID_ModuleNames); };

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


function PID_setParameters(alias_name, KP,KI,KD,Time,Unit)
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
		var variables = {	"K_P"     : KP,
					"K_I"     : KI,
					"K_D"     : KD,
					"RegulatorTime"     : Time,
					"TimeUnit"     : Unit };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "CONFIG_PARAMETER", variables))
		{
			Log("\033[32mCommand sent successfully to " + name + ".\033[0m");
			Log("\033[32mK_P = " + KP + "K_I = " + KI + "K_D = " + KD + "RegTime = " + Time + "Unit = " + Unit + ".\033[0m\n");
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
					
					last_value["Debug"] = { "P_term" : variables["P_term"],"I_term" : variables["I_term"],"D_term" : variables["D_term"],"Sum" : variables["Sum"], "timestamp" : get_time() };
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
					
					last_value["Sensor"] = { "sensorModuleType" : variables["sensorModuleType"],"sensorModuleId" : variables["sensorModuleId"],"sensorId" : variables["sensorId"], "timestamp" : get_time() };
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
					
					last_value["Actuator"] = { "actuatorModuleType" : variables["actuatorModuleType"],"actuatorModuleId" : variables["actuatorModuleId"],"actuatorId" : variables["actuatorId"], "timestamp" : get_time() };
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
					
					last_value["Parameters"] = { "K_P" : variables["K_P"],"K_I" : variables["K_I"],"K_D" : variables["K_D"],"TimeUnit" : variables["TimeUnit"],"RegulatorTime" : variables["RegulatorTime"], "timestamp" : get_time() };
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
					
					last_value["PID_status"] = { "Measurment" : variables["Measurment"],"Reference" : variables["Reference"],"PWM" : variables["PWM"],"Sum" : variables["Sum"], "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
	//log("PID status: mea:" + this.Measurment + " ref:" + this.Reference + " pwm:" + this.PWM/100 + " sum:" + this.Sum + "\n");
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
					
					last_value["Reference"] = { "Value" : variables["Value"], "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}	
				break;
			case "Report_Interval":
				this.myReportInterval = canMessage.getData("Time");
				break;
		}
	}
}
Module_RegisterToOnMessage("all", PID_OnMessage);
