
function HTTP()
{
}

HTTP.prototype.mySocket = null;
HTTP.prototype.myUserCallback = null;
HTTP.prototype.myBuffer = null;
HTTP.prototype.myRequest = null;

HTTP.prototype.request = function(callback, url)
{
	this.myUserCallback = callback;
	
	if (this.mySocket)
	{
		this.mySocket.stop();
	}
	
	var self = this;
	
	var urlData = this.parseUrl(url);
	
	/*log("hostname: " + urlData['hostname'] + "\n");
	log("path: " + urlData['path'] + "\n");
	log("port: " + urlData['port'] + "\n");*/
	
	this.myBuffer = "";
	
	this.mySocket = new Socket(function(event, data) { self.socketCallback(event, data); }, urlData['hostname'], urlData['port'], 0);
	if (this.mySocket.start())
	{
		this.myRequest = "GET " + urlData['path'] + " HTTP/1.1\r\n";
		this.myRequest += "Host: " + urlData['hostname'] + ":" + urlData['port'] + "\r\n";
		this.myRequest += "User-Agent: Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.1.2) Gecko/20090730 SUSE/3.5.2-1.1 Firefox/3.5.2\r\n";
		this.myRequest += "Connection: close\r\n";
		this.myRequest += "\r\n";
		this.mySocket.send(this.myRequest);
	}
}

HTTP.prototype.parseUrl = function(url)
{
	var result = new Array();
	result['port'] = 80;
	result['path'] = "/";

	var firstSlash = url.indexOf('/');
	
	if (firstSlash == -1)
	{
		result['hostname'] = url;
	}
	else
	{
		result['hostname'] = url.substr(0, firstSlash);
		result['path'] = url.substr(firstSlash);
	}
	
	var firstColon = result['hostname'].indexOf(':');
	
	if (firstColon != -1)
	{
		result['port'] = result['hostname'].substr(firstColon+1);
		result['hostname'] = result['hostname'].substr(0, firstColon);
	}

	return result;
}

HTTP.prototype.socketCallback = function(event, data)
{
	switch (event)
	{
	case "CONNECTED":
	this.mySocket.send(this.myRequest);
	break;
	
	case "DATA":
	this.myBuffer += unescape(data);
	break;
	
	case "RESET":
	case "CLOSED":
	case "DIED":
	this.mySocket.stop();
	this.processBuffer();
	this.mySocket = null;
	break;
	}
}

HTTP.prototype.processBuffer = function()
{
	///FIXME: This is a bit ugly code... also not fully HTTP/1.1 compliant look at http://www.jmarshall.com/easy/http/

	var pos = this.myBuffer.indexOf("\n\n");
	
	var headerData = this.myBuffer.substr(0, pos);
	var contentData = this.myBuffer.substr(pos+2);
	
	var headerLines = headerData.split("\n");
	
	var header = new Array();
	var result = headerLines[0];
	
	for (var n = 1; n < headerLines.length; n++)
	{
		var headerParts = headerLines[n].split(":", 2);
		header[headerParts[0]] = headerParts[1].trim(' ');
	}
	
	this.myBuffer = "";
	
	if (this.myUserCallback != null)
	{
		this.myUserCallback(result, header, contentData);
		this.myUserCallback = null;
	}
}
