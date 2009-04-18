
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

/* format: dat objects, just use the time */
Display.prototype.bookFromTime = null;
Display.prototype.bookToTime = null;

/* the shortname for this exchangeobject */
Display.prototype.shortName = null;	

/* the name for this display (shown on first screen) */
Display.prototype.niceName = null;

/* The display should regulary update and store the calendar */
Display.prototype.exchangeData = null;

Display.prototype.myLCDService = null;
Display.prototype.myRotaryService = null;
Display.prototype.mySoftPwmService = null;
Display.prototype.myInterval = null;

/*  */
Display.prototype.exchangeCalendar = null;
/*  */
Display.prototype.exchangeCalendarFirstMenuItem = null;
Display.prototype.exchangeCalendarLastMenuItem = null;
Display.prototype.statusMenuItem = null;
Display.prototype.bookMenuItem = null;

/* The currently displayed menuitem */
Display.prototype.currentMenuItem = null;


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
	this.statusMenuItem = new MenuItem(this);
	this.statusMenuItem.displayData[0] = this.lcdCenterText(this.niceName);
	this.statusMenuItem.doUpdate = this.updateStatusMenuItem;
	this.currentMenuItem = this.statusMenuItem;
	
	/* create the menuitem where you can choose to enter the booking sub-menu */
	this.bookMenuItem = new MenuItem(this);
	this.bookMenuItem.displayData[0] = this.lcdCenterText("Book room";
	this.bookMenuItem.displayData[1] = this.lcdCenterText("");
	this.bookMenuItem.doUpdate = this.updateBookMenuItem;

	/* connect the items as a linked list */
	this.statusMenuItem.setNextItem(this.bookMenuItem);
	this.statusMenuItem.setPrevItem(this.bookMenuItem);
	/* set the function that shall be executed when knob is turned */
	this.statusMenuItem.doRight = this.changeToNext;
	this.statusMenuItem.doLeft = this.changeToPrev;
	
	/* connect the items as a linked list */
	this.bookMenuItem.setNextItem(this.statusMenuItem);
	this.bookMenuItem.setPrevItem(this.statusMenuItem);
	/* set the function that shall be executed when knob is turned */
	this.bookMenuItem.doRight = this.changeToNext;
	this.bookMenuItem.doLeft = this.changeToPrev;
	
	/* create the menuitem where you can choose the to-time */
	var menuChooseTo = new MenuItem(this);
	menuChooseTo.displayData[1] = this.lcdCenterText("Ok     Cancel");
	menuChooseTo.doUpdate = this.chooseBookTimeTo;
	
	this.bookMenuItem.doPress = this.startBooking;
	this.bookMenuItem.setDescItem(menuChooseTo);
	
	/* create the menuitem where you can choose to book the room with OK */
	var menuChooseOk = new MenuItem(this);
	menuChooseOk.displayData[1] = this.lcdCenterText("<Ok>    Cancel");
	menuChooseOk.doUpdate = this.setBookTime;

	/* create the menuitem where you can choose to cancel the booking */
	var menuChooseCancel = new MenuItem(this);
	menuChooseCancel.displayData[1] = this.lcdCenterText("Ok    <Cancel>");
	menuChooseCancel.doUpdate = this.setBookTime;

	/* create the menuitem where you can choose the from-time */
	var menuChooseFrom = new MenuItem(this);
	menuChooseFrom.displayData[1] = this.lcdCenterText("Ok     Cancel");
	menuChooseFrom.doUpdate = this.chooseBookTimeFrom;

	/* connect the items as a linked list */
	menuChooseTo.setNextItem(menuChooseOk);
	menuChooseTo.setPrevItem(menuChooseFrom);
	/* set the function that shall be executed when knob is turned */
	menuChooseTo.doRight = this.changeToNext;
	menuChooseTo.doLeft = this.changeToPrev;
	menuChooseTo.doPress = this.changeToDesc;

	/* connect the items as a linked list */
	menuChooseOk.setNextItem(menuChooseCancel);
	menuChooseOk.setPrevItem(menuChooseTo);
	/* set the function that shall be executed when knob is turned */
	menuChooseOk.doRight = this.changeToNext;
	menuChooseOk.doLeft = this.changeToPrev;

	/* connect the items as a linked list */
	menuChooseCancel.setNextItem(menuChooseFrom);
	menuChooseCancel.setPrevItem(menuChooseOk);
	menuChooseCancel.setDescItem(this.bookMenuItem);
	/* set the function that shall be executed when knob is turned */
	menuChooseCancel.doRight = this.changeToNext;
	menuChooseCancel.doLeft = this.changeToPrev;
	menuChooseCancel.doPress = this.changeToDesc;

	/* connect the items as a linked list */
	menuChooseFrom.setNextItem(menuChooseTo);
	menuChooseFrom.setPrevItem(menuChooseCancel);
	/* set the function that shall be executed when knob is turned */
	menuChooseFrom.doRight = this.changeToNext;
	menuChooseFrom.doLeft = this.changeToPrev;
	menuChooseFrom.doPress = this.changeToDesc;

	/* create the menuitem where you can modify the to-time */
	var menuSetTo = new MenuItem(this);
	menuSetTo.doUpdate = this.setBookTimeTo;
	menuSetTo.displayData[0] = this.lcdCenterText("Set end time");
	menuSetTo.doRight = this.incBookTimeTo;
	menuSetTo.doLeft = this.decBookTimeTo;
	menuSetTo.doPress = this.changeToDesc;
	menuSetTo.setDescItem(menuChooseTo);
	menuChooseTo.setDescItem(menuSetTo);

	/* create the menuitem where you can modify the from-time */
	var menuSetFrom = new MenuItem(this);
	menuSetFrom.doUpdate = this.setBookTimeFrom;
	menuSetFrom.displayData[0] = this.lcdCenterText("Set start time");
	menuSetFrom.doRight = this.incBookTimeFrom;
	menuSetFrom.doLeft = this.decBookTimeFrom;
	menuSetFrom.doPress = this.changeToDesc;
	menuSetFrom.setDescItem(menuChooseFrom);
	menuChooseFrom.setDescItem(menuSetFrom);

}

Display.prototype.startBooking = function()
{
	this.parentDisplay.bookFromTime = new Date();
	this.parentDisplay.bookToTime = new Date();
	//this.bookFromTime.setHours(8);
	//this.bookToTime.setHours(8);
	if (this.parentDisplay.bookFromTime.getMinutes() < 30)
	{
		this.parentDisplay.bookFromTime.setMinutes(0);
		this.parentDisplay.bookToTime.setMinutes(30);
	}
	else
	{
		this.parentDisplay.bookFromTime.setMinutes(30);
		this.parentDisplay.bookToTime.setHours(this.parentDisplay.bookToTime.getHours()+1);
		this.parentDisplay.bookToTime.setMinutes(0);
	}

	this.parentDisplay.currentMenuItem = this.parentDisplay.currentMenuItem.descItem;
}

Display.prototype.changeToDesc = function()
{
	if (this.descItem)
	{
		this.parentDisplay.currentMenuItem = this.descItem;
	}
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

Display.prototype.incBookTimeTo = function()
{
	//FIXME: constraints?
	this.parentDisplay.bookToTime.setMinutes(this.parentDisplay.bookToTime.getMinutes() + 30);
}

Display.prototype.decBookTimeTo = function()
{
	//FIXME: constraints?
	this.parentDisplay.bookToTime.setMinutes(this.parentDisplay.bookToTime.getMinutes() - 30);
}

Display.prototype.incBookTimeFrom = function()
{
	//FIXME: constraints?
	this.parentDisplay.bookFromTime.setMinutes(this.parentDisplay.bookFromTime.getMinutes() + 30);
}

Display.prototype.decBookTimeFrom = function()
{
	//FIXME: constraints?
		this.parentDisplay.bookFromTime.setMinutes(this.parentDisplay.bookFromTime.getMinutes() - 30);
}

Display.prototype.setBookTimeFrom = function()
{
	this.displayData[1] = "      <"+this.parentDisplay.bookFromTime.getTimeShortFormated()+">       ";
}

Display.prototype.setBookTimeTo = function()
{
	this.displayData[1] = "      <"+this.parentDisplay.bookToTime.getTimeShortFormated()+">       ";
}

Display.prototype.chooseBookTimeTo = function()
{
	this.displayData[0] = "  "+this.parentDisplay.bookFromTime.getTimeShortFormated()+"  - <"
								+this.parentDisplay.bookToTime.getTimeShortFormated()+">  ";
}

Display.prototype.chooseBookTimeFrom = function()
{
	this.displayData[0] = " <"+this.parentDisplay.bookFromTime.getTimeShortFormated()+"> -  "
								+this.parentDisplay.bookToTime.getTimeShortFormated()+"   ";
}

Display.prototype.setBookTime = function()
{
	this.displayData[0] = "  "+this.parentDisplay.bookFromTime.getTimeShortFormated()+"  -  "
								+this.parentDisplay.bookToTime.getTimeShortFormated()+"   ";
}

Display.prototype.updateStatusMenuItem = function()
{
	var date = new Date();
	var dateAndTime = "" + date.getTimeShortFormated();
	this.displayData[1] = "       " + dateAndTime + "        ";
	
	if (this.parentDisplay.exchangeCalendarFirstMenuItem && this.parentDisplay.exchangeCalendarLastMenuItem)
	{
		this.parentDisplay.statusMenuItem.setNextItem(this.parentDisplay.exchangeCalendarFirstMenuItem);
		this.parentDisplay.bookMenuItem.setPrevItem(this.parentDisplay.exchangeCalendarLastMenuItem);
	}
}

Display.prototype.updateBookMenuItem = function()
{
	if (this.parentDisplay.exchangeCalendarFirstMenuItem && this.parentDisplay.exchangeCalendarLastMenuItem)
	{
		this.parentDisplay.statusMenuItem.setNextItem(this.parentDisplay.exchangeCalendarFirstMenuItem);
		this.parentDisplay.bookMenuItem.setPrevItem(this.parentDisplay.exchangeCalendarLastMenuItem);
	}
}

Display.prototype.exchangeCalendarLookupCallback = function(shortname, data)
{
log("callback: "+shortname+" \n");
	if (shortname == this.shortName)
	{
		//FIXME check if data contains error tag and error message, print to log and keep previous data?
		this.exchangeData = data;
		
		this.createCalendarMenu();
	}
}

Display.prototype.createCalendarMenu = function()
{
log("createCalendar \n");
	if (this.exchangeData)
	{
log("have data \n");
		var lastMenuItem;
		for (var i = 0; i < this.exchangeData.meetings.length; i++)
		{
log("looping \n");
			/* create the menuitem for a calendar meeting */
			var menu = new MenuItem(this);
			menu.displayData[0] = this.lcdCenterText(this.exchangeData.meetings[i].organizer);
			menu.displayData[1] = this.lcdCenterText(this.exchangeData.meetings[i].start.replace(":",".") + " - " 
										+ this.exchangeData.meetings[i].end.replace(":","."));
			menu.doRight = this.changeToNext;
			menu.doLeft = this.changeToPrev;
			
			if (i == 0)
			{
				this.exchangeCalendarFirstMenuItem = menu;
			}
			else
			{
				menu.setPrevItem(lastMenuItem);
				lastMenuItem.setNextItem(menu);
			}
			
			lastMenuItem = menu;
		}
		this.exchangeCalendarLastMenuItem = lastMenuItem;
		this.exchangeCalendarLastMenuItem.setNextItem(this.bookMenuItem);
		this.exchangeCalendarFirstMenuItem.setPrevItem(this.statusMenuItem);
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
	//this.myLCDService.clearScreen();
	
	/* send the data for the current menuitem to display */
	for (var i = 0; i < this.currentMenuItem.displayData.length; i++)
	{
		this.myLCDService.printText(0, i, this.currentMenuItem.displayData[i]);
	}
	
}

Display.prototype.lcdCenterText = function(text)
{
	returnText = text;
	var displayWidth = this.myLCDService.getWidth();
	/* pad on both sides with spaces to displayWidth */
	returnText = text.pad(displayWidth, " ", 2);
	
	/*if (displayWidth > text.length)
	{
		paddingLen = displayWidth - text.length;
		paddingLen = Math.round((paddingLen/2-0.5));
		returnText = text
	}*/
	
	return returnText;
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
			this.myInterval = new Interval(function() { self.timerUpdate() }, 10000);
		}
		
		this.myInterval.start();
		
		this.timerUpdate();
	}
}

Display.prototype.timerUpdate = function()
{
	/* If LCD service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		this.exchangeCalendar.lookup(this.shortName);
		
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

