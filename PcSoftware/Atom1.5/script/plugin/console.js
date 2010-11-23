
function Console() { }

Console.autocomplete_callbacks = new Array();
Console.help_text = new Array();

function Console_DoAutocomplete()
{
	var name = arguments[0];

	//Log("Console_DoAutocomplete(), name=" + name);
	
	var result = new Array();
	
	if (typeof Console.autocomplete_callbacks[name] != 'undefined')
	{
		//Log("calling autocomplete_callback");
		result = Console.autocomplete_callbacks[name](arguments);
	}
	
	var result_string = "";
	
	for (var n = 0; n < result.length; n++)
	{
		result_string += result[n] + "\n";
	}
	
	return result_string;
}

function RegisterConsoleCommand(command, autocomplete_callback)
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
	
	if (Console_RegisterConsoleCommand(name))
	{
		Console.help_text[name] = argument_string;
		
		
		if (autocomplete_callback)
		{
			Console.autocomplete_callbacks[name] = autocomplete_callback;
		}
		
		return true;
	}
	
	return false;
}

function StandardAutocomplete()
{
	// First argument is the command line split
	var command_line_parts = arguments[0];
	
	// command_line_parts[0] == command name
	var arg_index = arguments[0].length - 2;
	
	// Supplied parameters to complete
	var args_to_complete = arguments.length - 1;
	
	if (0 <= arg_index && arg_index < args_to_complete)
	{
		return arguments[arg_index + 1];
	}
	
	return new Array();
}

function help_complete(args)
{
	var result = new Array();
	
	var arg_index = args.length - 1;
	
	if (arg_index == 1) // command name
	{
		for (var n in Console.help_text)
		{
			result.push(n);
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
	
	if (typeof Console.help_text[command_name] != 'undefined')
	{
		var result = "";
		
		if (Console.autocomplete_callbacks[command_name])
		{
			result += "Argument autocompletion enabled for " + command_name + "\n";
		}
		
		result += "Syntax: " + command_name + " " + Console.help_text[command_name] + "\n";
		
		return result;
	}
	
	return "No help for " + command_name + " found\n";
}
RegisterConsoleCommand(help, help_complete);
