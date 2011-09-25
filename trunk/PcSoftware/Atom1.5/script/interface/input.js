
Input_ModuleNames    = [ "input"];
Input_Aliases        = function() { return Module_GetAliasNames(Input_ModuleNames); };
Input_AvailableIds   = function() { return Module_GetAvailableIds(Input_ModuleNames); };

//Log("\033[31mStarting input.\033[0m\n");


function Input_OnMessage(module_name, module_id, command, variables)
{
  //Log("\033[31mGot message "+module_name + " "+ module_id+" "+command+ "if: "+in_array(Input_ModuleNames, module_name)+".\033[0m\n");
	
	if (in_array(Input_ModuleNames, module_name))
	{
	//  Log("\033[31mGot message2 "+command+".\033[0m\n");
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "PinStatus":
			{
			//  Log("\033[31mGot PinStatus message.\033[0m\n");
				for (var alias_name in aliases_data)
				{
				  
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
Module_RegisterToOnMessage("all", Input_OnMessage);
