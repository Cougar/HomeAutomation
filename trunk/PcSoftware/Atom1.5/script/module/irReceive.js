
function irReceive(name)
{
	this.Module(name);
}

Extend(irReceive, Module);

irReceive.prototype.ReceiveMessage = function(id, command, variables)
{
	switch (command)
	{
		case "IR":
		{
			var channel = variables["Channel"];
			var data = variables["IRdata"];
			var protocol = variables["Protocol"];
			var status = variables["Status"];
			
			if (status == "Pressed")
			{
				this.EventBase.prototype.Trigger.call(this, "onPressed", id, channel, data, protocol);
			}
			else if (status == "Released")
			{
				this.EventBase.prototype.Trigger.call(this, "onReleased", id, channel, data, protocol);
			}
			else
			{
				Log("Unknown status of IR signal, " + status);
			}
			
			break;
		}
	}
	
	this.Module.prototype.ReceiveMessage.call(this, id, command, variables);
}

