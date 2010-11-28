
function createalias(alias, module_name, module_id, extra)
{
	var data = { "is_group" : false, "module_name" : module_name, "module_id" : module_id, "extra" : extra };
	
	return Storage_SetParameter("alias", alias, JSON.stringify(data));
}
RegisterConsoleCommand(createalias);

function creategroupalias(alias, aliases)
{
	var data = { "is_group" : true, "aliases" : aliases.split(",") };
	
	return Storage_SetParameter("alias", alias, JSON.stringify(data));
}
RegisterConsoleCommand(creategroupalias);

function getaliases()
{
	var alias_list = new Array();
	var parameters = Storage_GetParameters("alias");
	
	for (var key in parameters)
	{
		alias_list[key] = eval("(" + parameters[key] + ")");
	}
	
	return alias_list;
}

function resolvealias(alias)
{
	var alias_data = Storage_GetParameter("alias", alias);
	
	if (alias_data)
	{
		return eval("(" + alias_data + ")");
	}
	
	return null;
}
