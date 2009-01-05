
function Debug(type, name, id)
{
	this.CanService(type, name, id);
	
	this.myString = "";
}

extend(Debug, CanService);

Debug.prototype.myString = null;

Debug.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Printf":
		this.myString += unescape(canMessage.getData("Text"));

		if (this.myString[this.myString.length-1] == '\n')
		{
			log(this.myName + ":" + this.myId + "> Printf: " + this.myString);
			this.myString = "";
		}
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}
