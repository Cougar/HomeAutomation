
Rotary_ModuleNames    = [ "Rotary","RotaryHex"];
Rotary_Aliases        = function() { return Module_GetAliasNames(Rotary_ModuleNames); };
Rotary_AvailableIds   = function() { return Module_GetAvailableIds(Rotary_ModuleNames); };


function Rotary_OnMessage(module_name, module_id, command, variables)
{
  
	if (in_array(Rotary_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "Rotary_Switch":
			{
				for (var alias_name in aliases_data)
				{
					if (aliases_data[alias_name]["specific"]["SwitchId"] != variables["SwitchId"])
					{
						continue;
					}
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);
				
					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value["Direction"] = { "value" : variables["Direction"], "timestamp" : get_time() };
					last_value["Steps"] = { "value" : variables["Steps"], "timestamp" : get_time() };
					last_value["Position"] = { "value" : variables["Position"], "timestamp" : get_time() };
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				
				break;
			}
			case "Button":
			{
				for (var alias_name in aliases_data)
				{
					if (aliases_data[alias_name]["specific"]["SwitchId"] != variables["SwitchId"])
					{
						continue;
					}
					
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
Module_RegisterToOnMessage("all", Rotary_OnMessage);
