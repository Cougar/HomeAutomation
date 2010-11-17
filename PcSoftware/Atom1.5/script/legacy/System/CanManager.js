

CanServices = new Array();

function legacyOnState(fullId, moduleId, moduleName, available)
{
	var service = ServiceManager.getService("Can", moduleName, moduleId)
	
	if (service != null)
	{
		if (available)
		{
			service.setOnline();
		}
		else
		{
			service.setOffline();
		}
		
		CanServices[fullId] = service;
	}
	else
	{
		log("No such service found: " + moduleName + "\n");
	}
	
}

function legacyOnMessage(fullId, moduleId, moduleName, commandName, data)
{
	var canMessage = new CanMessage("", "From_Owner", moduleName, moduleId, commandName, data);
	
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



// This is used to send messages to the can network
function sendMessage(canMessage)
{
	var data = new Array();
	
	for (var key in canMessage.myData)
	{
		var obj = {};
		obj[key] = canMessage.myData[key];
		
		data[data.length] = obj;
	}
	
	SendModuleMessage(canMessage.getModuleName() + ":" + canMessage.getModuleId(), canMessage.getCommandName(), data);
}

