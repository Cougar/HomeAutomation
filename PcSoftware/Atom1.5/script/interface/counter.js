
Counter_ModuleNames    = [ "counter" ];
Counter_Intervals      = function() { return [ 1, 5, 10, 15, 20 ]; };
Counter_Count          = function() { return [ 0, 500, 1000, 5000, 10000 ]; };
Counter_Aliases        = function() { return Module_GetAliasNames(Counter_ModuleNames); };
Counter_AvailableIds   = function() { return Module_GetAvailableIds(Counter_ModuleNames); };

function Counter_SetCount(alias_name, count)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}

	var aliases_data = Module_ResolveAlias(alias_name, Counter_ModuleNames);
	var found = false;

	for (var name in aliases_data)
	{
		var variables = {"Count"     : count };

		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "setCounter", variables))
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
Console_RegisterCommand(Counter_SetCount, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Counter_Aliases(), Counter_Count()); });

function Counter_SetReportInterval(alias_name, interval)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}

	var aliases_data = Module_ResolveAlias(alias_name, Counter_ModuleNames);
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
Console_RegisterCommand(Counter_SetReportInterval, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Counter_Aliases(), Counter_Intervals()); });

function Counter_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(Counter_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});

		switch (command)
		{
			case "Counter":
			{
				for (var alias_name in aliases_data)
				{
					if (aliases_data[alias_name]["specific"]["PinId"] != variables["PinId"])
					{
						continue;
					}

					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}

					last_value[command] = { "value" : variables["Count"], "timestamp" : get_time() };

					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				break;
			}
		}
	}
}
Module_RegisterToOnMessage("all", Counter_OnMessage);
