
function SimpleDTMF(type, name, id)
{
	this.CanService(type, name, id);
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
		
		this.myLastPhonenumber = canMessage.getData("Number").toLowerCase();
		
		if (this.myLastPhonenumber == "b00c") // No number sent
		{
			log(this.myName + ":" + this.myId + "> New phonenumber: No number sent\n");
			this.myLastPhonenumber = "None";
			this.callEvent("newNoPhonenumber", null);
		}
		if (this.myLastPhonenumber == "b10c") // Hidden number sent
		{
			log(this.myName + ":" + this.myId + "> New phonenumber: Hidden number sent\n");
			this.myLastPhonenumber = "Hidden";
			this.callEvent("newHiddenPhonenumber", null);
		}
		else if (this.myLastPhonenumber[0] == 'a')
		{
			this.myLastPhonenumber = this.myLastPhonenumber.ltrim('a');
			
			var pos = this.myLastPhonenumber.indexOf('c');
		
			this.myLastPhonenumber = this.myLastPhonenumber.substr(0, pos);
		
			log(this.myName + ":" + this.myId + "> New phonenumber: " + this.myLastPhonenumber + "\n");
			this.callEvent("newPhonenumber", null);
		}
		else
		{
			this.myLastPhonenumber = this.myLastPhonenumber.trim('d');
			
			var pos = this.myLastPhonenumber.indexOf('c');
		
			this.myLastPhonenumber = this.myLastPhonenumber.substr(0, pos);
		
			log(this.myName + ":" + this.myId + "> New phonenumber: " + this.myLastPhonenumber + "\n");
			this.callEvent("newPhonenumber", null);
		
			//log(this.myName + ":" + this.myId + "> Received malformed phonenumber: " + this.myLastPhonenumber + "\n");
		}
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

SimpleDTMF.prototype.getLastPhonenumber = function()
{
	return this.myLastPhonenumber;
}
