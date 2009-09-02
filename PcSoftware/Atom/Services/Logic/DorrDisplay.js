
loadScript("Logic/gLCD_MenuItems/MainMenuItem.js");
loadScript("Logic/gLCD_MenuItems/SettingsMenuItem.js");

function DorrDisplay(type, name, id)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	this.Service(type, name, id);
}

/* We inherit from Service with this call, it must always be next
   after the constructor */
extend(DorrDisplay, Service);

/* Declaration of instance variables, for static variables remove prototype */

DorrDisplay.prototype.myGLCDService = null;
DorrDisplay.prototype.myRotaryService1 = null;

DorrDisplay.prototype.myInterval = null;
DorrDisplay.prototype.myIntervalAlways = null;

DorrDisplay.prototype.MainMenuItem = null;
DorrDisplay.prototype.SettingsMenuItem = null;

/* The currently displayed menuitem */
DorrDisplay.prototype.currentMenuItem = null;

/* counter for going to screensaver */
DorrDisplay.prototype.screenSaverCnt = null;

/* counter for going to main screen */
DorrDisplay.prototype.mainScreenCnt = null;

const DorrDisplay_mainScreenTimeout = 20;
const DorrDisplay_screenSaverTimeout = 25;


/* This function must always be declared, this is where all the startup code
   should be placed. Gets called with arguments like what ids to use etc. */
DorrDisplay.prototype.initialize = function(initialArguments)
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
/* create the first menu item */
	this.MainMenuItem = new MainMenuItem(this, this.myGLCDService);
this.SettingMenuItem = new SettingsMenuItem(this, this.myGLCDService);
	

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
	
	this.currentMenuItem = this.MainMenuItem;

	
	/* create the screensaver menu item */
	this.SettingMenuItem.LeftItem=this.MainMenuItem;
	this.SettingMenuItem.RightItem=this.MainMenuItem;

	/* set the function that shall be executed when knob is turned */
	/* create the screensaver menu item */
	this.MainMenuItem.LeftItem=this.SettingMenuItem;
	this.MainMenuItem.RightItem=this.SettingMenuItem;

}
// 
DorrDisplay.prototype.changeToLeft = function()
{
	if (this.currentMenuItem.LeftItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.LeftItem;
		this.currentMenuItem.onEnter();
	}
}

DorrDisplay.prototype.changeToRight = function()
{
	if (this.currentMenuItem.RightItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.RightItem;
		this.currentMenuItem.onEnter();
	}
}

DorrDisplay.prototype.changeToUp = function()
{
	if (this.currentMenuItem.UpItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.UpItem;
		this.currentMenuItem.onEnter();
	}
}
DorrDisplay.prototype.changeToDown = function()
{
	if (this.currentMenuItem.DownItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.DownItem;
		this.currentMenuItem.onEnter();
	}
}

DorrDisplay.prototype.changeToEnter = function()
{
	if (this.currentMenuItem.PressEnterItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.PressEnterItem;
		this.currentMenuItem.onEnter();
	}
}

DorrDisplay.prototype.changeToBack = function()
{
	if (this.currentMenuItem.PressBackItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.PressBackItem;
		this.currentMenuItem.onEnter();
	}
}

DorrDisplay.prototype.rotaryOnline = function()
{
}

DorrDisplay.prototype.rotaryPosUpdate1 = function(SwitchId)
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

DorrDisplay.prototype.rotaryBtnUpdate1 = function(SwitchId)
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

DorrDisplay.prototype.updateDisplay = function()
{
	this.currentMenuItem.update();
}

DorrDisplay.prototype.lcdCenterText = function(text)
{
	returnText = text;
	var displayWidth = 20;	//this.myLCDService.getWidth();
	/* pad on both sides with spaces to displayWidth */
	returnText = text.pad(displayWidth, " ", 2);
	
	return returnText;
}

DorrDisplay.prototype.lcdOffline = function()
{
}

DorrDisplay.prototype.lcdOnline = function()
{
	/* If service is not online do nothing */
	if (this.myGLCDService.isOnline())
	{
		/* Clear the LCD screen */
		this.myGLCDService.clearScreen("Standard");
		/* Set backlight to max */
		this.myGLCDService.setBacklight(60);
		this.timerUpdate();
	}
}


DorrDisplay.prototype.sendTimeStamp = function()
{
	var canMessage = new CanNMTMessage("nmt", "Time");
	canMessage.send();
}

DorrDisplay.prototype.timerUpdate = function()
{
	/* If LCD service is not online do nothing */
	if (this.myGLCDService.isOnline())
	{
		this.screenSaverCnt++;
		this.mainScreenCnt++;
		if (this.mainScreenCnt > DorrDisplay_mainScreenTimeout/5  && this.currentMenuItem != this.MainMenuItem)
		{
			/* Go to main screen (time) */
this.currentMenuItem.onExit();
			this.currentMenuItem = this.MainMenuItem;
this.currentMenuItem.onEnter();
			/* update the info on display */
			this.updateDisplay();

			this.mainScreenCnt = 0;

		}
		
		if (this.screenSaverCnt > DorrDisplay_screenSaverTimeout/5 && this.currentMenuItem != this.SettingMenuItem)
		{
			/* Go to screensaver menu */
this.currentMenuItem.onExit();
			this.currentMenuItem = this.SettingMenuItem;
this.currentMenuItem.onEnter();
//this.myGLCDService.setBacklight(0);
			/* update the info on display */
			this.updateDisplay();
			
			this.screenSaverCnt = 0;
		}

	}
}

