
function EggdropIRCpost(hostname, port, username, password)
{
	this.myPort = port;
	this.myHostname = hostname;
	this.myUsername = username;
	this.myPassword = password;
	this.myState = "DISCONNECTED";
}

EggdropIRCpost.prototype.connect = function()
{
	if (this.mySocket)
	{
		this.mySocket.stop();
	}
	
	var self = this;

	this.mySocket = new Socket(function(event, data) { self.socketCallback(event, data); }, this.myHostname, this.myPort, 0);
	this.mySocket.start();
	this.myState = "CONNECTING";
}

EggdropIRCpost.prototype.disconnect = function()
{
	if (this.mySocket)
	{
		/* Just send text ".quit" and the host will disconnect us */
		this.mySocket.send(".quit\n");
		this.myState = "DISCONNECTING"
	}
}
	
EggdropIRCpost.prototype.post = function(channel, text)
{
	this.mySendChn = channel;
	this.mySendText = text;
	
	if (this.mySocket && this.myState == "CONNECTED")
	{
		/* To send text to a channel send text ".say <channelname> <text>" */
		this.mySocket.send(".say "+channel+" "+text+"\n");
	}
	else if (this.myState == "DISCONNECTED")
	{
		/* Create the callback which should be called from socketCallback when reaching state==CONNECTED */
		this.myDataCallback = function() { self.DataCallback(); }
		this.connect();
	}
	else if (this.myState == "CONNECTING")
	{
		this.myDataCallback = function() { self.DataCallback(); }
	} 
}

EggdropIRCpost.prototype.DataCallback = function()
{
	if (this.mySocket && this.myState == "CONNECTED")
	{
		/* To send text to a channel send text ".say <channelname> <text>" */
		this.mySocket.send(".say "+this.mySendChn+" "+this.mySendText+"\n");
	}
	
	this.myDataCallback = null;
}

EggdropIRCpost.prototype.mySocket = null;
EggdropIRCpost.prototype.myState = null;
EggdropIRCpost.prototype.myDataCallback = null;
EggdropIRCpost.prototype.mySendChn = null;
EggdropIRCpost.prototype.mySendText = null;

EggdropIRCpost.prototype.myHostname = null;
EggdropIRCpost.prototype.myPort = null;
EggdropIRCpost.prototype.myUsername = null;
EggdropIRCpost.prototype.myPassword = null;

EggdropIRCpost.prototype.socketCallback = function(event, data)
{
	switch (event)
	{
	case "CONNECTED":
	break;
	
	case "DATA":
	/* When connected the host will first ask us for the username */
	if (data.indexOf("Please enter your nickname") > -1 && this.myState == "CONNECTING")
	{
		this.mySocket.send(this.myUsername+"\n");
	}
	/* Then the host will ask us for the password */
	if (data.indexOf("Enter your password") > -1 && this.myState == "CONNECTING")
	{
		this.mySocket.send(this.myPassword+"\n");
	}
	/* If credentials is correct we are connected */
	if (data.indexOf("joined the party line") > -1 && this.myState == "CONNECTING")
	{
		this.myState = "CONNECTED";
		if (this.myDataCallback)
		{
			this.myDataCallback();
		}
	}
	break;
	
	case "RESET":
	case "CLOSED":
	case "DIED":
	this.mySocket.stop();
	this.mySocket = null;
	this.myState = "DISCONNECTED";
	break;
	}
}

