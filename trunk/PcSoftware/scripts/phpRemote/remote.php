<?php

$DEBUG = false;
//$DEBUG = true;

include('config.php');

if (array_key_exists("remote", $_REQUEST) && array_key_exists("button", $_REQUEST) && array_key_exists("state", $_REQUEST) && array_key_exists("code", $_REQUEST))
{
	$remote = $_REQUEST["remote"];
	$state = $_REQUEST["state"];
	$code = $_REQUEST["code"];
	$button = $_REQUEST["button"];

	if ($DEBUG)
	{
		echo "remote=".$remote." button=".$button." code=".$code." state=".$state;
	}
	
	if ($state=="Pressed")
	{
		$message1 = "ServiceManager.getService(\"Can\", \"irTransmit\", ".$irTransmitterID.").sendButton(\"".$remote."\", \"".$button."\", true);\n";
	}
	else
	{
		$message1 = "ServiceManager.getService(\"Can\", \"irTransmit\", ".$irTransmitterID.").sendButton(\"".$remote."\", \"".$button."\", false);\n";
	}
	$message2 = "quit();\n";
	
	$socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("could not connect to atom");
	$result = socket_connect($socket, $host, $port) or die("Could not connect to atom");
	usleep (2000);
	socket_write($socket, $message1, strlen($message1)) or die("Could not send data to atom");
	usleep (2000);
	socket_write($socket, $message2, strlen($message2)) or die("Could not send data to atom");
	usleep (2000);
	socket_close($socket);
	
	echo "Sent $button successfully";
	
	exit;
}

?>



