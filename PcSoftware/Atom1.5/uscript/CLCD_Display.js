
Require("cLCD_MenuItems/MainMenuItem.js");
Require("cLCD_MenuItems/DimmerMenuItem.js");
Require("cLCD_MenuItems/SensorMenuItem.js");
Require("cLCD_MenuItems/HDMISwitchMenuItem.js");
Require("cLCD_MenuItems/DtmfMenuItem.js");

function CLCD_Display(aliasnameLCD, aliasnameRotary)
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	var self = this;
	this.myCLCDService = aliasnameLCD;
	this.myRotaryService = aliasnameRotary;

	Module_RegisterToOnMessage(this.myRotaryService, function(alias_name, command, variables) { self.Input_OnMessage(alias_name, command, variables) });
	Module_RegisterToOnChange(this.myRotaryService, function(alias_name, available,test) { self.rotaryOnline(alias_name, available,test) });
	Module_RegisterToOnChange(this.myCLCDService, function(alias_name, available) { self.lcdOnline(alias_name, available) });
	
	/* Start interval timer for sending timestamp to network. Arguments are the callback function and time in milliseconds 
	used to make sure an eth-node gets its init packet (600s) */
	this.myInterval = Timer_SetTimer(function(timer) {self.updateDisplay(timer)}, 5000, true);
	Log("\033[33mmyInterval = "+this.myInterval +"[0m\n");
	
	this.myIntervalScreenSaver = Timer_SetTimer(function(timer) {self.timerUpdate(timer)}, 5000, true);
	Log("\033[33mmyIntervalScreenSaver = "+this.myIntervalScreenSaver +"[0m\n");
	
	this.screenSaverCnt = 0;
	this.mainScreenCnt = 0;
	
	/* create the first menu item */
	this.MainMenuItem = new MainMenuItem(this, this.myCLCDService);
	this.SensorMenuItem = new SensorMenuItem(this, this.myCLCDService);
	this.DimmerMenuItem = new DimmerMenuItem(this, this.myCLCDService);
	this.HdmiMenuItem = new HDMISwitchMenuItem(this, this.myCLCDService);
	this.PIDMenuItem = new DtmfMenuItem(this, this.myCLCDService);
	
	this.currentMenuItem = this.MainMenuItem;

	
	/* create the screensaver menu item */
	this.SensorMenuItem.LeftItem=this.DimmerMenuItem;
	this.SensorMenuItem.RightItem=this.MainMenuItem;

	/* set the function that shall be executed when knob is turned */
	/* create the screensaver menu item */
	this.MainMenuItem.LeftItem=this.SensorMenuItem;
	this.MainMenuItem.RightItem=this.HdmiMenuItem;
	this.DimmerMenuItem.RightItem=this.SensorMenuItem;
	this.DimmerMenuItem.LeftItem=this.PIDMenuItem;
	this.HdmiMenuItem.LeftItem=this.MainMenuItem
	this.HdmiMenuItem.RightItem=this.PIDMenuItem
	this.PIDMenuItem.LeftItem=this.HdmiMenuItem
	this.PIDMenuItem.RightItem=this.DimmerMenuItem
}

/* Declaration of instance variables, for static variables remove prototype */

CLCD_Display.prototype.myCLCDService = null;
CLCD_Display.prototype.myCLCDServiceIsOnline = false;
CLCD_Display.prototype.myRotaryService = null;

CLCD_Display.prototype.myInterval = null;
CLCD_Display.prototype.myIntervalAlways = null;
CLCD_Display.prototype.myIntervalScreenSaver = null;

CLCD_Display.prototype.MainMenuItem = null;
CLCD_Display.prototype.SensorMenuItem = null;
CLCD_Display.prototype.DimmerMenuItem = null;
CLCD_Display.prototype.HdmiMenuItem = null;
CLCD_Display.prototype.PIDMenuItem = null;

/* The currently displayed menuitem */
CLCD_Display.prototype.currentMenuItem = null;

/* counter for going to screensaver */
CLCD_Display.prototype.screenSaverCnt = null;

/* counter for going to main screen */
CLCD_Display.prototype.mainScreenCnt = null;

const Display_mainScreenTimeout = 20;
const Display_screenSaverTimeout = 25;

CLCD_Display.prototype.defaultBacklight = 10;

CLCD_Display.prototype.changeToLeft = function()
{
	if (this.currentMenuItem.LeftItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.LeftItem;
		this.currentMenuItem.onEnter();
		var self = this;
		Timer_Cancel(this.myInterval);
		this.myInterval = Timer_SetTimer(function(timer) {self.updateDisplay(timer)}, this.currentMenuItem.UpdateTime, true);
	}
}

CLCD_Display.prototype.changeToRight = function()
{
	if (this.currentMenuItem.RightItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.RightItem;
		this.currentMenuItem.onEnter();
		var self = this;
		Timer_Cancel(this.myInterval);
		this.myInterval = Timer_SetTimer(function(timer) {self.updateDisplay(timer)}, this.currentMenuItem.UpdateTime, true);
	}
}

CLCD_Display.prototype.changeToUp = function()
{
	if (this.currentMenuItem.UpItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.UpItem;
		this.currentMenuItem.onEnter();
		var self = this;
		Timer_Cancel(this.myInterval);
		this.myInterval = Timer_SetTimer(function(timer) {self.updateDisplay(timer)}, this.currentMenuItem.UpdateTime, true);
	}
}
CLCD_Display.prototype.changeToDown = function()
{
	if (this.currentMenuItem.DownItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.DownItem;
		this.currentMenuItem.onEnter();
		var self = this;
		Timer_Cancel(this.myInterval);
		this.myInterval = Timer_SetTimer(function(timer) {self.updateDisplay(timer)}, this.currentMenuItem.UpdateTime, true);
	}
}

CLCD_Display.prototype.changeToEnter = function()
{
	if (this.currentMenuItem.PressEnterItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.PressEnterItem;
		this.currentMenuItem.onEnter();
		var self = this;
		Timer_Cancel(this.myInterval);
		this.myInterval = Timer_SetTimer(function(timer) {self.updateDisplay(timer)}, this.currentMenuItem.UpdateTime, true);
	}
}

CLCD_Display.prototype.changeToBack = function()
{
	if (this.currentMenuItem.PressBackItem)
	{
		this.currentMenuItem.onExit();
		this.currentMenuItem = this.currentMenuItem.PressBackItem;
		this.currentMenuItem.onEnter();
		var self = this;
		Timer_Cancel(this.myInterval);
		this.myInterval = Timer_SetTimer(function(timer) {self.updateDisplay(timer)}, this.currentMenuItem.UpdateTime, true);
	}
}

CLCD_Display.prototype.rotaryOnline = function(alias_name, available)
{
  Log("\033[33mRotary online/offline.\033[0m\n");
}

CLCD_Display.prototype.Input_OnMessage = function(alias_name, command, variables)
{
	switch (command)
	{
		case "Rotary_Switch":
		{
			this.screenSaverCnt = 0;
			this.mainScreenCnt = 0;
			var last_value_string = Storage_GetParameter("LastValues", this.myCLCDService);
			var last_value = eval("(" + last_value_string + ")");
			
			//Log("\033[33mValue: "+last_value["Strength"]["value"]+"\033[0m\n");
			if (last_value["Strength"]["value"] == 0) {
				Display_SetBacklight(this.myCLCDService, this.defaultBacklight);
			}
			for (var i = 0; i < variables["Steps"]; i++)
			{
				if (variables["Direction"] == "Clockwise")
				{
					this.currentMenuItem.processEvent("left");
				}
				else
				{
					this.currentMenuItem.processEvent("right");
				}
			}

			this.updateDisplay();
			
			break;
		}
		case "Button":
		{
			this.screenSaverCnt = 0;
			this.mainScreenCnt = 0;
			var last_value_string = Storage_GetParameter("LastValues", this.myCLCDService);
			var last_value = eval("(" + last_value_string + ")");
			if (last_value["Strength"]["value"] == 0) {
				Display_SetBacklight(this.myCLCDService, this.defaultBacklight);
			}
			if (variables["Status"] == "Released")
			{
				this.currentMenuItem.processEvent("enter");
				this.updateDisplay();
			}
		
			break;
		}
	}

}

CLCD_Display.prototype.updateDisplay = function(timer)
{
	if (this.myCLCDServiceIsOnline) {
		this.currentMenuItem.update();
	}
	//Log("\033[33mUpdate.\033[0m\n");
}

CLCD_Display.prototype.lcdCenterText = function(text)
{
	returnText = text;
	var displayWidth = 20;	//this.myLCDService.getWidth();
	/* pad on both sides with spaces to displayWidth */
	//returnText = text.pad(displayWidth, " ", 2);
	
	return returnText;
}

CLCD_Display.prototype.lcdOnline = function(alias_name, available)
{
 
	/* If service is not online do nothing */
	if (available)
	{
		Log("\033[33mLCD online.\033[0m\n");	
		this.myCLCDServiceIsOnline = true;
		/* Clear the LCD screen */
		Display_Clear(alias_name);
		/* Set backlight to max */
		Display_SetBacklight(alias_name, this.defaultBacklight);
		this.timerUpdate();
	} else {
		Log("\033[33mLCD offline.\033[0m\n");	
		this.myCLCDServiceIsOnline = false;
	}
}

CLCD_Display.prototype.timerUpdate = function(timer)
{
	/* If LCD service is not online do nothing */
	if (this.myCLCDServiceIsOnline)
	{
	    //Log("\033[33mTmr update.\033[0m\n");
		var last_value_string = Storage_GetParameter("LastValues", this.myCLCDService);
		var last_value = eval("(" + last_value_string + ")");
		
	    
		if (this.mainScreenCnt > Display_mainScreenTimeout/5  && this.currentMenuItem != this.MainMenuItem)
		{
			/* Go to main screen (time) */
			this.currentMenuItem.onExit();
			this.currentMenuItem = this.MainMenuItem;
			this.currentMenuItem.onEnter();
			/* update the info on display */
			this.updateDisplay();
		} else if (this.screenSaverCnt > Display_screenSaverTimeout/5 && last_value["Strength"]["value"] != 0)
		{
			/* Go to screensaver menu */
			Display_SetBacklight(this.myCLCDService, 0);
		} else {
			this.screenSaverCnt++;
			this.mainScreenCnt++;
		}
	}
}

