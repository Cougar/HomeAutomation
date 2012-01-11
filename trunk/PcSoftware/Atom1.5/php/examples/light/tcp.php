<?

error_reporting(E_ALL);

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
  $position = ftell($socket);
  
  fseek($socket, -1, SEEK_END);

  $has_data = ftell($socket) > $position;
  
  fseek($socket, $position, SEEK_SET);
  
  return $has_data;
}



function atomd_initialize($host, $port)
{
  $socket = atomd_connect($host, $port);

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

try
{
  $socket = atomd_initialize("127.0.0.1", 1202);

  atomd_send_command($socket, "Module_GetLastValue bokhylla");
  
  echo atomd_read_command_response($socket);
}
catch (Exception $e)
{
  die($e);
}


/*
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

if ($socket === false)
{
  die("Socket Creation Failed");
}

$result = socket_connect($socket, "127.0.0.1", 1202);

if ($result === false)
{
  die("Connection Failed");
}


$command = "help";
$length = strlen($command) + 1;

$data = "RESP" . str_pad($length, 4, "0", STR_PAD_LEFT) . $command . "\0\n";

echo "Sending:" . $data . "<br/>";

socket_write($socket, $data, strlen($data));

do
{
  $line = socket_read($socket, 1024, PHP_BINARY_READ);
  $output .= $line;
}
while ($line != "");

echo $output . "<br/>";
$output = "";

echo "Done first read!<br/>";

do
{
  $line = socket_read($socket, 1024, PHP_BINARY_READ);
  $output .= $line;
}
while ($line != "");

echo $output . "<br/>";
$output = "";

echo "Done!<br/>";

socket_close($socket);
*/
?>
