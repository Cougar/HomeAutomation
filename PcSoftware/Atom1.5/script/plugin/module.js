
Require("plugin/storage.js");

function Module_CreateAliasGroupAutoComplete(args)
{
	// args[0] == command name
	var arg_index = args.length - 2;
	
	if (arg_index == 0)
	{
		return Module_GetAliasNames();
	}
	else if (arg_index == 1)
	{
		return [ "Dimmer230", "irTransmit" ];
	}
	else if (arg_index >= 2)
	{
		return Module_GetAliasNames([ args[2] ])
	}
	
	return [];
}

function Module_CreateAliasGroup(alias_group_name, module_type, alias_name1, alias_name2)
{
	if (arguments.length < 3)
	{
		throw("Not enought parameters given");
	}
	
	alias_group_name = arguments[0];
	module_type = arguments[1];
	var aliases = [];
	
	for (var n = 2; n < arguments.length; n++)
	{
		if (arguments[n].length > 0)
		{
			var aliases_data = Module_ResolveAlias(arguments[n]);
			aliases.push(arguments[n]);
		}
	}
	
	if (aliases.length == 0)
	{
		throw("No valid aliases were found, aborting creation of group alias");
	}
	
	var alias_data = {
		"group"       : true,
		"name"        : alias_group_name,
		"module_name" : module_type,
		"aliases"     : aliases
	};
	
	return Storage_SetParameter("alias", alias_group_name, JSON.stringify(alias_data));
}
Console_RegisterCommand(Module_CreateAliasGroup, Module_CreateAliasGroupAutoComplete);

function Module_ResolveAlias(alias_name, filter_module_names)
{
	var aliases_data = {};
	var alias_data_string = Storage_GetParameter("alias", alias_name);
	
	if (alias_data_string)
	{
		var alias_data = eval("(" + alias_data_string + ")");
		
		if (alias_data["group"])
		{
			for (var n in alias_data["aliases"])
			{
				var aliases_data_sub = Module_ResolveAlias(alias_data["aliases"][n], filter_module_names);
				
				for (var name in aliases_data_sub)
				{
					aliases_data[name] = aliases_data_sub[name];
				}
			}
		}
		else
		{
			if (in_array(filter_module_names, alias_data["module_name"]))
			{
				aliases_data[alias_name] = alias_data;
			}
		}
	}
	
	return aliases_data;
}

function Module_LookupAliases(match_filter)
{
	var aliases_data_filtered = {};
	var aliases_data = Storage_GetParameters("alias");
	
	for (var name in aliases_data)
	{
		var alias_data = eval("(" + aliases_data[name] + ")");
		
		var valid = true;
		
		for (var key in match_filter)
		{
			if (alias_data[key] != match_filter[key])
			{
				valid = false;
				break;
			}
		}
		
		if (valid)
		{
			aliases_data_filtered[name] = alias_data;
		}
	}
	
	return aliases_data_filtered;
}

function Module_GetAliasNames(filter_module_names)
{
	var aliases_name_filtered = [];
	var aliases_data = Storage_GetParameters("alias");
	
	for (var name in aliases_data)
	{
		var alias_data = eval("(" + aliases_data[name] + ")");
		
		if (!filter_module_names || in_array(filter_module_names, alias_data["module_name"]))
		{
			aliases_name_filtered.push(name)
		}
	}
	
	return aliases_name_filtered;
}

function Module_GetAvailableIds(filter_module_names)
{
	var result_list = [];
	var available_modules = ModuleExport_GetAvailableModules();
	
	for (var n in available_modules)
	{
		var id_parts = available_modules[n].split(":", 2);
		
		if (in_array(filter_module_names, id_parts[0]))
		{
			result_list.push(id_parts[1]);
		}
	}
	
	return result_list;
}

function Module_SendMessage(module_name, module_id, command, variables)
{
	var args = [];
	
	args.push(module_name + ":" + module_id);
	args.push(command);
	
	var length = 0;
	for (var key in variables)
	{
		length++;
	}
	
	args.push(length);
	
	for (var key in variables)
	{
		args.push(key);
		args.push(variables[key]);
	}
	
	return ModuleExport_SendMessage.apply(null, Array.prototype.slice.call(args, 0));
}

var Module_MessageAliasLookupFunctions = {};
var Module_OnChangeFunctions = {};
var Module_OnMessageFunctions = {};

function Module_RegisterMessageAliasLookup(module_name, callback)
{
	Module_MessageAliasLookupFunctions[module_name] = callback;
}

function Module_RegisterToOnChange(alias_name, callback)
{
	if (!Module_OnChangeFunctions[alias_name])
	{
		Module_OnChangeFunctions[alias_name] = [];
	}
	
	Module_OnChangeFunctions[alias_name].push(callback);
}

function Module_RegisterToOnMessage(alias_name, callback)
{
	if (!Module_OnMessageFunctions[alias_name])
	{
		Module_OnMessageFunctions[alias_name] = [];
	}
	
	Module_OnMessageFunctions[alias_name].push(callback);
}

function Module_OnMessage(full_id, command, variables)
{
	var id_parts = full_id.split(":", 2);
	var aliases_data = {};
	
	if (Module_MessageAliasLookupFunctions[id_parts[0]])
	{
		aliases_data = Module_MessageAliasLookupFunctions[id_parts[0]](id_parts[1], command, variables);
	}
	
	for (var alias_name in alias_data)
	{
		if (Module_OnMessageFunctions[alias_name])
		{
			for (var n in Module_OnMessageFunctions[alias_name])
			{
				Module_OnMessageFunctions[alias_name][n](alias_name, command, variables);
			}
		}
	}
}

function Module_OnChange(full_id, available)
{
	var id_parts = full_id.split(":", 2);
	
	var aliases_data = Module_LookupAliases({
		"module_name" : id_parts[0],
		"module_id"   : id_parts[1],
	});
	
	for (var alias_name in alias_data)
	{
		if (Module_OnChangeFunctions[alias_name])
		{
			for (var n in Module_OnChangeFunctions[alias_name])
			{
				Module_OnChangeFunctions[alias_name][n](alias_name, available);
			}
		}
	}
}
