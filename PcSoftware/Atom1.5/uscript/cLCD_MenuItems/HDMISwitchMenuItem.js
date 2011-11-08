/* the display object who created the menu item */
HDMISwitchMenuItem.prototype.parentDisplay = null;

HDMISwitchMenuItem.prototype.switch = null;
HDMISwitchMenuItem.prototype.mode = 0;

HDMISwitchMenuItem.prototype.currentPosition = 0;

function HDMISwitchMenuItem(parentDisplay, hd44789Object)
{
	var self = this;
	this.parentDisplay = parentDisplay;
	this.display = hd44789Object;	
}

/* the display object who created the menu item */
HDMISwitchMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
HDMISwitchMenuItem.prototype.display = null;

/* How often the display shall update [ms]*/
HDMISwitchMenuItem.prototype.UpdateTime = 5000;

/* what HDMISwitchMenuItem is left of this item, if used */
HDMISwitchMenuItem.prototype.LeftItem = null;
/* what HDMISwitchMenuItem is right of this item, if used */
HDMISwitchMenuItem.prototype.RightItem = null;
/* what HDMISwitchMenuItem is after of this item, if used */
HDMISwitchMenuItem.prototype.PressEnterItem = null;

/*
Standard events can be:
left, right, enter, back, up, down....
*/
HDMISwitchMenuItem.prototype.processEvent = function (event)
{
	switch (event)
	{
	case "right":
		if (this.mode == 0) {
			this.parentDisplay.changeToRight();
		} else if (this.mode == 1){
			this.currentPosition--;
			if (this.currentPosition < 0) {
				this.currentPosition= 2;
			}
			Output_SetSwitch("HDMI_switch", this.currentPosition+1);
		} 
		break;

	case "left":
		if (this.mode == 0) {
			this.parentDisplay.changeToLeft();
		} else if (this.mode == 1){
			this.currentPosition++;
			if (this.currentPosition >= 3) {
				this.currentPosition=0;
			} 
			Output_SetSwitch("HDMI_switch", this.currentPosition+1);
		} 
		break;
	case "enter":
		if (this.mode == 0) {
			this.mode = 1;
		} else {
			this.mode = 0;
		}
		break;
	}
}

HDMISwitchMenuItem.prototype.onEnter = function ()
{
	this.mode = 0;
	Display_Clear(this.display);
}
HDMISwitchMenuItem.prototype.update = function ()
{
	Display_Clear(this.display);
	var last_value_string = Storage_GetParameter("LastValues", this.switch);
	var last_value = eval("(" + last_value_string + ")");
	if (this.mode == 0) {
	Display_Print(this.display, 0, 1, " HDMI mode:          ");
	} else {
	Display_Print(this.display, 0, 1, ">HDMI mode:          ");
	  
	}
	switch (this.currentPosition) {
	  case 0:
	      Display_Print(this.display, 11, 1, "Dator    ");
	      break;
	  case 1:
	      Display_Print(this.display, 11, 1, "TV       ");
	      break;
	  case 2:
	      Display_Print(this.display, 11, 1, "Annat    ");
	      break;
	  
	}
}

HDMISwitchMenuItem.prototype.onExit = function ()
{
	this.mode = 0;
}


