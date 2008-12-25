
function ServiceManager() {}

ServiceManager.Services = new Array();

ServiceManager.getService = function(type, name, id)
{
	var fullId = name + id;

	if (!ServiceManager.Services[fullId])
	{
		if (eval("typeof " + name + " != 'function'"))///FIXME: eval = evil!
		{
			if (!loadScript(type + "/" + name + ".js"))
			{
				//log("Could not load " + name + ".js\n");
				return null;
			}
			else
			{
				log("Successfully loaded " + name + ".js\n");
			}
		}
		
		ServiceManager.Services[fullId] = eval("(new " + name + "('" + name + "', " + id + "))");///FIXME: Can we do this without eval??
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
