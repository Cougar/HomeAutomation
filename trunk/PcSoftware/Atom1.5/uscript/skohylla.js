

function skohylla(aliasnameSensor, aliasnameDimmer)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	var self = this;
	this.mySensor = aliasnameSensor;
	this.myDimmer = aliasnameDimmer;

	Module_RegisterToOnMessage(aliasnameSensor, function(alias_name, command, variables) { self.sensorOnMessage(alias_name, command, variables) });
	Module_RegisterToOnChange( aliasnameSensor, function(alias_name, available,test) { self.sensorOnline(alias_name, available,test) });
	Module_RegisterToOnChange( aliasnameDimmer, function(alias_name, available) { self.dimmerOnline(alias_name, available) });
	
	/* Start interval timer for sending timestamp to network. Arguments are the callback function and time in milliseconds 
	used to make sure an eth-node gets its init packet (600s) */
	this.myTimer = Timer_SetTimer(function(timer) {self.timerUpdate(timer)}, 5000, true);
	Log("\033[33mskohylla created.\033[0m\n");
	
}

/* Declaration of instance variables, for static variables remove prototype */
skohylla.prototype.myDimmer = null;
skohylla.prototype.mySensor = null;
skohylla.prototype.outputStatus = "Low";
skohylla.prototype.myInterval = null;
skohylla.prototype.oldPwmValue = 0;
skohylla.prototype.turnOffCnt = 0;
const SkoMovementTimeout = 100;

skohylla.prototype.sensorOnline = function(alias_name, available)
{
  
}

skohylla.prototype.dimmerOnline = function(alias_name, available)
{
  
}

skohylla.prototype.sensorOnMessage = function(alias_name, command, variables)
{
//Log("\033[33mMessage.\033[0m\n");
	switch (command)
	{
		case "PinStatus":
		{
			
			if (variables["Status"] == "Low") {
			  //Log("\033[33mPin low.\033[0m\n");
				var last_value_string = Storage_GetParameter("LastValues", this.myDimmer);
				var last_value = eval("(" + last_value_string + ")");
				if (last_value["Level"]["value"] == 0) { 
				      //log(this.myName + ":" + this.myId + "> Light on.\n");
					if (this.oldPwmValue == 0) {
						this.oldPwmValue = 255;
					}
					Dimmer_AbsoluteFade(this.myDimmer, 129, this.oldPwmValue);
				}
		  
			} else {
			  //Log("\033[33mPin high.\033[0m\n");
			}
			if (this.turnOffCnt <  SkoMovementTimeout) {
				this.turnOffCnt = SkoMovementTimeout;
			}
			break;
		}
	}

}

skohylla.prototype.timerUpdate = function(timer)
{
  //Log("\033[33mTimer.\033[0m\n");
	if (this.turnOffCnt > 0) {
		this.turnOffCnt -= 5;
		if (this.turnOffCnt < 0) {
			this.turnOffCnt = 0;
		}
		if (this.turnOffCnt == 0) {
			//Turn off light
			//log(this.myName + ":" + this.myId + "> Light off.\n");
			var last_value_string = Storage_GetParameter("LastValues", this.myDimmer);
			var last_value = eval("(" + last_value_string + ")");
			this.oldPwmValue = last_value["Level"]["value"];
			//this.myPWM.setPWMValue(0,2);
			Dimmer_AbsoluteFade(this.myDimmer, 132, 0);
			//getDimmerService('Skohylla').absFade(2,129, 0);
		}
	}
}


