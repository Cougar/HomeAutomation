
Require("plugin/module.js");

Dimmer_ModuleNames    = [ "Dimmer230", "hwPWM" ];
Dimmer_SpeedLevels    = function() { return [ 50, 135, 200, 255 ]; };
Dimmer_StrengthLevels = function() { return [ 0, 50, 100, 150, 200, 255 ]; };
Dimmer_StepsLevels    = function() { return [ 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ]; };
Dimmer_Directions     = function() { return [ "Increase", "Decrease" ]; };
Dimmer_Channels       = function() { return [ 0 ]; };
Dimmer_Aliases        = function() { return Module_GetAliasNames(Dimmer_ModuleNames); };
Dimmer_AvailableIds   = function() { return Module_GetAvailableIds(Dimmer_ModuleNames); };

function Dimmer_StartFade(alias_name, speed, direction)
{
    if (arguments.length < 3)
    {
        Log("\033[31mNot enough parameters given.\033[0m\n");
        return false;
    }
    
    var aliases_data = Module_ResolveAlias(alias_name, Dimmer_ModuleNames);
    var found = false;
    
    for (var name in aliases_data)
    {
        var variables = {
            "Channel"   : aliases_data[name]["specific"]["Channel"],
            "Speed"     : speed,
            "Direction" : direction };
            
            if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Start_Fade", variables))
            {
                //Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
            }
            else
            {
                Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
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
Console_RegisterCommand(Dimmer_StartFade, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Dimmer_Aliases(), Dimmer_SpeedLevels(), Dimmer_Directions()); });

function Dimmer_StopFade(alias_name)
{
    if (arguments.length < 1)
    {
        Log("\033[31mNot enough parameters given.\033[0m\n");
        return false;
    }
    
    var aliases_data = Module_ResolveAlias(alias_name, Dimmer_ModuleNames);
    var found = false;
    
    for (var name in aliases_data)
    {
        var variables = {
            "Channel" : aliases_data[name]["specific"]["Channel"] };
            
            if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Stop_Fade", variables))
            {
                //Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
            }
            else
            {
                Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
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
Console_RegisterCommand(Dimmer_StopFade, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Dimmer_Aliases()); });

function Dimmer_AbsoluteFade(alias_name, speed, level)
{
    if (arguments.length < 3)
    {
        Log("\033[31mNot enough parameters given.\033[0m\n");
        return false;
    }
    
    var aliases_data = Module_ResolveAlias(alias_name, Dimmer_ModuleNames);
    var found = false;
    var level_set = level;
    
    for (var name in aliases_data)
    {
        if (aliases_data[name]["module_name"]=="hwPWM") {
            level_set = level*39;
	    if (level_set > 9940) {
	      level_set = 10000;
	    }
        } else {
            level_set = level;
	}
        
        //Check for undefined Channel parameter in alias
        if (typeof aliases_data[name]["specific"]["Channel"] == 'undefined') {
            Log("\033[31mFailed to send command to " + name + ", Alias does not contain specific parameter \"Channel\".\033[0m\n");
            break;
        }
        
        var variables = {
            "Channel"   : aliases_data[name]["specific"]["Channel"],
            "Speed"     : speed,
            "EndValue"  : level_set
        };
        
        if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Abs_Fade", variables))
        {
            //Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
        }
        else
        {
            Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
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
Console_RegisterCommand(Dimmer_AbsoluteFade, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Dimmer_Aliases(), Dimmer_SpeedLevels(), Dimmer_StrengthLevels()); });

function Dimmer_RelativeFade(alias_name, speed, direction, steps)
{
    if (arguments.length < 4)
    {
        Log("\033[31mNot enough parameters given.\033[0m\n");
        return false;
    }
    
    var aliases_data = Module_ResolveAlias(alias_name, Dimmer_ModuleNames);
    var found = false;
    var steps_set = steps;
    
    for (var name in aliases_data)
    {
        if (aliases_data[name]["module_name"]=="hwPWM") {
            steps_set = steps*39;
	    if (steps_set > 9940) {
	      level_set = 10000;
	    }
        } else {
            steps_set = steps;
	}
        var variables = {
            "Channel"   : aliases_data[name]["specific"]["Channel"],
            "Speed"     : speed,
            "Direction" : direction,
            "Steps"     : steps_set
        };
        
        //Check for undefined Channel parameter in alias
        if (typeof aliases_data[name]["specific"]["Channel"] == 'undefined') {
            Log("\033[31mFailed to send command to " + name + ", Alias does not contain specific parameter \"Channel\".\033[0m\n");
            break;
        }
        
        if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Rel_Fade", variables))
        {
            //Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
        }
        else
        {
            Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
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
Console_RegisterCommand(Dimmer_RelativeFade, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Dimmer_Aliases(), Dimmer_SpeedLevels(), Dimmer_Directions(), Dimmer_StepsLevels()); });

function Dimmer_Demo(alias_name, speed, steps)
{
    if (arguments.length < 3)
    {
        Log("\033[31mNot enough parameters given.\033[0m\n");
        return false;
    }
    
    var aliases_data = Module_ResolveAlias(alias_name, Dimmer_ModuleNames);
    var found = false;
    var steps_set = steps;
    
    for (var name in aliases_data)
    {
        if (aliases_data[name]["module_name"]=="hwPWM") {
            steps_set = steps*39;
	    if (steps_set > 9940) {
	      level_set = 10000;
	    }
        } else {
            steps_set = steps;
	}
        var variables = {
            "Channel"   : aliases_data[name]["specific"]["Channel"],
            "Speed"     : speed,
            "Steps"     : steps_set
        };
        
        //Check for undefined Channel parameter in alias
        if (typeof aliases_data[name]["specific"]["Channel"] == 'undefined') {
            Log("\033[31mFailed to send command to " + name + ", Alias does not contain specific parameter \"Channel\".\033[0m\n");
            break;
        }
        
        if (Module_SendMessage(aliases_data[name]["module_name"], aliases_data[name]["module_id"], "Demo", variables))
        {
            //Log("\033[32mCommand sent successfully to " + name + ".\033[0m\n");
        }
        else
        {
            Log("\033[31mFailed to send command to " + name + ".\033[0m\n");
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
Console_RegisterCommand(Dimmer_Demo, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, Dimmer_Aliases(), Dimmer_SpeedLevels(), Dimmer_StepsLevels()); });

function Dimmer_OnMessage(module_name, module_id, command, variables)
{
    if (in_array(Dimmer_ModuleNames, module_name))
    {
        var aliases_data = Module_LookupAliases({
            "module_name" : module_name,
            "module_id"   : module_id,
            "group"       : false
        });
        
        switch (command)
        {
            case "Netinfo":
            {
                for (var alias_name in aliases_data)
                {
                    if (aliases_data[alias_name]["specific"]["Channel"] != variables["Channel"])
                    {
                        continue;
                    }
                    
                    var last_value = {};
                    var last_value_string = Storage_GetParameter("LastValues", alias_name);
                    
                    if (last_value_string)
                    {
                        last_value = eval("(" + last_value_string + ")");
                    }
                    
                    last_value["Connection"] = { "value" : variables["Connection"], "timestamp" : get_time() };
                    last_value["Frequency"] = { "value" : variables["Frequency"], "timestamp" : get_time() };
                    last_value["Level"] = { "value" : variables["DimmerValue"], "timestamp" : get_time() };
                    
                    Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
                }
                
                break;
            }
            case "Pwm":
            {
                for (var alias_name in aliases_data)
                {
                    if (aliases_data[alias_name]["specific"]["Channel"] != variables["Channel"])
                    {
                        continue;
                    }
                    
                    var last_value = {};
                    var last_value_string = Storage_GetParameter("LastValues", alias_name);
                    
                    if (last_value_string)
                    {
                        last_value = eval("(" + last_value_string + ")");
                    }
                    var value = 0 + variables["Value"];
                    value = value / 39;
                    value = parseInt(value).toString();
                    last_value["Level"] = { "value" : value, "timestamp" : get_time() };
                    
                    Storage_SetParameter("LastValues", alias_name, JSON.stringify(last_value));
                }
                
                break;
            }
        }
    }
}
Module_RegisterToOnMessage("all", Dimmer_OnMessage);
