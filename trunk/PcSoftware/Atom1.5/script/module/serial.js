
Serial_ModuleName     = "Serial";
Serial_Aliases        = function() { return Module_GetAliasNames([ Serial_ModuleName ]); };
Serial_AvailableIds   = function() { return Module_GetAvailableIds([ Serial_ModuleName ]); };

function Serial_MessageAliasLookup(module_id, command, variables)
{
	var aliases_data = {};
	
	aliases_data = Module_LookupAliases({
		"module_name" : Serial_ModuleName,
		"module_id"   : module_id
	});
	
	return aliases_data;
}
Module_RegisterMessageAliasLookup(Serial_ModuleName, Serial_MessageAliasLookup);

function Serial_CreateAlias(alias_name, module_id, channel)
{
	var alias_data = {
		"group"       : false,
		"name"        : alias_name,
		"module_name" : Serial_ModuleName,
		"module_id"   : module_id
	};
	
	return Storage_SetParameter("alias", alias_name, JSON.stringify(alias_data));
}
Console_RegisterCommand(Serial_CreateAlias, function(args) { return Console_StandardAutocomplete(args, Serial_Aliases(), Serial_AvailableIds()); });


function Serial_Configure(alias_name)
{
	if (arguments.length < 1)
	{
		throw("Not enought parameters given");
	}
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, [ Serial_ModuleName ]);
	
	for (var name in aliases_data)
	{
		var variables = {
			"BaudRate"  : "9600",
			"PhysicalFormat" : "RS232",
			"DataBits"   : "8",
			"StopBits"   : "1",
			"Parity"     : "ParityNone"
		};
			
			if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "SerialConfig", variables))
			{
				result_text += "Configure sent successfully to " + name + "\n";
			}
			else
			{
				result_text += "Configure failed to send to " + name + "\n";
			}
	}
	
	if (result_text.length == 0)
	{
		result_text = "No aliases by the name " + alias_name + " were applicable to Configure.";
	}
	
	return result_text;
}
Console_RegisterCommand(Serial_Configure, function(args) { return Console_StandardAutocomplete(args, Serial_Aliases()); });

function Serial_Send(alias_name)
{
	if (arguments.length < 1)
	{
		throw("Not enought parameters given");
	}
	
	var data = "";
	
	for (var n = 1; n < arguments.length; n++)
	{
	  data += arguments[n] + " ";
	}
	
	data = data.trim(" ") + "\r";
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, [ Serial_ModuleName ]);
	
	for (var name in aliases_data)
	{
		var variables = {
			"Text"  : data
		};
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "SerialData", variables))
		{
			result_text += "Sent \"" + data + "\" successfully to " + name + "\n";
		}
		else
		{
			result_text += "Send failed to send to " + name + "\n";
		}
	}
	
	if (result_text.length == 0)
	{
		result_text = "No aliases by the name " + alias_name + " were applicable to Send.";
	}
	
	return result_text;
}
Console_RegisterCommand(Serial_Send, function(args) { return Console_StandardAutocomplete(args, Serial_Aliases()); });

