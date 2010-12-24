
function Storage_ListParameters(store_name)
{
	if (arguments.length < 1)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var parameters = StorageExport_GetParameters(store_name);
	
	var result_text = "";
	
	for (var key in parameters)
	{
		result_text += key + "=" + parameters[key] + "\n";
	}
	
	if (result_text.length == 0)
	{
		Log("033[31mNo parameters found.\033[0m\n");
		return false;
	}
	
	Log(result_text);
	return true;
}
Console_RegisterCommand(Storage_ListParameters);

function Storage_GetParameter(store_name, parameter_name)
{
	return StorageExport_GetParameter(store_name, parameter_name);
}
Console_RegisterCommand(Storage_GetParameter);

function Storage_SetParameter(store_name, parameter_name, parameter_value)
{
	return StorageExport_SetParameter(store_name, parameter_name, parameter_value);
}
Console_RegisterCommand(Storage_SetParameter);

function Storage_GetParameters(store_name)
{
	return StorageExport_GetParameters(store_name);
}
