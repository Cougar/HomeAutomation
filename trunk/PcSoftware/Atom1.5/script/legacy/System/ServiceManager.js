
function ServiceManager() {}

ServiceManager.Services = new Array();
ServiceManager.Callbacks = new Array();

ServiceManager.addCallback = function(callback)
{
	ServiceManager.Callbacks[ServiceManager.Callbacks.length] = callback;
}

ServiceManager.getService = function(type, name, id)
{
	var fullId = name + ":" + id;

	if (!ServiceManager.Services[fullId])
	{
		if (eval("typeof " + name + " != 'function'"))///FIXME: eval = evil!
		{
			if (!LoadScript("legacy/" + type + "/" + name + ".js"))
			{
				//log("Could not load " + name + ".js\n");
				return null;
			}
		}
		
		ServiceManager.Services[fullId] = eval("(new " + name + "('" + type + "', '" + name + "', " + id + "))");///FIXME: Can we do this without eval??
		
		for (var n in ServiceManager.Callbacks)
		{
			ServiceManager.Callbacks[n](ServiceManager.Services[fullId]);
		}
	}
	
	return ServiceManager.Services[fullId];
}

ServiceManager.startService = function(type, name, id, args)
{
	var service = ServiceManager.getService(type, name, id);
	if (service != null)
	{
		service.initialize(args);
	}
}

ServiceManager.numServices = function()
{
	return ServiceManager.Services.length;
}

ServiceManager.numCallbacks = function()
{
        return ServiceManager.Callbacks.length;
}

