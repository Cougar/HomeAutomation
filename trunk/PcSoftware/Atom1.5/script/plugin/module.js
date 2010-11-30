
var modules = new Array();

function RequireModule(name)
{
	GetModule(name);
}

function GetModule(name)
{
	if (typeof modules[name] == 'undefined')
	{
		LoadScript("module/" + name + ".js");
		
		if (eval("typeof " + name + " != 'function'"))
		{
			Log("Failed to load " + name + " module.");
			return;
		}
		
		modules[name] = eval("(new " + name + "('" + name + "'))");
	}
	
	return modules[name];
}

function Module_OnChange(full_id, available)
{
	var parts = full_id.split(":", 2);
	
	if (global_legacy)
	{
		legacyOnState(full_id, parts[1], parts[0], available);
		return;
	}
	
	var module = GetModule(parts[0]);
	
	if (module)
	{
		module.SetStatus(parts[1], available);
	}
}

function Module_OnMessage()
{
	var full_id = arguments[0];
	var command = arguments[1];
	var variables = arguments[2];
	
	var parts = full_id.split(":", 2);
	
	if (global_legacy)
	{
		legacyOnMessage(full_id, parts[1], parts[0], command, variables);
		return;
	}
	
	var module = GetModule(parts[0]);
	
	if (module)
	{
		module.ReceiveMessage(parts[1], command, variables);
	}
	else
	{
		//Log("No one to receive this message, ignoring...");
	}
}

function SendModuleMessage(full_id, command, variables)
{
	var args = new Array();
	
	args[args.length] = full_id;
	args[args.length] = command;
	args[args.length] = variables.length;
	
	for (var i = 0; i < variables.length; i++)
	{
		var obj = variables[i];
		
		for (var key in obj)
		{
			args[args.length] = key;
			args[args.length] = obj[key];
		}
	}
	
	Module_SendMessage.apply(null, Array.prototype.slice.call(args, 0));
}


function Module(name)
{
	this.name_ = name;
	this.ids_ = new Array();
	
	this.EventBase();
}

Extend(Module, EventBase);

Module.prototype.name_;
Module.prototype.ids_;

Module.prototype.SetStatus = function(id, available)
{
	if (typeof this.ids_[id] == 'undefined')
	{
		this.ids_[id] = available;
		this.EventBase.prototype.Trigger.call(this, "onStatusChange", id, available);
	}
	else if (this.ids[id] != available)
	{
		this.EventBase.prototype.Trigger.call(this, "onStatusChange", id, available);
	}
}

Module.prototype.ReceiveMessage = function(id, command, variables)
{
	//Log("received " + command);
}

Module.prototype.GetAvailableIds = function()
{
	var ids = new Array();
	
	for (var id in this.ids_)
	{
		if (this.ids_[id] && typeof this.ids_[id] != 'function')
		{
			ids[ids.length] = id;
		}
	}
	
	return ids;
}

Module.prototype.GetAvailableNames = function()
{
	var result = new Array();
	var available_ids = this.GetAvailableIds();
	
	var module_aliases = getaliases();
	
	for (var alias in module_aliases)
	{
		if (module_aliases[alias]["is_group"])
		{
			for (var n = 0; n < module_aliases[alias]["aliases"].length; n++)
			{
				var group_alias = module_aliases[alias]["aliases"][n];

				if (module_aliases[group_alias]["module_name"] == this.name_ && ArrayContains(available_ids, module_aliases[group_alias]["module_id"]))
				{
					result.push(alias);
					break;
				}
			}
		}
		else
		{
			if (module_aliases[alias]["module_name"] == this.name_ && ArrayContains(available_ids, module_aliases[alias]["module_id"]))
			{
				result.push(alias);
			}
		}
	}
	
	return result;
}

Module.prototype.IsAvailable = function(id)
{
	if (typeof this.ids_[id] == 'undefined' || this.ids_[id] == false)
	{
		return false;
	}
	
	return true;
}

Module.prototype.SendMessage = function(id, command, variables)
{
	if (this.IsAvailable(id))
	{
		SendModuleMessage(this.name_ + ":" + id, command, variables);
	}
}


// Console Commands

function modulelist()
{
	var result = "";
	
	for (var name in modules)
	{
		result += name + ":  " + modules[name].GetAvailableIds().toString() + "\n";
	}
	
	return result;
}

RegisterConsoleCommand(modulelist);

