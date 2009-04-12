<?php

require(dirname(__FILE__)."/config.php"); 

if (isset($_GET["function"]))
{
	switch ($_GET["function"])
	{
		case "getMeetingsRestOfDay":
		if (isset($_GET["shortname"]))
		{
			echo getMeetingsRestOfDay($_GET["shortname"]);
		}
		break;
		
		case "bookMeeting":
		break;
		
		//case "":
		//
		//break;
}

function getMeetingsRestOfDay($shortname)
{
	global $Calender;
	global $wsdl;
	$founditem = false;	
	
	foreach($Calender as $item)
	{
		if ($item->Shortname == $shortname)
		{
			$founditem = $item;
			break;
		}
	}
	
	if ($founditem !== false)
	{
		$user = $founditem->UserName;
		$pass = $founditem->Password;
		
		$FindItem->Traversal = "Shallow"; 
		$FindItem->ItemShape->BaseShape = "AllProperties"; 
		$FindItem->ParentFolderIds->DistinguishedFolderId->Id = "calendar"; 
		$FindItem->CalendarView->StartDate = "2009-04-07T00:00:00Z"; 
		$FindItem->CalendarView->EndDate = "2009-04-08T00:00:00Z";
		
		stream_wrapper_unregister('https'); 
		stream_wrapper_register('https', 'NTLMStream') or die("Failed to register protocol");
		 
		try
		{
			$client = new NTLMSoapClient($wsdl); 
			$result = $client->FindItem($FindItem);
			if ($result)
			{
			  //print_array($result);
			  $calendaritems = $result->ResponseMessages->FindItemResponseMessage->RootFolder->Items->CalendarItem; 
			  //print_array($calendaritems);
			  foreach($calendaritems as $item) 
			  { 
			    return "Organizer: ".$item->Organizer->Mailbox->Name." Start: ".$item->Start." End: ".$item->End."<br>"; 
			  } 
			}
			
		}
		catch (SoapFault $exception)
		{
			return "<error>".$exception."</error>"; 
		}

		stream_wrapper_restore('https'); 
	}
	else 
	{
		return "<error>Unknown shortname</error>";
	}


//echo "<br>OBS tidszon+sommartid?? nu sommartid, ett möte som ovan gav start 13:00 var egentligen 15:00<br>OBS utf? teckenkodning<br>";
}


function print_array($var)
{
    echo "<pre>\n";
    print_r($var);
    echo "</pre>\n";
}

class NTLMSoapClient extends SoapClient { 
  function __doRequest($request, $location, $action, $version) 
  { 
  	global $user;
  	global $pass;
  	global $exchangeurl;
    $location=$exchangeurl; //override url to exchange server

    $headers = array( 'Method: POST', 
			'Connection: Keep-Alive', 
			'User-Agent: PHP-SOAP-CURL', 
			'Content-Type: text/xml; charset=utf-8', 
			'SOAPAction: "'.$action.'"', 
			); 
    $this->__last_request_headers = $headers; 
    $ch = curl_init($location); 
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); 
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers); 
    curl_setopt($ch, CURLOPT_POST, true ); 
    curl_setopt($ch, CURLOPT_POSTFIELDS, $request); 
    curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1); 
    curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_NTLM); 
    curl_setopt($ch, CURLOPT_USERPWD, $user.':'.$pass); 
    $response = curl_exec($ch); 

//$myfile = "/tmp/debugex.txt";
//$fh = fopen($myfile, 'a+') or die("can't open file");
//fwrite($fh, "req: ". $request ."loc: ". $location ." act: ". $action ." ver: ". $version."\n");
//fwrite($fh, "reponse: ". $response."\n\n");
//fclose($fh);
    
    return $response; 
  } 
  
  function __getLastRequestHeaders() 
  { 
    return implode("\n", $this->__last_request_headers)."\n"; 
  } 
}


class NTLMStream 
{ 
  private $path; 
  private $mode; 
  private $options; 
  private $opened_path; 
  private $buffer; 
  private $pos; 

	/**
	 * Open the stream 
	 *
	 * @param unknown_type $path
	 * @param unknown_type $mode
	 * @param unknown_type $options
	 * @param unknown_type $opened_path
	 * @return unknown
	 */
  public function stream_open($path, $mode, $options, $opened_path) 
  { 
    echo "[NTLMStream::stream_open] $path , mode=$mode \n";
    $this->path = $path;
    $this->mode = $mode;
    $this->options = $options;
    $this->opened_path = $opened_path;
    $this->createBuffer($path);
    return true;
  } 

	/**
	 * Close the stream
	 *
	 */
  public function stream_close() 
  {
    echo "[NTLMStream::stream_close] \n";
    curl_close($this->ch);
  }

	/**
	 * Read the stream
	 *
	 * @param int $count number of bytes to read
	 * @return content from pos to count
	 */
  public function stream_read($count)
  {
    echo "[NTLMStream::stream_read] $count \n";
    if(strlen($this->buffer) == 0)
    {
      return false;
    }
    $read = substr($this->buffer,$this->pos, $count);
    $this->pos += $count;
    return $read;
  }

	/***
	 * write the stream
	 *
	 * @param $data
	 * @return
	 */
  public function stream_write($data)
  {
    echo "[NTLMStream::stream_write] \n";
    if(strlen($this->buffer) == 0)
    {
      return false;
    }
    return true;
  }

	/**
	 *
	 * @return true if eof else false
	 */
  public function stream_eof()
  {
    echo "[NTLMStream::stream_eof] ";
    if($this->pos > strlen($this->buffer))
    {
      echo "true \n";
      return true;
    }
    echo "false \n";
    return false;
  }

	/**
	 * @return int the position of the current read pointer
	 */
  public function stream_tell()
  {
    echo "[NTLMStream::stream_tell] \n";
    return $this->pos;
  }

	/**
	 * Flush stream data
	 */
  public function stream_flush()
  {
    echo "[NTLMStream::stream_flush] \n";
    $this->buffer = null;
    $this->pos = null;
  }

	/**
	 * Stat the file, return only the size of the buffer
	 *
	 * @return array stat information
	 */
  public function stream_stat()
  {
    echo "[NTLMStream::stream_stat] \n";
    $this->createBuffer($this->path);
    $stat = array( 'size' => strlen($this->buffer), );
    return $stat;
  }

	/**
	 * Stat the url, return only the size of the buffer
	 *
	 * @return array stat information
	 */
  public function url_stat($path, $flags)
  {
    echo "[NTLMStream::url_stat] \n";
    $this->createBuffer($path);
    $stat = array( 'size' => strlen($this->buffer), );
    return $stat;
  }

  /* Create the buffer by requesting the url through cURL */
  private function createBuffer($path)
  {
  	global $user;
  	global $pass;
    if($this->buffer)
    {
      return;
    }
    echo "[NTLMStream::createBuffer] create buffer from : $path \n";
    $this->ch = curl_init($path);
    curl_setopt($this->ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($this->ch, CURLOPT_SSL_VERIFYHOST, false);
    curl_setopt($this->ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($this->ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_setopt($this->ch, CURLOPT_HTTPAUTH, CURLAUTH_NTLM);
    curl_setopt($this->ch, CURLOPT_USERPWD, $user.':'.$pass);
    echo $this->buffer = curl_exec($this->ch);
    
    echo "[NTLMStream::createBuffer] buffer size : ".strlen($this->buffer)."bytes \n";
    $this->pos = 0;
  }
}

?>
