

function TouchPanel(aliasnameRGBLED_red,aliasnameRGBLED_green, aliasnameRGBLED_blue, aliasnameTouch, aliasnameDimmer1, aliasnameDimmer2, aliasnameDimmer3, aliasnameDimmer4, aliasnameDimmer5, aliasnameDimmer6,aliasnameDimmer7, aliasnameIR )
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
	this.myDimmer4 = aliasnameDimmer4;
	this.myDimmer5 = aliasnameDimmer5;
	this.myDimmer6 = aliasnameDimmer6;
	this.myDimmer7 = aliasnameDimmer7;
	this.myIR = aliasnameIR;
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
TouchPanel.prototype.myDimmer4 = null;
TouchPanel.prototype.myDimmer5 = null;
TouchPanel.prototype.myDimmer6 = null;
TouchPanel.prototype.myDimmer7 = null;
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
					Dimmer_AbsoluteFade(this.myDimmer4, 132, 255);
					Dimmer_AbsoluteFade(this.myDimmer5, 132, 255);
					Dimmer_AbsoluteFade(this.myDimmer6, 132, 255);
					this.setLED(0,255,0);
					Timer_Cancel(this.myInterval);
					this.myInterval = Timer_SetTimer(function(timer) {self.timerUpdate(timer)}, 5000, true);
					
					
					/* Check if it is time to turn on ljusstake i sovrummet */
					
					if (get_time() > TouchGetTsFromTime("7:00") && get_time() < TouchGetTsFromTime("9:30"))
					{
						TouchstartSend("rf", "NexaRemote", "Dosa_3_On", 4);
					}
					if (get_time() > TouchGetTsFromTime("16:00") && get_time() < TouchGetTsFromTime("22:00"))
					{
						TouchstartSend("rf", "NexaRemote", "Dosa_3_On", 4);
					}
					
					
				}
				else if (gesture == "JOIN")
				{
					//getDimmerService('Badrum').absFade(2,132, 0);
					//getDimmerService('TakHall').absFade(0,132, 0);
					//getDimmerService('TakSovrum').absFade(0,132, 0);
					Dimmer_AbsoluteFade(this.myDimmer1, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer2, 132, 0);
					var shorttime = new Date().getTimeShortFormated();
					/* Remove leading zero if any */
					var data = parseInt(shorttime.substr(0,2));
					//if (data < 22 && data >= 6) {
	   					Dimmer_AbsoluteFade(this.myDimmer3, 132, 0);
					//}
					Dimmer_AbsoluteFade(this.myDimmer4, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer5, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer6, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer7, 132, 0);
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
					Dimmer_AbsoluteFade(this.myDimmer3, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer4, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer5, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer6, 132, 0);
					Dimmer_AbsoluteFade(this.myDimmer7, 132, 0);
					Timer_Cancel(this.myInterval);
					this.myInterval = Timer_SetTimer(function(timer) {self.timerUpdate(timer)}, 5000, true);
				} else if (gesture == "INSERT_LINE")
				{
					Log("Starting Roomba\n");
					this.setLED(255,0,0);
					IROut_SendBurst(this.myIR, "Roomba", "Clean");			
				}

			}
			
			
			break;
		case "Raw":

			break;
	}

}

function TouchGetTsFromTime(time)
{
	var date = new Date().getDateFormated();
	var timestamp = Date.parse(date + " " + time);
	return timestamp/1000;
}

/* Start sending command, could be command to start heating or stop heating */
/* A command is sent <count> number of times to make sure its actually sent */
function TouchstartSend(alias, remote, button, count)
{
	if (count > 0)
	{
//Log("carHeater: Start sending heater command, count="+count);
		sendCounter = count;
		sendTimer_id = Timer_SetTimer(function(timer) {send(alias, remote, button)}, 5000, true);
		send(alias, remote, button);
	}
}

function Touchsend(alias, remote, button)
{
	if (sendCounter > 0)
	{
//Log("carHeater: Sending heater command, count="+sendCounter);
		IROut_SendBurst(alias, remote, button);
	}
	sendCounter--;
	if (sendCounter == 0)
	{
		Timer_Cancel(sendTimer_id);
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


