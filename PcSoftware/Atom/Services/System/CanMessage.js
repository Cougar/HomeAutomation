
function CanMessage(className, directionFlag, moduleName, moduleId, commandName, data)
{
	this.myClassName = className;
	this.myDirectionFlag = directionFlag;
	this.myModuleName = moduleName;
	this.myModuleId = moduleId;
	this.myCommandName = commandName;
	
	if (!data)
		data = new Array();

	this.myData = data;
}

CanMessage.prototype.myClassName = null;
CanMessage.prototype.myDirectionFlag = null;
CanMessage.prototype.myModuleName = null;
CanMessage.prototype.myModuleId = null;
CanMessage.prototype.myCommandName = null;
CanMessage.prototype.myData = null;

CanMessage.prototype.setClassName = function(className)
{
	this.myClassName = className;
}

CanMessage.prototype.getClassName = function()
{
	return this.myClassName;
}

CanMessage.prototype.setDirectionFlag = function(directionFlag)
{
	this.myDirectionFlag = directionFlag;
}

CanMessage.prototype.getDirectionFlag = function()
{
	return this.myDirectionFlag;
}

CanMessage.prototype.setModuleName = function(moduleName)
{
	this.myModuleName = moduleName;
}

CanMessage.prototype.getModuleName = function()
{
	return this.myModuleName;
}

CanMessage.prototype.setModuleId = function(moduleId)
{
	this.myModuleId = moduleId;
}

CanMessage.prototype.getModuleId = function()
{
	return this.myModuleId;
}

CanMessage.prototype.setCommandName = function(commandName)
{
	this.myCommandName = commandName;
}

CanMessage.prototype.getCommandName = function()
{
	return this.myCommandName;
}

CanMessage.prototype.setData = function(name, value)
{
	this.myData[name] = value;
}

CanMessage.prototype.getData = function(name)
{
	return this.myData[name];
}

CanMessage.prototype.getDataString = function()
{
	var str = "";
	
	for (var key in this.myData)
	{
		str += key + ":" + this.myData[key] + ",";
	}
	
	return str.rtrim(",");
}

CanMessage.prototype.toString = function()
{
	return "[CanMessage] " + 	this.getClassName() + ", " + 
					this.getDirectionFlag() + ", " + 
					this.getModuleName() + ", " + 
					this.getModuleId() + ", " + 
					this.getDataString();
					
}
