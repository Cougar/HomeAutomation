

function CanService(name, id)
{
	this.Service(name, id);

	this.myIsOnline = false;
	this.myHardwareId = null;
	this.myHeartbeatTime = 0;
}

extend(CanService, Service);

CanService.prototype.myIsOnline = null;
CanService.prototype.myHardwareId = null;
CanService.prototype.myHeartbeatTime = null;

CanService.prototype.canMessageHandler = function(canMessage)
{
}

CanService.prototype.onlineHandler = function()
{
	log(this.myName + ":" + this.myId + "> Service went online\n");
	this.myHeartbeatTime = time();
	this.myIsOnline = true;
	this.callEvent("online", null);
}

CanService.prototype.heartbeatHandler = function()
{
	this.myHeartbeatTime = time();
	this.myIsOnline = true;
	this.callEvent("heartbeat", null);
}

CanService.prototype.checkOffline = function()
{
	if (this.myIsOnline)
	{
		if (this.myHeartbeatTime + 10 < time())
		{
			log(this.myName + ":" + this.myId + "> Service went offline\n");
			this.myIsOnline = false;
			this.callEvent("offline", null);
		}
	}
}

CanService.prototype.isOnline = function()
{
	return this.myIsOnline;
}

CanService.prototype.setHardwareId = function(hardwareId)
{
	this.myHardwareId = hardwareId;
}

CanService.prototype.getHardwareId = function()
{
	return this.myHardwareId;
}
