/* the display object who created the menu item */
SensorMenuItem.prototype.parentDisplay = null;
SensorMenuItem.prototype.sensors = null;
SensorMenuItem.prototype.sensorNames = null;
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
					Display_Clear(this.display);
				}
			}
			if (this.currentSensorItem < this.window_low ) {
				this.window_low--;
				this.window_high--;
				Display_Clear(this.display);
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
				Display_Clear(this.display);
			}
			if (this.currentSensorItem > this.window_high ) {
				this.window_low++;
				this.window_high++;
				Display_Clear(this.display);
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
	var SensorMenuData = Storage_GetParameter("CLCD_config", "SensorMenuItem");
	
	//Log("\033[33mon Enter1: "+SensorMenuData+".\033[0m\n");	
	var alias_data = eval("(" + SensorMenuData + ")");
	this.sensors = alias_data["Sensors"];
	this.sensorNames = alias_data["SensorNames"];
  //Log("\033[33mon Enter: "+alias_data["Sensors"][1].toString()+". "+alias_data["Sensors"].length+"\033[0m\n");	
  //this.sensors = getSensorList();
//	var i = 0;
//	for (i=0; i< this.sensors.length;i++)
//	{
//	  Log("\033[33mdata: "+this.sensors[i]+".\033[0m\n");	
//	}
	
	this.mode = 0;
	Display_Clear(this.display);
}
SensorMenuItem.prototype.update = function ()
{
	Display_Clear(this.display);
	row = 0;
	for (var i = this.window_low; i < this.sensors.length && i <= this.window_high; i++) {
		var last_value_string = Storage_GetParameter("LastValues", this.sensors[i]);
		var last_value = eval("(" + last_value_string + ")");
		if (this.mode == 1 && this.currentSensorItem == i) {
			Display_Print(this.display, 0, row, ">"+ this.sensorNames[i]);
		} else {
			Display_Print(this.display, 0, row, " "+ this.sensorNames[i]);
		}
		if (last_value["Temperature_Celsius"]) {
			Display_Print(this.display, 14, row,parseFloat(last_value["Temperature_Celsius"]["value"]).toFixed(1).toString()+"¤C");
		} else if (last_value["Humidity_Percent"]){
			Display_Print(this.display, 14, row,parseFloat(last_value["Humidity_Percent"]["value"]).toFixed(1).toString()+"%");
		} else if (last_value["Voltage"]){
			Display_Print(this.display, 14, row,parseFloat(last_value["Voltage"]["value"]).toFixed(1).toString()+"V");
		} else if (last_value["Current"]){
			Display_Print(this.display, 14, row,parseFloat(last_value["Current"]["value"]).toFixed(1).toString()+"A");
		} else {
			Display_Print(this.display, 14, "Error");
		}
		row++;
	}
	//Display_Print(this.display,0, 0, this.parentDisplay.lcdCenterText("Tillbaka"));
}

SensorMenuItem.prototype.onExit = function ()
{
	this.mode = 0;
}


