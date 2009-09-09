
function SettingsMenuItem(parentDisplay, ks0108Object)
{
	this.parentDisplay = parentDisplay;
	this.display = ks0108Object;
}

/* the display object who created the menu item */
SettingsMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
SettingsMenuItem.prototype.display = null;

/* what SettingsMenuItem is left of this item, if used */
SettingsMenuItem.prototype.LeftItem = null;
/* what SettingsMenuItem is right of this item, if used */
SettingsMenuItem.prototype.RightItem = null;
/* what SettingsMenuItem is after of this item, if used */
SettingsMenuItem.prototype.PressEnterItem = null;
/* what SettingsMenuItem is before of this item, if used */
SettingsMenuItem.prototype.PressBackItem = null;
/* what SettingsMenuItem is below of this item, if used */
SettingsMenuItem.prototype.DownItem = null;
/* what SettingsMenuItem is abowe of this item, if used */
SettingsMenuItem.prototype.UpItem = null;

/*
Standard events can be:
left, right, enter, back, up, down....
*/
SettingsMenuItem.prototype.processEvent = function (event)
{
	switch (event)
	{
	case "left":
		this.parentDisplay.changeToLeft();
		break;

	case "right":
		this.parentDisplay.changeToRight();
		break;
	
	case "up":
this.parentDisplay.defaultBacklight += 10;
if (this.parentDisplay.defaultBacklight > 255)
this.parentDisplay.defaultBacklight = 255;
this.display.setBacklight(this.parentDisplay.defaultBacklight);
		//parentDisplay.changeToUp();
		break;
	
	case "down":
this.parentDisplay.defaultBacklight -= 10;
if (this.parentDisplay.defaultBacklight < 0)
this.parentDisplay.defaultBacklight = 0;
this.display.setBacklight(this.parentDisplay.defaultBacklight);
		//parentDisplay.changeToDown();
		break;
	
	case "enter":
this.display.printText(3,3,"enter ","Standard","Standard");
		//parentDisplay.changeToEnter();
		break;
	
	case "back":
this.display.printText(3,3,"back ","Standard","Standard");
		//parentDisplay.changeToBack();
		break;
	}
}

SettingsMenuItem.prototype.onEnter = function ()
{
	this.display.clearScreen("Standard");
this.display.printText(0,0,"settingsMenu","Standard","Standard");
}
SettingsMenuItem.prototype.update = function ()
{
	

this.display.printText(2,2,"Backlight: "+this.parentDisplay.defaultBacklight + "  " ,"Standard","Standard");
this.display.DrawRect(120,17,25,5,"Inverted","Fill",0);
this.display.DrawRect(120,17,this.parentDisplay.defaultBacklight/10,5,"Standard","Fill",0);
this.display.DrawRect(120,17,25,5,"Standard","NoFill",0);

}
SettingsMenuItem.prototype.onExit = function ()
{
	
}

