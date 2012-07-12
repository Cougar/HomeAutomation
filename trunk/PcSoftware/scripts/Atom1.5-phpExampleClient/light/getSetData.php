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



function SetDimmerValue($alias2, $level)
{
  global $AtomURL;
  global $AtomPort;

  $level2 = 0;
  $buffer = "";

  try
  {
    $socket = atomd_initialize($AtomURL, $AtomPort);

    atomd_send_command($socket, "Dimmer_AbsoluteFade $alias2 255 $level");
    
    $buffer = atomd_read_command_response($socket);
  }
  catch (Exception $e)
  {
    die($e);
  }
}

?>

