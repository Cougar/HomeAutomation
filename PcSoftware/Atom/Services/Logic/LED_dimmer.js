
loadScript("Logic/MenuItem.js");

function LED_dimmer(type, name, id)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	this.Service(type, name, id);
}

/* We inherit from Service with this call, it must always be next
   after the constructor */
extend(LED_dimmer, Service);

/* Declaration of instance variables, for static variables remove prototype */


/* The display should regulary update and store the calendar */


LED_dimmer.prototype.myRotaryService = null;
LED_dimmer.prototype.myPWM = null;
LED_dimmer.prototype.myOutput = null;
LED_dimmer.prototype.myInput = null;
LED_dimmer.prototype.pwmValue = 50;
LED_dimmer.prototype.outputStatus = "Low";
LED_dimmer.prototype.myInterval = null;
LED_dimmer.prototype.oldPwmValue = 0;
LED_dimmer.prototype.turnOffCnt = 0;
const MovementTimeout = 120;
const AdjustTimeout = 240;
/*  */
/*  */


/* This function must always be declared, this is where all the startup code
   should be placed. Gets called with arguments like what ids to use etc. */
LED_dimmer.prototype.initialize = function(initialArguments)
{
	/* We must always call the parent initialize */
	this.Service.prototype.initialize.call(this, initialArguments);

	/* This is used for function declarations like the callbacks below */
	var self = this;


	if (!this.myInitialArguments["RotaryHex"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, RotaryHex-config missing from config.\n");
		return;
	}
	
	if (!this.myInitialArguments["output"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, output-config missing from config.\n");
		return;
	}
	if (!this.myInitialArguments["input"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, input-config missing from config.\n");
		return;
	}

	this.myInterval = new Interval(function() { self.timerUpdate() }, 5000);
	this.myInterval.start();

	/* Get the Rotary service that we want from the ServiceManager, it takes type, service name, service id */
	this.myRotaryService = ServiceManager.getService("Can", "RotaryHex", this.myInitialArguments["RotaryHex"]["Id"]);
	/* Add a callback for when the encoder reports a new position */
	this.myRotaryService.registerEventCallback("newPosition", function(args) { self.rotaryPosUpdate(args); });
	/* Add a callback for when the encoder reports a new button status */
	this.myRotaryService.registerEventCallback("newBtnStatus", function(args) { self.rotaryBtnUpdate(args); });
	/* Add a callback for when the service goes online */
	this.myRotaryService.registerEventCallback("online", function(args) { self.rotaryOnline(); });
	/* If the service is already online we should call the handler here */
	this.rotaryOnline();

	//this.myPWM = ServiceManager.getService("Can", "hwPWM", this.myInitialArguments["hwPWM"]["Id"]);
	this.myOutput = ServiceManager.getService("Can", "output", this.myInitialArguments["output"]["Id"]);
	this.myInput = ServiceManager.getService("Can", "input", this.myInitialArguments["input"]["Id"]);
	/* Add a callback for when the PIR reports detection */
	this.myInput.registerEventCallback("newValue", function(args) { self.pirUpdate(args); });
	
}

LED_dimmer.prototype.timerUpdate = function()
{
	if (this.turnOffCnt > 0) {
		this.turnOffCnt -= 5;
		if (this.turnOffCnt < 0) {
			this.turnOffCnt = 0;
		}
		if (this.turnOffCnt == 0) {
			//Turn off light
			log(this.myName + ":" + this.myId + "> Light off.\n");
			this.oldPwmValue = getDimmerValue('Badrum');
			//this.myPWM.setPWMValue(0,2);
			getDimmerService('Badrum').absFade(2,129, 0);
		}
	}
}

LED_dimmer.prototype.rotaryOnline = function()
{
}

LED_dimmer.prototype.rotaryPosUpdate = function(SwitchId)
{
	
	for (var i = 0; i < this.myRotaryService.getSteps(SwitchId); i++)
	{
		if (this.myRotaryService.getDirection(SwitchId) == "Clockwise")
		{
			getDimmerService('Badrum').relFade(2,132, "Increase", 25);
this.pwmValue = this.pwmValue +10;
			if (this.pwmValue > 100) {
				this.pwmValue = 100;
			}
		}
		else
		{
			getDimmerService('Badrum').relFade(2,132, "Decrease", 25);
			this.pwmValue = this.pwmValue -10;
			if (this.pwmValue < 0) {
				this.pwmValue = 0;
			}
		}
	}
	if (this.turnOffCnt <  AdjustTimeout) {
		this.turnOffCnt = AdjustTimeout;
	}
//log(this.myName + ":" + this.myId + "> Sent pwm value: "+ this.pwmValue+"\n");
	//this.myPWM.setPWMValue(this.pwmValue,2);
	
}

LED_dimmer.prototype.rotaryBtnUpdate = function(SwitchId)
{
	if (this.myRotaryService.getButtonStatus(SwitchId) == "Pressed") {
		if (getDimmerValue('Badrum') == 0) { 
			if (this.oldPwmValue == 0) {
				this.oldPwmValue = 50;
			}
			getDimmerService('Badrum').absFade(2,129, 128);
			//this.myPWM.setPWMValue(this.oldPwmValue,2);
		}
		if (this.turnOffCnt <  AdjustTimeout) {
			this.turnOffCnt = AdjustTimeout;
		}
		if (this.myOutput.getValue(0) == "High") { 
			this.myOutput.setPin("Low",0);
		} else {
			this.myOutput.setPin("High",0);
		}
	}
}

LED_dimmer.prototype.pirUpdate = function(SwitchId)
{
	//log(this.myName + ":" + this.myId + "> Value is: "+ this.myInput.getValue(SwitchId)+"\n");
	if (this.myInput.getValue(SwitchId) == "Low") {
		  getDimmerService('Badrum').absFade(1,129, 255);
		//this.myOutput.setPin("High",0);
		log(this.myName + ":" + this.myId + "> Trigg.\n");
		if (getDimmerValue('Badrum') == 0) { 
			log(this.myName + ":" + this.myId + "> Light on.\n");
			if (this.oldPwmValue == 0) {
				this.oldPwmValue = 50;
			}
			//this.myPWM.setPWMValue(this.oldPwmValue,2);
			getDimmerService('Badrum').absFade(2,129, 128);
		}
	} else {
		getDimmerService('Badrum').absFade(1,129, 0);
	}
	if (this.turnOffCnt <  MovementTimeout) {
		this.turnOffCnt = MovementTimeout;
	}
}
