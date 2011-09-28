

function TouchPanel(aliasnameRGBLED_red,aliasnameRGBLED_green, aliasnameRGBLED_blue, aliasnameTouch, aliasnameDimmer1, aliasnameDimmer2, aliasnameDimmer3)
{

/* Declaration of instance variables, for static variables remove prototype */

	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	var self = this;
	this.myTouch = aliasnameTouch;
	this.myRGBLED_red = aliasnameRGBLED_red;
	this.myRGBLED_blue = aliasnameRGBLED_blue;
	this.myRGBLED_green = aliasnameRGBLED_green;
	this.myDimmer1 = aliasnameDimmer1;
	this.myDimmer2 = aliasnameDimmer2;
	this.myDimmer3 = aliasnameDimmer3;
	this.myTouchMode = "Gesture";
	this.myTouchRawX = 0;
	this.myTouchRawY = 0;
	this.myTouchRawYlast = 0;
	Module_RegisterToOnMessage(aliasnameTouch, function(alias_name, command, variables) { self.touchOnMessage(alias_name, command, variables) });
	
	/* Start interval timer for sending timestamp to network. Arguments are the callback function and time in milliseconds 
	used to make sure an eth-node gets its init packet (600s) */
	this.myTimer = Timer_SetTimer(function(timer) {self.timerUpdate(timer)}, 5000, true);
	Log("\033[33mTouchpanel created.\033[0m");
	
}

/* Declaration of instance variables, for static variables remove prototype */
TouchPanel.prototype.myDimmer1 = null;
TouchPanel.prototype.myDimmer2 = null;
TouchPanel.prototype.myDimmer3 = null;
TouchPanel.prototype.myTouch = null;
TouchPanel.prototype.myRGBLED_red = null;
TouchPanel.prototype.myRGBLED_green = null;
TouchPanel.prototype.myRGBLED_blue = null;
//TouchPanel.prototype.outputStatus = "Low";
TouchPanel.prototype.myInterval = null;
//TouchPanel.prototype.oldPwmValue = 0;
//TouchPanel.prototype.turnOffCnt = 0;

TouchPanel.prototype.myTouchMode = null;
TouchPanel.prototype.myTouchRawX = null;
TouchPanel.prototype.myTouchRawY = null;
TouchPanel.prototype.myTouchRawYlast = null;

const XMIN = 40;
const XMAX = 215;
//const XLIMIT1 = XMIN + (XMAX-XMIN)/3; 
//const XLIMIT2 = XMIN*2 + (XMAX-XMIN)/3;
const XLIMIT1 = 100; 
const XLIMIT2 = 155;



TouchPanel.prototype.touchOnMessage = function(alias_name, command, variables)
{
//Log("\033[33mMessage.\033[0m\n");
	switch (command)
	{
		case "Gesture":
			var gesture = null;
			gesture = Touch_lookupGesture(variables["f1"],variables["f2"],variables["f3"],variables["f4"],variables["f5"],variables["f6"],variables["f7"],variables["f8"],variables["f9"]);
			if (this.myTouchMode == "Gesture")
			{
				var self = this;
				Log("New gesture: "+gesture);
				if (gesture == "INSERT")
				{
					////getDimmerService('Badrum').absFade(2,132, 255);
					//getDimmerService('TakHall').absFade(0,132, 255);
					//getDimmerService('TakSovrum').absFade(0,132, 255);
					//Dimmer_AbsoluteFade(this.myDimmer1, 132, 255);
					Dimmer_AbsoluteFade(this.myDimmer2, 132, 255);
					Dimmer_AbsoluteFade(this.myDimmer3, 132, 255);
					this.setLED(0,255,0);
					Timer_Cancel(this.myInterval);
					this.myInterval = Timer_SetTimer(function(timer) {self.timerUpdate(timer)}, 5000, true);
				}
				else if (gesture == "JOIN")
				{
					//getDimmerService('Badrum').absFade(2,132, 0);
					//getDimmerService('TakHall').absFade(0,132, 0);
					//getDimmerService('TakSovrum').absFade(0,132, 0);
					Dimmer_AbsoluteFade(this.myDimmer1, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer2, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer3, 132, 0);
					this.setLED(0,0,255);
					Timer_Cancel(this.myInterval);
					this.myInterval = Timer_SetTimer(function(timer) {self.timerUpdate(timer)}, 5000, true);
					
				}
				/*else if (gesture == "UNDO")
				{
					Log("Going to raw mode\n");
					this.setLED(255,0,0);
					this.myTouchMode = "Raw";
				
				} */
				else if (gesture == "COPY")
				{
					this.setLED(0,255,255);
					//getDimmerService('Badrum').absFade(2,132, 0);
					//getDimmerService('TakHall').absFade(0,117, 0);
					//getDimmerService('TakSovrum').absFade(0,105, 0);
					Dimmer_AbsoluteFade(this.myDimmer1, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer2, 117, 0);
					Dimmer_AbsoluteFade(this.myDimmer3, 103, 0);
					Timer_Cancel(this.myInterval);
					this.myInterval = Timer_SetTimer(function(timer) {self.timerUpdate(timer)}, 5000, true);
				}
			}
			
			
			break;
		case "Raw":

			break;
	}

}

TouchPanel.prototype.timerUpdate = function(timer)
{
	this.myTouchMode = "Gesture"
	Log("Switching back to Gesture mode\n");
	this.setLED(0,0,0);
	Timer_Cancel(timer);
	//this.myInterval = Timer_SetTimer(function(timer) {self.updateDisplay(timer)}, this.currentMenuItem.UpdateTime, true);
}


TouchPanel.prototype.setLED = function(red, green, blue)
{
	//log("setting LED\n");
	softPWM_SetPWMValue(this.myRGBLED_red, 10000-(red&0xff)*39);
	softPWM_SetPWMValue(this.myRGBLED_green, 10000-(green&0xff)*39);
	softPWM_SetPWMValue(this.myRGBLED_blue, 10000-(blue&0xff)*39);
}


/*

TouchPanel.prototype.incomingStatusChanged = function()
{
	if (this.myTouchMode == "Raw")
	{
		if (this.myTouchPanelService.getLastState() == "Pressed")
		{
			this.myTouchRawX = this.myTouchPanelService.getLastX();
			this.myTouchRawY = this.myTouchPanelService.getLastY();
			this.myTouchRawYlast = this.myTouchPanelService.getLastY();
		}
		else if (this.myTouchPanelService.getLastState() == "Released")
		{
			*//*var currX = this.myTouchPanelService.getLastX();
			var currY = this.myTouchPanelService.getLastY();
			log("stored x "+this.myTouchRawX+" currx "+currX+" stored y "+this.myTouchRawY+" curry "+currY+"\n");
			if (this.myTouchRawX < currX+8 && this.myTouchRawX > currX-8 && this.myTouchRawY < currY+8 && this.myTouchRawY > currY-8)
			{
				log("activating next lightdevice\n");
				if (this.currentLightDevice.nextItem)
				{
					this.currentLightDevice = this.currentLightDevice.nextItem;
					this.currentLightDevice.doActivate();
				}
			}
*//*
//log("starting timer\n");
		this.myInterval.start();
		}
	}
}

TouchPanel.prototype.incomingRaw = function()
{
	
	if (this.myTouchMode == "Raw")
	{
		if (this.myTouchPanelService.getLastState() == "Pressed")
		{
			var yDiff = this.myTouchPanelService.getLastY() - this.myTouchRawYlast;
			//var xPos = this.myTouchPanelService.getLastX();
			if (yDiff > 0)
			{	
				if (this.myTouchRawX < XLIMIT1) {
				//this.currentLightDevice.doRelIncrease(yDiff*2)
					getDimmerService('Badrum').relFade(2,132, "Increase", yDiff*2);
				} else if (this.myTouchRawX < XLIMIT2) {
					getDimmerService('TakHall').relFade(0,132, "Increase", yDiff*2);
				} else {
					getDimmerService('TakSovrum').relFade(0,132, "Increase", yDiff*2);
				}
			}
			else if (yDiff < 0)
			{
				if (this.myTouchRawX < XLIMIT1) {
				//this.currentLightDevice.doRelIncrease(yDiff*2)
					getDimmerService('Badrum').relFade(2,132, "Decrease", yDiff*2);
				} else if (this.myTouchRawX < XLIMIT2) {
					getDimmerService('TakHall').relFade(0,132, "Decrease", yDiff*2);
				} else {
					getDimmerService('TakSovrum').relFade(0,132, "Decrease", yDiff*2);
				}
			}
			this.myTouchRawYlast = this.myTouchPanelService.getLastY();
		}
		//this.myInterval.stop();
		//this.myInterval.start();
	}
}
*/


