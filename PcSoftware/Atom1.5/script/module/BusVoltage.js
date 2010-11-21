
function BusVoltage(name)
{
	this.Module(name);
	BusVoltage.instance_ = this;
}

Extend(BusVoltage, Module);

BusVoltage.instance_ = null;

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

function BusVoltage_SetReportInterval(id, time)
{
	var variables = [
	{ "Time"  : time } ];
	
	BusVoltage.instance_.Module.prototype.SendMessage.call(BusVoltage.instance_, id, "Report_Interval", variables);
	return "OK";
}
RegisterConsoleCommand(BusVoltage_SetReportInterval);
