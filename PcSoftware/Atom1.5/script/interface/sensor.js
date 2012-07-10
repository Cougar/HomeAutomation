
Sensor_ModuleNames    = [ "DS18x20", "FOST02", "BusVoltage", "SimpleDTMF", "DHT11" ];
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
Console_RegisterCommand(Sensor_SetReportInterval, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Sensor_Aliases(), Sensor_Intervals()); });

Sensor_OnNewPhonenumberFunctions = null;

function Sensor_RegisterToNewPhonenumber(alias_name, callback)
{
	if (Sensor_OnNewPhonenumberFunctions == null)
	{
		Sensor_OnNewPhonenumberFunctions = new Array();
	}
	
	if (!Sensor_OnNewPhonenumberFunctions[alias_name])
	{
		Sensor_OnNewPhonenumberFunctions[alias_name] = new Array();
	}
	
	Sensor_OnNewPhonenumberFunctions[alias_name].push(callback);
}

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
					if (variables["Number"].length > 6) 
					{
						var direction;
						var start = 0;
						if (variables["Number"].charAt(0) == 'A') {
						  direction = "in";
						  start = 1;
						} else {
						    direction = "out";
						}
						  var endIndex = variables["Number"].indexOf('C');
						if (endIndex == -1) {
							endIndex = variables["Number"].length;
						 }
						 variables["Number"] = variables["Number"].substring(start,endIndex);
						var last_value = {};
						var last_value_string = Storage_GetParameter("LastValues", alias_name);
					
						if (last_value_string)
						{
							last_value = eval("(" + last_value_string + ")");
						}
						
						last_value[command] = { "value" : variables["Number"], "timestamp" : get_time(), "direction" : direction };
						
						Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));

						var all_numbers = Storage_GetParameters("PhoneCalls");
						var newString = "{\"number\":\"" + variables["Number"] + "\",\"module\":\"" + alias_name + "\",\"direction\":\"" + direction + "\"}";
						Storage_SetParameter("PhoneCalls", get_time(), newString);
						Log("New number: "+newString +"\n");
						Sensor_StoreNumberInPhonebook(variables["Number"]);
						if (Sensor_OnNewPhonenumberFunctions[alias_name])
						{
							for (var n in Sensor_OnNewPhonenumberFunctions[alias_name])
							{
								/* Call callback with arguments number, direction */
								Sensor_OnNewPhonenumberFunctions[alias_name][n](alias_name, variables["Number"], direction);
							}
						}
					}
					else if (variables["Number"].length > 1) 
					{
						if (variables["Number"] == "B00C")
						{
							/* Unknown number */
							var newString = "{\"number\":\"" + variables["Number"] + "\",\"module\":\"" + alias_name + "\",\"direction\":\"" + "in" + "\"}";
							Storage_SetParameter("PhoneCalls", get_time(), newString);
							if (Sensor_OnNewPhonenumberFunctions[alias_name])
							{
								for (var n in Sensor_OnNewPhonenumberFunctions[alias_name])
								{
									/* Call callback with arguments number, direction */
									Sensor_OnNewPhonenumberFunctions[alias_name][n](alias_name, variables["Number"], "in");
								}
							}
						}
						else if (variables["Number"] == "B10C")
						{
							/* Secret/protected number */
							var newString = "{\"number\":\"" + variables["Number"] + "\",\"module\":\"" + alias_name + "\",\"direction\":\"" + "in" + "\"}";
							Storage_SetParameter("PhoneCalls", get_time(), newString);
							if (Sensor_OnNewPhonenumberFunctions[alias_name])
							{
								for (var n in Sensor_OnNewPhonenumberFunctions[alias_name])
								{
									/* Call callback with arguments number, direction */
									Sensor_OnNewPhonenumberFunctions[alias_name][n](alias_name, variables["Number"], "in");
								}
							}
						}
						else
						{
							Log("Incoming phonecall, short number: "+variables["Number"]+" length: "+variables["Number"].length);
						}
					}
				}
				break;
			}
			case "Voltage":
			case "Temperature_Celsius":
			{
				if ((module_name == "DS18x20") && (variables["Value"] == 85))
				{
					Log("\033[31mDS18x20: Temperature conversion error: " + module_name + ":" + module_id + ", SensorId=" + variables["SensorId"] + "\033[0m");
					break;
				}
			}
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

function Sensor_StoreNumberInPhonebook(number) 
{
	var phonebook = Storage_GetJsonParamter("PhoneBook",number)
	if (!phonebook)
	{
		if (number.charAt(0) != '0') 
		{
			if (typeof sensorDefaultPhoneAreaCode === 'undefined')
			{
				Log("\033[31mOnlinePhonebook: please add sensorDefaultPhoneAreaCode = yourAreaCode to autostart.js, defaulting to 031\033[0m\n");  
			    number = "031"+number;
			}
			else
			{
			    number = sensorDefaultPhoneAreaCode+number;
			}
   		}
   		
   		var persons = new Array();
		url = "wap.eniro.se/query?search_word=" + number + "&what=mobwp";
		Log("\033[31mURL: http://"+url+"\033[0m\n");
		Http_Request(url, 
			function(socket_id, result, header, content_data) {
				if (result.indexOf("200 OK") != -1)
				{
					//Log("\033[31mOnlinePhonebook: result was: " + content_data + "\033[0m\n");
					var lines = content_data.split("\n");
					
					var endString = "<anchor>Tillbaka<prev/></anchor>";
					var nameStartString = "<td class=\"hTd2\">";
					var nameEndString = "</table>";
					for (var n = 1; n < lines.length; n++)
					{
						var line = lines[n].trim("\n");
						
						if (line.indexOf(endString) != -1)
						{
							break;
						}
						if (line.indexOf(nameStartString) != -1)
						{
							line = lines[n+1].trim("\n");
							splitted = line.split("b>");
							persons[persons.length] = splitted[1].substr(0, splitted[1].length -2);
							break;
						}
						
					}
					
					//Log("\033[31mOnlinePhonebook: Line: " + lines[1] + "\033[0m\n");
					//Log("\033[31mOnlinePhonebook: Line: " + lines[2] + "\033[0m\n");
					//Log("\033[31mOnlinePhonebook: Line: " + lines[3] + "\033[0m\n");
					if (persons.length > 0)
					{
						Storage_SetJsonParameter("PhoneBook", number, persons);
						
						//CAll all listeners!
						//....
						
						Log("\033[31mOnlinePhonebook: Found: " + JSON.stringify(persons) + "\033[0m\n");
					}	
				}
				else
				{
					Log("\033[31mOnlinePhonebook: Failed to do name lookup, result was: " + result + "\033[0m\n");
				} 
			}
		); 
		if (persons.length == 0) {
			if (typeof sensorRKSEEK_API === 'undefined') 
			{
			  ;
			}
			else
			{
				url = "rkseek.oblivioncreations.se/?client="+sensorRKSEEK_API+"&n=" + number + "&out=text";
				Log("\033[31mURL: http://"+url+"\033[0m\n");
				Http_Request(url, 
					function(socket_id, result, header, content_data) 
					{
						var persons = new Array();
						if (result.indexOf("200 OK") != -1)
						{
							//Log("\033[31mOnlinePhonebook: result was: " + content_data + "\033[0m\n");
							var lines = content_data.split("\n");
							//Log("\033[31mOnlinePhonebook: Line: " + lines[1] + "\033[0m\n");
							//Log("\033[31mOnlinePhonebook: Line: " + lines[2] + "\033[0m\n");
							//Log("\033[31mOnlinePhonebook: Line: " + lines[3] + "\033[0m\n");
							if (lines[2].length > 0) {
								persons = lines[2];
								Storage_SetJsonParameter("PhoneBook", number, persons);	
								Log("\033[31mOnlinePhonebook: Found: " + JSON.stringify(persons) + "\033[0m\n");
							}
						}
						else
						{
							Log("\033[31mOnlinePhonebook: Failed to do name lookup, result was: " + result + "\033[0m\n");
						} 
					}
				);
			}
		}
   		
   		/*
		Http_Request("wap.hitta.se/default.aspx?Who=" + number + "&Where=&PageAction=White", 
			function(socket_id, result, header, content_data) {
				var persons = new Array();
				if (result.indexOf("200 OK") != -1)
				{
					var endString = "<anchor>Tillbaka<prev/></anchor>";
					var nameStartString = " title=\"Link\">";
					var nameEndString = "</a>";
				
					var lines = content_data.split("<br/>");
					
					for (var n = 1; n < lines.length; n++)
					{
						var line = lines[n].trim(" \n");
						
						if (line.indexOf(endString) != -1)
						{
							break;
						}
						var pos = line.indexOf(nameStartString + (persons.length+1) + ".");
						
						if (pos != -1)
						{
							pos += nameStartString.length + 2
							persons[persons.length] = line.substr(pos, line.length-pos-nameEndString.length).html_entity_decode().replace("  ", " ");
						}
					}
					if (persons.length > 0) {
						Storage_SetJsonParameter("PhoneBook", number, persons);
						
						//CAll all listeners!
						//....
						
						Log("\033[31mOnlinePhonebook: Found: " + JSON.stringify(persons) + "\033[0m\n");
					}
				}
				else
				{
					Log("\033[31mOnlinePhonebook: Failed to do name lookup, result was: " + result + "\033[0m\n");
				} 
			}
		);
		*/
	} else {
		Log("Number already stored\n");
	}
}
Console_RegisterCommand(Sensor_StoreNumberInPhonebook, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args); });



