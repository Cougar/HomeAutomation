<?php
require 'config.php';

function GetLastData($alias2)
{
  global $AtomURL;
  global $AtomPort;

  $level2 = 0;
  $buffer = "";
  $result = array();
  try
  {
    $socket = atomd_initialize($AtomURL, $AtomPort);
    atomd_send_command($socket, "Module_GetLastDataRaw $alias2");

    $buffer = atomd_read_command_response($socket);
    $array = json_decode(trim($buffer), true);
  }
  catch (Exception $e)
  {
    die($e);
  }
  return $array["results"];
}



function SendGenericCommand($command)
{
  global $AtomURL;
  global $AtomPort;

  $buffer = "";

  try
  {
    $socket = atomd_initialize($AtomURL, $AtomPort);

    atomd_send_command($socket, $command);
    
    $buffer = atomd_read_command_response($socket);
  }
  catch (Exception $e)
  {
    die($e);
  }
  return $buffer;
}



function SetDimmerValue($alias2, $level)
{
    SendGenericCommand("Dimmer_AbsoluteFade $alias2 255 $level");
}


function SetPidTemperature($alias, $temperature)
{
  SendGenericCommand("PID_setValue $alias $temperature");
}


function SetPidParameters($alias, $KP, $KI, $KD, $Time, $Unit)
{
  SendGenericCommand("PID_setParameters $alias $KP $KI $KD $Time $Unit");
}

?>

