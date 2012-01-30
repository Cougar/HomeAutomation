
LgTv_ModuleNames    = [ "Serial" ];
LgTv_Aliases        = function() { return Module_GetAliasNames(LgTv_ModuleNames); };
LgTv_AvailableIds   = function() { return Module_GetAvailableIds(LgTv_ModuleNames); };

function LgTv_PowerMode(alias_name, mode)
{
  if (arguments.length < 2)
  {
    Log("\033[31mNot enough parameters given.\033[0m\n");
    return false;
  }

  if (mode == "on")
  {
    return Serial_Send(alias_name, "ka 0 01");
  }
  else if (mode == "off")
  {
    return Serial_Send(alias_name, "ka 0 00");
  }
  
  Log("\033[31mInvalid mode given, " + mode + ".\033[0m\n");
  return false;
}
Console_RegisterCommand(LgTv_PowerMode, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, LgTv_Aliases(), [ "on", "off" ]); });


function LgTv_DisplayStrength(alias_name, level)
{
  if (arguments.length < 2)
  {
    Log("\033[31mNot enough parameters given.\033[0m\n");
    return false;
  }

  if (level == "max")
  {
    level = 64;
  }
  else if (level == "off")
  {
    return Serial_Send(alias_name, "kd 0 01");
  }
  
  Serial_Send(alias_name, "kd 0 00");
  return Serial_Send(alias_name, "mg 0 " + level);
}
Console_RegisterCommand(LgTv_DisplayStrength, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, LgTv_Aliases(), [ "off", "0", "16", "32", "48", "64", "max" ]); });


function LgTv_OSDMode(alias_name, mode)
{
  if (arguments.length < 2)
  {
    Log("\033[31mNot enough parameters given.\033[0m\n");
    return false;
  }

  if (mode == "on")
  {
    return Serial_Send(alias_name, "kl 0 01");
  }
  else if (mode == "off")
  {
    return Serial_Send(alias_name, "kl 0 00");
  }
  
  Log("\033[31mInvalid mode given, " + mode + ".\033[0m\n");
  return false;
}
Console_RegisterCommand(LgTv_OSDMode, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, LgTv_Aliases(), [ "on", "off" ]); });


function LgTv_IRMode(alias_name, mode)
{
  if (arguments.length < 2)
  {
    Log("\033[31mNot enough parameters given.\033[0m\n");
    return false;
  }

  if (mode == "on")
  {
    return Serial_Send(alias_name, "km 0 01");
  }
  else if (mode == "off")
  {
    return Serial_Send(alias_name, "km 0 00");
  }
  
  Log("\033[31mInvalid mode given, " + mode + ".\033[0m\n");
  return false;
}
Console_RegisterCommand(LgTv_IRMode, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, LgTv_Aliases(), [ "on", "off" ]); });


function LgTv_Volume(alias_name, level)
{
  if (arguments.length < 2)
  {
    Log("\033[31mNot enough parameters given.\033[0m\n");
    return false;
  }

  if (level == "mute")
  {
    return Serial_Send(alias_name, "ke 0 00");
  }
  
  Serial_Send(alias_name, "ke 0 01");
  return Serial_Send(alias_name, "kf 0 " + level);
}
Console_RegisterCommand(LgTv_Volume, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, LgTv_Aliases(), [ "mute", "0", "16", "32", "48", "64" ]); });
