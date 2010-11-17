
function CanService(type, name, id)
{
	this.Service(type, name, id);

	this.myIsOnline = false;
}

extend(CanService, Service);

CanService.prototype.myIsOnline = null;

CanService.prototype.canMessageHandler = function(canMessage)
{
}

CanService.prototype.setOnline = function()
{
	this.myIsOnline = true;
	log(this.myName + ":" + this.myId + "> Service went online\n");
	this.callEvent("online", null);
}

CanService.prototype.setOffline = function()
{
	this.myIsOnline = false;
	log(this.myName + ":" + this.myId + "> Service went offline\n");
	this.callEvent("offline", null);
}

CanService.prototype.isOnline = function()
{
	return this.myIsOnline;
}

