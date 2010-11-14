
function BusVoltage(name)
{
	this.Module(name);
}

Extend(BusVoltage, Module);

BusVoltage.prototype.ReceiveMessage = function(id, command, variables)
{
	switch (command)
	{
		case "Voltage":
		{
			this.EventBase.prototype.Trigger.call(this, "onVoltage", id, variables["SensorId"], variables["value"]);
			break;
		}
		case "Report_Interval":
		{
			this.EventBase.prototype.Trigger.call(this, "onReportInterval", id, variables["Time"]);
			break;
		}
	}
	
	this.Module.prototype.ReceiveMessage.call(this, id, command, variables);
}

BusVoltage.prototype.SetReportInterval = function(id, time)
{
	var variables = [
	{ "Time"  : time } ];
	
	this.Module.prototype.SendMessage.call(this, id, "Report_Interval", variables);
}
