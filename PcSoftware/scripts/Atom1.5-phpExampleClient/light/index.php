<?
$aliases = array("sovrum", "bardisk");


function atomd_connect($host, $port)
{
  $socket = pfsockopen("127.0.0.1", 1202, $errno, $errstr); 

  if (!$socket)
  { 
    throw new Exception("$errstr ($errno)"); 
  }
  
  return $socket;
}

function atomd_read_packet($socket)
{
  if (feof($socket))
  {
    throw new Exception("socket not open"); 
  }
  
  $command = fread($socket, 4);
  
  if ($command === FALSE)
  {
    throw new Exception("got EOF while reading command from socket"); 
  }
  
  $payload_length = fread($socket, 4);
  
  if ($payload_length === FALSE)
  {
    throw new Exception("got EOF while reading payload_length from socket"); 
  }
  
  $payload = "";
  
  if ($payload_length > 0)
  {
    $payload = fread($socket, intval($payload_length));
    
    if ($payload === FALSE)
    {
      throw new Exception("got EOF while reading payload from socket"); 
    }
  }
  
  return $command . $payload_length . $payload;
}

function atomd_write_packet($socket, $command, $payload)
{
  if (feof($socket))
  {
    throw new Exception("socket not open"); 
  }
  
  $packet = $command . str_pad(strlen($payload) + 1, 4, "0", STR_PAD_LEFT) . $payload . "\0";
  
  if (fwrite($socket, $packet) === FALSE)
  {
     throw new Exception("got error while writing payload to socket"); 
  }
}

function atomd_data_available($socket)
{
  $read   = array($socket);
  $write  = NULL;
  $except = NULL;
  
  if (false === ($num_changed_streams = stream_select($read, $write, $except, 0)))
  {
    throw new Exception("could not do select on socket");
  }

  return $num_changed_streams > 0;
  /*


  $position = ftell($socket);
  
  fseek($socket, -1, SEEK_END);

  $has_data = ftell($socket) > $position;
  
  fseek($socket, $position, SEEK_SET);
  
  return $has_data;*/
}



function atomd_initialize($host, $port)
{
  $socket = atomd_connect("127.0.0.1", 1202);

  while (atomd_data_available($socket))
  {
    $packet = atomd_read_packet($socket); // Read initial prompt
  }
  
  return $socket;
}

function atomd_send_command($socket, $command)
{
  atomd_write_packet($socket, "RESP", $command);
}

function atomd_read_command_response($socket)
{
  $response = "";
  
  while (true)
  {
    $packet = atomd_read_packet($socket);
    
    if (substr($packet, 0, 4) != "TEXT")
    {
      break;
    }
    
    $response .= substr($packet, 8) . "\n";
  }
  
  return $response;
}



function GetCurrentValue($alias2)
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

function SetCurrentValue($alias2, $level)
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

$response = array();


if (isset($_GET["strength"]))
{
  if (intval($_GET["strength"]) < 10)
  {
    $_GET["strength"] = 0;
  }
  
  SetCurrentValue($_GET["alias"], $_GET["strength"]);
  
  echo json_encode($response);
  exit(0);
}
else if (isset($_GET["getvalue"]))
{
  foreach ($aliases as $alias)
  {
    $response[$alias] = GetCurrentValue($alias);
  }
  
  echo json_encode($response);
  exit(0);
}




?>


<!DOCTYPE html> 
<html> 
<head> 
  <meta charset="utf-8"> 
  <meta name="viewport" content="width=device-width, initial-scale=1"> 
  <title>Light switch</title> 
  <link rel="stylesheet"  href="jquery.mobile-1.0b1.css" /> 
  <script src="jquery-1.6.1.min.js"></script> 
  <script src="jquery.mobile-1.0b1.min.js"></script> 
  
  <script>

    $(document).bind("pagecreate", function(event, ui)
    {
      <?
      foreach ($aliases as $alias)
      {
      ?>
        $('#slider<?=$alias?>' ).bind('change', function(event, ui){ makeAjaxChange("<?=$alias?>"); });
      <?
      }
      ?>
      delayed_start_of_poll();
    });

    var current_value = {};
    var next_value = {};
    
    <?
    foreach ($aliases as $alias)
    {
    ?>
      current_value['<?=$alias?>'] = 0;
      next_value['<?=$alias?>'] = -1;
    <?
    }
    ?>
    var updating_current = false;
    var timer = null;

    function checkCurrentValue()
    {
      updating_current = true;
      jQuery.getJSON("?getvalue=1", "", function(data)
      {
        
        
        jQuery.each(data, function(alias, value)
        {
          current_value[alias] = value;
          $('#slider' + alias).val(value).slider("refresh");
        });
        
        updating_current = false;
      });
    }

    function makeAjaxChange(alias)
    {
      if (updating_current)
      {
        return;
      }
    
      if ($('#slider' + alias).val() == current_value)
      {
        return;
      }
      
      if (next_value[alias] == -1) // Nothing in progress, do a call
      {
        do_call(alias, $('#slider' + alias).val());
      }
      else
      {
        next_value[alias] = $('#slider' + alias).val();
      }
      // console.log($('#slider').val());
    }
    
    function do_call(alias, strength)
    {
      next_value[alias] = strength;
      
      //clearInterval(timer);
      timer = null;
      
      jQuery.get("?alias=" + alias + "&strength=" + strength, "", function()
      {
        current_value[alias] = strength;
        
        if (next_value[alias] != current_value[alias])
        {
          do_call(alias, next_value[alias]);
        }
        else
        {
          //timer = setTimeout(delayed_start_of_poll, 3000);
          next_value[alias] = -1;
        }
      });
    }
    
    function delayed_start_of_poll()
    {
      timer = setTimeout(checkCurrentValue, 1000);
    }

  </script>
  
</head> 
<body> 
<div data-role="page" id="jqm-home" class="type-home" data-theme="a"> 
  <div data-role="content"> 
    
     <?
    foreach ($aliases as $alias)
    {
    ?>
      <div data-role="fieldcontain" id="slider-container">
        <label for="slider<?=$alias?>"><?=ucfirst($alias)?>:</label>
        <input type="range" name="slider<?=$alias?>" id="slider<?=$alias?>" value="<?=intval($level)?>" min="0" max="255"  />
      </div>
    <?
    }
    ?>
   
 
  </div> 
  
</div> 
</body> 
</html> 