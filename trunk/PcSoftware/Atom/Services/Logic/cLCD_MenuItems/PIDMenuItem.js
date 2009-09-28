/* the display object who created the menu item */
PIDMenuItem.prototype.parentDisplay = null;
//PIDMenuItem.prototype.sensors = null;
PIDMenuItem.prototype.mode = 0;
PIDMenuItem.prototype.sub_mode = 0;
PIDMenuItem.prototype.tempVariable = 0;
PIDMenuItem.prototype.myPIDService = null;
//PIDMenuItem.prototype.currentSensorItem = 0;
//PIDMenuItem.prototype.window_low = 0;
//PIDMenuItem.prototype.window_high = 3; //set to nummber of rows you want to display minus 1


function PIDMenuItem(parentDisplay, hd44789Object)
{
	var self = this;
	this.parentDisplay = parentDisplay;
	this.display = hd44789Object;
	
	if (!this.parentDisplay.myInitialArguments["PID"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, PID-config missing from config.\n");
		return;
	}
	
	/* Get the PWM service that we want from the ServiceManager, it takes type, service name, service id */
	this.myPIDService = ServiceManager.getService("Can", "PID", this.parentDisplay.myInitialArguments["PID"]["Id"]);
}

/* the display object who created the menu item */
PIDMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
PIDMenuItem.prototype.display = null;

/* How often the display shall update [ms]*/
PIDMenuItem.prototype.UpdateTime = 5000;

/* what PIDMenuItem is left of this item, if used */
PIDMenuItem.prototype.LeftItem = null;
/* what PIDMenuItem is right of this item, if used */
PIDMenuItem.prototype.RightItem = null;
/* what PIDMenuItem is after of this item, if used */
PIDMenuItem.prototype.PressEnterItem = null;

/*
Standard events can be:
left, right, enter, back, up, down....
*/
PIDMenuItem.prototype.processEvent = function (event)
{
	switch (this.mode)
	{
	case 0:
		switch (event)
		{
		case "right":
			this.parentDisplay.changeToRight();
			break;
		case "left":
			this.parentDisplay.changeToLeft();
			break;
		case "enter":
			this.mode = 1;
			this.sub_mode = 0;
			this.display.clearScreen();
			break;
		}
		break;
	case 1:
		switch (event)
		{
		case "right":
			this.sub_mode++;
			if (this.sub_mode > 2) {
				this.sub_mode = 0;
			}
			break;
		case "left":
			if (this.sub_mode == 0) {
				this.sub_mode = 2;
			}
			this.sub_mode--;
			break;
		case "enter":
			switch (this.sub_mode)
			{
			case 0:
				this.mode = 2;
				this.sub_mode = 0;
				this.tempVariable = this.myPIDService.Reference;
				break;
			case 1:
				this.mode = 3;
				this.sub_mode = 0;
				break;
			case 2:
				this.mode = 0;
				this.sub_mode = 0;
				break;
			}
			this.display.clearScreen();
			break;
		}
		break;
	case 2:
		switch (event)
		{
		case "right":
			if (this.sub_mode == 3) {
				this.tempVariable = this.tempVariable + 0.5;
			} else {
				this.sub_mode++;
				if (this.sub_mode > 3) {
					this.sub_mode = 0;
				}
			}
			break;
		case "left":
			if (this.sub_mode == 3) {
				this.tempVariable = this.tempVariable - 0.5;
			} else {
				if (this.sub_mode == 0) {
					this.sub_mode = 3;
				}
				this.sub_mode--;
			}
			break;
		case "enter":
			switch (this.sub_mode)
			{
			case 0:
				this.sub_mode = 3;
				break;
			case 1:
				//spara
				this.myPIDService.setValue(this.tempVariable);
				this.mode = 0;
				this.sub_mode = 0;
				break;
			case 2:
				this.mode = 0;
				this.sub_mode = 0;
				break;
			case 3:
				this.sub_mode = 0;
				break;
			}
			break;
		}
		break;
	case 3:
		switch (event)
		{
		case "right":
			break;
		case "left":
			break;
		case "enter":
			this.mode = 0;
			this.sub_mode = 0;
			break;
		}
		break;
	}
}

PIDMenuItem.prototype.onEnter = function ()
{
	this.mode = 0;
	this.display.clearScreen();
}
PIDMenuItem.prototype.update = function ()
{
	//this.display.clearScreen();
	switch (this.mode)
	{
	case 1:
		switch (this.sub_mode)
		{
		case 0:
			this.display.printText(0, 3,"> Set Temperature");
			break;
		case 1:
			this.display.printText(0, 3,"> Settings");
			break;
		case 2:
			this.display.printText(0, 3,"> Tillbaka");
			break;
		}
	case 0:
		this.display.printText(0, 0,"Reference:     "+this.myPIDService.Reference.toFixed(1).toString()+"¤C");
		this.display.printText(0, 1,"Measurment:    "+this.myPIDService.Reference.toFixed(1).toString()+"¤C");
		this.display.printText(0, 2,"Out: "+this.myPIDService.PWM.toFixed(1).toString()+"%   Sum: "+this.myPIDService.sum.toFixed(1).toString());
		break;
	case 2:
		this.display.printText(0, 0,"Set temperature:");
		switch (this.sub_mode)
		{
		case 0:
			this.display.printText(0, 1,"   > "+this.tempVariable.toFixed(1).toString()+"¤C");
			this.display.printText(0, 3,"  Spara     Ångra");
			break;
		case 1:
			this.display.printText(0, 1,"     "+this.tempVariable.toFixed(1).toString()+"¤C")
			this.display.printText(0, 3,"> Spara     Ångra");
			break;
		case 2:
			this.display.printText(0, 1,"     "+this.tempVariable.toFixed(1).toString()+"¤C")
			this.display.printText(0, 3,"  Spara   > Ångra");
			break;
		case 3:
			this.display.printText(0, 1,"  <> "+this.tempVariable.toFixed(1).toString()+"¤C")
			this.display.printText(0, 3,"  Spara     Ångra");
			break;
		}
		break;
	case 3:
		this.display.printText(0, 0,"  Settings");
		break;
	}
}

PIDMenuItem.prototype.onExit = function ()
{
	this.mode = 0;
	this.sub_mode = 0;
}


