<?php

require_once("config.php");
require_once("atomjs_interface.php");

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
    $socket = atomjs_connect($atomdHostname, $atomdPort);

    atomjs_write($socket, $_GET["command"]);

    echo trim(atomjs_read($socket));
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