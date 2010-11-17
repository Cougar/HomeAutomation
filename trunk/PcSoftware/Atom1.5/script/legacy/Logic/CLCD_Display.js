
loadScript("Personal/cLCD_MenuItems/MainMenuItem.js");
loadScript("Personal/cLCD_MenuItems/DimmerMenuItem.js");
loadScript("Personal/cLCD_MenuItems/SensorMenuItem.js");

function CLCD_Display(type, name, id)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	this.Service(type, name, id);
}

/* We inherit from Service with this call, it must always be next
   after the constructor */
extend(CLCD_Display, Service);

/* Declaration of instance variables, for static variables remove prototype */

CLCD_Display.prototype.myCLCDService = null;
CLCD_Display.prototype.myRotaryService = null;

CLCD_Display.prototype.myInterval = null;
CLCD_Display.prototype.myIntervalAlways = null;
CLCD_Display.prototype.myIntervalScreenSaver = null;

CLCD_Display.prototype.MainMenuItem = null;
CLCD_Display.prototype.SensorMenuItem = null;
CLCD_Display.prototype.DimmerMenuItem = null;

/* The currently displayed menuitem */
CLCD_Display.prototype.currentMenuItem = null;

/* counter for going to screensaver */
CLCD_Display.prototype.screenSaverCnt = null;

/* counter for going to main screen */
CLCD_Display.prototype.mainScreenCnt = null;

const Display_mainScreenTimeout = 20;
const Display_screenSaverTimeout = 25;

CLCD_Display.prototype.defaultBacklight = 10;

/* This function must always be declared, this is where all the startup code
   should be placed. Gets called with arguments like what ids to use etc. */
CLCD_Display.prototype.initialize = function(initialArguments)
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

	/* Start interval timer for sending timestamp to network. Arguments are the callback function and time in milliseconds 
	used to make sure an eth-node gets its init packet (600s) */
	this.myIntervalAlways = new Interval(function() { self.sendTimeStamp() }, 600000);
	this.myIntervalAlways.start();

	this.myInterval = new Interval(function() { self.updateDisplay() }, 5000);
	this.myInterval.start();

	this.myIntervalScreenSaver = new Interval(function() { self.timerUpdate() }, 5000);
	this.myIntervalScreenSaver.start();

	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;

	
	/* Get the LCD service that we want from the ServiceManager, it takes type, service name, service id */
	this.myCLCDService = ServiceManager.getService("Can", "HD44789", this.myInitialArguments["HD44789"]["Id"]);
	/* Add a callback for when the service goes online */
	this.myCLCDService.registerEventCallback("online", function(args) { self.lcdOnline(); });
	/* If the service is already online we should call the handler here */
	this.lcdOnline();
	/* Add a callback for when the service goes offline */
	this.myCLCDService.registerEventCallback("offline", function(args) { self.lcdOffline(); });
	
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

		/* create the first menu item */
	this.MainMenuItem = new MainMenuItem(this, this.myCLCDService);
this.SensorMenuItem = new SensorMenuItem(this, this.myCLCDService);
this.DimmerMenuItem = new DimmerMenuItem(this, this.myCLCDService);
	
	this.currentMenuItem = this.MainMenuItem;

	
	/* create the screensaver menu item */
	this.SensorMenuItem.LeftItem=this.DimmerMenuItem;
	this.SensorMenuItem.RightItem=this.MainMenuItem;

	/* set the function that shall be executed when knob is turned */
	/* create the screensaver menu item */
	this.MainMenuItem.LeftItem=this.SensorMenuItem;
	this.MainMenuItem.RightItem=this.DimmerMenuItem;
	this.DimmerMenuItem.RightItem=this.SensorMenuItem;
	this.DimmerMenuItem.LeftItem=this.MainMenuItem;
}

// 
CLCD_Display.prototype.changeToLeft = function()
{
	if (this.currentMenuItem.LeftItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.LeftItem;
		this.currentMenuItem.onEnter();
		this.myInterval.setTimeout(this.currentMenuItem.UpdateTime);
	}
}

CLCD_Display.prototype.changeToRight = function()
{
	if (this.currentMenuItem.RightItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.RightItem;
		this.currentMenuItem.onEnter();
		this.myInterval.setTimeout(this.currentMenuItem.UpdateTime);
	}
}

CLCD_Display.prototype.changeToUp = function()
{
	if (this.currentMenuItem.UpItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.UpItem;
		this.currentMenuItem.onEnter();
		this.myInterval.setTimeout(this.currentMenuItem.UpdateTime);
	}
}
CLCD_Display.prototype.changeToDown = function()
{
	if (this.currentMenuItem.DownItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.DownItem;
		this.currentMenuItem.onEnter();
		this.myInterval.setTimeout(this.currentMenuItem.UpdateTime);
	}
}

CLCD_Display.prototype.changeToEnter = function()
{
	if (this.currentMenuItem.PressEnterItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.PressEnterItem;
		this.currentMenuItem.onEnter();
		this.myInterval.setTimeout(this.currentMenuItem.UpdateTime);
	}
}

CLCD_Display.prototype.changeToBack = function()
{
	if (this.currentMenuItem.PressBackItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.PressBackItem;
		this.currentMenuItem.onEnter();
		this.myInterval.setTimeout(this.currentMenuItem.UpdateTime);
	}
}

CLCD_Display.prototype.rotaryOnline = function()
{
}

CLCD_Display.prototype.rotaryPosUpdate = function(SwitchId)
{
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	if (this.myCLCDService.getBacklight() == 0) {
		this.myCLCDService.setBacklight(this.defaultBacklight);
	}

	for (var i = 0; i < this.myRotaryService.getSteps(SwitchId); i++)
	{
		if (this.myRotaryService.getDirection(SwitchId) == "Clockwise")
		{
			this.currentMenuItem.processEvent("left");
		}
		else
		{
			this.currentMenuItem.processEvent("right");
		}
	}

	this.updateDisplay();
}

CLCD_Display.prototype.rotaryBtnUpdate = function(SwitchId)
{
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	if (this.myCLCDService.getBacklight() == 0) {
		this.myCLCDService.setBacklight(this.defaultBacklight);
	}
	if (this.myRotaryService.getButtonStatus(SwitchId) == "Released")
	{
		this.currentMenuItem.processEvent("enter");
		this.updateDisplay();
	}

}

CLCD_Display.prototype.updateDisplay = function()
{
	this.currentMenuItem.update();
}

CLCD_Display.prototype.lcdCenterText = function(text)
{
	returnText = text;
	var displayWidth = 20;	//this.myLCDService.getWidth();
	/* pad on both sides with spaces to displayWidth */
	returnText = text.pad(displayWidth, " ", 2);
	
	return returnText;
}

CLCD_Display.prototype.lcdOffline = function()
{
}

CLCD_Display.prototype.lcdOnline = function()
{
	/* If service is not online do nothing */
	if (this.myCLCDService.isOnline())
	{
		/* Clear the LCD screen */
		this.myCLCDService.clearScreen("Standard");
		/* Set backlight to max */
		this.myCLCDService.setBacklight(this.defaultBacklight);
		this.timerUpdate();
	}
}


CLCD_Display.prototype.sendTimeStamp = function()
{
	var canMessage = new CanNMTMessage("nmt", "Time");
	canMessage.send();
}

CLCD_Display.prototype.timerUpdate = function()
{
	/* If LCD service is not online do nothing */
	if (this.myCLCDService.isOnline())
	{
		if (this.mainScreenCnt > Display_mainScreenTimeout/5  && this.currentMenuItem != this.MainMenuItem)
		{
			/* Go to main screen (time) */
			this.currentMenuItem.onExit();
			this.currentMenuItem = this.MainMenuItem;
			this.currentMenuItem.onEnter();
			/* update the info on display */
			this.updateDisplay();
		} else if (this.screenSaverCnt > Display_screenSaverTimeout/5 && this.myCLCDService.getBacklight() != 0)
		{
			/* Go to screensaver menu */
			this.myCLCDService.setBacklight(0);
		} else {
			this.screenSaverCnt++;
			this.mainScreenCnt++;
		}
	}
}

