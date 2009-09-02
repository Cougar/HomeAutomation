
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
this.display.printText(3,3,"up  ","Standard","Standard");
		//parentDisplay.changeToUp();
		break;
	
	case "down":
this.display.printText(3,3,"down ","Standard","Standard");
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
}
SettingsMenuItem.prototype.update = function ()
{
	this.display.printText(2,2,"settingsMenu","Standard","Standard");
}
SettingsMenuItem.prototype.onExit = function ()
{
	
}

