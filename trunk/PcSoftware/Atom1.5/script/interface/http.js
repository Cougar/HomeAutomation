
Http_Buffers = {};

function Http_Request(url, callback)
{
  var parsed_url = Http_ParseUrl(url);
  
  var socket_id = Socket_Connect(parsed_url["host"], parsed_url["port"], function(socket_id, data)
  {
    ///FIXME: This is a bit ugly code... also not fully HTTP/1.1 compliant look at http://www.jmarshall.com/easy/http/

    Http_Buffers[socket_id] += data;
    
  }, function(socket_id, state)
  {
    if (state == SOCKET_STATE_CONNECTED)
    {
      var data = Http_Buffers[socket_id];
      
      delete Http_Buffers[socket_id];
      
      var pos = data.indexOf("\r\n\r\n");

      var header_data = data.substr(0, pos);
      var content_data = data.substr(pos+2);

      var header_lines = header_data.split("\r\n");

      var header = [];
      var result = header_lines[0];

      for (var n = 1; n < header_lines.length; n++)
      {
	var header_parts = header_lines[n].split(":", 2);
	header[header_parts[0]] = header_parts[1].trim(' ');
      }

      callback(socket_id, result, header, content_data);
    }
  });
  
  if (socket_id > 0)
  {
      var request = "";
      
      Http_Buffers[socket_id] = "";

      request = "GET " + parsed_url['path'] + " HTTP/1.1\r\n";
      request += "Host: " + parsed_url['host'] + ":" + parsed_url['port'] + "\r\n";
      request += "User-Agent: Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.1.2) Gecko/20090730 SUSE/3.5.2-1.1 Firefox/3.5.2\r\n";
      request += "Connection: close\r\n";
      request += "\r\n";

      Socket_Send(socket_id, request);
  }
  else
  {
    Log("Could not connect to " + url + "\n");
  }
  
  return socket_id;
}

function Http_ParseUrl(url)
{
	var result = {};
	result['port'] = 80;
	result['path'] = "/";

	var firstSlash = url.indexOf('/');
	
	if (firstSlash == -1)
	{
		result['host'] = url;
	}
	else
	{
		result['host'] = url.substr(0, firstSlash);
		result['path'] = url.substr(firstSlash);
	}
	
	var firstColon = result['host'].indexOf(':');
	
	if (firstColon != -1)
	{
		result['port'] = result['host'].substr(firstColon+1);
		result['host'] = result['host'].substr(0, firstColon);
	}

	return result;
}
