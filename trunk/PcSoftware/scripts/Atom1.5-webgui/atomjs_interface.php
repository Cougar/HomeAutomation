<?php

error_reporting(E_ALL);

function atomjs_connect($host, $port)
{
  $socket = pfsockopen($host, $port, $errno, $errstr); 

  if (!$socket)
  { 
    throw new Exception("$errstr ($errno)"); 
  }
  
  return $socket;
}

function atomjs_read($socket)
{
  if (feof($socket))
  {
    throw new Exception("socket not open"); 
  }

  $result = fread($socket, 8096);
  
  return $result;
}

function atomjs_write($socket, $command)
{
  if (feof($socket))
  {
    throw new Exception("socket not open"); 
  }
  
  if (fwrite($socket, $command) === FALSE)
  {
    throw new Exception("failed to write");
  }
}

function atomjs_data_available($socket)
{
  $read   = array($socket);
  $write  = NULL;
  $except = NULL;
  
  if (false === ($num_changed_streams = stream_select($read, $write, $except, 0)))
  {
    throw new Exception("could not do select on socket");
  }

  return $num_changed_streams > 0;
}

?>