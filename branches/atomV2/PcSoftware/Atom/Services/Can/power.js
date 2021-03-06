
function power(type, name, id)
{
	this.CanService(type, name, id);
	
	this.myLastValue = new Array();
}

extend(power, CanService);

power.prototype.myReportInterval = null;
power.prototype.myLastValuePower = null;
power.prototype.myLastValuePower4 = null;
power.prototype.myLastValuePower32 = null;
power.prototype.myLastValueEngergy = null;

power.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "ElectricPower":
		//log(this.myName + ":" + this.myId + "> New value: " + canMessage.getData("Value") + "\n");
		this.myLastValuePower = canMessage.getData("Power");
		this.myLastValuePower4 = canMessage.getData("Power4");
		this.myLastValuePower32 = canMessage.getData("Power32");
		this.myLastValueEnergy = canMessage.getData("EnergySum");
		this.callEvent("newValue");
		break;
		
		case "Report_Interval":
		log(this.myName + ":" + this.myId + "> New report interval: " + canMessage.getData("Time") + "\n");
		this.myReportInterval = canMessage.getData("Time");
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

power.prototype.getPower = function()
{
	return this.myLastValuePower;
}
power.prototype.getPower4 = function()
{
	return this.myLastValuePower4;
}
power.prototype.getPower32 = function()
{
	return this.myLastValuePower32;
}
power.prototype.getEnergy = function()
{
	return this.myLastValueEnergy;
}
power.prototype.setEnergy = function(energy)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "setEnergy");
	canMessage.setData("Energy", energy);
	sendMessage(canMessage);
}

power.prototype.setReportInterval = function(time)
{
	var canMessage = new CanMessage("sns", "To_Owner", this.myName, this.myId, "Report_Interval");
	canMessage.setData("Time", time);
	sendMessage(canMessage);
}
