
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
Display.prototype.myIntervalAlways = null;

/*  */
Display.prototype.exchangeCalendar = null;
/*  */
Display.prototype.exchangeCalendarFirstMenuItem = null;
Display.prototype.exchangeCalendarLastMenuItem = null;
Display.prototype.statusMenuItem = null;
Display.prototype.bookMenuItem = null;
Display.prototype.bookResultMenuItem = null;
Display.prototype.screenSaverMenuItem = null;


/* The currently displayed menuitem */
Display.prototype.currentMenuItem = null;

/* counter for calling exchange update */
Display.prototype.exchangeUpdateCnt = null;

/* counter for going to screensaver */
Display.prototype.screenSaverCnt = null;

/* counter for going to main screen */
Display.prototype.mainScreenCnt = null;

/* LED status */
Display.prototype.LEDstatus = null;

const exchangeUpdateTimeout = 50;
const mainScreenTimeout = 40;
const screenSaverTimeout = 120;

const bookingIncrease = 15;

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

	/* Start interval timer for sending timestamp to network. Arguments are the callback function and time in milliseconds 
	used to make sure an eth-node gets its init packet (600s) */
	this.myIntervalAlways = new Interval(function() { self.sendTimeStamp() }, 600000);
	this.myIntervalAlways.start();

	this.myInterval = new Interval(function() { self.timerUpdate() }, 5000);
	this.myInterval.start();

	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	this.exchangeUpdateCnt = exchangeUpdateTimeout-10;
	
	this.LEDstatus = "none";
	
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
	this.exchangeCalendarBook = new ExchangeCalendar(function(shortname, data) { self.exchangeCalendarBookCallback(shortname, data); });
	
	/* create the first menu item */
	this.statusMenuItem = new MenuItem(this);
	this.statusMenuItem.displayData[0] = this.lcdCenterText(this.niceName);
	this.statusMenuItem.doUpdate = function(args) { self.updateStatusMenuItem(); };
	this.currentMenuItem = this.statusMenuItem;

	/* create the screensaver menu item */
	this.screenSaverMenuItem = new MenuItem(this);
	this.screenSaverMenuItem.displayData[0] = this.lcdCenterText("");
	this.screenSaverMenuItem.displayData[1] = this.lcdCenterText("");
	this.screenSaverMenuItem.doRight = function(args) { self.changeToNext(); };
	this.screenSaverMenuItem.doLeft = function(args) { self.changeToPrev(); };
	this.screenSaverMenuItem.doUpdate = function(args) { self.updateBookMenuItem(); };
	this.screenSaverMenuItem.setNextItem(this.statusMenuItem);
	this.screenSaverMenuItem.setPrevItem(this.statusMenuItem);

	/* create the menuitem where you can choose to enter the booking sub-menu */
	this.bookMenuItem = new MenuItem(this);
	this.bookMenuItem.displayData[0] = this.lcdCenterText("Book room");
	this.bookMenuItem.displayData[1] = this.lcdCenterText("(Push button)");
	this.bookMenuItem.doUpdate = function(args) { self.updateBookMenuItem(); };

	/* connect the items as a linked list */
	this.statusMenuItem.setNextItem(this.bookMenuItem);
	this.statusMenuItem.setPrevItem(this.bookMenuItem);
	/* set the function that shall be executed when knob is turned */
	this.statusMenuItem.doRight = function(args) { self.changeToNext(); };
	this.statusMenuItem.doLeft = function(args) { self.changeToPrev(); };
	
	/* connect the items as a linked list */
	this.bookMenuItem.setNextItem(this.statusMenuItem);
	this.bookMenuItem.setPrevItem(this.statusMenuItem);
	/* set the function that shall be executed when knob is turned */
	this.bookMenuItem.doRight = function(args) { self.changeToNext(); };
	this.bookMenuItem.doLeft = function(args) { self.changeToPrev(); };
	
	/* create the menuitem where you can choose the to-time */
	var menuChooseTo = new MenuItem(this);
	menuChooseTo.displayData[1] = this.lcdCenterText(" Ok     Cancel ");
	menuChooseTo.doUpdate = function(args) { self.chooseBookTimeTo(); };
	
	this.bookMenuItem.doPress = function(args) { self.prepareBookingMenu(); };
	this.bookMenuItem.setDescItem(menuChooseTo);
	
	/* create the menuitem where you can choose to book the room with OK */
	var menuChooseOk = new MenuItem(this);
	menuChooseOk.displayData[1] = this.lcdCenterText("<Ok>    Cancel ");
	menuChooseOk.doUpdate = function(args) { self.setBookTime(); };

	/* create the menuitem where you can choose to cancel the booking */
	var menuChooseCancel = new MenuItem(this);
	menuChooseCancel.displayData[1] = this.lcdCenterText(" Ok    <Cancel>");
	menuChooseCancel.doUpdate = function(args) { self.setBookTime(); };

	/* create the menuitem where you can choose the from-time */
	var menuChooseFrom = new MenuItem(this);
	menuChooseFrom.displayData[1] = this.lcdCenterText(" Ok     Cancel ");
	menuChooseFrom.doUpdate = function(args) { self.chooseBookTimeFrom(); };

	/* create the menuitem where you see the result of a booking */
	this.bookResultMenuItem = new MenuItem(this);
	this.bookResultMenuItem.displayData[0] = this.lcdCenterText("Booking...");
	this.bookResultMenuItem.displayData[1] = this.lcdCenterText("(Push to return)");
	this.bookResultMenuItem.setDescItem(this.bookMenuItem);
	this.bookResultMenuItem.doPress = function(args) { self.changeToDesc(); };

	/* connect the items as a linked list */
	menuChooseTo.setNextItem(menuChooseOk);
	menuChooseTo.setPrevItem(menuChooseFrom);
	/* set the function that shall be executed when knob is turned */
	menuChooseTo.doRight = function(args) { self.changeToNext(); };
	menuChooseTo.doLeft = function(args) { self.changeToPrev(); };
	menuChooseTo.doPress = function(args) { self.changeToDesc(); };

	/* connect the items as a linked list */
	menuChooseOk.setNextItem(menuChooseCancel);
	menuChooseOk.setPrevItem(menuChooseTo);
	menuChooseOk.setDescItem(this.bookResultMenuItem);
	/* set the function that shall be executed when knob is turned */
	menuChooseOk.doRight = function(args) { self.changeToNext(); };
	menuChooseOk.doLeft = function(args) { self.changeToPrev(); };
	menuChooseOk.doPress = function(args) { self.doBooking(); };

	/* connect the items as a linked list */
	menuChooseCancel.setNextItem(menuChooseFrom);
	menuChooseCancel.setPrevItem(menuChooseOk);
	menuChooseCancel.setDescItem(this.bookMenuItem);
	/* set the function that shall be executed when knob is turned */
	menuChooseCancel.doRight = function(args) { self.changeToNext(); };
	menuChooseCancel.doLeft = function(args) { self.changeToPrev(); };
	menuChooseCancel.doPress = function(args) { self.changeToDesc(); };

	/* connect the items as a linked list */
	menuChooseFrom.setNextItem(menuChooseTo);
	menuChooseFrom.setPrevItem(menuChooseCancel);
	/* set the function that shall be executed when knob is turned */
	menuChooseFrom.doRight = function(args) { self.changeToNext(); };
	menuChooseFrom.doLeft = function(args) { self.changeToPrev(); };
	menuChooseFrom.doPress = function(args) { self.changeToDesc(); };

	/* create the menuitem where you can modify the to-time */
	var menuSetTo = new MenuItem(this);
	menuSetTo.doUpdate = function(args) { self.setBookTimeTo(); };
	menuSetTo.displayData[0] = this.lcdCenterText("Set end time");
	menuSetTo.doRight = function(args) { self.incBookTimeTo(); };
	menuSetTo.doLeft = function(args) { self.decBookTimeTo(); };
	menuSetTo.doPress = function(args) { self.changeToDesc(); };
	menuSetTo.setDescItem(menuChooseTo);
	menuChooseTo.setDescItem(menuSetTo);

	/* create the menuitem where you can modify the from-time */
	var menuSetFrom = new MenuItem(this);
	menuSetFrom.doUpdate = function(args) { self.setBookTimeFrom(); };
	menuSetFrom.displayData[0] = this.lcdCenterText("Set start time");
	menuSetFrom.doRight = function(args) { self.incBookTimeFrom(); };
	menuSetFrom.doLeft = function(args) { self.decBookTimeFrom(); };
	menuSetFrom.doPress = function(args) { self.changeToDesc(); };
	menuSetFrom.setDescItem(menuChooseFrom);
	menuChooseFrom.setDescItem(menuSetFrom);

}

/* Callback for a booking call to exchange */
Display.prototype.exchangeCalendarBookCallback = function(shortname, data)
{
	if (shortname == this.shortName)
	{
		//FIXME check if data contains error tag and error message, print to log and keep previous data?
		if (data.result)
		{
			if (data.error)
			{
				this.bookResultMenuItem.displayData[0] = this.lcdCenterText("Booking failed");
				log("Display:"+this.myId+"> Got this error from exchange server: " + data.error + "\n");
			}
			else
			{
				this.bookResultMenuItem.displayData[0] = this.lcdCenterText("Booking succeeded");
				log("Display:"+this.myId+"> Successfully booked room\n");
				
				/* do an exchange lookup */
				this.exchangeCalendar.lookup(this.shortName);
			}
			
		}
		else if (data.error)
		{
			this.bookResultMenuItem.displayData[0] = this.lcdCenterText("Booking failed");
			log("Display:"+this.myId+"> Got this error from exchange.php: " + data.error + "\n");
		}
		this.updateDisplay();
	}
}

/* When pressing <OK> in booking menu */
Display.prototype.doBooking = function()
{
	this.bookResultMenuItem.displayData[0] = this.lcdCenterText("Booking...");
	if (this.currentMenuItem.descItem)
	{
		this.currentMenuItem = this.currentMenuItem.descItem;
	}
	//this.changeToDesc();

	var bookFrom = this.bookFromTime.getTimeShortFormated();
	var bookTo = this.bookToTime.getTimeShortFormated();
	this.exchangeCalendarBook.book(this.shortName, bookFrom, bookTo);
	
	//bookFromTime bookToTime
}

/* When entering the booking menu, set up some parameters */
Display.prototype.prepareBookingMenu = function()
{
	this.bookFromTime = new Date();
	this.bookToTime = new Date();
	//this.bookFromTime.setHours(8);
	//this.bookToTime.setHours(8);
	if (this.bookFromTime.getMinutes() < 30)
	{
		this.bookFromTime.setMinutes(0);
		this.bookToTime.setMinutes(30);
	}
	else
	{
		this.bookFromTime.setMinutes(30);
		this.bookToTime.setHours(this.bookToTime.getHours()+1);
		this.bookToTime.setMinutes(0);
	}

	this.currentMenuItem = this.currentMenuItem.descItem;
}

Display.prototype.changeToDesc = function()
{
	if (this.currentMenuItem.descItem)
	{
		this.currentMenuItem = this.currentMenuItem.descItem;
	}
}

Display.prototype.changeToNext = function()
{
	if (this.currentMenuItem.nextItem)
	{
		this.currentMenuItem = this.currentMenuItem.nextItem;
	}
}

Display.prototype.changeToPrev = function()
{
	if (this.currentMenuItem.prevItem)
	{
		this.currentMenuItem = this.currentMenuItem.prevItem;
	}
}

Display.prototype.incBookTimeTo = function()
{
	//FIXME: constraints?
	if (this.bookToTime.getHours() < 23)
	{
		this.bookToTime.setMinutes(this.bookToTime.getMinutes() + bookingIncrease);
	}
}

Display.prototype.decBookTimeTo = function()
{
	//FIXME: constraints?
	var now = new Date();
	if (this.bookToTime.getHours() >= now.getHours())
	{
		this.bookToTime.setMinutes(this.bookToTime.getMinutes() - bookingIncrease);
	}
}

Display.prototype.incBookTimeFrom = function()
{
	//FIXME: constraints?
	if (this.bookFromTime.getHours() < 23)
	{
		this.bookFromTime.setMinutes(this.bookFromTime.getMinutes() + bookingIncrease);
	}
}

Display.prototype.decBookTimeFrom = function()
{
	//FIXME: constraints?
	var now = new Date();
	if (this.bookFromTime.getHours() >= now.getHours())
	{
		this.bookFromTime.setMinutes(this.bookFromTime.getMinutes() - bookingIncrease);
	}
}

Display.prototype.setBookTimeFrom = function()
{
	this.currentMenuItem.displayData[1] = "      <"+this.bookFromTime.getTimeShortFormated()+">       ";
}

Display.prototype.setBookTimeTo = function()
{
	this.currentMenuItem.displayData[1] = "      <"+this.bookToTime.getTimeShortFormated()+">       ";
}

Display.prototype.chooseBookTimeTo = function()
{
	this.currentMenuItem.displayData[0] = "  "+this.bookFromTime.getTimeShortFormated()+"  - <"
								+this.bookToTime.getTimeShortFormated()+">  ";
}

Display.prototype.chooseBookTimeFrom = function()
{
	this.currentMenuItem.displayData[0] = " <"+this.bookFromTime.getTimeShortFormated()+"> -  "
								+this.bookToTime.getTimeShortFormated()+"   ";
}

Display.prototype.setBookTime = function()
{
	this.currentMenuItem.displayData[0] = "  "+this.bookFromTime.getTimeShortFormated()+"  -  "
								+this.bookToTime.getTimeShortFormated()+"   ";
}

Display.prototype.updateStatusMenuItem = function()
{
	var date = new Date();
	var dateAndTime = "" + date.getTimeShortFormated();
	this.statusMenuItem.displayData[1] = this.lcdCenterText(dateAndTime);
}

Display.prototype.updateBookMenuItem = function()
{
	if (this.exchangeCalendarFirstMenuItem && this.exchangeCalendarLastMenuItem)
	{
		this.statusMenuItem.setNextItem(this.exchangeCalendarFirstMenuItem);
		this.bookMenuItem.setPrevItem(this.exchangeCalendarLastMenuItem);
	}
	else
	{
		this.statusMenuItem.setNextItem(this.bookMenuItem);
		this.bookMenuItem.setPrevItem(this.statusMenuItem);
	}
}

/* Callback for an exchange calendar lookup*/
Display.prototype.exchangeCalendarLookupCallback = function(shortname, data)
{
	if (shortname == this.shortName)
	{
		//FIXME check if data contains error tag and error message, print to log and keep previous data?
		if (data.error)
		{
			log("Display:"+this.myId+"> got this error from exchange.php: " + data.error + "\n");
		}
		else
		{
			this.exchangeData = data;
			this.createCalendarMenu();
		}
	}
}

/* this function parses calendar data from exchange-script and creates a menu */
Display.prototype.createCalendarMenu = function()
{
	/* This is used for function declarations like the callbacks below */
	var self = this;

	this.exchangeCalendarLastMenuItem = null;
	this.exchangeCalendarFirstMenuItem = null;
	
	if (this.exchangeData)
	{
		var lastMenuItem;
		for (var i = 0; i < this.exchangeData.meetings.length; i++)
		{
			/* create the menuitem for a calendar meeting */
			var menu = new MenuItem(this);
			menu.displayData[0] = this.lcdCenterText(this.replaceAumlauts(this.exchangeData.meetings[i].subject));
			menu.displayData[1] = this.lcdCenterText(this.exchangeData.meetings[i].start + " - " 
										+ this.exchangeData.meetings[i].end);
			menu.doRight = function(args) { self.changeToNext(); };
			menu.doLeft = function(args) { self.changeToPrev(); };
			
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
		if (lastMenuItem)
		{
			this.exchangeCalendarLastMenuItem = lastMenuItem;
			this.exchangeCalendarLastMenuItem.setNextItem(this.bookMenuItem);
			this.exchangeCalendarFirstMenuItem.setPrevItem(this.statusMenuItem);
		}
		/* to link in the newly created menuitems with exchangeinformation the display must be showing 
		   either the statusmenu or the bookingmenu */
		if (this.currentMenuItem == this.statusMenuItem || this.currentMenuItem == this.bookMenuItem || this.currentMenuItem == this.bookResultMenuItem || this.currentMenuItem == this.screenSaverMenuItem)
		{
			/* if there is one or more exchangemenuitems */
			if (this.exchangeCalendarFirstMenuItem && this.exchangeCalendarLastMenuItem)
			{
				this.statusMenuItem.setNextItem(this.exchangeCalendarFirstMenuItem);
				this.bookMenuItem.setPrevItem(this.exchangeCalendarLastMenuItem);
			}
			/* if not then link statusmenu to bookingmenu */
			else
			{
				this.statusMenuItem.setNextItem(this.bookMenuItem);
				this.bookMenuItem.setPrevItem(this.statusMenuItem);
			}
		}
	}
}

Display.prototype.replaceAumlauts = function(intext)
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

Display.prototype.softPwmOnline = function()
{
	this.LEDstatus = "none";
}

Display.prototype.rotaryOnline = function()
{
}

Display.prototype.rotaryPosUpdate = function(SwitchId)
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

Display.prototype.rotaryBtnUpdate = function(SwitchId)
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
	var displayWidth = 20;	//this.myLCDService.getWidth();
	/* pad on both sides with spaces to displayWidth */
	returnText = text.pad(displayWidth, " ", 2);
	
	return returnText;
}

Display.prototype.lcdOffline = function()
{
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

		this.timerUpdate();
	}
}
Display.prototype.setLEDtoCalendar = function()
{
	if (this.exchangeData)
	{
		var now = new Date();
		//for (var i = 0; i < this.exchangeData.meetings.length; i++)
		if (this.exchangeData.meetings.length > 0)
		{
			/* check if current time is before start of first meeting 
			first meeting is always the next meeting or current meeting */
			//var startDate = new Date.parse(this.exchangeData.meetings[0].start);
			var startTimeSplit = this.exchangeData.meetings[0].start.split(":");
			var endTimeSplit = this.exchangeData.meetings[0].end.split(":");
			if (now.getHours() < startTimeSplit[0] || 
			now.getHours() > endTimeSplit[0] ||
			now.getHours() == startTimeSplit[0] && now.getMinutes() < startTimeSplit[1])
			{
				if (this.LEDstatus != "green")
				{
					this.LEDstatus = "green";
					this.setLEDgreen();
				}
			}
			else
			{
				if (this.LEDstatus != "red")
				{
					this.LEDstatus = "red";
					this.setLEDred();
				}
			}
		}
		else
		{
			if (this.LEDstatus != "green")
			{
				this.LEDstatus = "green";
				this.setLEDgreen();
			}
		}
	}
}

Display.prototype.setLEDred = function()
{
	this.setLED(255, 0, 0);
}

Display.prototype.setLEDgreen = function()
{
	this.setLED(0, 255, 0);
}

Display.prototype.setLED = function(red, green, blue)
{
log("Display:"+this.myId+"> Trying to set leds to red " +red+" green "+green+" blue "+blue+"\n");
	if (this.mySoftPwmService.isOnline())
	{
		this.mySoftPwmService.setPWMValue(255-(red&0xff), 1);
		this.mySoftPwmService.setPWMValue(255-(green&0xff), 2);
		this.mySoftPwmService.setPWMValue(255-(blue&0xff), 0);
	}
}

Display.prototype.sendTimeStamp = function()
{
	var canMessage = new CanNMTMessage("nmt", "Time");
	canMessage.send();
}

Display.prototype.timerUpdate = function()
{
	/* If LCD service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		this.screenSaverCnt++;
		this.mainScreenCnt++;
		this.exchangeUpdateCnt++;

		if (this.mainScreenCnt > mainScreenTimeout/5 && this.currentMenuItem != this.screenSaverMenuItem)
		{
			/* Go to main screen (time) */
			this.currentMenuItem = this.statusMenuItem;

			/* update the info on display */
			this.updateDisplay();

			this.mainScreenCnt = 0;
		}
		
		if (this.screenSaverCnt > screenSaverTimeout/5)
		{
			/* Go to screensaver menu */
			this.currentMenuItem = this.screenSaverMenuItem;

			/* update the info on display */
			this.updateDisplay();
			
			this.screenSaverCnt = 0;
		}

		if (this.exchangeUpdateCnt > exchangeUpdateTimeout/5)
		{
			/* update the info on display */
			this.updateDisplay();
			
			/* do an exchange lookup */
			this.exchangeCalendar.lookup(this.shortName);
			
			this.exchangeUpdateCnt = 0;
		}

		this.setLEDtoCalendar();
	}
}

