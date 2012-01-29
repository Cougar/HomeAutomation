

function badrumsdimmer(aliasnameSensor, aliasnameDimmer1, aliasnameRotary, aliasnameDimmer2, aliasnameOutput)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	var self = this;
	this.mySensor = aliasnameSensor;
	this.myDimmer1 = aliasnameDimmer1;
	this.myOutput = aliasnameOutput;
	this.myDimmer2 = aliasnameDimmer2;
	this.myRotary = aliasnameRotary;
	this.myMode = 3;

	Module_RegisterToOnMessage(aliasnameSensor, function(alias_name, command, variables) { self.sensorOnMessage(alias_name, command, variables) });
	Module_RegisterToOnMessage(this.myRotary, function(alias_name, command, variables) { self.rotary_OnMessage(alias_name, command, variables) });
	//Module_RegisterToOnChange(this.myRotary, function(alias_name, available,test) { self.rotaryOnline(alias_name, available,test) });
	
	//Module_RegisterToOnChange( aliasnameSensor, function(alias_name, available,test) { self.sensorOnline(alias_name, available,test) });
	//Module_RegisterToOnChange( aliasnameDimmer, function(alias_name, available) { self.dimmerOnline(alias_name, available) });
	
	/* Start interval timer for sending timestamp to network. Arguments are the callback function and time in milliseconds 
	used to make sure an eth-node gets its init packet (600s) */
	this.myTimer = Timer_SetTimer(function(timer) {self.timerUpdate1(timer)}, 5000, true);
	Log("\033[33mbadrumsdimmer created.\033[0m\n");
	
}

/* Declaration of instance variables, for static variables remove prototype */
badrumsdimmer.prototype.myDimmer1 = null;
badrumsdimmer.prototype.myDimmer2 = null;
badrumsdimmer.prototype.myMode = 1;
badrumsdimmer.prototype.mySensor = null;
badrumsdimmer.prototype.myOutput = null;
badrumsdimmer.prototype.myRotary = null;
badrumsdimmer.prototype.outputStatus = "Low";
badrumsdimmer.prototype.myInterval = null;
badrumsdimmer.prototype.oldPwmValue = 0;
badrumsdimmer.prototype.turnOffCnt = 0;
const MovementTimeout = 300;
const AdjustTimeout = 480;
const ButtonTimeout = 900;

badrumsdimmer.prototype.sensorOnline = function(alias_name, available)
{
  
}

badrumsdimmer.prototype.dimmerOnline = function(alias_name, available)
{
  
}

badrumsdimmer.prototype.sensorOnMessage = function(alias_name, command, variables)
{
//Log("\033[33mMessage.\033[0m\n");
	switch (command)
	{
		case "PinStatus":
		{
			
			if (variables["Status"] == "Low") {

				if (this.oldPwmValue == 0) {
					this.oldPwmValue = 255;
				}
				if (this.myMode == 1) {
					Dimmer_AbsoluteFade(this.myDimmer1, 135, this.oldPwmValue);
				} else if (this.myMode == 2) {
					Dimmer_AbsoluteFade(this.myDimmer2, 135, this.oldPwmValue);
				} else {
					Dimmer_AbsoluteFade(this.myDimmer1, 135, this.oldPwmValue);
					Dimmer_AbsoluteFade(this.myDimmer2, 135, this.oldPwmValue);
				}
		  
			} else {
			  //Log("\033[33mknapp hög.\033[0m\n");
			  
//			  Dimmer_AbsoluteFade(this.myDimmerIndication, 129, 0);
			  //Log("\033[33mPin high.\033[0m\n");
			}
			if (this.turnOffCnt <  MovementTimeout) {
				this.turnOffCnt = MovementTimeout;
			}
			break;
		}
	}

}

badrumsdimmer.prototype.timerUpdate1 = function(timer)
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
			//this.myPWM.setPWMValue(0,2);
			Dimmer_AbsoluteFade(this.myDimmer1, 129, 0);
			Dimmer_AbsoluteFade(this.myDimmer2, 129, 0);
			//getDimmerService('Skohylla').absFade(2,129, 0);
		}
	}
}
	
badrumsdimmer.prototype.rotary_OnMessage = function(alias_name, command, variables)
{
	switch (command)
	{
		case "Rotary_Switch":
		{
			this.oldPwmValue = 17*variables["Position"];
			/*for (var i = 0; i < variables["Steps"]; i++)
			{
					
				if (variables["Direction"] == "Clockwise")
				{
					Dimmer_RelativeFade(this.myDimmer, 132, "Increase", 25);
					Log("\033[33mclock.\033[0m\n");
			  
				}
				else
				{
					Dimmer_RelativeFade(this.myDimmer, 132, "Decrease", 25);
					Log("\033[33m!clock.\033[0m\n");
			  
				}
				
			}*/
			if (this.myMode == 1) {
				Dimmer_AbsoluteFade(this.myDimmer1, 135, this.oldPwmValue);
			} else if (this.myMode == 2) {
				Dimmer_AbsoluteFade(this.myDimmer2, 135, this.oldPwmValue);
			} else {
				Dimmer_AbsoluteFade(this.myDimmer1, 135, this.oldPwmValue);
				Dimmer_AbsoluteFade(this.myDimmer2, 135, this.oldPwmValue);
			}
				
			
			
			
			if (this.turnOffCnt <  AdjustTimeout) {
				this.turnOffCnt = AdjustTimeout;
			}
			
			break;
		}
		case "Button":
		{
			if (variables["Status"] == "Pressed")
			{
				
			  
				if (this.myMode == 1) {
					this.myMode =2;
					Log("\033[33mknapp pressed, Mode 2\033[0m\n");
				} else if (this.myMode == 2) {
					this.myMode =3;
					Log("\033[33mknapp pressed, Mode 3\033[0m\n");
				} else {
					this.myMode =1;
					Log("\033[33mknapp pressed, Mode 1\033[0m\n");
				}
				
				if (this.myMode == 1) {
					Dimmer_AbsoluteFade(this.myDimmer1, 135, this.oldPwmValue);
					Dimmer_AbsoluteFade(this.myDimmer2, 135, 0);
				} else if (this.myMode == 2) {
					Dimmer_AbsoluteFade(this.myDimmer2, 135, this.oldPwmValue);
					Dimmer_AbsoluteFade(this.myDimmer1, 135, 0);
				} else {
					Dimmer_AbsoluteFade(this.myDimmer1, 135, this.oldPwmValue);
					Dimmer_AbsoluteFade(this.myDimmer2, 135, this.oldPwmValue);
				}
				
				if (this.turnOffCnt < ButtonTimeout) {
					this.turnOffCnt = ButtonTimeout;
				}
			}
		
			break;
		}
	}

}

