
function CanNode(hardwareId, numberOfModules)
{
	this.myHardwareId = hardwareId;
	this.myNumberOfModules = numberOfModules;
	this.myModules = new Array();
	this.myHeartbeatTime = 0;
	this.myIsOnline = false;
}

CanNode.prototype.myIsOnline = null;
CanNode.prototype.myHardwareId = null;
CanNode.prototype.myNumberOfModules = null;
CanNode.prototype.myModules = null;
CanNode.prototype.myHeartbeatTime = null;

CanNode.prototype.isOnline = function()
{
	return this.myIsOnline;
}

CanNode.prototype.reset = function()
{
	var canMessage = new CanNMTMessage("nmt", "Reset");
	canMessage.setData("HardwareId", this.myHardwareId);
	sendNMTMessage(canMessage);
}

CanNode.prototype.startApplication = function()
{
	var canMessage = new CanNMTMessage("nmt", "Start_App");
	canMessage.setData("HardwareId", this.myHardwareId);
	sendNMTMessage(canMessage);
}

CanNode.prototype.handleHeartbeat = function(numberOfModules)
{
	this.myNumberOfModules = numberOfModules;

	if (this.myNumberOfModules > this.myModules.length)
	{
		var canMessage = new CanMessage("mnmt", "To_Owner", "", 0, "List");
		canMessage.setData("HardwareId", this.myHardwareId);
		sendMessage(canMessage);
	}
	
	this.onlineHandler();
}

CanNode.prototype.addService = function(sequenceNumber, service)
{
	this.myModules[sequenceNumber] = service;
	service.setOnline();
}

CanNode.prototype.onlineHandler = function()
{
	var date = new Date();
	this.myHeartbeatTime = date.getTimestamp();

	if (!this.myIsOnline)
	{
		this.myIsOnline = true;

		log(uint2hex(this.myHardwareId, 32) + "> Node went online\n");
		
		for (var n in this.myModules)
		{
			this.myModules[n].setOnline();
		}
	}
}

CanNode.prototype.checkOffline = function()
{
	if (this.myIsOnline)
	{
		var date = new Date();
		if (this.myHeartbeatTime + 10 < date.getTimestamp())
		{
			this.setOffline();
		}
	}
}

CanNode.prototype.setOffline = function()
{
	if (this.myIsOnline)
	{
		this.myIsOnline = false;

		log(uint2hex(this.myHardwareId, 32) + "> Node went offline\n");
		
		for (var n in this.myModules)
		{
			this.myModules[n].setOffline();
		}
	}
}
