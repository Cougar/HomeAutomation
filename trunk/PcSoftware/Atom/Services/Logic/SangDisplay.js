
loadScript("Logic/MenuItem.js");

function SangDisplay(type, name, id)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	this.Service(type, name, id);
}

/* We inherit from Service with this call, it must always be next
   after the constructor */
extend(SangDisplay, Service);

/* Declaration of instance variables, for static variables remove prototype */

SangDisplay.prototype.myLCDService = null;
SangDisplay.prototype.myRotaryService = null;
SangDisplay.prototype.myTempService = null;
SangDisplay.prototype.myInterval = null;
SangDisplay.prototype.myIntervalAlways = null;

SangDisplay.prototype.mainMenuItem = null;
SangDisplay.prototype.screenSaverMenuItem = null;


/* The currently displayed menuitem */
SangDisplay.prototype.currentMenuItem = null;

/* counter for going to screensaver */
SangDisplay.prototype.screenSaverCnt = null;

/* counter for going to main screen */
SangDisplay.prototype.mainScreenCnt = null;

const outsideTemperatureSensorId = 236;
const insideTemperatureSensorId = 204;
var outsideTemperature = 0;
var insideTemperature= 0;
var CurrentPower = 0;
var CurrentEnergy = 0;

const mainScreenTimeout = 5;
const screenSaverTimeout = 10;


/* This function must always be declared, this is where all the startup code
   should be placed. Gets called with arguments like what ids to use etc. */
SangDisplay.prototype.initialize = function(initialArguments)
{
	/* We must always call the parent initialize */
	this.Service.prototype.initialize.call(this, initialArguments);

	/* This is used for function declarations like the callbacks below */
	var self = this;

	if (!this.myInitialArguments["HD44789"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, HD44789-config missing from config.\n");
		return;
	}
	
	if (!this.myInitialArguments["Rotary"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, Rotary-config missing from config.\n");
		return;
	}
if (!this.myInitialArguments["DS18x20"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, DS18x20-config missing from config.\n");
		return;
	}
if (!this.myInitialArguments["power"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, power-config missing from config.\n");
		return;
	}
	/* Start interval timer for sending timestamp to network. Arguments are the callback function and time in milliseconds 
	used to make sure an eth-node gets its init packet (600s) */
	this.myIntervalAlways = new Interval(function() { self.sendTimeStamp() }, 600000);
	this.myIntervalAlways.start();

	this.myInterval = new Interval(function() { self.timerUpdate() }, 5000);
	this.myInterval.start();

	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	
	
	/* Get the LCD service that we want from the ServiceManager, it takes type, service name, service id */
	this.myLCDService = ServiceManager.getService("Can", "HD44789", this.myInitialArguments["HD44789"]["Id"]);
	/* Add a callback for when the service goes online */
	this.myLCDService.registerEventCallback("online", function(args) { self.lcdOnline(); });
	/* If the service is already online we should call the handler here */
	this.lcdOnline();
	/* Add a callback for when the service goes offline */
	this.myLCDService.registerEventCallback("offline", function(args) { self.lcdOffline(); });
	
	/* Get the Rotary service that we want from the ServiceManager, it takes type, service name, service id */
	this.myRotaryService = ServiceManager.getService("Can", "Rotary", this.myInitialArguments["Rotary"]["Id"]);
	/* Add a callback for when the encoder reports a new position */
	this.myRotaryService.registerEventCallback("newPosition", function(args) { self.rotaryPosUpdate(args); });
	/* Add a callback for when the encoder reports a new button status */
	this.myRotaryService.registerEventCallback("newBtnStatus", function(args) { self.rotaryBtnUpdate(args); });
	/* Add a callback for when the service goes online */
	this.myRotaryService.registerEventCallback("online", function(args) { self.rotaryOnline(); });
	/* If the service is already online we should call the handler here */
	this.rotaryOnline();

	/* Get the LCD service that we want from the ServiceManager, it takes type, service name, service id */
	//this.myTempService = ServiceManager.getService("Can", "DS18x20", this.myInitialArguments["DS18x20"]["Id"]);
	/* Add a callback for when the sensor reports a new value */
	//this.myTempService.registerEventCallback("newValue", function(args) { self.temperatureUpdate(args); });
	/* Add a callback for when the service goes online */
	//this.myTempService.registerEventCallback("online", function(args) { self.tempOnline(); });
	/* If the service is already online we should call the handler here */
	//this.tempOnline();

/* Get the PWM service that we want from the ServiceManager, it takes type, service name, service id */
	this.myPowerService = ServiceManager.getService("Can", "power", this.myInitialArguments["power"]["Id"]);
	/* Add a callback for when the service goes online */
	this.myPowerService.registerEventCallback("online", function(args) { self.PowerOnline(); });
	/* Add a callback for when the sensor reports a new value */
	this.myPowerService.registerEventCallback("newValue", function(args) { self.powerUpdate(); });
	/* If the service is already online we should call the handler here */
	this.PowerOnline();	
	
	/* create the first menu item */
	this.mainMenuItem = new MenuItem(this);


	
this.mainMenuItem.displayData[0] = this.lcdCenterText("Main");

	this.mainMenuItem.doUpdate = function(args) { self.updateMainMenuItem(); };
	this.currentMenuItem = this.mainMenuItem;

	
	/* create the screensaver menu item */
	this.screenSaverMenuItem = new MenuItem(this);
	this.screenSaverMenuItem.displayData[0] = this.lcdCenterText("Saver");
	this.screenSaverMenuItem.displayData[1] = this.lcdCenterText("MenuItem");
this.screenSaverMenuItem.displayData[2] = this.lcdCenterText("");
this.screenSaverMenuItem.displayData[3] = this.lcdCenterText("");
	this.screenSaverMenuItem.doRight = function(args) { self.changeToNext(); };
	this.screenSaverMenuItem.doLeft = function(args) { self.changeToPrev(); };
	this.screenSaverMenuItem.setNextItem(this.mainMenuItem);
	this.screenSaverMenuItem.setPrevItem(this.mainMenuItem);

	/* create the menuitem where you can choose to enter the booking sub-menu */


	/* connect the items as a linked list */

	/* set the function that shall be executed when knob is turned */
	this.mainMenuItem.doRight = function(args) { self.changeToNext(); };
	this.mainMenuItem.doLeft = function(args) { self.changeToPrev(); };
	this.mainMenuItem.setNextItem(this.screenSaverMenuItem);
	this.mainMenuItem.setPrevItem(this.screenSaverMenuItem);
}


SangDisplay.prototype.tempOnline = function()
{
	/* If service is not online do nothing */
	if (this.myTempService.isOnline())
	{
		/* Set report interval to 5 seconds */
		this.myTempService.setReportInterval(5);
	}
}

SangDisplay.prototype.temperatureUpdate = function(sensorId)
{
	if (sensorId == outsideTemperatureSensorId) {
		outsideTemperature = this.myTempService.getValue(sensorId).toFixed(2).toString();
	}
	if (sensorId == insideTemperatureSensorId) {
		insideTemperature = this.myTempService.getValue(sensorId).toFixed(2).toString();
	}
}
SangDisplay.prototype.PowerOnline = function()
{
	/* If service is not online do nothing */
	if (this.myPowerService.isOnline())
	{
		log("Power is online!\n");
		/* Set report interval to 2 seconds */
		this.myPowerService.setReportInterval(2);
	}
}

SangDisplay.prototype.powerUpdate = function()
{
	CurrentPower = this.myPowerService.getPower();
	CurrentEnergy = this.myPowerService.getEnergy();
	if (this.currentMenuItem == this.mainMenuItem) 
	{
		this.updateDisplay();
	}
}

SangDisplay.prototype.changeToDesc = function()
{
	if (this.currentMenuItem.descItem)
	{
		this.currentMenuItem = this.currentMenuItem.descItem;
	}
}

SangDisplay.prototype.changeToNext = function()
{
	if (this.currentMenuItem.nextItem)
	{
		this.currentMenuItem = this.currentMenuItem.nextItem;
	}
}

SangDisplay.prototype.changeToPrev = function()
{
	if (this.currentMenuItem.prevItem)
	{
		this.currentMenuItem = this.currentMenuItem.prevItem;
	}
}



SangDisplay.prototype.updateMainMenuItem = function()
{
	var date = new Date();
	var dateAndTime = "" + date.getTimeShortFormated();
	this.mainMenuItem.displayData[0] = this.lcdCenterText(dateAndTime);
this.mainMenuItem.displayData[1] = "";
	this.mainMenuItem.displayData[2] = this.lcdCenterText(""+getSensorValue('Vardagsrumssensor').toFixed(1).toString() + " C " + getSensorValue('Västersensor').toFixed(1).toString() + " C");
	this.mainMenuItem.displayData[3] = this.lcdCenterText(""+CurrentPower.toString()+" Watt "+(CurrentEnergy/1000).toString() + "kWh");
this.myLCDService.setBacklight(10);
}



SangDisplay.prototype.replaceAumlauts = function(intext)
{
	intext = intext.replace(/Å/, String.fromCharCode(1));
	intext = intext.replace(/å/, String.fromCharCode(0));
	intext = intext.replace(/Ä/, String.fromCharCode(2));
	intext = intext.replace(/ä/, String.fromCharCode(225));
	intext = intext.replace(/Ö/, String.fromCharCode(3));
	intext = intext.replace(/ö/, String.fromCharCode(239));
	//intext = intext.replace(/:/, ";");
	//intext = intext.replace(/,/, ".");
	return intext;
}



SangDisplay.prototype.rotaryOnline = function()
{
}

SangDisplay.prototype.rotaryPosUpdate = function(SwitchId)
{
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	for (var i = 0; i < this.myRotaryService.getSteps(SwitchId); i++)
	{
		if (this.myRotaryService.getDirection(SwitchId) == "Clockwise")
		{
			if (this.currentMenuItem.doRight)
			{
				this.currentMenuItem.doRight();
			}
		}
		else
		{
			if (this.currentMenuItem.doLeft)
			{
				this.currentMenuItem.doLeft();
			}
		}
	}
	
	this.updateDisplay();
}

SangDisplay.prototype.rotaryBtnUpdate = function(SwitchId)
{
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	if (this.myRotaryService.getButtonStatus(SwitchId) == "Released")
	{
		if (this.currentMenuItem.doPress)
		{
			this.currentMenuItem.doPress();
		}
		this.updateDisplay();
	}
}

SangDisplay.prototype.updateDisplay = function()
{
	/* see if the current menuitem wants to update itself */
	if (this.currentMenuItem.doUpdate)
	{
		this.currentMenuItem.doUpdate();
	}

	/* Clear the LCD screen */
	//this.myLCDService.clearScreen();
	
	/* send the data for the current menuitem to display */
	for (var i = 0; i < this.currentMenuItem.displayData.length; i++)
	{
		this.myLCDService.printText(0, i, this.currentMenuItem.displayData[i]);
	}
	
}

SangDisplay.prototype.lcdCenterText = function(text)
{
	returnText = text;
	var displayWidth = 20;	//this.myLCDService.getWidth();
	/* pad on both sides with spaces to displayWidth */
	returnText = text.pad(displayWidth, " ", 2);
	
	return returnText;
}

SangDisplay.prototype.lcdOffline = function()
{
}

SangDisplay.prototype.lcdOnline = function()
{
	/* If service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		/* Clear the LCD screen */
		this.myLCDService.clearScreen();
		/* Set backlight to max */
		this.myLCDService.setBacklight(0);

		this.timerUpdate();
	}
}


SangDisplay.prototype.sendTimeStamp = function()
{
	var canMessage = new CanNMTMessage("nmt", "Time");
	canMessage.send();
}

SangDisplay.prototype.timerUpdate = function()
{
	/* If LCD service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		this.screenSaverCnt++;
		this.mainScreenCnt++;
		if (this.mainScreenCnt > mainScreenTimeout/5 && this.currentMenuItem != this.screenSaverMenuItem)
		{
			/* Go to main screen (time) */
			this.currentMenuItem = this.mainMenuItem;

			/* update the info on display */
			this.updateDisplay();

			this.mainScreenCnt = 0;
this.myLCDService.setBacklight(10);
		}
		
		if (this.screenSaverCnt > screenSaverTimeout/5)
		{
			/* Go to screensaver menu */
			this.currentMenuItem = this.screenSaverMenuItem;
this.myLCDService.setBacklight(0);
			/* update the info on display */
			this.updateDisplay();
			
			this.screenSaverCnt = 0;
		}

		

	}
}

