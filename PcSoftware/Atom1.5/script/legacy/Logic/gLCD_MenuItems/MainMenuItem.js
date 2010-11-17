
function MainMenuItem(parentDisplay, ks0108Object)
{
	this.parentDisplay = parentDisplay;
	this.display = ks0108Object;
}

/* the display object who created the menu item */
MainMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
MainMenuItem.prototype.display = null;

/* what MainMenuItem is left of this item, if used */
MainMenuItem.prototype.LeftItem = null;
/* what MainMenuItem is right of this item, if used */
MainMenuItem.prototype.RightItem = null;
/* what MainMenuItem is after of this item, if used */
MainMenuItem.prototype.PressEnterItem = null;
/* what MainMenuItem is before of this item, if used */
MainMenuItem.prototype.PressBackItem = null;
/* what MainMenuItem is below of this item, if used */
MainMenuItem.prototype.DownItem = null;
/* what MainMenuItem is abowe of this item, if used */
MainMenuItem.prototype.UpItem = null;

/*
Standard events can be:
left, right, enter, back, up, down....
*/
MainMenuItem.prototype.processEvent = function (event)
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

MainMenuItem.prototype.onEnter = function ()
{
	this.display.clearScreen("Standard");
}
MainMenuItem.prototype.update = function ()
{
	this.display.printText(2,2,"mainMenu","Standard","Standard");
}
MainMenuItem.prototype.onExit = function ()
{
	
}

