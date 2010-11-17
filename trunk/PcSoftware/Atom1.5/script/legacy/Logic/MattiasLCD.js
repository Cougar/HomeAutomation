
function MattiasLCD(type, name, id)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	this.Service(type, name, id);
}

/* We inherit from Service with this call, it must always be next
   after the constructor */
extend(MattiasLCD, Service);

/* Declaration of instance variables, for static variables remove prototype */
MattiasLCD.prototype.myLCDService = null;
MattiasLCD.prototype.myDTMFService = null;
MattiasLCD.prototype.myOnlinePhonebook = null;

/* This function must always be declared, this is where all the startup code
   should be placed. Gets called with arguments like what ids to use etc. */
MattiasLCD.prototype.initialize = function(initialArguments)
{
	/* We must always call the parent initialize */
	this.Service.prototype.initialize.call(this, initialArguments);

	/* This is used for function decalrations like the callbacks below */
	var self = this;

	if (!this.myInitialArguments["HD44789"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, HD44789-config missing from config.\n");
		return;
	}
	
	if (!this.myInitialArguments["SimpleDTMF"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, SimpleDTMF-config missing from config.\n");
		return;
	}
	
	/* Get the LCD service that we want from the ServiceManager, it takes type, service name, service id */
	this.myLCDService = ServiceManager.getService("Can", "HD44789", this.myInitialArguments["HD44789"]["Id"]);
	/* Add a callback for when the service goes online */
	this.myLCDService.registerEventCallback("online", function(args) { self.lcdOnline(); });
	/* If the service is already online we should call the handler here */
	this.lcdOnline();
	/* Add a callback for when the service goes offline */
	this.myLCDService.registerEventCallback("offline", function(args) { self.lcdOffline(); });
	
	
	this.myDTMFService = ServiceManager.getService("Can", "SimpleDTMF", this.myInitialArguments["SimpleDTMF"]["Id"]);
	this.myDTMFService.registerEventCallback("newPhonenumber", function(args) { self.dtmfUpdate(args); });
	//this.myDTMFService.registerEventCallback("online", function(args) { self.dtmfOnline(); });
	this.dtmfOnline();

	this.myOnlinePhonebook = new OnlinePhonebook(function(phonenumber, persons) { self.phonebookLookupCallback(phonenumber, persons); });
}

MattiasLCD.prototype.dtmfOnline = function()
{
}

MattiasLCD.prototype.dtmfUpdate = function(args)
{
	this.myOnlinePhonebook.lookup(this.myDTMFService.getLastPhonenumber());
}

MattiasLCD.prototype.phonebookLookupCallback = function(phonenumber, persons)
{
	if (this.myLCDService.isOnline())
	{
		var date = new Date();
		/* Get the current date time on the format YYYY-mm-dd HH.ii.ss */
		var dateAndTime = date.getDateTimeFormated();
		/* Print the text to the LCD */
		this.myLCDService.printText(0, 0, dateAndTime);
	
		log(this.myName + ":" + this.myId + "> Call from " + phonenumber + "\n");
	
		for (var n = 0; n < persons.length; n++)
		{
			
			this.myLCDService.printText(0, n+1, persons[n].pad(18, ' ', 1));
			/* Print out what we are doing to the console */
			log(this.myName + ":" + this.myId + "> Call from " + persons[n] + "\n");
		}
		
		for (var c = n+1; c < 4; c++)
		{
			var str = "                  ";
			this.myLCDService.printText(0, c, str);
		}
	}
}

MattiasLCD.prototype.lcdOnline = function()
{
	/* If service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		/* Clear the LCD screen */
		this.myLCDService.clearScreen();
		/* Set backlight to max */
		this.myLCDService.setBacklight(255);
	}
}

MattiasLCD.prototype.lcdOffline = function()
{
	/* If we have no interval timer running do nothing */
	if (this.myInterval != null)
	{
		/* Stop the interval */
		this.myInterval.stop();
	}
}
