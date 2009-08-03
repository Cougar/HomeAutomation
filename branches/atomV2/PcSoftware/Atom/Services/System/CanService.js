
function CanService(type, name, id)
{
	this.Service(type, name, id);

	this.myNode = null;
}

extend(CanService, Service);

CanService.prototype.myNode = null;

CanService.prototype.canMessageHandler = function(canMessage)
{
}

CanService.prototype.setOnline = function()
{
	log(this.myName + ":" + this.myId + "> Service went online\n");
	this.callEvent("online", null);
}

CanService.prototype.setOffline = function()
{
	log(this.myName + ":" + this.myId + "> Service went offline\n");
	this.callEvent("offline", null);
}

CanService.prototype.isOnline = function()
{
	if (this.myNode)
	{
		return this.myNode.isOnline();
	}
	
	return false;
}

CanService.prototype.setNode = function(node)
{
	this.myNode = node;
}

CanService.prototype.getNode = function()
{
	return this.myNode;
}
