
loadScript("Logic/gLCD_MenuItems/MainMenuItem.js");
loadScript("Logic/gLCD_MenuItems/SettingsMenuItem.js");
loadScript("Logic/gLCD_MenuItems/DimmerGlcdMenuItem.js");
loadScript("Logic/gLCD_MenuItems/SensorGlcdMenuItem.js");

function GLCD_Display(type, name, id)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	this.Service(type, name, id);
}

/* We inherit from Service with this call, it must always be next
   after the constructor */
extend(GLCD_Display, Service);

/* Declaration of instance variables, for static variables remove prototype */

GLCD_Display.prototype.myGLCDService = null;
GLCD_Display.prototype.myRotaryService1 = null;

GLCD_Display.prototype.myInterval = null;
GLCD_Display.prototype.myIntervalAlways = null;

GLCD_Display.prototype.MainMenuItem = null;
GLCD_Display.prototype.SettingMenuItem = null;
GLCD_Display.prototype.DimmerGlcdMenuItem = null;
GLCD_Display.prototype.SensorGlcdMenuItem = null;

/* The currently displayed menuitem */
GLCD_Display.prototype.currentMenuItem = null;

/* counter for going to screensaver */
GLCD_Display.prototype.screenSaverCnt = null;

/* counter for going to main screen */
GLCD_Display.prototype.mainScreenCnt = null;

const GLCD_Display_mainScreenTimeout = 20;
const GLCD_Display_screenSaverTimeout = 25;

GLCD_Display.prototype.defaultBacklight = 60;

/* This function must always be declared, this is where all the startup code
   should be placed. Gets called with arguments like what ids to use etc. */
GLCD_Display.prototype.initialize = function(initialArguments)
{
	/* We must always call the parent initialize */
	this.Service.prototype.initialize.call(this, initialArguments);



	/* This is used for function declarations like the callbacks below */
	var self = this;

	if (!this.myInitialArguments["KS0108"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, KS0108-config missing from config.\n");
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

	this.myInterval = new Interval(function() { self.timerUpdate() }, 5000);
	this.myInterval.start();

	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;

	
	/* Get the LCD service that we want from the ServiceManager, it takes type, service name, service id */
	this.myGLCDService = ServiceManager.getService("Can", "KS0108", this.myInitialArguments["KS0108"]["Id"]);
	/* Add a callback for when the service goes online */
	this.myGLCDService.registerEventCallback("online", function(args) { self.lcdOnline(); });
	/* If the service is already online we should call the handler here */
	this.lcdOnline();
	/* Add a callback for when the service goes offline */
	this.myGLCDService.registerEventCallback("offline", function(args) { self.lcdOffline(); });
	
	/* Get the Rotary service that we want from the ServiceManager, it takes type, service name, service id */
	this.myRotaryService1 = ServiceManager.getService("Can", "Rotary", this.myInitialArguments["Rotary"]["Id"]);
	/* Add a callback for when the encoder reports a new position */
	this.myRotaryService1.registerEventCallback("newPosition", function(args) { self.rotaryPosUpdate1(args); });
	/* Add a callback for when the encoder reports a new button status */
	this.myRotaryService1.registerEventCallback("newBtnStatus", function(args) { self.rotaryBtnUpdate1(args); });
	/* Add a callback for when the service goes online */
	this.myRotaryService1.registerEventCallback("online", function(args) { self.rotaryOnline(); });
	/* If the service is already online we should call the handler here */
	this.rotaryOnline();

		/* create the first menu item */
	this.MainMenuItem = new MainMenuItem(this, this.myGLCDService);
this.SettingMenuItem = new SettingsMenuItem(this, this.myGLCDService);
this.DimmerGlcdMenuItem = new DimmerGlcdMenuItem(this, this.myGLCDService);
this.SensorGlcdMenuItem = new SensorGlcdMenuItem(this, this.myGLCDService);
	
	this.currentMenuItem = this.MainMenuItem;

	
	/* create the screensaver menu item */
	

	/* set the function that shall be executed when knob is turned */
	/* create the screensaver menu item */
	this.MainMenuItem.LeftItem=this.SensorGlcdMenuItem;
	this.MainMenuItem.RightItem=this.SettingMenuItem;

	this.SettingMenuItem.LeftItem=this.MainMenuItem;
	this.SettingMenuItem.RightItem=this.DimmerGlcdMenuItem;

	this.DimmerGlcdMenuItem.LeftItem=this.SettingMenuItem;
	this.DimmerGlcdMenuItem.RightItem=this.SensorGlcdMenuItem;

	this.SensorGlcdMenuItem.LeftItem=this.DimmerGlcdMenuItem;
	this.SensorGlcdMenuItem.RightItem=this.MainMenuItem;

}
// 
GLCD_Display.prototype.changeToLeft = function()
{
	if (this.currentMenuItem.LeftItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.LeftItem;
		this.currentMenuItem.onEnter();
	}
}

GLCD_Display.prototype.changeToRight = function()
{
	if (this.currentMenuItem.RightItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.RightItem;
		this.currentMenuItem.onEnter();
	}
}

GLCD_Display.prototype.changeToUp = function()
{
	if (this.currentMenuItem.UpItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.UpItem;
		this.currentMenuItem.onEnter();
	}
}
GLCD_Display.prototype.changeToDown = function()
{
	if (this.currentMenuItem.DownItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.DownItem;
		this.currentMenuItem.onEnter();
	}
}

GLCD_Display.prototype.changeToEnter = function()
{
	if (this.currentMenuItem.PressEnterItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.PressEnterItem;
		this.currentMenuItem.onEnter();
	}
}

GLCD_Display.prototype.changeToBack = function()
{
	if (this.currentMenuItem.PressBackItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.PressBackItem;
		this.currentMenuItem.onEnter();
	}
}

GLCD_Display.prototype.rotaryOnline = function()
{
}

GLCD_Display.prototype.rotaryPosUpdate1 = function(SwitchId)
{
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
if (SwitchId != 1) {
	for (var i = 0; i < this.myRotaryService1.getSteps(SwitchId); i++)
	{
		if (this.myRotaryService1.getDirection(SwitchId) == "Clockwise")
		{
			this.currentMenuItem.processEvent("right");
		}
		else
		{
			this.currentMenuItem.processEvent("left");
		}
	}
} else {
for (var i = 0; i < this.myRotaryService1.getSteps(SwitchId); i++)
	{
		if (this.myRotaryService1.getDirection(SwitchId) == "Clockwise")
		{
			this.currentMenuItem.processEvent("up");
		}
		else
		{
			this.currentMenuItem.processEvent("down");
		}
	}
}
	this.updateDisplay();
}

GLCD_Display.prototype.rotaryBtnUpdate1 = function(SwitchId)
{
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	if (SwitchId != 1) {
if (this.myRotaryService1.getButtonStatus(SwitchId) == "Released")
	{
		this.currentMenuItem.processEvent("enter");
		this.updateDisplay();
	}
}else {
if (this.myRotaryService1.getButtonStatus(SwitchId) == "Released")
	{
		this.currentMenuItem.processEvent("back");
		this.updateDisplay();
	}
}
}

GLCD_Display.prototype.updateDisplay = function()
{
if (this.myGLCDService.getBacklight() == 0) {
this.myGLCDService.setBacklight(this.defaultBacklight);
}
	this.currentMenuItem.update();
}

GLCD_Display.prototype.lcdCenterText = function(text)
{
	returnText = text;
	var displayWidth = 20;	//this.myLCDService.getWidth();
	/* pad on both sides with spaces to displayWidth */
	returnText = text.pad(displayWidth, " ", 2);
	
	return returnText;
}

GLCD_Display.prototype.lcdOffline = function()
{
}

GLCD_Display.prototype.lcdOnline = function()
{
	/* If service is not online do nothing */
	if (this.myGLCDService.isOnline())
	{
		/* Clear the LCD screen */
		this.myGLCDService.clearScreen("Standard");
		/* Set backlight to max */
		this.myGLCDService.setBacklight(this.defaultBacklight);
		this.timerUpdate();
	}
}


GLCD_Display.prototype.sendTimeStamp = function()
{
	var canMessage = new CanNMTMessage("nmt", "Time");
	canMessage.send();
}

GLCD_Display.prototype.timerUpdate = function()
{
	/* If LCD service is not online do nothing */
	if (this.myGLCDService.isOnline())
	{
		this.screenSaverCnt++;
		this.mainScreenCnt++;
		if (this.mainScreenCnt > GLCD_Display_mainScreenTimeout/5  && this.currentMenuItem != this.MainMenuItem)
		{
			/* Go to main screen (time) */
this.currentMenuItem.onExit();
			this.currentMenuItem = this.MainMenuItem;
this.currentMenuItem.onEnter();
			/* update the info on display */
			this.updateDisplay();

			this.mainScreenCnt = 0;

		}
		
		if (this.screenSaverCnt > GLCD_Display_screenSaverTimeout/5)
		{
			/* Go to screensaver menu */
this.myGLCDService.setBacklight(0);
			
			this.screenSaverCnt = 0;
		}

	}
}

