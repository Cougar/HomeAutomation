
CanServices = new Array();
CanOfflineTimer = new Interval(offlineCheck, 10000);

// This is called when a message has been received from the can network
function handleMessage(className, directionFlag, moduleName, moduleId, commandName, data)
{
	var canMessage = new CanMessage(className, directionFlag, moduleName, moduleId, commandName, eval("(" + data + ")"));
	var fullId = canMessage.getModuleName() + canMessage.getModuleId();

	if (canMessage.getCommandName() == "List" && canMessage.getDirectionFlag() == "From_Owner")
	{
		if (!CanServices[fullId])
		{
			var service = ServiceManager.getService("Can", canMessage.getModuleName(), canMessage.getModuleId())

			if (service != null)
			{
				CanServices[fullId] = service;
				CanServices[fullId].setHardwareId(canMessage.getData("HardwareId"));
				CanServices[fullId].onlineHandler();
			}
			else
			{
				log("No such service found: " + canMessage.getModuleName() + "\n");
			}
		}
	}
	else
	{
		if (CanServices[fullId])
		{
			// Tell the service to handle the message, a module service must implement canMessageHandler or derive from CanService-class
			CanServices[fullId].canMessageHandler(canMessage);
		}
		else if (canMessage.getModuleName() == "Debug")
		{
			var service = ServiceManager.getService("Can", canMessage.getModuleName(), canMessage.getModuleId())
			if (service != null)
			{
				CanServices[fullId] = service;
				CanServices[fullId].canMessageHandler(canMessage);
			}
		}
	}
}

// This is used to send messages to the can network
function sendMessage(canMessage)
{
	sendCanMessage(	canMessage.getClassName(), 
			canMessage.getDirectionFlag(), 
			canMessage.getModuleName(), 
			canMessage.getModuleId(), 
			canMessage.getCommandName(), 
			canMessage.getDataString());
}

function startOfflineCheck()
{
	CanOfflineTimer.start();
}

// This will be called at reqular intervals
function offlineCheck()
{
	for (var n in CanServices)
	{
		CanServices[n].checkOffline();
	}
}

function setAllOffline()
{
	for (var n in CanServices)
	{
		CanServices[n].setOffline();
	}
}

// This is called when a node sends a heartbeat
function handleHeartbeat(hardwareId)
{
	var found = false;

	for (var n in CanServices)
	{
		if (CanServices[n].getHardwareId() == hardwareId)
		{
			found = true;
			
			if (CanServices[n].isOnline())
			{
				CanServices[n].heartbeatHandler();
			}
			else
			{
				CanServices[n].onlineHandler();
			}
		}
	}
	
	if (!found)
	{
		log("Received heartbeat from unknown node, requesting module listing for hardware id " + hardwareId + "\n");
		var canMessage = new CanMessage("nmt", "To_Owner", "", 0, "List");
		canMessage.setData("HardwareId", hardwareId);
		sendMessage(canMessage);
	}
}
