<?php

stream_wrapper_unregister('https'); 
stream_wrapper_register('https', 'ExchangeNTLMStream') or die("Failed to register protocol"); 

$wsdl = "/var/www/exchange/Services.wsdl"; 
$client = new ExchangeNTLMSoapClient($wsdl); 

//print_array($client->__getFunctions()); 
echo "test<br>";

$FindItem->Traversal = "Shallow"; 
$FindItem->ItemShape->BaseShape = "AllProperties"; 
$FindItem->ParentFolderIds->DistinguishedFolderId->Id = "calendar"; 
$FindItem->CalendarView->StartDate = "2009-04-07T00:00:00Z"; 
$FindItem->CalendarView->EndDate = "2009-04-08T00:00:00Z"; 

$result = $client->FindItem($FindItem); 
if ($result) 
{
  //echo "<br>result:<br>";
  //print_array($result);
  $calendaritems = $result->ResponseMessages->FindItemResponseMessage->RootFolder->Items->CalendarItem; 
  print_array($calendaritems);
  foreach($calendaritems as $item) 
  { 
    echo "Organizer: ".$item->Organizer->Mailbox->Name." Start: ".$item->Start." End: ".$item->End."<br>"; 
  } 
}

echo "<br>OBS tidszon+sommartid?? nu sommartid, ett möte som ovan gav start 13:00 var egentligen 15:00<br>OBS utf? teckenkodning<br>";


echo "<br><br>slut test<br>";

stream_wrapper_restore('https'); 



function print_array($var)
{
    echo "<pre>\n";
    print_r($var);
    echo "</pre>\n";
}

class NTLMSoapClient extends SoapClient { 
  function __doRequest($request, $location, $action, $version) 
  { 
    $headers = array( 'Method: POST', 'Connection: Keep-Alive', 'User-Agent: PHP-SOAP-CURL', 'Content-Type: text/xml; charset=utf-8', 'SOAPAction: "'.$action.'"', ); 
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
    curl_setopt($ch, CURLOPT_USERPWD, $this->user.':'.$this->password); 
    $response = curl_exec($ch); 
    return $response; 
  } 
  
  function __getLastRequestHeaders() 
  { 
    return implode("\n", $this->__last_request_headers)."\n"; 
  } 
}

class ExchangeNTLMSoapClient extends NTLMSoapClient 
{ 
  protected $user = 'xyz'; 
  protected $password = 'xyz'; 
} 

class NTLMStream 
{ 
  private $path; 
  private $mode; 
  private $options; 
  private $opened_path; 
  private $buffer; 
  private $pos; 

  public function stream_open($path, $mode, $options, $opened_path) 
  { 
    echo "[NTLMStream::stream_open] $path , mode=$mode <br>"; 
    $this->path = $path; 
    $this->mode = $mode; 
    $this->options = $options; 
    $this->opened_path = $opened_path; 
    $this->createBuffer($path); 
    return true; 
  } 

  public function stream_close() 
  { 
    echo "[NTLMStream::stream_close] <br>"; 
    curl_close($this->ch); 
  } 

  public function stream_read($count) 
  { 
    echo "[NTLMStream::stream_read] $count <br>"; 
    if(strlen($this->buffer) == 0) 
    { 
      return false; 
    } 
    $read = substr($this->buffer,$this->pos, $count); 
    $this->pos += $count; return $read; 
  } 

  public function stream_write($data) 
  { 
    echo "[NTLMStream::stream_write] <br>"; 
    if(strlen($this->buffer) == 0) 
    { 
      return false; 
    } 
    return true; 
  } 

  public function stream_eof() 
  { 
    echo "[NTLMStream::stream_eof] "; 
    if($this->pos > strlen($this->buffer)) 
    { 
      echo "true <br>"; 
      return true; 
    } 
    echo "false <br>"; 
    return false; 
  } 

  /* return the position of the current read pointer */ 
  public function stream_tell() 
  { 
    echo "[NTLMStream::stream_tell] <br>"; 
    return $this->pos; 
  } 

  public function stream_flush() 
  { 
    echo "[NTLMStream::stream_flush] <br>"; 
    $this->buffer = null; 
    $this->pos = null; 
  } 

  public function stream_stat() 
  { 
    echo "[NTLMStream::stream_stat] <br>"; 
    $this->createBuffer($this->path); 
    $stat = array( 'size' => strlen($this->buffer), ); 
    return $stat; 
  } 

  public function url_stat($path, $flags) 
  { 
    echo "[NTLMStream::url_stat] <br>"; 
    $this->createBuffer($path); 
    $stat = array( 'size' => strlen($this->buffer), ); 
    return $stat; 
  } 

  /* Create the buffer by requesting the url through cURL */ 
  private function createBuffer($path) 
  { 
    if($this->buffer) 
    { 
      return; 
    } 
    echo "[NTLMStream::createBuffer] create buffer from : $path <br>"; 
    $this->ch = curl_init($path); 
    curl_setopt($this->ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($this->ch, CURLOPT_SSL_VERIFYHOST, false);
    curl_setopt($this->ch, CURLOPT_RETURNTRANSFER, true); 
    curl_setopt($this->ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1); 
    curl_setopt($this->ch, CURLOPT_HTTPAUTH, CURLAUTH_NTLM); 
    curl_setopt($this->ch, CURLOPT_USERPWD, 
    $this->user.':'.$this->password); 
    echo $this->buffer = curl_exec($this->ch); 
    echo "[NTLMStream::createBuffer] buffer size : ".strlen($this->buffer)."bytes <br>"; 
    $this->pos = 0; 
  } 
} 

class ExchangeNTLMStream extends NTLMStream 
{ 
  protected $user = 'xyz'; 
  protected $password = 'xyz'; 
} 



?>
