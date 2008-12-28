
function SimpleDTMF(name, id)
{
	this.CanService(name, id);
}

extend(SimpleDTMF, CanService);

SimpleDTMF.prototype.myLastPhonenumber = null;

SimpleDTMF.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Phonenumber":
		
		this.myLastPhonenumber = canMessage.getData("Number");
		
		if (this.myLastPhonenumber[this.myLastPhonenumber.length-1] == 'f')
		{
			this.myLastPhonenumber = this.myLastPhonenumber.substr(0, this.myLastPhonenumber.length-1);
		}
		
		log(this.myName + ":" + this.myId + "> New phonenumber: " + this.myLastPhonenumber + "\n");
		
		this.callEvent("newPhonenumber", null);
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

SimpleDTMF.prototype.getLastPhonenumber = function()
{
	return this.myLastPhonenumber;
}
