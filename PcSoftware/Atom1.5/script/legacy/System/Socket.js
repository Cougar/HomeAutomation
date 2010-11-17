
function Socket(callback, address, port, reconnectTimeout)
{
	this.myCallback = callback;
	this.myReconnectTimeout = reconnectTimeout;
	this.myAddress = address;
	this.myPort = port;
}

Socket.mySockets = new Array();
Socket.prototype.myId = null;
Socket.prototype.myReconnectTimeout = null;
Socket.prototype.myCallback = null;
Socket.prototype.myAddress = null;
Socket.prototype.myPort = null;

Socket.triggerSocketCallback = function(id, event, data)
{
	if (Socket.mySockets[id])
	{
		Socket.mySockets[id].triggerCallback(event, data);
	}
	else
	{
		log("Socket:" + id + "> Encountered a thread that have no javascript Socket object. This is not supposed to be possible, report this.\n");
		
		if (!stopSocketThread(id))
		{
			log("Socket:" + id + "> Socket thread could not be stopped.\n");
		}
	}
}

Socket.prototype.triggerCallback = function(event, data)
{
	this.myCallback(event, data);
}

Socket.prototype.send = function(data)
{
	if (this.myId != null)
	{
		sendToSocketThread(this.myId, data);
	}
}

Socket.prototype.start = function()
{
	if (this.myId == null)
	{
		this.myId = startSocketThread(this.myAddress, this.myPort, this.myReconnectTimeout)
		
		if (this.myId == 0)
		{
			return false;
		}
		
		Socket.mySockets[this.myId] = this;
	}
	
	return true;
}

Socket.prototype.stop = function()
{
	if (this.myId != null)
	{
		if (!stopSocketThread(this.myId))
		{
			log("Socket:" + this.myId + "> Socket thread could not be stopped.\n");
		}
		
		delete Socket.mySockets[this.myId];
		this.myId = null;
	}
}
