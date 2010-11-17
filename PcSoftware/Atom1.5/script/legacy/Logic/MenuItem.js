
function MenuItem(parentDisplay)
{
	this.parentDisplay = parentDisplay;
	this.displayData = new Array();
}

/* the display object who created the menu item */
MenuItem.prototype.parentDisplay = null;

/* the data to display for this item, may be updated before sent to display via the doUpdate function */
MenuItem.prototype.displayData = null;
/* function to call when this menu item is active and left is choosen */
MenuItem.prototype.doLeft = null;
/* function to call when this menu item is active and right is choosen */
MenuItem.prototype.doRight = null;
/* function to call when this menu item is active and button is pressed */
MenuItem.prototype.doPress = null;
/* function to call before this menu item is displayed */
MenuItem.prototype.doUpdate = null;
/* what MenuItem is previous item, if used */
MenuItem.prototype.nextItem = null;
/* what MenuItem is next item, if used */
MenuItem.prototype.prevItem = null;
/* what MenuItem to descend to, if used */
MenuItem.prototype.descItem = null;

MenuItem.prototype.setNextItem = function (nextItem)
{
	this.nextItem = nextItem;
}
MenuItem.prototype.setPrevItem = function (prevItem)
{
	this.prevItem = prevItem;
}
MenuItem.prototype.setDescItem = function (descItem)
{
	this.descItem = descItem;
}

