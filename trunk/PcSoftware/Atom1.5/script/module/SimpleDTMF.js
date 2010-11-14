
function SimpleDTMF(name)
{
	this.Module(name);
}

Extend(SimpleDTMF, Module);

SimpleDTMF.prototype.ReceiveMessage = function(id, command, variables)
{
	switch (command)
	{
		case "Phonenumber":
		{
			var phonenumber = variables["Number"];
			
			if (phonenumber == "b00c") // No number sent
			{
				this.EventBase.prototype.Trigger.call(this, "onUnknownPhonenumber", id);
			}
			else if (phonenumber == "b10c") // Hidden number sent
			{
				this.EventBase.prototype.Trigger.call(this, "onHiddenPhonenumber", id);
			}
			else if (phonenumber[0] == 'a')
			{
				phonenumber = phonenumber.substr(0, phonenumber.indexOf('c')).ltrim('a');
				
				this.EventBase.prototype.Trigger.call(this, "onPhonenumber", id, phonenumber);
			}
			else
			{
				phonenumber = phonenumber.substr(0, phonenumber.indexOf('c')).trim('d');
				
				this.EventBase.prototype.Trigger.call(this, "onPhonenumber", id, phonenumber);
			}
			
			break;
		}
	}
	
	this.Module.prototype.ReceiveMessage.call(this, id, command, variables);
}
