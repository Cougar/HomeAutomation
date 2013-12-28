function bedroomDisplay(aliasDisplay, aliasPower, aliasOutsideTemp, aliasStek)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	var self = this;
	
	/* Store the alias of the hardware that is used */
	this.myDisplay = aliasDisplay;
	this.myPower = aliasPower;
	this.myStek=aliasStek;
	this.myOutsideTemp = aliasOutsideTemp;

	Log("\033[33mBedroom display controller created.\033[0m");

	/* Register callbacks for information on the hardware online/offline status */
	Module_RegisterToOnChange(this.myDisplay, function(alias_name, available) { self.displayOnline(alias_name, available) });
	Module_RegisterToOnChange(this.myPower, function(alias_name, available) { self.powerOnline(alias_name, available) });
	Module_RegisterToOnChange(this.myOutsideTemp, function(alias_name, available) { self.outsideTempOnline(alias_name, available) });

	/* Start a timer which will call a function every 5 seconds */
	this.myUpdater = Timer_SetTimer(function(timer) {self.updateDisplay(timer)}, 10000, true);

	/* Reset counter for displayitem */
	this.displayItemCounter=0;
}

bedroomDisplay.prototype.myDisplay = null;
bedroomDisplay.prototype.myDisplayIsOnline = false;
bedroomDisplay.prototype.myPower = null;
bedroomDisplay.prototype.myStek = null;
bedroomDisplay.prototype.myPowerIsOnline = false;
bedroomDisplay.prototype.myOutsideTemp = null;
bedroomDisplay.prototype.myOutsideTempIsOnline = false;
bedroomDisplay.prototype.myUpdater = null;
bedroomDisplay.prototype.displayItemCounter = null;
bedroomDisplay.prototype.lastTimeHour = 0;
bedroomDisplay.prototype.lastTimeMinute = 0;

/* When the online status of the display changes */
bedroomDisplay.prototype.displayOnline = function(alias_name, available)
{
	this.myDisplayIsOnline = available;
	if (available)
	{
		//Log("\033[33m--Display online.\033[0m");
		/* Clear the LCD screen */
		//Display_Clear(alias_name);
		/* Set backlight to max */
		//Display_SetBacklight(alias_name, this.defaultBacklight);
		
		/* Send to display */
		this.updateDisplay();
	} else {
		//Log("\033[33m--Display offline.\033[0m");
	}
}

/* When the online status of the power meter changes */
bedroomDisplay.prototype.powerOnline = function(alias_name, available)
{
	this.myPowerIsOnline = available;
	if (this.myPowerIsOnline)
	{
		//Log("\033[33m--Power meter online.\033[0m");
	} else {
		//Log("\033[33m--Power meter offline.\033[0m");
	}
}

/* When the online status of the outside temperature sensor changes */
bedroomDisplay.prototype.outsideTempOnline = function(alias_name, available)
{
	this.myOutsideTempIsOnline = available;
	if (this.myOutsideTempIsOnline)
	{
		//Log("\033[33m--Outside temperature sensor online.\033[0m");
	} else {
		//Log("\033[33m--Outside temperature sensor offline.\033[0m");
	}
}

bedroomDisplay.prototype.displayTime = function()
{
	var shorttime = new Date().getTimeShortFormated();
	/* Remove leading zero if any */
	var data = parseInt(shorttime.substr(0,2));
	if (data != this.lastTimeHour) {
	  //Log("\033[33m--New Time.\033[0m");
	  this.lastTimeHour = data;
	  if (data < 7 || data >= 22) {
	    GDisplay_SetBacklight(this.myDisplay, 2);
	    //Log("\033[33m--Setting backlight at 5.\033[0m");
	  } else if (data < 9 || data >= 21) {
	    GDisplay_SetBacklight(this.myDisplay, 20);
	    //Log("\033[33m--Setting backlight at 20.\033[0m");
	  } else {
	    GDisplay_SetBacklight(this.myDisplay, 128);
	    //Log("\033[33m--Setting backlight at 128.\033[0m");
	  }
	}
	data = parseInt(shorttime.substr(3,2));
	if (data != this.lastTimeMinute) {
	  this.lastTimeMinute = data;
	  if (shorttime.substr(0,1) == "0")
	  {
		  shorttime = shorttime.substr(1,4);
	  } 
	  this.displayPrintRightAdj(shorttime,0);
	}
}

bedroomDisplay.prototype.displayElectricPower = function()
{
	if (this.myPowerIsOnline)
	{
		var last_value_string = Storage_GetParameter("LastValues", this.myPower);
		var last_value = eval("(" + last_value_string + ")");
	
		var powerusage = last_value["Power32"]["value"];
		var timestamp = last_value["Power32"]["timestamp"];
		if (timestamp+120 > get_time())
		{
			//Log("\033[33m--"+timestamp+" "+get_time()+".\033[0m");
			powerusage = Math.round(powerusage/10)*10;
			var unit = "W";
			if (powerusage > 9999)
			{
				powerusage = Math.round(powerusage/1000);
				unit = "kW";
			}
			else if (powerusage > 999)
			{
				powerusage = Math.round(powerusage/100)/10;
				unit = "kW";
			}
			this.displayPrintRightAdj(powerusage+unit,1);
		}
		else
		{
			//Log("\033[33m--Data to old, electricpower, timestamp: "+timestamp+" current time: "+get_time()+".\033[0m");
			/* Show time */
			this.displayPrintRightAdj("  -  ",1);
		}
	}
	else
	{
		/* Show time */
		this.displayPrintRightAdj("  -  ",1);
	}
}


bedroomDisplay.prototype.displayOutsideTemperature = function()
{
	if (this.myOutsideTempIsOnline)
	{
		var last_value_string = Storage_GetParameter("LastValues", this.myOutsideTemp);
		var last_value = eval("(" + last_value_string + ")");
	
		var temperature = last_value["Temperature_Celsius"]["value"];
		var timestamp = last_value["Temperature_Celsius"]["timestamp"];
		if (timestamp+120 > get_time())
		{
			temperature = Math.round(temperature);
			var unit = "¤C";
			this.displayPrintRightAdj(temperature+unit,1);
		}
		else
		{
			//Log("\033[33m--Data to old, temperature, timestamp: "+timestamp+" current time: "+get_time()+".\033[0m");
			/* Show time */
			this.displayPrintRightAdj("  -  ",1);
		}
	}
	else
	{
		/* Show time */
		this.displayPrintRightAdj("  -  ",1);
	}
}

bedroomDisplay.prototype.displayPrintRightAdj = function(text, row)
{
	/* Find adjustment in x axis, 5 chars fits in the display */
	text = text.pad(5, " ", 0);
	GDisplay_Print(this.myDisplay, 0, row, "Standard", "Standard", text);
}

/* When the timer executes */
bedroomDisplay.prototype.updateDisplay = function(timer)
{
	/* If display service is not online do nothing */
	if (this.myDisplayIsOnline)
	{
		this.displayTime();
	  //Log("\033[33m--Update.\033[0m");

		var last_value_string = Storage_GetParameter("LastValues", this.myStek);
		var last_value = eval("(" + last_value_string + ")");
	
		var temperature = last_value["Temperature_Celsius"]["value"];
		var timestamp = last_value["Temperature_Celsius"]["timestamp"];
		if (timestamp+30 > get_time() && temperature > 28)
		{
			temperature = Math.round(temperature);
			var unit = "¤C";
			this.displayPrintRightAdj(temperature+unit,1);
		}
		else
		{
		switch (this.displayItemCounter)
		{
		case 0:
			/* Show outdoor temperature */
			this.displayOutsideTemperature();
			break;
		case 1:
			/* Show electrical power usage */
			this.displayElectricPower();
			break;
		}
		}
		/* Clear position indicator (the LED on the right side of the display) */
		GDisplay_DrawLine(this.myDisplay, 31, 0, 31, 7, "Inverted");
		/* Set position indicator */
		GDisplay_DrawLine(this.myDisplay, 31, this.displayItemCounter, 31, this.displayItemCounter, "Standard");
		
		this.displayItemCounter++;
		if (this.displayItemCounter == 2)
		{
			this.displayItemCounter = 0;
		}
	} else {
	//	Log("\033[33m--Update failed.\033[0m");
	  
	}
}

