
IROut_ModuleNames    = [ "irTransmit", "rfTransceive" ];
IROut_Channels       = function() { return [ 0 ]; };
IROut_Statuses       = function() { return [ "Pressed", "Released", "Burst" ]; };
IROut_Protocols      = function() { return [ "RC5", "RC6", "RCMM", "SIRC", "Sharp", "NEC", "Samsung", "Marantz", "Panasonic", "Sky", "Nexa2", "Nexa" ]; };
IROut_Aliases        = function() { return Module_GetAliasNames(IROut_ModuleNames); };
IROut_AvailableIds   = function() { return Module_GetAvailableIds(IROut_ModuleNames); };

function IROut_StopSend(alias_name)
{
	if (arguments.length < 1)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, IROut_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
		"Channel"  : aliases_data[name]["specific"]["Channel"],
		"Status"   : "Released" };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "IR", variables))
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
Console_RegisterCommand(IROut_StopSend, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, IROut_Aliases()); });

function IROut_StartSendAutoComplete(arg_index, args)
{
	if (arg_index == 0)
	{
		return IROut_Aliases();
	}
	else if (arg_index == 1)
	{
		return get_keys(Storage_GetParameters("RemoteList"));
	}
	else if (arg_index == 2)
	{
		var remote_storage_name = Storage_GetParameter("RemoteList", args[arg_index - 1]);

		return get_keys(Storage_GetParameters(remote_storage_name));
	}
	
	return [];
}

function IROut_Send_Raw(alias_name, protocol, data, status)
{
	if (arguments.length < 4)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, IROut_ModuleNames);
	
	for (var name in aliases_data)
	{
		var variables = {
		"Channel"  : aliases_data[name]["specific"]["Channel"],
		"Protocol" : protocol,
		"IRdata"   : data,
		"Status"   : status };
		Log("will send data to: "+aliases_data[name]["module_name"]+":"+aliases_data[name]["module_id"]+" Data: "+ data + " Prot: "+protocol+" Sts: "+status+"\n");
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "IR", variables))
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
Console_RegisterCommand(IROut_SendBurst, IROut_StartSendAutoComplete);

function IROut_Send(alias_name, remote_name, button_name, status)
{
	if (arguments.length < 4)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var remote_storage_name = Storage_GetParameter("RemoteList", remote_name);
	
	if (!remote_storage_name)
	{
		Log("\033[31mNo such remote name in list, " + remote_name + ".\033[0m\n");
		return false;
	}
	
	var button_string = Storage_GetParameter(remote_storage_name, button_name);
	
	if (!button_string)
	{
		Log("\033[31mNo such button " + button_name + " found on remote " + remote + ".\033[0m\n");
		return false;
	}
	
	var button_data = eval("(" + button_string + ")");
	
	if (typeof button_data["protocol"] == 'undefined' || typeof button_data["data"] == 'undefined')
	{
		Log("\033[31mButton data is corrupt.\033[0m\n");
		return false;
	}
	
	return IROut_Send_Raw(alias_name, button_data["protocol"], button_data["data"], status);
}

function IROut_StartSend(alias_name, remote_name, button_name)
{
	return IROut_Send(alias_name, remote_name, button_name, "Pressed");
}
Console_RegisterCommand(IROut_StartSend, IROut_StartSendAutoComplete);

function IROut_SendBurst(alias_name, remote_name, button_name)
{
	return IROut_Send(alias_name, remote_name, button_name, "Burst");
}
Console_RegisterCommand(IROut_SendBurst, IROut_StartSendAutoComplete);

function IROut_SendBurst_Raw(alias_name, protocol, data)
{
	return IROut_Send_Raw(alias_name, protocol, data, "Burst");
}
Console_RegisterCommand(IROut_SendBurst_Raw, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, IROut_Aliases(), IROut_Protocols()); });

function IROut_SendDimmer(alias_name, remote_name, button_name, level)
{
	if (arguments.length < 4)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var remote_storage_name = Storage_GetParameter("RemoteList", remote_name);
	
	if (!remote_storage_name)
	{
		Log("\033[31mNo such remote name in list, " + remote_name + ".\033[0m\n");
		return false;
	}
	
	var button_string = Storage_GetParameter(remote_storage_name, button_name);
	
	if (!button_string)
	{
		Log("\033[31mNo such button " + button_name + " found on remote " + remote_name + ".\033[0m\n");
		return false;
	}
	
	var button_data = eval("(" + button_string + ")");
	
	if (typeof button_data["protocol"] == 'undefined' || typeof button_data["data"] == 'undefined')
	{
		Log("\033[31mButton data is corrupt.\033[0m\n");
		return false;
	}	
	levelInt = parseInt(level);
	dataInt = parseInt(button_data["data"]);
	if (levelInt  >= 0 && levelInt <= 15) {
		switch(levelInt) {
		  case 0:
		    levelInt = 15;
		    break;
		  case 1:
		    levelInt = 7;
		    break;
		  case 2:
		    levelInt = 11;
		    break;
		  case 3:
		    levelInt = 3;
		    break;
		  case 4:
		    levelInt = 13;
		    break;
		  case 5:
		    levelInt = 5;
		    break;
		  case 6:
		    levelInt = 9;
		    break;
		  case 7:
		    levelInt = 1;
		    break;
		  case 8:
		    levelInt = 14;
		    break;
		  case 9:
		    levelInt = 6;
		    break;
		  case 10:
		    levelInt = 10;
		    break;
		  case 11:
		    levelInt = 2;
		    break;
		  case 12:
		    levelInt = 12;
		    break;
		  case 13:
		    levelInt = 4;
		    break;
		  case 14:
		    levelInt = 8;
		    break;
		  case 15:
		    levelInt = 0;
		    break;
		}
		dataInt += 549755813888; //Set MSB
		dataInt += levelInt*4294967296//Add dimmer level
		Log("button_data[protocol]=" + button_data["protocol"] + "\n");
		return IROut_Send_Raw(alias_name, button_data["protocol"], dataInt.toString(), "Burst");	
	} else {
		Log("\033[31mDimmer level out of range [0 -> 16].\033[0m\n");
		return false;
	}
}
Console_RegisterCommand(IROut_SendDimmer, IROut_StartSendAutoComplete);



function IROut_SendDuration(alias_name, remote_name, button_name, duration_ms)
{
	IROut_Send(alias_name, remote_name, button_name, "Pressed");
	Timer_SetTimer(function(timer) {IROut_StopSend(alias_name);}, duration_ms, false);
}
Console_RegisterCommand(IROut_SendDuration, IROut_StartSendAutoComplete);

