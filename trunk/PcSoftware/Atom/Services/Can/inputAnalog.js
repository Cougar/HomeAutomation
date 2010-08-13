
function inputAnalog(type, name, id)
{
	this.CanService(type, name, id);
}

extend(inputAnalog, CanService);

inputAnalog.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "BLIPPBLOPP":
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

/* Arguments: channel 0-3, pullupenable true/false, refenable true/false */
inputAnalog.prototype.PeriodicConfig = function(Channel, Periodicity, VoltageThes, PullupEnable, ReferenceEnable)
{
	var canMessage = new CanMessage("sns", "From_Owner", this.myName, this.myId, "AnalogConfig");
	canMessage.setData("SensorId", Channel);
	canMessage.setData("Setting", "PeriodicMeasure");	//config as periodic read
	canMessage.setData("Pullup", PullupEnable ? "Enable" : "Disable");
	canMessage.setData("Reference", ReferenceEnable ? "Enable" : "Disable");
	canMessage.setData("VoltageLowTh", VoltageThes);	//theshold used for triggering sending
	canMessage.setData("VoltageHighTh", 0);				//not used in this config
	canMessage.setData("Periodicity", Periodicity);		//how often to send data minimum
	sendMessage(canMessage);
}

/* Arguments: channel 0-3, pullupenable true/false, refenable true/false */
inputAnalog.prototype.DigialInputConfig = function(Channel, VoltageLowThes, VoltageHighThes, PullupEnable, ReferenceEnable)
{
	var canMessage = new CanMessage("sns", "From_Owner", this.myName, this.myId, "AnalogConfig");
	canMessage.setData("SensorId", Channel);
	canMessage.setData("Setting", "DigitalInput");			//config as digital input
	canMessage.setData("Pullup", PullupEnable ? "Enable" : "Disable");
	canMessage.setData("Reference", ReferenceEnable ? "Enable" : "Disable");
	canMessage.setData("VoltageLowTh", VoltageLowThes);		//low level theshold
	canMessage.setData("VoltageHighTh", VoltageHighThes);	//high level theshold
	canMessage.setData("Periodicity", 0);					//not used in this config
	sendMessage(canMessage);
}

