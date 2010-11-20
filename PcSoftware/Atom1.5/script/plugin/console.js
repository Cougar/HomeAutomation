
function Console() { }

Console.autocomplete_callbacks = new Array();

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

function RegisterConsoleCommand(name, autocomplete_callback)
{
	if (Console_RegisterConsoleCommand(name))
	{
		if (autocomplete_callback)
		{
			Console.autocomplete_callbacks[name] = autocomplete_callback;
		}
		
		return true;
	}
	
	return false;
}
