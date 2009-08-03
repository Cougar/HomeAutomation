
CanNodes = new Array();
CanServices = new Array();
CanOfflineTimer = new Interval(offlineCheck, 10000);
CanProgrammingNode = null;

function handleNMTMessage(className, commandName, data)
{
	var canMessage = new CanNMTMessage(className, commandName, data);

	switch (canMessage.getCommandName())
	{
	case "Reset":
	// Someone sent a reset command... should we do something?
	break;
	
	case "Bios_Start":
	var node = CanNodes[canMessage.getData("HardwareId")];
	
	if (!node)
	{
		log("A can node with hardware id " + uint2hex(canMessage.getData("HardwareId"), 32) + " was not found, starting.\n");
		node = new CanNode(canMessage.getData("HardwareId"), null);
		CanNodes[canMessage.getData("HardwareId")] = node;
	}
	
	node.handleBiosStart(canMessage.getData("BiosVersion"), canMessage.getData("HasApplication"));
	break;
	
	case "Pgm_Start":
	// Someone sent a programming start command... should we do something?
	break;
	
	case "Pgm_Data":
	// Someone sent a programming data command... should we do something?
	break;
	
	case "Pgm_End":
	// Someone sent a programming end command... should we do something?
	break;
	
	case "Pgm_Copy":
	// Someone sent a programming copy command... should we do something?
	break;
	
	case "Pgm_Ack":
	if (CanProgrammingNode)
	{
		CanProgrammingNode.handleAck(canMessage.getData("Data"));
	}
	//else
	//{
		// A node sent pgm ack but we are not in the progress of programming, probably to someone else
	//}
	break;
	
	case "Pgm_Nack":
	if (CanProgrammingNode)
	{
		CanProgrammingNode.handleNack(canMessage.getData("Data"));
	}
	// I do not now what this is for...
	break;
	
	case "Start_App":
	// Someone sent a app start command... should we do something?
	break;
	
	case "App_Start":
	var node = CanNodes[canMessage.getData("HardwareId")];
	
	if (!node)
	{
		log("A can node with hardware id " + uint2hex(canMessage.getData("HardwareId"), 32) + " was not found, starting.\n");
		node = new CanNode(canMessage.getData("HardwareId"), null);
		CanNodes[canMessage.getData("HardwareId")] = node;
	}
	
	node.handleHeartbeat();
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
