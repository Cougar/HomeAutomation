
function CanNMTMessage(className, commandName, data)
{
	this.myClassName = className;
	this.myCommandName = commandName;
	
	if (!data)
		data = new Array();

	this.myData = data;
}

CanNMTMessage.prototype.myClassName = null;
CanNMTMessage.prototype.myCommandName = null;
CanNMTMessage.prototype.myData = null;

CanNMTMessage.prototype.setClassName = function(className)
{
	this.myClassName = className;
}

CanNMTMessage.prototype.getClassName = function()
{
	return this.myClassName;
}

CanNMTMessage.prototype.setCommandName = function(commandName)
{
	this.myCommandName = commandName;
}

CanNMTMessage.prototype.getCommandName = function()
{
	return this.myCommandName;
}

CanNMTMessage.prototype.setData = function(name, value)
{
	this.myData[name] = value;
}

CanNMTMessage.prototype.getData = function(name)
{
	return this.myData[name];
}

CanNMTMessage.prototype.getDataString = function()
{
	var str = "";
	
	for (var key in this.myData)
	{
		str += key + ":" + this.myData[key] + ",";
	}
	
	return str.rtrim(",");
}

CanNMTMessage.prototype.toString = function()
{
	return "[CanNMTMessage] " + 	this.getClassName() + ", " + 
					this.getCommandName() + ", " + 
					this.getDataString();
					
}
