<?php

function GetLastValue($alias2)
{
  $level2 = 0;
  $buffer = "";

  try
  {
    $socket = atomd_initialize("127.0.0.1", 1202);

    atomd_send_command($socket, "Module_GetLastValue $alias2");
    
    $buffer = atomd_read_command_response($socket);
  }
  catch (Exception $e)
  {
    die($e);
  }

  $output = explode("\n", $buffer);

  foreach ($output as $line)
  {
    if (strpos($line, "Level: ") !== FALSE)
    {
      list($dummy1, $level2) = explode(" ", $line);
      
      $start = strpos($level2, "m") + 1;
      
      $level2 = substr($level2, $start, strpos($level2, "[", $start) - $start);
    }
  }

  return intval($level2);
}


function GetLastData($alias2)
{
  $level2 = 0;
  $buffer = "";
  $result = array();
  try
  {
    $socket = atomd_initialize("127.0.0.1", 1202);
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
  $level2 = 0;
  $buffer = "";

  try
  {
    $socket = atomd_initialize("127.0.0.1", 1202);

    atomd_send_command($socket, "Dimmer_AbsoluteFade $alias2 255 $level");
    
    $buffer = atomd_read_command_response($socket);
  }
  catch (Exception $e)
  {
    die($e);
  }
}

?>

