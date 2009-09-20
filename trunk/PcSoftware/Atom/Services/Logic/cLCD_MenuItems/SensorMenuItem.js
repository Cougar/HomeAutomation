/* the display object who created the menu item */
SensorMenuItem.prototype.parentDisplay = null;
SensorMenuItem.prototype.sensors = null;
SensorMenuItem.prototype.mode = 0;
SensorMenuItem.prototype.currentSensorItem = 0;
SensorMenuItem.prototype.window_low = 0;
SensorMenuItem.prototype.window_high = 3; //set to nummber of rows you want to display minus 1


function SensorMenuItem(parentDisplay, hd44789Object)
{
	var self = this;
	this.parentDisplay = parentDisplay;
	this.display = hd44789Object;	
}

/* the display object who created the menu item */
SensorMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
SensorMenuItem.prototype.display = null;

/* How often the display shall update [ms]*/
SensorMenuItem.prototype.UpdateTime = 5000;

/* what SensorMenuItem is left of this item, if used */
SensorMenuItem.prototype.LeftItem = null;
/* what SensorMenuItem is right of this item, if used */
SensorMenuItem.prototype.RightItem = null;
/* what SensorMenuItem is after of this item, if used */
SensorMenuItem.prototype.PressEnterItem = null;

/*
Standard events can be:
left, right, enter, back, up, down....
*/
SensorMenuItem.prototype.processEvent = function (event)
{
	switch (event)
	{
	case "right":
		if (this.mode == 0) {
			this.parentDisplay.changeToRight();
		} else {
			this.currentSensorItem--;
			if (this.currentSensorItem == -1) {
				this.currentSensorItem= this.sensors.length-1;
if (this.sensors.length-1 - (this.window_high-this.window_low) >= 0) {
				this.window_low = this.sensors.length-1 - (this.window_high-this.window_low);
				this.window_high = this.sensors.length-1;
				this.display.clearScreen();
}
			}
			if (this.currentSensorItem < this.window_low ) {
				this.window_low--;
				this.window_high--;
				this.display.clearScreen();
			}
		}
		break;
	case "left":
		if (this.mode == 0) {
			this.parentDisplay.changeToLeft();
		} else {
			this.currentSensorItem++;
			if (this.currentSensorItem == this.sensors.length) {
				this.currentSensorItem=0;
				this.window_high = this.window_high-this.window_low;
				this.window_low = 0;
				this.display.clearScreen();
			}
			if (this.currentSensorItem > this.window_high ) {
				this.window_low++;
				this.window_high++;
				this.display.clearScreen();
			} 
		} 
		break;
	case "enter":
		if (this.mode == 0) {
			if (this.sensors.length > 0) {
				this.currentSensorItem = 0;
				this.mode = 1;
			}
		} else {
			this.mode = 0;
		}
		break;
	}
}

SensorMenuItem.prototype.onEnter = function ()
{
	this.sensors = getSensorList();
	this.mode = 0;
	this.display.clearScreen();
}
SensorMenuItem.prototype.update = function ()
{
	//this.display.clearScreen();
	row = 0;
	for (var i = this.window_low; i < this.sensors.length && i <= this.window_high; i++) {
		if (this.mode == 1 && this.currentSensorItem == i) {
			this.display.printText(0, row, ">"+ this.sensors[i]['shortName']);
			this.display.printText(14, row,getSensorValue(this.sensors[i]['name']).toFixed(1).toString()+"¤C");

		} else {
			this.display.printText(0, row, " "+ this.sensors[i]['shortName']);
			this.display.printText(14, row,getSensorValue(this.sensors[i]['name']).toFixed(1).toString()+"¤C");
		}
		row++;
	}
}

SensorMenuItem.prototype.onExit = function ()
{
	this.mode = 0;
}


