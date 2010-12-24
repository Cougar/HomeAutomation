
Require("plugin/storage.js");

function Module_CreateAliasAutoComplete(arg_index, args)
{
	if (arg_index == 0)
	{
		return Module_GetAliasNames();
	}
	else if (arg_index == 1)
	{
		return Module_GetNames();
	}
	else if (arg_index == 2)
	{
		return Module_GetAvailableIds([ args[1] ])
	}
	
	return [];
}

function Module_CreateAlias(alias_name, module_name, module_id, specific)
{
	if (arguments.length < 3)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	if (alias_name == "all")
	{
		Log("\033[31m\"all\" is not allowed as alias name.\033[0m\n");
		return false;
	}
	
	var specific_list = {};
	
	if (specific)
	{
		var specific_parts = specific.split(",");
		
		// TODO: Check length
		
		for (var index in specific_parts)
		{
			var parts = specific_parts[index].split("=");
			
			// TODO: Check length
			
			specific_list[parts[0]] = parts[1];
		}
	}
	
	var alias_data = {
		"group"       : false,
		"name"        : alias_name,
		"module_name" : module_name,
		"module_id"   : module_id,
		"specific"    : specific_list
	};
	
	return Storage_SetParameter("alias", alias_name, JSON.stringify(alias_data));
}
Console_RegisterCommand(Module_CreateAlias, Module_CreateAliasAutoComplete);

function Module_CreateAliasGroupAutoComplete(arg_index, args)
{
	if (arg_index == 0)
	{
		return Module_GetAliasNames();
	}
	else if (arg_index == 1)
	{
		return Module_GetNames();
	}
	else if (arg_index >= 2)
	{
		return Module_GetAliasNames([ args[1] ])
	}
	
	return [];
}

function Module_CreateAliasGroup(alias_group_name, module_type, alias_name1, alias_name2)
{
	if (arguments.length < 3)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	if (alias_group_name == "all")
	{
		Log("\033[31m\"all\" is not allowed as alias name.\033[0m\n");
		return false;
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
		Log("\033[31mNo valid aliases were found, aborting creation of group alias.\033[0m\n");
		return false;
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
			if (!filter_module_names || in_array(filter_module_names, alias_data["module_name"]))
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

function Module_GetNames()
{
	return [ "Dimmer230", "irTransmit", "irReceive", "SimpleDTMF", "BusVoltage", "DS18x20", "FOST02", "HD44789" ];
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

var Module_OnChangeFunctions = {};
var Module_OnMessageFunctions = {};

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

	var aliases_data = Module_LookupAliases({
		"module_name" : id_parts[0],
		"module_id"   : id_parts[1],
		"group"       : false
	});
	
	for (var alias_name in aliases_data)
	{
		var ok = true;

		if (aliases_data[alias_name]["specific"])
		{
			var ok = true;
			
			for (var name in variables)
			{
				if (aliases_data[alias_name]["specific"][name] && aliases_data[alias_name]["specific"][name] != variables[name])
				{
					ok = false;
					break;
				}
			}
		}
		
		if (Module_OnMessageFunctions[alias_name] && ok)
		{
			for (var n in Module_OnMessageFunctions[alias_name])
			{
				Module_OnMessageFunctions[alias_name][n](alias_name, command, variables);
			}
		}
	}
	
	if (Module_OnMessageFunctions["all"])
	{
		for (var n in Module_OnMessageFunctions["all"])
		{
			Module_OnMessageFunctions["all"][n](id_parts[0], id_parts[1], command, variables);
		}
	}
}

function Module_OnChange(full_id, available)
{
	var id_parts = full_id.split(":", 2);
	var aliases_data = {};
	
	var aliases_data_all = Module_LookupAliases({
		"module_name" : id_parts[0],
		"module_id"   : id_parts[1],
		"group"       : false
	});
	
	for (var alias_name in aliases_data)
	{
		var ok = true;
		
		if (aliases_data[alias_name]["specific"])
		{
			var ok = true;
			
			for (var name in variables)
			{
				if (aliases_data[alias_name]["specific"][name] && aliases_data[alias_name]["specific"][name] != variables[name])
				{
					ok = false;
					break;
				}
			}
		}
		
		if (Module_OnChangeFunctions[alias_name] && ok)
		{
			for (var n in Module_OnChangeFunctions[alias_name])
			{
				Module_OnChangeFunctions[alias_name][n](alias_name, available);
			}
		}
	}
	
	if (Module_OnChangeFunctions["all"])
	{
		for (var n in Module_OnChangeFunctions["all"])
		{
			Module_OnChangeFunctions["all"][n](id_parts[0], id_parts[1], available);
		}
	}
}

function Module_GetLastValue(alias_name)
{
	if (arguments.length < 1)
	{
		Log("\033[31mNot enough parameters given.\033[0m\n");
		return false;
	}
	
	var aliases_data = Module_ResolveAlias(alias_name);
	var found = false;
	
	for (var name in aliases_data)
	{
		var last_value_string = Storage_GetParameter("LastValues", alias_name);
		
		if (last_value_string)
		{
			var last_value = eval("(" + last_value_string + ")");
			
			for (var type_name in last_value)
			{
				var date = new Date(last_value[type_name]["timestamp"] * 1000);
				
				Log("\033[96m" + type_name + ": \033[0;1m" + last_value[type_name]["value"] + "\033[0m at " + date.toString() + "\n");
			}
		}
		else
		{
			Log("\033[32mNo value is stored for " + name + ".\033[0m\n");
		}
		
		found = true;
	}
	
	if (!found)
	{
		Log("\033[31mNo aliases by the name " + alias_name + " were applicable for this command.\033[0m\n");
		return false;
	}
	
	return true;
}
Console_RegisterCommand(Module_GetLastValue, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Module_GetAliasNames()); });
