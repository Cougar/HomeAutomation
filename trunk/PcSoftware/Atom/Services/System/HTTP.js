
function HTTP()
{
}

HTTP.prototype.mySocket = null;
HTTP.prototype.myUserCallback = null;
HTTP.prototype.myBuffer = null;

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
	this.mySocket.start();
	
	var request = "GET " + urlData['path'] + " HTTP/1.1\n";
	request += "Host: " + urlData['hostname'] + "\n";
	request += "Connection: close\n";
	request += "\n";
	
	this.mySocket.send(request);
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
	case "EVENT_DATA":
	this.myBuffer += unescape(data);
	break;
	
	case "EVENT_CLOSED":
	case "EVENT_DIED":
	this.mySocket.stop();
	this.processBuffer();
	break;
	
	case "EVENT_INACTIVITY":
	break;
	}
}

HTTP.prototype.processBuffer = function()
{
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
	
	this.myUserCallback(result, header, contentData);
}
