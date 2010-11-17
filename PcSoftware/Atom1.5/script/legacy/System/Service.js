
function Service(type, name, id)
{
	this.myType = type;
	this.myName = name;
	this.myId = id;
	this.myEventCallbacks = new Array();
	this.myInitialArguments = new Array();
}

Service.prototype.myType = null;
Service.prototype.myName = null;
Service.prototype.myId = null;
Service.prototype.myInitialArguments = null;
Service.prototype.myEventCallbacks = null;

Service.prototype.initialize = function(initialArguments)
{
	this.myInitialArguments = initialArguments;
}

Service.prototype.getType = function()
{
	return this.myType;
}

Service.prototype.getName = function()
{
	return this.myName;
}

Service.prototype.getId = function()
{
	return this.myId;
}

Service.prototype.registerEventCallback = function(eventName, callback)
{
	if (!this.myEventCallbacks[eventName])
		this.myEventCallbacks[eventName] = new Array();
		
	this.myEventCallbacks[eventName].push(callback);
}

Service.prototype.callEvent = function(eventName, args)
{
	if (this.myEventCallbacks[eventName])
	{
		for (var n = 0; n < this.myEventCallbacks[eventName].length; n++)
		{
			this.myEventCallbacks[eventName][n](args);
		}
	}
}
