
loadScript("Logic/MenuItem.js");

function DisplayLinus(type, name, id)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	this.Service(type, name, id);
}

/* We inherit from Service with this call, it must always be next
   after the constructor */
extend(DisplayLinus, Service);

/* Declaration of instance variables, for static variables remove prototype */


/* The display should regulary update and store the calendar */

DisplayLinus.prototype.myLCDService = null;
DisplayLinus.prototype.myRotaryService = null;
DisplayLinus.prototype.myInterval = null;

/*  */
/*  */

/* The currently displayed menuitem */
DisplayLinus.prototype.currentMenuItem = null;

/* counter for going to screensaver */
DisplayLinus.prototype.screenSaverCnt = null;

/* counter for going to main screen */
DisplayLinus.prototype.mainScreenCnt = null;

DisplayLinus.prototype.CurrentPower = 0;
DisplayLinus.prototype.CurrentEnergy = 0;

DisplayLinus.prototype.nextPhoneItem = 0;
DisplayLinus.prototype.firstPhoneItem = 0;

insideTemperature = 0;
outsideTemperature = 0;

const outsideTemperatureSensorId = 236;
const insideTemperatureSensorId = 204;

const mainScreenTimeout = 40;
const screenSaverTimeout = 15;
const defaultBacklight = 255;

const numberOfItemsInPhonebook = 5;

/* This function must always be declared, this is where all the startup code
   should be placed. Gets called with arguments like what ids to use etc. */
DisplayLinus.prototype.initialize = function(initialArguments)
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
	
	if (!this.myInitialArguments["power"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, power-config missing from config.\n");
		return;
	}
	
	if (!this.myInitialArguments["DS18x20"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, DS18x20-config missing from config.\n");
		return;
	}

	if (!this.myInitialArguments["SimpleDTMF"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, SimpleDTMF-config missing from config.\n");
		return;
	}
	
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

	/* Get the PWM service that we want from the ServiceManager, it takes type, service name, service id */
	this.myPowerService = ServiceManager.getService("Can", "power", this.myInitialArguments["power"]["Id"]);
	/* Add a callback for when the service goes online */
	this.myPowerService.registerEventCallback("online", function(args) { self.PowerOnline(); });
	/* Add a callback for when the sensor reports a new value */
	this.myPowerService.registerEventCallback("newValue", function(args) { self.powerUpdate(); });
	/* If the service is already online we should call the handler here */
	this.PowerOnline();

	this.myDTMFService = ServiceManager.getService("Can", "SimpleDTMF", this.myInitialArguments["SimpleDTMF"]["Id"]);
	this.myDTMFService.registerEventCallback("newPhonenumber", function(args) { self.dtmfUpdate(args); });
	//this.myDTMFService.registerEventCallback("online", function(args) { self.dtmfOnline(); });
	this.dtmfOnline();

	this.myOnlinePhonebook = new OnlinePhonebook(function(phonenumber, persons) { self.phonebookLookupCallback(phonenumber, persons); });

	/* Get the LCD service that we want from the ServiceManager, it takes type, service name, service id */
	this.myTempService = ServiceManager.getService("Can", "DS18x20", this.myInitialArguments["DS18x20"]["Id"]);
	/* Add a callback for when the sensor reports a new value */
	this.myTempService.registerEventCallback("newValue", function(args) { self.temperatureUpdate(args); });

	/* create the first menu item */
	this.statusMenuItem = new MenuItem(this);
	this.statusMenuItem.displayData[0] = this.lcdCenterText("Meny1");
	this.statusMenuItem.displayData[1] = this.lcdCenterText("");
	this.statusMenuItem.displayData[2] = this.lcdCenterText("");
	this.statusMenuItem.displayData[3] = this.lcdCenterText("");
	this.statusMenuItem.doUpdate = this.updateStatusMenuItem;
	this.currentMenuItem = this.statusMenuItem;
	
	/* create the menuitem where you can choose to enter the booking sub-menu */
	this.dtmfMenuItem = new MenuItem(this);
	this.dtmfMenuItem.displayData[0] = this.lcdCenterText("Senaste nummer");
	this.dtmfMenuItem.displayData[1] = this.lcdCenterText("");
	this.dtmfMenuItem.displayData[2] = this.lcdCenterText("");
	this.dtmfMenuItem.displayData[3] = this.lcdCenterText("");
	//this.dtmfMenuItem.doUpdate = function(args) { self.updateDtmfMenuItem(); };

	this.dtmfReturnMenuItem = new MenuItem(this);
	this.dtmfReturnMenuItem.displayData[0] = this.lcdCenterText("Tryck for att ga");
	this.dtmfReturnMenuItem.displayData[1] = this.lcdCenterText("till huvudmenyn");
	this.dtmfReturnMenuItem.displayData[2] = this.lcdCenterText("");
	this.dtmfReturnMenuItem.displayData[3] = this.lcdCenterText("");
	//this.dtmfReturnMenuItem.doUpdate = function(args) { self.updateDtmfMenuItem(); };

	this.phoneItem = new Array();
	for (n=0; n< numberOfItemsInPhonebook; n++)
	{
		this.phoneItem[n] = new MenuItem(this);
		this.phoneItem[n].id = n;
	}

	/* connect the items as a linked list */
	this.statusMenuItem.setNextItem(this.dtmfMenuItem);
	this.statusMenuItem.setPrevItem(this.dtmfMenuItem);
	/* set the function that shall be executed when knob is turned */
	this.statusMenuItem.doRight = function(args) { self.changeToNext(); };
	this.statusMenuItem.doLeft = function(args) { self.changeToPrev(); };
	
	/* connect the items as a linked list */
	this.dtmfMenuItem.setNextItem(this.statusMenuItem);
	this.dtmfMenuItem.setPrevItem(this.statusMenuItem);
	this.dtmfMenuItem.setDescItem(this.dtmfReturnMenuItem);
	/* set the function that shall be executed when knob is turned */
	this.dtmfMenuItem.doRight = function(args) { self.changeToNext(); };
	this.dtmfMenuItem.doLeft = function(args) { self.changeToPrev(); };
	this.dtmfMenuItem.doPress = function(args) { self.changeToDesc(); };

	/* connect the items as a linked list */
	this.dtmfReturnMenuItem.setNextItem(this.dtmfReturnMenuItem);
	this.dtmfReturnMenuItem.setPrevItem(this.dfmfReturnMenuItem);
	this.dtmfReturnMenuItem.setDescItem(this.dtmfMenuItem);
	this.dtmfReturnMenuItem.id = 255;
	/* set the function that shall be executed when knob is turned */
	this.dtmfReturnMenuItem.doRight = function(args) { self.changeToNext(); };
	this.dtmfReturnMenuItem.doLeft = function(args) { self.changeToPrev(); };
	this.dtmfReturnMenuItem.doPress = function(args) { self.changeToDesc(); };
}

DisplayLinus.prototype.changeToDesc = function()
{
	if (this.currentMenuItem.descItem)
	{
		this.currentMenuItem = this.currentMenuItem.descItem;
	}
}

DisplayLinus.prototype.changeToNext = function()
{
	if (this.currentMenuItem.nextItem)
	{
		this.currentMenuItem = this.currentMenuItem.nextItem;
	}
}

DisplayLinus.prototype.changeToPrev = function()
{
	if (this.currentMenuItem.prevItem)
	{
		this.currentMenuItem = this.currentMenuItem.prevItem;
	}
}


DisplayLinus.prototype.updateStatusMenuItem = function()
{
	var date = new Date();
	/* Get the current date time on the format YYYY-mm-dd HH.ii.ss */
	var dateAndTime = date.getDateTimeFormated();
	//var date = new Date();
	//var dateAndTime = "" + date.getTimeShortFormated();
	this.displayData[1] = this.parentDisplay.lcdCenterText(""+dateAndTime);

this.displayData[2] = this.parentDisplay.lcdCenterText(""+insideTemperature + " C   " + outsideTemperature + " C");

	this.displayData[3] = this.parentDisplay.lcdCenterText(""+this.parentDisplay.CurrentPower.toString()+" Watt "+(this.parentDisplay.CurrentEnergy/1000).toString() + "kWh");
}

DisplayLinus.prototype.temperatureUpdate = function(sensorId)
{
	if (sensorId == outsideTemperatureSensorId) {
		outsideTemperature = this.myTempService.getValue(sensorId).toFixed(2).toString();
	}
	if (sensorId == insideTemperatureSensorId) {
		insideTemperature = this.myTempService.getValue(sensorId).toFixed(2).toString();
	}
}

DisplayLinus.prototype.phonebookLookupCallback = function(phonenumber, persons)
{
      //log(this.myName + ":" + this.myId + "> New phonenumber\n");
	// Update main dtmf menu
	var date = new Date();
	/* Get the current date time on the format YYYY-mm-dd HH.ii.ss */
	var dateAndTime = date.getDateTimeFormated();
	this.dtmfMenuItem.displayData[1] = this.lcdCenterText(""+dateAndTime);
	this.dtmfMenuItem.displayData[2] = this.lcdCenterText(""+phonenumber);
	this.dtmfMenuItem.displayData[3] = this.lcdCenterText(""+persons[0]);
	
	/* This is used for function declarations like the callbacks below */
	var self = this;

	if (this.dtmfReturnMenuItem.nextItem == this.dtmfReturnMenuItem) //the list is empty
	{
		// connect the items as a linked list 
		this.phoneItem[0].setNextItem(this.dtmfReturnMenuItem);
		this.phoneItem[0].setPrevItem(this.dtmfReturnMenuItem);
		this.phoneItem[0].setDescItem(this.statusMenuItem);
		this.dtmfReturnMenuItem.setNextItem(this.phoneItem[0]);
		this.dtmfReturnMenuItem.setPrevItem(this.phoneItem[0]);
		this.nextPhoneItem = 1 % numberOfItemsInPhonebook;
		this.lastPhoneItem = 0;
		this.phoneItem[0].doRight = function(args) { self.changeToNext(); };
		this.phoneItem[0].doLeft = function(args) { self.changeToPrev(); };
		this.phoneItem[0].doPress = function(args) { self.changeToDesc(); };
		this.phoneItem[0].displayData[0] = this.lcdCenterText(""+dateAndTime);
		this.phoneItem[0].displayData[1] = this.lcdCenterText(""+phonenumber);
		if (persons.length >= 1) {
			this.phoneItem[0].displayData[2] = this.lcdCenterText(""+persons[0]);
		} else {
			this.phoneItem[0].displayData[2] = this.lcdCenterText("");
		}
		if (persons.length >= 2) {
			this.phoneItem[0].displayData[3] = this.lcdCenterText(""+persons[1]);
		} else {
			this.phoneItem[0].displayData[3] = this.lcdCenterText("");
		}	
	
	} else if (this.nextPhoneItem != this.lastPhoneItem) {		//Not full
		// connect the items as a linked list 
		this.phoneItem[this.nextPhoneItem].setNextItem(this.phoneItem[(this.nextPhoneItem-1 + numberOfItemsInPhonebook) % numberOfItemsInPhonebook]);
		this.phoneItem[this.nextPhoneItem].setPrevItem(this.dtmfReturnMenuItem);
		this.phoneItem[this.nextPhoneItem].setDescItem(this.statusMenuItem);
		this.dtmfReturnMenuItem.setNextItem(this.phoneItem[this.nextPhoneItem]);
		this.phoneItem[(this.nextPhoneItem-1+ numberOfItemsInPhonebook) % numberOfItemsInPhonebook].setPrevItem(this.phoneItem[this.nextPhoneItem]);
		this.phoneItem[this.nextPhoneItem].doRight = function(args) { self.changeToNext(); };
		this.phoneItem[this.nextPhoneItem].doLeft = function(args) { self.changeToPrev(); };
		this.phoneItem[this.nextPhoneItem].doPress = function(args) { self.changeToDesc(); };
		this.phoneItem[this.nextPhoneItem].displayData[0] = this.lcdCenterText(""+dateAndTime);
		this.phoneItem[this.nextPhoneItem].displayData[1] = this.lcdCenterText(""+phonenumber);
		if (persons.length >= 1) {
			this.phoneItem[this.nextPhoneItem].displayData[2] = this.lcdCenterText(""+persons[0]);
		} else {
			this.phoneItem[this.nextPhoneItem].displayData[2] = this.lcdCenterText("");
		}
		if (persons.length >= 2) {
			this.phoneItem[this.nextPhoneItem].displayData[3] = this.lcdCenterText(""+persons[1]);
		} else {
			this.phoneItem[this.nextPhoneItem].displayData[3] = this.lcdCenterText("");
		}
		this.nextPhoneItem = (this.nextPhoneItem + 1) % numberOfItemsInPhonebook;
	} else  {	//buffer full, start removing oldest item
		// connect the items as a linked list (replace last item)
		this.phoneItem[this.nextPhoneItem].setNextItem(this.phoneItem[(this.nextPhoneItem-1+ numberOfItemsInPhonebook) % numberOfItemsInPhonebook]);
		this.phoneItem[this.nextPhoneItem].setPrevItem(this.dtmfReturnMenuItem);
		this.phoneItem[this.nextPhoneItem].setDescItem(this.statusMenuItem);
		this.dtmfReturnMenuItem.setNextItem(this.phoneItem[this.nextPhoneItem]);
		this.phoneItem[(this.nextPhoneItem-1+ numberOfItemsInPhonebook) % numberOfItemsInPhonebook].setPrevItem(this.phoneItem[this.nextPhoneItem]);
		this.phoneItem[this.nextPhoneItem].doRight = function(args) { self.changeToNext(); };
		this.phoneItem[this.nextPhoneItem].doLeft = function(args) { self.changeToPrev(); };
		this.phoneItem[this.nextPhoneItem].doPress = function(args) { self.changeToDesc(); };
		this.phoneItem[this.nextPhoneItem].displayData[0] = this.lcdCenterText(""+dateAndTime);
		this.phoneItem[this.nextPhoneItem].displayData[1] = this.lcdCenterText(""+phonenumber);
		if (persons.length >= 1) {
			this.phoneItem[this.nextPhoneItem].displayData[2] = this.lcdCenterText(""+persons[0]);
		} else {
			this.phoneItem[this.nextPhoneItem].displayData[2] = this.lcdCenterText("");
		}
		if (persons.length >= 2) {
			this.phoneItem[this.nextPhoneItem].displayData[3] = this.lcdCenterText(""+persons[1]);
		} else {
			this.phoneItem[this.nextPhoneItem].displayData[3] = this.lcdCenterText("");
		}
		//this.nextPhoneItem = (this.nextPhoneItem + 1) % numberOfItemsInPhonebook;
		this.lastPhoneItem = (this.lastPhoneItem + 1) % numberOfItemsInPhonebook;
		this.dtmfReturnMenuItem.setPrevItem(this.phoneItem[this.lastPhoneItem]);
		this.nextPhoneItem = (this.nextPhoneItem + 1) % numberOfItemsInPhonebook;
	}

	/* Go to dtmf screen (time) */
	this.currentMenuItem = this.dtmfMenuItem;
	
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	if (this.myLCDService.getBacklight() != defaultBacklight) 
	{
	      this.myLCDService.setBacklight(defaultBacklight);
	}
	/* update the info on display */
	this.updateDisplay();

}

DisplayLinus.prototype.replaceAumlauts = function(intext)
{
	intext = intext.replace(/Å/, "A");
	intext = intext.replace(/å/, "a");
	intext = intext.replace(/Ä/, "A");
	intext = intext.replace(/ä/, "a");
	intext = intext.replace(/Ö/, "O");
	intext = intext.replace(/ö/, "o");
	return intext;
}


DisplayLinus.prototype.rotaryOnline = function()
{
}

DisplayLinus.prototype.rotaryPosUpdate = function(SwitchId)
{
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	if (this.myLCDService.getBacklight() != defaultBacklight) 
	{
	      this.myLCDService.setBacklight(defaultBacklight);
	}
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

DisplayLinus.prototype.rotaryBtnUpdate = function(SwitchId)
{
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	if (this.myLCDService.getBacklight() != defaultBacklight) 
	{
	      this.myLCDService.setBacklight(defaultBacklight);
	}
	if (this.myRotaryService.getButtonStatus(SwitchId) == "Released")
	{
		if (this.currentMenuItem.doPress)
		{
			this.currentMenuItem.doPress();
		}
		this.updateDisplay();
	}
}

DisplayLinus.prototype.updateDisplay = function()
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


DisplayLinus.prototype.lcdCenterText = function(text)
{
	returnText = text;
	var displayWidth = 20;	//this.myLCDService.getWidth();
	/* pad on both sides with spaces to displayWidth */
	returnText = text.pad(displayWidth, " ", 2);
	
	return returnText;
}

DisplayLinus.prototype.lcdOffline = function()
{
	/* If we have no interval timer running do nothing */
	if (this.myInterval != null)
	{
		/* Stop the interval */
		this.myInterval.stop();
	}
}

DisplayLinus.prototype.lcdOnline = function()
{
	/* If service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		/* Clear the LCD screen */
		this.myLCDService.clearScreen();
		/* Set backlight to max */
		this.myLCDService.setBacklight(defaultBacklight);

		/* If we have no interval timer running start it */
		if (this.myInterval == null)
		{
			var self = this;
		
			/* Start interval timer for our printout. Arguments are the callback function and time in milliseconds */
			this.myInterval = new Interval(function() { self.timerUpdate() }, 5000);
		}
		
		this.myInterval.start();
		
		this.timerUpdate();

		this.updateDisplay();
	}
}

DisplayLinus.prototype.PowerOnline = function()
{
	/* If service is not online do nothing */
	if (this.myPowerService.isOnline())
	{
		log("Power is online!\n");
		/* Set report interval to 2 seconds */
		this.myPowerService.setReportInterval(2);
	}
}

DisplayLinus.prototype.powerUpdate = function()
{
	this.CurrentPower = this.myPowerService.getPower();
	this.CurrentEnergy = this.myPowerService.getEnergy();
	if (this.currentMenuItem == this.statusMenuItem) 
	{
		this.updateDisplay();
	}
}

DisplayLinus.prototype.dtmfOnline = function()
{
}

DisplayLinus.prototype.dtmfUpdate = function(args)
{
	this.myOnlinePhonebook.lookup(this.myDTMFService.getLastPhonenumber());
}

DisplayLinus.prototype.timerUpdate = function()
{
	/* If LCD service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		this.screenSaverCnt++;
		this.mainScreenCnt++;

		if (this.mainScreenCnt > mainScreenTimeout/5)
		{
			/* Go to main screen (time) */
			this.currentMenuItem = this.statusMenuItem;

			/* update the info on display */
			this.updateDisplay();
		}
		
		if (this.screenSaverCnt > screenSaverTimeout/5)
		{
			if (this.myLCDService.getBacklight() != 0) 
			{
			      this.myLCDService.setBacklight(0);
			}
		}

		
	}
	
}

