
Output_ModuleNames    = [ "output","multiSwitch"];
Output_ModuleNamesOutput    = [ "output"];
Output_ModuleNamesSwitch = [ "multiSwitch"];
Output_Aliases  = function() { return Module_GetAliasNames(Output_ModuleNames); };
Output_Aliases_output  = function() { return Module_GetAliasNames(Output_ModuleNamesOutput); };
Output_Aliases_Switch  = function() { return Module_GetAliasNames(Output_ModuleNamesSwitch); };
Output_SwitchState    = function() { return [ "0", "1", "2", "3" ]; };
Output_AvailableIds   = function() { return Module_GetAvailableIds(Output_ModuleNames); };
Output_AvailableIdsOutput   = function() { return Module_GetAvailableIds(Output_ModuleNamesOutput); };
Output_AvailableIdsSwitch   = function() { return Module_GetAvailableIds(Output_ModuleNamesSwitch); };
Output_pin            = function() { return [ "High", "Low" ]; };

//Log("\033[31mStarting input.\033[0m\n");


function Output_OnMessage(module_name, module_id, command, variables)
{
  //Log("\033[31mGot message "+module_name + " "+ module_id+" "+command+ "if: "+in_array(Output_ModuleNames, module_name)+".\033[0m\n");
	
	if (in_array(Output_ModuleNames, module_name))
	{
	//  Log("\033[31mGot message2 "+command+".\033[0m\n");
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "SetPin":
			{
			//  Log("\033[31mGot PinStatus message.\033[0m\n");
				for (var alias_name in aliases_data)
				{
					if (aliases_data[alias_name]["module_name"] == "multiSwitch") 
					{
						var last_value = {};
						var last_value_string = Storage_GetParameter("LastValues", alias_name);

						if (last_value_string)
						{
							last_value = eval("(" + last_value_string + ")");
						}
						
						last_value["Status"] = { "value" : variables["PinId"], "timestamp" : get_time() };
						
						Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
					    continue;
					}
					if (aliases_data[alias_name]["specific"]["PinId"] != variables["PinId"])
					{
						continue;
					}
				//	Log("\033[31mMessage from: "+alias_name+".\033[0m\n");
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);

					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Status"] = { "value" : variables["Status"], "timestamp" : get_time() };
					
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
		}
	}
}
Module_RegisterToOnMessage("all", Output_OnMessage);

function Output_SetPin(alias_name, status)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, Output_ModuleNamesOutput);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
			"PinId"   : aliases_data[name]["specific"]["PinId"],
			"Status"     : status};
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "SetPin", variables))
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
Console_RegisterCommand(Output_SetPin, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Output_Aliases_output(), Output_pin()); });


function Output_SetSwitch(alias_name, position)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, Output_ModuleNamesSwitch);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
			"PinId"   : position,
			"Status"     : "High"};
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "SetPin", variables))
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
Console_RegisterCommand(Output_SetSwitch, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Output_Aliases_Switch(), Output_SwitchState()); });
