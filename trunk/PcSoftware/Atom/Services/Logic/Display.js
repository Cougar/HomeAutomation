
loadScript("Logic/MenuItem.js");

function Display(type, name, id)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	this.Service(type, name, id);
}

/* We inherit from Service with this call, it must always be next
   after the constructor */
extend(Display, Service);

/* Declaration of instance variables, for static variables remove prototype */
/* The currently displayed menuitem */
Display.prototype.currentMenuItem = null;

/* format: minutes since 00:00 (480/60 = 8:00) */
Display.prototype.bookFromTime = 480;
Display.prototype.bookToTime = 510;

/* the shortname for this exchangeobject */
Display.prototype.shortName = "";	

/* the name for this display (shown on first screen) */
Display.prototype.niceName = "";	

/* The display should regulary update and store the calendar */
Display.prototype.exchangeData = null;

Display.prototype.myLCDService = null;
Display.prototype.myRotaryService = null;
Display.prototype.mySoftPwmService = null;
Display.prototype.myInterval = null;
Display.prototype.exchangeCalendar = null;


/* This function must always be declared, this is where all the startup code
   should be placed. Gets called with arguments like what ids to use etc. */
Display.prototype.initialize = function(initialArguments)
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
	
	if (!this.myInitialArguments["softPWM"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, softPWM-config missing from config.\n");
		return;
	}
	
	if (!this.myInitialArguments["Name"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, Name-config missing from config.\n");
		return;
	}
	this.niceName = this.myInitialArguments["Name"];

	if (!this.myInitialArguments["ShortName"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, Name-config missing from config.\n");
		return;
	}
	this.shortName = this.myInitialArguments["ShortName"];
	
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
	
	/* Get the softPWM service that we want from the ServiceManager, it takes type, service name, service id */
	this.mySoftPwmService = ServiceManager.getService("Can", "softPWM", this.myInitialArguments["softPWM"]["Id"]);
	/* Add a callback for when the service goes online */
	this.mySoftPwmService.registerEventCallback("online", function(args) { self.softPwmOnline(); });
	/* If the service is already online we should call the handler here */
	this.softPwmOnline();
	
	this.exchangeCalendar = new ExchangeCalendar(function(shortname, data) { self.exchangeCalendarLookupCallback(shortname, data); });
	
	
	/* create the first menu item */
	this.currentMenuItem = new MenuItem(this);
	this.currentMenuItem.displayData[0] = "       " + this.niceName;
	this.currentMenuItem.doUpdate = this.setTime;
	
	/* create a test item */
	var meny = new MenuItem(this);
	meny.displayData[0] = "Hej  ";
	meny.displayData[1] = "   ";
	
	/* connect the items as a linked list */
	this.currentMenuItem.setNextItem(meny);
	this.currentMenuItem.setPrevItem(meny);
	/* set the function that shall be executed when knob is turned */
	this.currentMenuItem.doRight = this.changeToNext;
	this.currentMenuItem.doLeft = this.changeToPrev;
	
	/* connect the items as a linked list */
	meny.setNextItem(this.currentMenuItem);
	meny.setPrevItem(this.currentMenuItem);
	/* set the function that shall be executed when knob is turned */
	meny.doRight = this.changeToNext;
	meny.doLeft = this.changeToPrev;
	
	
}

Display.prototype.changeToNext = function()
{
	if (this.nextItem)
	{
		this.parentDisplay.currentMenuItem = this.nextItem;
	}
	
}

Display.prototype.changeToPrev = function()
{
	if (this.prevItem)
	{
		this.parentDisplay.currentMenuItem = this.prevItem;
	}
}

Display.prototype.setTime = function()
{
	var date = new Date();
	/* Get the current date time on the format YYYY-mm-dd HH.ii.ss */
	var dateAndTime = "" + date.getHours().toString() + "." + date.getMinutes().toString().pad(2, "0", 0);
	this.displayData[1] = "       " + dateAndTime;
	//this.currentMenuItem.displayData[1] = "       " + dateAndTime;
}


Display.prototype.exchangeCalendarLookupCallback = function(shortname, data)
{
	if (shortname == this.shortName)
	{
		//FIXME check if data contains error tag and error message, print to log and keep previous data?
		this.exchangeData = data;
	}
}

Display.prototype.softPwmOnline = function()
{
}

Display.prototype.rotaryOnline = function()
{
}

Display.prototype.rotaryPosUpdate = function(SwitchId)
{
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

Display.prototype.rotaryBtnUpdate = function(SwitchId)
{
	if (this.myRotaryService.getButtonStatus(SwitchId) == "Released")
	{
		if (this.currentMenuItem.doPress)
		{
			this.currentMenuItem.doPress();
		}
	}
	this.updateDisplay();
}

Display.prototype.updateDisplay = function()
{
	/* see if the current menuitem wants to update itself */
	if (this.currentMenuItem.doUpdate)
	{
		this.currentMenuItem.doUpdate();
	}

	/* Clear the LCD screen */
	this.myLCDService.clearScreen();
	
	/* send the data for the current menuitem to display */
	for (var i = 0; i < this.currentMenuItem.displayData.length; i++)
	{
		this.myLCDService.printText(0, i, this.currentMenuItem.displayData[i]);
	}
	
}

Display.prototype.lcdOffline = function()
{
	/* If we have no interval timer running do nothing */
	if (this.myInterval != null)
	{
		/* Stop the interval */
		this.myInterval.stop();
	}
}

Display.prototype.lcdOnline = function()
{
	/* If service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		/* Clear the LCD screen */
		this.myLCDService.clearScreen();
		/* Set backlight to max */
		this.myLCDService.setBacklight(255);

		/* If we have no interval timer running start it */
		if (this.myInterval == null)
		{
			var self = this;
		
			/* Start interval timer for our printout. Arguments are the callback function and time in milliseconds */
			this.myInterval = new Interval(function() { self.timerUpdate() }, 60000);
		}
		
		this.myInterval.start();
		
		this.updateDisplay();
	}
}

Display.prototype.timerUpdate = function()
{
	/* If LCD service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		//this.exchangeCalendar.lookup(this.shortName);
		
		/* update the info on display */
		this.updateDisplay();
		
		//var date = new Date();
		/* Get the current date time on the format YYYY-mm-dd HH.ii.ss */
		//var dateAndTime = date.getDateTimeFormated();
		/* Print the text to the LCD */
		//this.myLCDService.printText(0, 1, dateAndTime);
		/* Print out what we are doing to the console */
		//log(this.myName + ":" + this.myId + "> Trying to print " + dateAndTime + " to LCD\n");
	}
}

