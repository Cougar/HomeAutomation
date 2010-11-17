//    <command id="69" name="Rotary_Switch" type="physical" module="" description="Command to transmit a rotary encoder switch status">
//      <variables>
//        <variable name="SwitchId" type="uint" start_bit="0" bit_length="8" unit="" description=""/>
//        <variable name="Direction" type="uint" start_bit="8" bit_length="8" unit="" description=""/>
//        <variable name="Steps" type="uint" start_bit="16" bit_length="8" unit="" description=""/>
//        <variable name="Position" type="uint" start_bit="24" bit_length="8" unit="" description=""/>
//      </variables>
//    </command>

//    <command id="70" name="Button" type="physical" module="" description="">
//      <variables>
//        <variable name="SwitchId" type="uint" start_bit="0" bit_length="8" unit="" description=""/>
//        <variable name="Status" type="enum" start_bit="8" bit_length="8" unit="" description="">
//          <value id="1" name="Pressed"/>
//          <value id="0" name="Released"/>
//        </variable>
//      </variables>
//    </command>

function RotaryHex(type, name, id)
{
	this.CanService(type, name, id);
	this.myLastDirection = new Array();
	this.myLastSteps = new Array();
	this.myLastPosition = new Array();
	this.myLastButtonStatus = new Array();
}

extend(RotaryHex, CanService);

RotaryHex.prototype.myLastDirection = null;
RotaryHex.prototype.myLastSteps = null;
RotaryHex.prototype.myLastPosition = null;
RotaryHex.prototype.myLastButtonStatus = "Released";


RotaryHex.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Rotary_Switch":
		//log(this.myName + ":" + this.myId + "> New direction: " + canMessage.getData("Direction") + ", steps: " + canMessage.getData("Steps") + ", position: " + canMessage.getData("Position") + "\n");
		this.myLastDirection[canMessage.getData("SwitchId")] = canMessage.getData("Direction");
		this.myLastSteps[canMessage.getData("SwitchId")] = canMessage.getData("Steps");
		this.myLastPosition[canMessage.getData("SwitchId")] = canMessage.getData("Position");
		this.callEvent("newPosition", canMessage.getData("SwitchId"));
		break;
		
		case "Button":
		//log(this.myName + ":" + this.myId + "> New button status: " + canMessage.getData("Status") + "\n");
		this.myLastButtonStatus[canMessage.getData("SwitchId")] = canMessage.getData("Status");
		this.callEvent("newBtnStatus", canMessage.getData("SwitchId"));
		break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

RotaryHex.prototype.getDirection = function(switchId)
{
	return this.myLastDirection[switchId];
}

RotaryHex.prototype.getPosition = function(switchId)
{
	return this.myLastPosition[switchId];
}

RotaryHex.prototype.getSteps = function(switchId)
{
	return this.myLastSteps[switchId];
}

RotaryHex.prototype.getButtonStatus = function(switchId)
{
	return this.myLastButtonStatus[switchId];
}
