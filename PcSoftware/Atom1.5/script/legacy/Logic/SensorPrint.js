
function SensorPrint(type, name, id)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	this.Service(type, name, id);
}

/* We inherit from Service with this call, it must always be next
   after the constructor */
extend(SensorPrint, Service);

/* Declaration of instance variables, for static variables remove prototype */
SensorPrint.prototype.myLCDService = null;
SensorPrint.prototype.myDTMFService = null;
SensorPrint.prototype.myTempService = null;
SensorPrint.prototype.myVoltService = null;
SensorPrint.prototype.myInterval = null;
SensorPrint.prototype.myOnlinePhonebook = null;

/* This function must always be declared, this is where all the startup code
   should be placed. Gets called with arguments like what ids to use etc. */
SensorPrint.prototype.initialize = function(initialArguments)
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
	
	if (!this.myInitialArguments["DS18x20"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, DS18x20-config missing from config.\n");
		return;
	}
	
	if (!this.myInitialArguments["BusVoltage"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, BusVoltage-config missing from config.\n");
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
	
	/* Get the LCD service that we want from the ServiceManager, it takes type, service name, service id */
	this.myTempService = ServiceManager.getService("Can", "DS18x20", this.myInitialArguments["DS18x20"]["Id"]);
	/* Add a callback for when the sensor reports a new value */
	this.myTempService.registerEventCallback("newValue", function(args) { self.temperatureUpdate(args); });
	/* Add a callback for when the service goes online */
	this.myTempService.registerEventCallback("online", function(args) { self.tempOnline(); });
	/* If the service is already online we should call the handler here */
	this.tempOnline();
	
	/* Get the LCD service that we want from the ServiceManager, it takes type, service name, service id */
	this.myVoltService = ServiceManager.getService("Can", "BusVoltage", this.myInitialArguments["BusVoltage"]["Id"]);
	/* Add a callback for when the sensor reports a new value */
	this.myVoltService.registerEventCallback("newValue", function(args) { self.voltageUpdate(args); });
	/* Add a callback for when the service goes online */
	this.myVoltService.registerEventCallback("online", function(args) { self.voltOnline(); });
	/* If the service is already online we should call the handler here */
	this.voltOnline();
	
	
	this.myDTMFService = ServiceManager.getService("Can", "SimpleDTMF", this.myInitialArguments["SimpleDTMF"]["Id"]);
	this.myDTMFService.registerEventCallback("newPhonenumber", function(args) { self.dtmfUpdate(args); });
	//this.myDTMFService.registerEventCallback("online", function(args) { self.dtmfOnline(); });
	this.dtmfOnline();

	this.myOnlinePhonebook = new OnlinePhonebook(function(phonenumber, persons) { self.phonebookLookupCallback(phonenumber, persons); });
}

SensorPrint.prototype.dtmfOnline = function()
{
}

SensorPrint.prototype.dtmfUpdate = function(args)
{
	this.myOnlinePhonebook.lookup(this.myDTMFService.getLastPhonenumber());
}

SensorPrint.prototype.phonebookLookupCallback = function(phonenumber, persons)
{
	if (this.myLCDService.isOnline())
	{
		var personString = "";
	
		for (var n = 0; n < persons.length; n++)
		{
			this.myLCDService.printText(0, n+1, persons[n].pad(18, ' ', 1));
			/* Print out what we are doing to the console */
			log(this.myName + ":" + this.myId + "> Trying to print " + persons[n] + " to LCD\n");
		}
		
		for (var c = n+1; c < 4; c++)
		{
			var str = "                  ";
			this.myLCDService.printText(0, c, str);
		}
	}
}

SensorPrint.prototype.lcdOnline = function()
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
		Log("starting interval!");
			/* Start interval timer for our printout. Arguments are the callback function and time in seconds */
			this.myInterval = new Interval(function() { self.timerUpdate() }, 1000, false);
		}
		
		this.myInterval.start();
	}
}

SensorPrint.prototype.lcdOffline = function()
{
	/* If we have no interval timer running do nothing */
	if (this.myInterval != null)
	{
		/* Stop the interval */
		this.myInterval.stop();
	}
}

SensorPrint.prototype.tempOnline = function()
{
	/* If service is not online do nothing */
	if (this.myTempService.isOnline())
	{
		/* Set report interval to 5 seconds */
		this.myTempService.setReportInterval(5);
	}
}

SensorPrint.prototype.voltOnline = function()
{
	/* If service is not online do nothing */
	if (this.myVoltService.isOnline())
	{
		/* Set report interval to 5 seconds */
		this.myVoltService.setReportInterval(5);
	}
}

SensorPrint.prototype.temperatureUpdate = function(sensorId)
{
	/* If LCD service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		/* Format our data to the right format */
		var data = (this.myTempService.getValue(sensorId).toFixed(2).toString() + " C").pad(7, " ", 1);
		/* Print the text to the LCD */
		this.myLCDService.printText(0, 0, data);
		/* Print out what we are doing to the console */
		log(this.myName + ":" + this.myId + "> Trying to print " + data + " value to LCD\n");
	}
}

SensorPrint.prototype.voltageUpdate = function(sensorId)
{
	/* If LCD service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		/* Format our data to the right format */
		var data = (this.myVoltService.getValue(sensorId).toString() + " V").pad(7, " ", 1);
		/* Print the text to the LCD */
		this.myLCDService.printText(8, 0, data);
		/* Print out what we are doing to the console */
		log(this.myName + ":" + this.myId + "> Trying to print " + data + " value to LCD\n");
	}
}

SensorPrint.prototype.timerUpdate = function()
{
	Log("TimerUpdate called!");
	/* If LCD service is not online do nothing */
	if (this.myLCDService.isOnline())
	{
		var date = new Date();
		/* Get the current date time on the format YYYY-mm-dd HH.ii.ss */
		var dateAndTime = date.getDateTimeFormated();
		/* Print the text to the LCD */
		this.myLCDService.printText(0, 3, dateAndTime);
		/* Print out what we are doing to the console */
		log(this.myName + ":" + this.myId + "> Trying to print " + dateAndTime + " to LCD\n");
	}
}
