
IRIn_ModuleName     = "irReceive";
IRIn_Channels       = function() { return [ 0 ]; };

function IRIr_MessageAliasLookup(module_id, command, variables)
{
    var aliases_data = {};
    
    aliases_data = Module_LookupAliases({
        "module_name" : IRIn_ModuleName,
        "module_id"   : module_id,
        "channel"     : channel
    });
    
    return aliases_data;
}
Module_RegisterMessageAliasLookup(IROut_ModuleName, IROut_MessageAliasLookup);













function irReceive(name)
{
	this.Module(name);
	irReceive.instance_ = this;
}

Extend(irReceive, Module);

irReceive.instance_ = null;

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

