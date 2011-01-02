
Console_Functions = {};
Console_CurrentFunction = null;
Console_DefaultPrompt = "\033[29;1m[atomic] \033[0m";
Console_LastClientId = null;

function Console_GetFunctionNames()
{
	return get_keys(Console_Functions);
}

// C++ callable functions

function Console_AutocompleteRequest(client_id, arg_index, commandline)
{
	Console_LastClientId = client_id;
	
	var result = [];
	var parts = [];
	var name = "Console_Shell";
	
	if (commandline.length > 0 && arg_index != 0)
	{
		parts = commandline.split(" ");
		name = parts.shift();
		arg_index--;
	}

	if (Console_Functions[name] && Console_Functions[name]["autocomplete"])
	{
		//Log("calling autocomplete_callback: " + name + "\n");
		result = Console_Functions[name]["autocomplete"](arg_index, parts);
	}
	
	var result_string = result.join("\n");
	
	ConsoleExport_AutoCompleteResponse(Console_LastClientId, result_string);
	return true;
}

function Console_PromptResponse(client_id, response)
{
	Console_LastClientId = client_id;
	
	var parts = array_remove_empty(response.split(" "));

	var current_function = Console_CurrentFunction;
	Console_CurrentFunction = Console_Shell;
	
	var result = current_function.apply(null, Array.prototype.slice.call(parts, 0));
	
	if (Console_CurrentFunction == Console_Shell)
	{
		Console_SetDefaultPrompt();
	}
	
	return result;
}

function Console_NewConnection(client_id)
{
	Console_LastClientId = client_id;
	
	return Console_SetDefaultPrompt();
}

// User script functions

function Console_GetClientId()
{
	return Console_LastClientId;
}

function Console_LogToClient(client_id, text)
{
	if (client_id)
	{
		return ConsoleExport_LogToClient(client_id, text);
	}
	
	return false;
}

function Console_PreventDefaultPrompt()
{
	Console_CurrentFunction = null;
}

function Console_SetDefaultPrompt()
{
	return Console_PromptRequest(Console_DefaultPrompt, Console_Shell);
}

function Console_PromptRequest(prompt, callback)
{
	Console_CurrentFunction = callback;
	return ConsoleExport_PromptRequest(Console_LastClientId, prompt);
}

function Console_RegisterCommand(command, autocomplete_callback)
{
	var command_string = command.toString();

	var pos_end = command_string.indexOf("\n") - 1;
	var pos_start = command_string.indexOf(" ") + 1;
	
	var full_name = command_string.substr(pos_start, pos_end - pos_start);
	
	var pos_split = full_name.indexOf("(");
	
	var name = full_name.substr(0, pos_split);
	
	var argument_string = full_name.substr(pos_split + 1);
	
	if (argument_string.length > 0)
	{
		argument_string = argument_string.replace(/,/g, "");
		argument_string = "<" + argument_string.replace(/ /g, "> <") + ">";
	}
	
	Console_Functions[name] = { "command" : command, "autocomplete" : autocomplete_callback, "help" : argument_string };
	return true;
}

function Console_StandardAutocomplete()
{
	var arg_index = arguments[0];
	var args = arguments[1];
	
	if (0 <= arg_index && arg_index < arguments.length - 2)
	{
		return arguments[arg_index + 2];
	}
	
	return [];
}

// Default call function

function Console_Shell()
{
	var args = to_array(arguments);
	var name = args.shift();
	
	if (Console_Functions[name] && Console_Functions[name]["command"])
	{
		return Console_Functions[name]["command"].apply(null, Array.prototype.slice.call(args, 0));
	}
	else
	{
		Log("\033[31mNo such command found, " + name + ".\033[0m\n");
	}
	
	return false;
}
Console_RegisterCommand(Console_Shell, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Console_GetFunctionNames()); });

// Help functions

function help_complete(arg_index, args)
{
	var result = [];
	
	if (arg_index == 1) // command name
	{
		for (var name in Console_Functions)
		{
			result.push(name);
		}
	}
	
	return result;
}

function help(command_name)
{
	if (!command_name)
	{
		command_name = "help";
	}
   
	if (Console_Functions[command_name] && Console_Functions[command_name]["help"])
	{
		var result = "";
 
		if (Console_Functions[command_name] && Console_Functions[command_name]["autocomplete"])
		{
			result += "Argument autocompletion enabled for " + command_name + "\n";
		}
 
		result += "Syntax: " + command_name + " " + Console_Functions[command_name]["help"] + "\n";
 
		Log(result);
		return true;
	}
 
	Log("\033[31mNo help for " + command_name + " found.\033[0m\n");
	return false;
}
Console_RegisterCommand(help, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Console_GetFunctionNames()); });


function quit()
{
    Log("Goodbye!\n");
    
    ConsoleExport_DisconnectClient(Console_LastClientId);
    Console_LastClientId = null;
	return false;
}
Console_RegisterCommand(quit);
