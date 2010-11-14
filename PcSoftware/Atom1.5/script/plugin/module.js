
function Module_OnChange(full_id, available)
{
	Log(full_id + " is now " + (available ? "available" : "unavailable"));
}

function Module_OnMessage()
{
	var full_id = arguments[0];
	var command = arguments[1];
	var number_of_variables = arguments[2];
	
	var variables = new Array();
	
	for (var i = 3; i < number_of_variables * 2 + 3; i += 2)
	{
		//Log(arguments[i] + " = " + arguments[i + 1]);
		variables[arguments[i]] = arguments[i + 1];
	}
	
	variables.length = number_of_variables;
	
	//Log(full_id + " got message: " + command + " with " + variables.length + " arguments");
}

function SendModuleMessage(full_id, command, variables)
{
	var args = new Array();
	
	args[args.length] = full_id;
	args[args.length] = command;
	args[args.length] = variables.length;
	
	for (var n in variables)
	{
		args[args.length] = n;
		args[args.length] = variables[n];
	}
	
	Module_SendMessage.apply(null, Array.prototype.slice.call(args, 0));
}
