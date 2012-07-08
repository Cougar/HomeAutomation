
IRIn_ModuleNames    = [ "irReceive", "rfTransceive" ];
IRIn_Channels       = function() { return [ 0 ]; };
IRIn_Aliases        = function() { return Module_GetAliasNames(IRIn_ModuleNames); };
IRIn_AvailableIds   = function() { return Module_GetAvailableIds(IRIn_ModuleNames); };
IRIn_Remotes        = function() { return get_keys(Storage_GetParameters("RemoteList")); };

IRIn_RecordRemoteName = null;
IRIn_RecordAliasData = null;
IRIn_RecordLast = null;
IRIn_RecordClientId = null;
IRIn_OnMessageFunctions = null;

for (var i=0; i<IRIn_Aliases().length; i++)
{
	/* Register to messages from all ir aliases */
	Module_RegisterToOnMessage(IRIn_Aliases()[i], IRIn_IrMessage);
}
function IRIn_IrMessage(alias_name, command, variables)
{
	/* First check if there are any callbacks registered */
	if (IRIn_OnMessageFunctions == null)
	{
		return false;
	}
	/* Also check if there are any callbacks registered to this alias */
	if (!IRIn_OnMessageFunctions[alias_name])
	{
		return false;
	}
	/* Check if code and protocol maches one or more remote */
	var remotes = get_keys(Storage_GetParameters("RemoteList"));
	for (var i=0; i < remotes.length; i++)
	{
		var buttons = get_keys(Storage_GetParameters("Remote_"+remotes[i]));
		for (var j=0; j < buttons.length; j++)
		{
			var button_string = Storage_GetParameter("Remote_"+remotes[i], buttons[j]);
			var button_data = eval("(" + button_string + ")");
	
			if (typeof button_data["protocol"] == 'undefined' || typeof button_data["data"] == 'undefined')
			{
				Log("\033[31mButton data is corrupt.\033[0m\n");
				return false;
			}
			
			if (button_data["protocol"] == variables["Protocol"] && button_data["data"] == variables["IRdata"])
			{
				//Log("Found match: "+remotes[i]+" "+buttons[j]+" "+button_data["protocol"]+" "+button_data["data"]+"");
				if (IRIn_OnMessageFunctions[alias_name])
				{
					for (var n in IRIn_OnMessageFunctions[alias_name])
					{
						//Log("Found callback, calling");
						/* Call callback with arguments remotes[i] buttons[j] and variables["Status"]*/
						IRIn_OnMessageFunctions[alias_name][n](alias_name, remotes[i], buttons[j], variables["Status"]);
					}
				}
			}
		}
	}
	return true;
}

function IRIn_RegisterToMessage(alias_name, callback)
{
	if (IRIn_OnMessageFunctions == null)
	{
		IRIn_OnMessageFunctions = new Array();
	}
	
	if (!IRIn_OnMessageFunctions[alias_name])
	{
		IRIn_OnMessageFunctions[alias_name] = new Array();
	}
	
	IRIn_OnMessageFunctions[alias_name].push(callback);
}

function IRIn_CodeToName(data, protocol, remote_name)
{
	var remote_storage_name = Storage_GetParameter("RemoteList", remote_name);

        if (!remote_storage_name)
        {
		Log("No such remote " + remote_name);
                return "";
        }

	var buttons = Storage_GetParameters(remote_storage_name);

	for (var button_name in buttons)
	{
		var button_data = eval("(" + buttons[button_name]+ ")");

		Log(buttons[button_name]);

		if (button_data["data"] == (data + '') && button_data["protocol"] == protocol)
		{
			return button_name;
		}
	}

	Log("Found no matching button on remote " + remote_name);
	return "";
}

function IRIn_Record(alias_name, remote_name)
{
	if (arguments.length < 2)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	IRIn_RecordRemoteName = Storage_GetParameter("RemoteList", remote_name);
	
	if (!IRIn_RecordRemoteName)
	{
		IRIn_RecordRemoteName = "Remote_" + remote_name;
		
		Log("\033[32mNo such remote name in list, " + remote_name + ", creating...\033[0m\n");
		
		Storage_SetParameter("RemoteList", remote_name, IRIn_RecordRemoteName);
	}
	
	var aliases_data = Module_ResolveAlias(alias_name, IRIn_ModuleNames);
	
	for (var name in aliases_data)
	{
		if (aliases_data[name]["group"])
		{
			Log("\033[31mAlias must not be an alias group.\033[0m\n");
			return false;
		}
		
		IRIn_RecordClientId = Console_GetClientId();
		
		IRIn_RecordAliasData = aliases_data[name];
		Log("To finish press a random key on the remote and then enter dot as name.\n");
		
		Log("\033[29mPress a button on you remote...\033[0m\n");
		Console_PreventDefaultPrompt();
	
		Module_RegisterToOnMessage(alias_name, IRIn_RecordOnIrMessage);
		return true;
	}
	
	Log("\033[31mNo such alias name found, " + alias_name + ".\033[0m\n");
	return false;
	
}
Console_RegisterCommand(IRIn_Record, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, IRIn_Aliases(), IRIn_Remotes()); });

function IRIn_RecordOnResponse(response)
{
	if (response == ".")
	{
		IRIn_RecordClientId = null;
		return true;
	}
	
	Storage_SetParameter(IRIn_RecordRemoteName, response, JSON.stringify(IRIn_RecordLast));
	IRIn_RecordLast = null;
	
	Log("\033[29mPress a button on you remote...\033[0m\n");
	Console_PreventDefaultPrompt();
	
	return true;
}

function IRIn_RecordOnIrMessage(alias_name, command, variables)
{
	if (variables["Status"] != "Pressed" || IRIn_RecordLast != null)
	{
		return;
	}
	
	ConsoleExport_LogToClient(IRIn_RecordClientId, "\033[32mGot code " + variables["IRdata"] + " on protocol " + variables["Protocol"] + ".\033[0m\n");
	
	IRIn_RecordLast = { "protocol" : variables["Protocol"], "data" : variables["IRdata"] };
	
	Console_PromptRequest("\033[29mEnter name: \033[0m", IRIn_RecordOnResponse);
	
	return true;
}
