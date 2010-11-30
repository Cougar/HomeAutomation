
function irTransmit(name)
{
	this.Module(name);
	irTransmit.instance_ = this;
}

Extend(irTransmit, Module);

irTransmit.instance_ = null;

function irTransmit_SendCode(id, channel, status, protocol, data)
{
	var variables = [
	{ "Channel"  : channel },
	{ "Protocol" : protocol },
	{ "IRdata"   : data },
	{ "Status"   : status } ];
	
	irTransmit.instance_.Module.prototype.SendMessage.call(irTransmit.instance_, id, "IR", variables);
	return "OK";
}
RegisterConsoleCommand(irTransmit_SendCode, function(args) { return StandardAutocomplete(args, irTransmit.instance_.GetAvailableIds(), [ 0], [ "Pressed", "Released", "Burst" ], [ "RC5", "RC6", "RCMM", "SIRC", "Sharp", "NEC", "Samsung", "Marantz", "Panasonic", "Sky", "Nexa2" ]); });
