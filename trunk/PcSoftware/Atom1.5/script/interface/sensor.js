
Sensor_ModuleNames    = [ "DS18x20", "FOST02", "BusVoltage", "SimpleDTMF", "DHT11" ];
SensorRf_ModuleNames    = [ "rfTransceive" ];
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

Sensor_OnNewPhonenumberFunctions = [];

function Sensor_RegisterToNewPhonenumber(alias_name, callback)
{
  if (!Sensor_OnNewPhonenumberFunctions[alias_name])
  {
    Sensor_OnNewPhonenumberFunctions[alias_name] = [];
  }

  Sensor_OnNewPhonenumberFunctions[alias_name].push(callback);
}

Sensor_OnDeviceStateChangeFunctions = [];

function Sensor_RegisterToDeviceStateChange(alias_name, callback)
{
  if (!Sensor_OnDeviceStateChangeFunctions[alias_name])
  {
    Sensor_OnDeviceStateChangeFunctions[alias_name] = [];
  }

  Sensor_OnDeviceStateChangeFunctions[alias_name].push(callback);
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
          var number = variables["Number"];
          var incommingCall = true;
          var checkName = true;

          Log("received from DTMF:\"" + number + "\"");

          if (variables["Number"].length >= 6)
          {
            /* A seems to mean it is an incomming call! */
            if (number.charAt(0) === 'A' || number.charAt(0) === 'D')
            {
              incommingCall = true;

              /* Remove leading A */
              number = number.substring(1);
            }
            else /* Outgoing call */
            {
              incommingCall = false;
            }

            /* A number can have a trailing C */
            var endIndex = number.indexOf('C');

            if (endIndex !== -1)
            {
              /* Remove trailing C */
              number = number.substring(0, endIndex);
            }
          }
          else if (number == "B00C")
          {
            /* Unknown number */
            number = "Unknown";
            incommingCall = true;
            checkName = false;
          }
          else if (number == "B10C")
          {
            /* Secret/protected number */
            number = "Protected";
            incommingCall = true;
            checkName = false;
          }
          else
          {
            Log("DTMF decoded number is to short, number: \"" + number + "\", length: " + number.length);
            return;
          }
          
          /* Adding area code prefix if needed */
          if (number.charAt(0) != '0' && checkName)
          {
            if (typeof sensorDefaultPhoneAreaCode === 'undefined')
            {
              Log("\033[31mOnlinePhonebook: please add sensorDefaultPhoneAreaCode = yourAreaCode to autostart.js, defaulting to 031\033[0m\n");
              number = "031" + number;
            }
            else
            {
                number = sensorDefaultPhoneAreaCode + number;
            }
          }

          /* Store number and direction in last values */
          var lastValue = {};
          var lastValueString = Storage_GetParameter("LastValues", alias_name);

          if (lastValueString)
          {
            lastValue = JSON.parse(lastValueString);
          }

          var timestamp = get_time();

          lastValue["Phonenumber"] = { "value" : number, "timestamp" : timestamp };
          lastValue["Direction"] = { "value" : incommingCall ? "in" : "out", "timestamp" : timestamp };

          Storage_SetParameter("LastValues", alias_name, JSON.stringify(lastValue));


          /* Store number in phone call history */
          var newValue = { "number" : number, "direction" : incommingCall ? "in" : "out", "module" : alias_name, "names" : [], "timestamp" : timestamp };

          Storage_SetParameter("PhoneCalls", timestamp, JSON.stringify(newValue));


          /* Log the number */
          Log("New number: " + number + ", direction: " + (incommingCall ? "in" : "out"));
          
          /* Lookup name */
          if (checkName)
          {
            Sensor_StoreNumberInPhonebook(number, timestamp);
          }

          var phonebookNumbers = Storage_GetJsonParamter("PhoneBook", number);
          if (!phonebookNumbers)
          {
            phonebookNumbers = [];
          }
          
          /* Call any potential subscribers */
          if (Sensor_OnNewPhonenumberFunctions[alias_name])
          {
            for (var n in Sensor_OnNewPhonenumberFunctions[alias_name])
            {
              /* Call callback with arguments alias, number, direction */
              Sensor_OnNewPhonenumberFunctions[alias_name][n](alias_name, number, incommingCall ? "in" : "out", phonebookNumbers);
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

function SensorRf_OnMessage(module_name, module_id, command, variables)
{
	if (in_array(SensorRf_ModuleNames, module_name))
	{
		var aliases_data = Module_LookupAliases({
		"module_name" : module_name,
		"module_id"   : module_id,
		"group"       : false	});

		/* Must check sensor here in case no alias is stored the adress needs to be printed */
		var VikingSuccess = "NoVikingSensor";
		if (variables["Protocol"] == "Viking")
		{
			/*
							  ??? aaaaaaaa sttttttttttt hhhhhhhh cccccccc
			341731651126	0b100 11111001 000011001011 00101110 00110110	20.3 46%
			a = address, s = sign, t = temperature, h = humidity, c = crc
			*/
			var data = variables["IRdata"];
			var crc = data&0xFF;
			var humidity = (data>>>8)&0xFF;
			var temp = (data>>>16)&0xFFF;
			var addr = rshift(data,28)&0xFF;
			var unknown = rshift(data,36)&0xF;
			var calccrc=crc8(rshift(data,8), 32);

			var sign = (temp>>>11)&1;
			temp = temp&0x7FF;
			if (sign > 0)
			{
				temp = -temp;
			}
			temp = temp/10;

			VikingSuccess = "VikingNotFound";
			
			if (unknown != 4)
			{
				Log("\033[33mWarning: VikingSensor, unknown part="+unknown+", data="+data+"\033[0m");
			}
		}
				
		loopAliases:	/* Label to break nested */
		for (var alias_name in aliases_data)
		{
			if (aliases_data[alias_name]["specific"]["Channel"] == variables["Channel"] &&
				aliases_data[alias_name]["specific"]["Proto"] == variables["Protocol"])
			{
				switch (variables["Protocol"])
				{
					case "Viking":
					{	/* Add alias with these specifics: Channel=0,Proto=Viking,Address=<address of sensor> */
						if (crc != calccrc)
						{
							Log("\033[31mError: VikingSensor, incorrect CRC, data="+data+", CRC="+crc+", calcCRC="+calccrc+"\033[0m");
							break loopAliases;
						}
						if (addr == aliases_data[alias_name]["specific"]["Address"])
						{
							VikingSuccess = "VikingFound";

							var last_value = {};
							var last_value_string = Storage_GetParameter("LastValues", alias_name);

							if (last_value_string)
							{
								last_value = eval("(" + last_value_string + ")");
							}

							var timestamp = get_time();
							last_value["Temperature_Celsius"] = { "value" : temp.toString(), "timestamp" : timestamp };
							last_value["Humidity_Percent"] = { "value" : humidity.toString(), "timestamp" : timestamp };

							Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
							
							break loopAliases;
						}
						break;
					}
					case "Nexa2":
					{	/* Add alias with these specifics: Channel=0,Proto=Nexa2,OnData=<data for on>,OffData=<data for off> */
						var deviceStateCommand = "";
						if (variables["IRdata"] == aliases_data[alias_name]["specific"]["OnData"] && variables["Status"] == "Pressed")
						{
							deviceStateCommand = "On";
						}
						else if (variables["IRdata"] == aliases_data[alias_name]["specific"]["OffData"] && variables["Status"] == "Pressed")
						{
							deviceStateCommand = "Off";
						}
						else
						{
							break;
						}
						
						var last_value = {};
						var last_value_string = Storage_GetParameter("LastValues", alias_name);

						if (last_value_string)
						{
							last_value = eval("(" + last_value_string + ")");
						}
						
						if (( typeof(last_value["State"]) == "undefined" ) || 
							( deviceStateCommand == "Off" && last_value["State"]["value"] == "On" ) ||
							( deviceStateCommand == "On" && last_value["State"]["value"] == "Off" ))
						{
							last_value["State"] = { "value" : deviceStateCommand, "timestamp" : get_time() };

							Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));

							/* Call any potential subscribers */
							if (Sensor_OnDeviceStateChangeFunctions[alias_name])
							{
								for (var n in Sensor_OnDeviceStateChangeFunctions[alias_name])
								{
									/* Call callback with arguments alias, state */
									Sensor_OnDeviceStateChangeFunctions[alias_name][n](alias_name, deviceStateCommand);
								}
							}
						}
						break;
					}
				}
			}
		}
		if (VikingSuccess == "VikingNotFound")
		{
			Log("Found unknown vikingsensor address="+addr+" temperature="+temp+" humidity="+humidity+" unknown="+unknown);
		}
	}
}
Module_RegisterToOnMessage("all", SensorRf_OnMessage);

function Sensor_StoreNumberInPhonebook(number, timestamp)
{
  var phonebookNumbers = Storage_GetJsonParamter("PhoneBook", number);

  if (!phonebookNumbers)
  {
    phonebookNumbers = [];

    url = "wap.eniro.se/query?search_word=" + number + "&what=mobwp";
    //Log("\033[31mURL: http://"+url+"\033[0m\n");

    Http_Request(url, function(socket_id, result, header, content_data)
    {
      //Log("\033[31mOnlinePhonebook-eniro: result was: " + result + "\033[0m\n");
      //Log("\033[31mOnlinePhonebook-eniro: content was: " + content_data + "\033[0m\n");

      if (result.indexOf("200 OK") != -1)
      {

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
            line = lines[n + 1].trim("\n");
            splitted = line.split("b>");

            phonebookNumbers.push(splitted[1].substr(0, splitted[1].length - 2));
            break;
          }
        }

        //Log("\033[31mOnlinePhonebook: Line: " + lines[1] + "\033[0m\n");
        //Log("\033[31mOnlinePhonebook: Line: " + lines[2] + "\033[0m\n");
        //Log("\033[31mOnlinePhonebook: Line: " + lines[3] + "\033[0m\n");
        if (phonebookNumbers.length > 0)
        {
          Storage_SetJsonParameter("PhoneBook", number, phonebookNumbers);

          Sensor_UpdateNameInPhonecalls(timestamp, phonebookNumbers);

          Log("\033[31mOnlinePhonebook-eniro: Found: " + JSON.stringify(phonebookNumbers) + "\033[0m\n");
        }
      }
      else
      {
        Log("\033[31mOnlinePhonebook-eniro: Failed to do name lookup, result was: " + result + "\033[0m\n");
      }

      if (phonebookNumbers.length == 0)
      {
        if (typeof sensorRKSEEK_API !== 'undefined')
        {
          url = "rkseek.oblivioncreations.se/?client="+sensorRKSEEK_API+"&n=" + number + "&out=text";
          Log("\033[31mURL: http://"+url+"\033[0m\n");

          Http_Request(url, function(socket_id, result, header, content_data)
          {
            phonebookNumbers = [];

            if (result.indexOf("200 OK") != -1)
            {
              //Log("\033[31mOnlinePhonebook: result was: " + content_data + "\033[0m\n");
              var lines = content_data.split("\n");
              //Log("\033[31mOnlinePhonebook: Line: " + lines[1] + "\033[0m\n");
              //Log("\033[31mOnlinePhonebook: Line: " + lines[2] + "\033[0m\n");
              //Log("\033[31mOnlinePhonebook: Line: " + lines[3] + "\033[0m\n");
              if (lines[2].length > 0)
              {
                phonebookNumbers = [ lines[2] ]; // TODO Is this really correct?!
                Storage_SetJsonParameter("PhoneBook", number, phonebookNumbers);
                
                Sensor_UpdateNameInPhonecalls(timestamp, phonebookNumbers);

                Log("\033[31mOnlinePhonebook-rseek: Found: " + JSON.stringify(phonebookNumbers) + "\033[0m\n");
              }
            }
            else
            {
              Log("\033[31mOnlinePhonebook-rseek: Failed to do name lookup, result was: " + result + "\033[0m\n");
            }
          });
        }
      }
    });
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

  }
  else
  {
    //Log("Number already stored\n");

    Sensor_UpdateNameInPhonecalls(timestamp, phonebookNumbers);
  }
  
  return phonebookNumbers;
}
Console_RegisterCommand(Sensor_StoreNumberInPhonebook, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args); });

function Sensor_StoreNumberInPhonebookManual(number, name)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var completename = arguments[1];
	for (var i=2; i<arguments.length; i++)
	{
		completename = completename+" "+arguments[i];
	}
	
	phonebookNumbers = [];
	phonebookNumbers.push(completename);
	Storage_SetJsonParameter("PhoneBook", number, phonebookNumbers);
}
Console_RegisterCommand(Sensor_StoreNumberInPhonebookManual, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args); });

function Sensor_UpdateNameInPhonecalls(timestamp, numbers)
{
  if (numbers.length > 0)
  {
    var lastValueString = Storage_GetParameter("PhoneCalls", timestamp);

    if (lastValueString)
    {
      var lastValue = JSON.parse(lastValueString);

      lastValue.names = numbers;

      Storage_SetParameter("PhoneCalls", timestamp, JSON.stringify(lastValue));
    }
  }
}

