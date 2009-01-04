
CanNodes = new Array();
CanServices = new Array();
CanOfflineTimer = new Interval(offlineCheck, 10000);

function handleNMTMessage(className, commandName, data)
{
	var canMessage = new CanNMTMessage(className, commandName, data);

	switch (canMessage.getCommandName())
	{
	case "Reset":
	break;
	
	case "Bios_Start":
	break;
	
	case "Pgm_Start":
	break;
	
	case "Pgm_Data":
	break;
	
	case "Pgm_End":
	break;
	
	case "Pgm_Copy":
	break;
	
	case "Pgm_Ack":
	break;
	
	case "Pgm_Nack":
	break;
	
	case "Start_App":
	break;
	
	case "App_Start":
	break;
	
	case "Heartbeat":
	var node = CanNodes[canMessage.getData("HardwareId")];
	
	if (!node)
	{
		log("A can node with hardware id " + uint2hex(canMessage.getData("HardwareId"), 32) + " was not found, starting.\n");
		node = new CanNode(canMessage.getData("HardwareId"), canMessage.getData("NumberOfModules"));
		CanNodes[canMessage.getData("HardwareId")] = node;
	}
	
	node.handleHeartbeat(canMessage.getData("NumberOfModules"));
	break;
	}
}

// This is used to send messages to the can network
function sendNMTMessage(canMessage)
{
	sendCanNMTMessage(	canMessage.getClassName(), 
				canMessage.getCommandName(), 
				canMessage.getDataString());
}

// This is called when a message has been received from the can network
function handleMessage(className, directionFlag, moduleName, moduleId, commandName, data)
{
	var canMessage = new CanMessage(className, directionFlag, moduleName, moduleId, commandName, data);

	var fullId = canMessage.getModuleName() + canMessage.getModuleId();

	if (canMessage.getCommandName() == "List" && canMessage.getDirectionFlag() == "From_Owner")
	{
		if (!CanServices[fullId])
		{
			var service = ServiceManager.getService("Can", canMessage.getModuleName(), canMessage.getModuleId())

			if (service != null)
			{
				var node = CanNodes[canMessage.getData("HardwareId")];

				if (node)
				{
					service.setNode(node);
					node.addService(canMessage.getData("SequenceNumber"), service);
					CanNodes[canMessage.getData("HardwareId")] = node;
				}

				CanServices[fullId] = service;
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

// This will be called at reqular intervals
function offlineCheck()
{
	for (var n in CanNodes)
	{
		CanNodes[n].checkOffline();
	}
}

function setAllOffline()
{
	for (var n in CanNodes)
	{
		CanNodes[n].setOffline();
	}
}
