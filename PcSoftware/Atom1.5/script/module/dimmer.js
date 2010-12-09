
Require("plugin/module.js");

Dimmer_ModuleName     = "Dimmer230";
Dimmer_SpeedLevels    = function() { return [ 50, 135, 200, 255 ]; };
Dimmer_StrengthLevels = function() { return [ 0, 50, 100, 150, 200, 255 ]; };
Dimmer_StepsLevels    = function() { return [ 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ]; };
Dimmer_Directions     = function() { return [ "Increase", "Decrease" ]; };
Dimmer_Channels       = function() { return [ 0 ]; };
Dimmer_Aliases        = function() { return Module_GetAliasNames([ Dimmer_ModuleName ]); };
Dimmer_AvailableIds   = function() { return Module_GetAvailableIds([ Dimmer_ModuleName ]); };

function Dimmer_MessageAliasLookup(module_id, command, variables)
{
	var aliases_data = {};
	
	if (command == "Netinfo")
	{
		aliases_data = Module_LookupAliases({
			"module_name" : Dimmer_ModuleName,
			"module_id"   : module_id,
			"channel"     : channel
		});
	}
	
	return aliases_data;
}
Module_RegisterMessageAliasLookup(Dimmer_ModuleName, Dimmer_MessageAliasLookup);

function Dimmer_CreateAlias(alias_name, module_id, channel)
{
	var alias_data = {
		"group"       : false,
		"name"        : alias_name,
		"module_name" : Dimmer_ModuleName,
		"module_id"   : module_id,
		"channel"     : channel
	};

	return Storage_SetParameter("alias", alias_name, JSON.stringify(alias_data));
}
Console_RegisterCommand(Dimmer_CreateAlias, function(args) { return Console_StandardAutocomplete(args, Dimmer_Aliases(), Dimmer_AvailableIds(), Dimmer_Channels()); });

function Dimmer_StartFade(alias_name, speed, direction)
{
	if (arguments.length < 3)
	{
		throw("Not enought parameters given");
	}
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, [ Dimmer_ModuleName ]);
	
	for (var name in aliases_data)
	{
		var variables = {
			"Channel"   : aliases_data[name]["channel"],
			"Speed"     : speed,
			"Direction" : direction };
	
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Start_Fade", variables))
		{
			result_text += "StartFade sent successfully to " + name + "\n";
		}
		else
		{
			result_text += "StartFade failed to send to " + name + "\n";
		}
	}
	
	if (result_text.length == 0)
	{
		result_text = "No aliases by the name " + alias_name + " were applicable to StartFade.";
	}
	
	return result_text;
}
Console_RegisterCommand(Dimmer_StartFade, function(args) { return Console_StandardAutocomplete(args, Dimmer_Aliases(), Dimmer_SpeedLevels(), Dimmer_Directions()); });

function Dimmer_StopFade(alias_name)
{
	if (arguments.length < 1)
	{
		throw("Not enought parameters given");
	}
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, [ Dimmer_ModuleName ]);
	
	for (var name in aliases_data)
	{
		var variables = {
			"Channel" : aliases_data[name]["channel"] };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Stop_Fade", variables))
		{
			result_text += "StopFade sent successfully to " + name + "\n";
		}
		else
		{
			result_text += "StopFade failed to send to " + name + "\n";
		}
	}
	
	if (result_text.length == 0)
	{
		result_text = "No aliases by the name " + alias_name + " were applicable to StopFade.";
	}
	
	return result_text;
}
Console_RegisterCommand(Dimmer_StopFade, function(args) { return Console_StandardAutocomplete(args, Dimmer_Aliases()); });

function Dimmer_AbsoluteFade(alias_name, speed, level)
{
	if (arguments.length < 3)
	{
		throw("Not enought parameters given");
	}
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, [ Dimmer_ModuleName ]);
	
	for (var name in aliases_data)
	{
		var variables = {
			"Channel"   : aliases_data[name]["channel"],
			"Speed"     : speed,
			"EndValue"  : level };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Abs_Fade", variables))
		{
			result_text += "AbsolutFade sent successfully to " + name + "\n";
		}
		else
		{
			result_text += "AbsolutFade failed to send to " + name + "\n";
		}
	}
	
	if (result_text.length == 0)
	{
		result_text = "No aliases by the name " + alias_name + " were applicable to AbsolutFade.";
	}
	
	return result_text;
}
Console_RegisterCommand(Dimmer_AbsoluteFade, function(args) { return Console_StandardAutocomplete(args, Dimmer_Aliases(), Dimmer_SpeedLevels(), Dimmer_StrengthLevels()); });

function Dimmer_RelativeFade(alias_name, speed, direction, steps)
{
	if (arguments.length < 4)
	{
		throw("Not enought parameters given");
	}
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, [ Dimmer_ModuleName ]);
	
	for (var name in aliases_data)
	{
		var variables = {
			"Channel"   : aliases_data[name]["channel"],
			"Speed"     : speed,
			"Direction" : direction,
			"Steps"     : steps };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Rel_Fade", variables))
		{
			result_text += "RelativeFade sent successfully to " + name + "\n";
		}
		else
		{
			result_text += "RelativeFade failed to send to " + name + "\n";
		}
	}
	
	if (result_text.length == 0)
	{
		result_text = "No aliases by the name " + alias_name + " were applicable to RelativeFade.";
	}
	
	return result_text;
}
Console_RegisterCommand(Dimmer_RelativeFade, function(args) { return Console_StandardAutocomplete(args, Dimmer_Aliases(), Dimmer_SpeedLevels(), Dimmer_Directions(), Dimmer_StepsLevels()); });

function Dimmer_Demo(alias_name, speed, steps)
{
	if (arguments.length < 3)
	{
		throw("Not enought parameters given");
	}
	
	var result_text = "";
	var aliases_data = Module_ResolveAlias(alias_name, [ Dimmer_ModuleName ]);
	
	for (var name in aliases_data)
	{
		var variables = {
			"Channel"   : aliases_data[name]["channel"],
			"Speed"     : speed,
			"Steps"     : steps };
		
		if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Demo", variables))
		{
			result_text += "Demo sent successfully to " + name + "\n";
		}
		else
		{
			result_text += "Demo failed to send to " + name + "\n";
		}
	}
	
	if (result_text.length == 0)
	{
		result_text = "No aliases by the name " + alias_name + " were applicable to Demo.";
	}
	
	return result_text;
}
Console_RegisterCommand(Dimmer_Demo, function(args) { return Console_StandardAutocomplete(args, Dimmer_Aliases(), Dimmer_SpeedLevels(), Dimmer_StepsLevels()); });
