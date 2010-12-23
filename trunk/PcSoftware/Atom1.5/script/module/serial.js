
Serial_ModuleNames    = [ "Serial" ];
Serial_Aliases        = function() { return Module_GetAliasNames(Serial_ModuleNames); };
Serial_AvailableIds   = function() { return Module_GetAvailableIds(Serial_ModuleNames); };

function Serial_Configure(alias_name)
{
	if (arguments.length < 1)
	{
		Log("\033[31;1mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, Serial_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
		"BaudRate"       : 9600,
		"PhysicalFormat" : "RS232",
		"DataBits"       : 8,
		"StopBits"       : 1,
		"Parity"         : "ParityNone" };
			
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "SerialConfig", variables))
		{
			Log("\033[32;1mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31;1mFailed to send command to " + name + ".\033[0m\n");
		}
		
		found = true;
	}
	
	if (!found)
	{
		Log("\033[31;1mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	
	return true;
}
Console_RegisterCommand(Serial_Configure, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Serial_Aliases()); });

function Serial_Send(alias_name)
{
	if (arguments.length < 1)
	{
		Log("\033[31;1mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var data = "";
	
	for (var n = 1; n < arguments.length; n++)
	{
	  data += arguments[n] + " ";
	}
	
	data = data.trim(" ") + "\r";
	
	var aliases_data = Module_ResolveAlias(alias_name, Serial_ModuleNames);
	var found = false;
	
	for (var name in aliases_data)
	{
		var variables = {
		"Text"  : data };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "SerialData", variables))
		{
			Log("\033[32;1mCommand sent successfully to " + name + ".\033[0m\n");
		}
		else
		{
			Log("\033[31;1mFailed to send command to " + name + ".\033[0m\n");
		}
		
		found = true;
	}
	
	if (!found)
	{
		Log("\033[31;1mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	
	return true;
}
Console_RegisterCommand(Serial_Send, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Serial_Aliases()); });

