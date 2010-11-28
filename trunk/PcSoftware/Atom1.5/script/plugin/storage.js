
function listparameters(store_name)
{
	var parameters = Storage_GetParameters(store_name);
	var result = "";
	
	for (var key in parameters)
	{
		result += key + "=" + parameters[key] + "\n";
	}
	
	return result;
}
RegisterConsoleCommand(listparameters);


function getparameter(store_name, parameter_name)
{
	return Storage_GetParameter(store_name, parameter_name);
}
RegisterConsoleCommand(getparameter);


function setparameter(store_name, parameter_name, parameter_value)
{
	return Storage_SetParameter(store_name, parameter_name, parameter_value);
}
RegisterConsoleCommand(setparameter);

LoadScript("common/alias.js");
