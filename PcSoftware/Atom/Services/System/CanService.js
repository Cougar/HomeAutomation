

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
	var date = new Date();
	this.myHeartbeatTime = date.getTimestamp();
	this.myIsOnline = true;
	this.callEvent("online", null);
}

CanService.prototype.heartbeatHandler = function()
{
	var date = new Date();
	this.myHeartbeatTime = date.getTimestamp();
	this.myIsOnline = true;
	this.callEvent("heartbeat", null);
}

CanService.prototype.checkOffline = function()
{
	if (this.myIsOnline)
	{
		var date = new Date();
		if (this.myHeartbeatTime + 10 < date.getTimestamp())
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
