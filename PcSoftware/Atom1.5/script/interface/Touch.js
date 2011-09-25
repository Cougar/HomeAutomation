
Touch_ModuleNames    = [ "Touch" ];
Touch_Aliases        = function() { return Module_GetAliasNames(Touch_ModuleNames); };
Touch_AvailableIds   = function() { return Module_GetAvailableIds(Touch_ModuleNames); };


function Touch_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(Touch_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
			"module_name" : module_name,
			"module_id"   : module_id,
			"group"       : false
		});
		
		switch (command)
		{
			case "Gesture":
				var gesture = null;
				gesture = Touch_lookupGesture(variables["f1"],variables["f2"],variables["f3"],variables["f4"],variables["f5"],variables["f6"],variables["f7"],variables["f8"],variables["f9"]);
				for (var alias_name in aliases_data)
				{
					if (gesture == null)
					{
						Log("\033[31m" + alias_name + ": New Gesture, f1: "+ variables["f1"] + ", f2: " + variables["f2"] + ", f3: " + variables["f3"] + ", f4: " + variables["f4"] + ", f5: " + variables["f5"] + ", f6: " + variables["f6"] + ", f7: " + variables["f7"] + ", f8: " + variables["f8"] + ", f9: " + variables["f9"] +".\033[0m\n");
					}
					else
					{
						//Log("\033[31m"+alias_name+": New Gesture: "+ gesture +".\033[0m\n");
						
					}	
					var last_value = {};
					var last_value_string = Storage_GetParameter("LastValues", alias_name);
				
					if (last_value_string)
					{
						last_value = eval("(" + last_value_string + ")");
					}
					
					last_value[command] = { "Gesture" : gesture, "timestamp" : get_time() };
					
					Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
				}
				break;
			case "Raw":

				break;
		}
	}
}
Module_RegisterToOnMessage("all", Touch_OnMessage);

function Touch_lookupGesture(f1, f2, f3, f4, f5, f6, f7, f8, f9)
{
	var gestures = Storage_GetParameters("Gestures");
	
	for (var gesture in gestures)
	{
		gesture_value = eval("(" + gestures[gesture] + ")");
		
		//log(this.myName + ":" + this.myId + "> Checking : " + gestureStore['gestures'][n]['name'] + "\n");
		var result = 1;
		if (gesture_value['masks']['f1_lower'] > f1 || gesture_value['masks']['f1_upper'] < f1) 
		{
		//	log("1");
			result = 0;
			continue;
		}
		if (gesture_value['masks']['f2'] == 1 && gesture_value['functions']['f2'] != f2)
		{
		//	log("2");
			result = 0;
			continue;
		}
		if (gesture_value['masks']['f3'] == 1 && gesture_value['functions']['f3'] != f3)
		{
		//	log("3");
			result = 0;
			continue;
		}
		if (gesture_value['masks']['f4'] == 1 && gesture_value['functions']['f4'] != f4)
		{
		//	log("4");
			result = 0;
			continue;
		}
		if (gesture_value['masks']['f5'] == 1 && gesture_value['functions']['f5'] != f5)
		{
		//	log("5");
			result = 0;
			continue;
		}
		if (gesture_value['masks']['f6'] == 1 && gesture_value['functions']['f6'] != f6)
		{
		//	log("6");
			result = 0;
			continue;
		}
		if (gesture_value['masks']['f7_lower'] > f7 || gesture_value['masks']['f7_upper'] < f7) 
		{
		//	log("7");
			result = 0;
			continue;
		}
		if (gesture_value['masks']['f8'] == 1 && gesture_value['functions']['f8'] != f8)
		{
		//	log("8");
			result = 0;
			continue;
		}
		if (gesture_value['masks']['f9'] == 1 && gesture_value['functions']['f9'] != f9)
		{
		//	log("9");
			result = 0;
			continue;
		}
		if (result == 1) {
			return gesture;
		}
	}
}

