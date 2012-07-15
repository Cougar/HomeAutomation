<?php

require_once("config.php");

if ($atomdOld)
{
  require_once("atom_interface_old.php");
}
else
{
  require_once("atom_interface_new.php");
}


function GetSubfolders($directory)
{
  $folders = array();

  if (file_exists($directory))
  {
    $directory_handle = opendir($directory);

    while (false !== ($filename = readdir($directory_handle)))
    {
      if (is_dir($directory . "/" . $filename) && $filename[0] != ".")
      {
        $folders[] = $filename;
      }
    }

    closedir($directory_handle);

    if (count($folders) > 0)
    {
      natcasesort($folders);
      $folders = array_values($folders);
    }
  }

  return $folders;
}

if (isset($_GET["ajax"]))
{
  global $atomdHostname;
  global $atomdPort;

  $buffer = "";

  try
  {
    $socket = atomd_initialize($atomdHostname, $atomdPort);

    atomd_send_command($socket, $_GET["command"]);

    echo trim(atomd_read_command_response($socket));
  }
  catch (Exception $e)
  {
    die($e);
  }
  
  exit(0);  
}
else
{
  $pages = GetSubfolders("pages/");
}

?>