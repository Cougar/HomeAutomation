/* the display object who created the menu item */
DimmerMenuItem.prototype.parentDisplay = null;

DimmerMenuItem.prototype.dimmers = null;
DimmerMenuItem.prototype.mode = 0;
DimmerMenuItem.prototype.currentDimmerItem = 0;
DimmerMenuItem.prototype.window_low = 0;
DimmerMenuItem.prototype.window_high = 3;

function DimmerMenuItem(parentDisplay, hd44789Object)
{
	var self = this;
	this.parentDisplay = parentDisplay;
	this.display = hd44789Object;	
}

/* the display object who created the menu item */
DimmerMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
DimmerMenuItem.prototype.display = null;

/* How often the display shall update [ms]*/
DimmerMenuItem.prototype.UpdateTime = 5000;

/* what DimmerMenuItem is left of this item, if used */
DimmerMenuItem.prototype.LeftItem = null;
/* what DimmerMenuItem is right of this item, if used */
DimmerMenuItem.prototype.RightItem = null;
/* what DimmerMenuItem is after of this item, if used */
DimmerMenuItem.prototype.PressEnterItem = null;

/*
Standard events can be:
left, right, enter, back, up, down....
*/
DimmerMenuItem.prototype.processEvent = function (event)
{
	switch (event)
	{
	case "right":
		if (this.mode == 0) {
			this.parentDisplay.changeToRight();
		} else if (this.mode == 1){
			this.currentDimmerItem--;
			if (this.currentDimmerItem < 0) {
				this.mode = 2;
this.currentDimmerItem= this.dimmers.length-1;
			} 
		} else if (this.mode == 3){
this.dimmers[this.currentDimmerItem]['service'].relFade(this.dimmers[this.currentDimmerItem]['channel'],132, "Decrease", 25);
		} else {
this.mode = 1;
this.currentDimmerItem= this.dimmers.length-1;
if (this.dimmers.length-1 - (this.window_high-this.window_low) >= 0) {
				this.window_low = this.dimmers.length-1 - (this.window_high-this.window_low);
				this.window_high = this.dimmers.length-1;
				this.display.clearScreen();
}
}
if (this.currentDimmerItem < this.window_low ) {
				this.window_low--;
				this.window_high--;
				this.display.clearScreen();
			}
		break;

	case "left":
		if (this.mode == 0) {
			this.parentDisplay.changeToLeft();
		} else if (this.mode == 1){
			this.currentDimmerItem++;
			if (this.currentDimmerItem >= this.dimmers.length) {
				this.mode = 2;
this.currentDimmerItem=0;
			} 
		} else if (this.mode == 3){
this.dimmers[this.currentDimmerItem]['service'].relFade(this.dimmers[this.currentDimmerItem]['channel'],132, "Increase", 25);
		} else {
this.mode = 1;
this.currentDimmerItem=0;
				this.window_high = this.window_high-this.window_low;
				this.window_low = 0;
				this.display.clearScreen();

}
if (this.currentDimmerItem > this.window_high ) {
				this.window_low++;
				this.window_high++;
				this.display.clearScreen();
			} 
		break;
	case "enter":
if (this.mode == 0) {
			if (this.dimmers.length > 0) {
				//this.currentDimmerItem = 0;
				this.mode = 1;
			}
		} else if (this.mode == 1){
			this.mode = 3;
		} else if (this.mode == 3) {
			this.mode = 1;
		} else {
			this.mode = 0;
		}
		break;
	}
}

DimmerMenuItem.prototype.onEnter = function ()
{
this.dimmers = getDimmerList();
this.mode = 0;
	this.display.clearScreen();
}
DimmerMenuItem.prototype.update = function ()
{
	//this.display.clearScreen();
if (this.mode != 2) {
row = 0;

	for (var i = this.window_low; i < this.dimmers.length && i <= this.window_high; i++) {
if (this.mode == 1 && this.currentDimmerItem == i) {
this.display.printText(0, row, this.parentDisplay.lcdCenterText("-"+ this.dimmers[i]['name'] + " " +getDimmerValue(this.dimmers[i]['name'])+"   "));
} else if (this.mode == 3 && this.currentDimmerItem == i) {
this.display.printText(0, row, this.parentDisplay.lcdCenterText("<>"+ this.dimmers[i]['name'] + " " +getDimmerValue(this.dimmers[i]['name'])+"   "));
} else {
this.display.printText(0, row, this.parentDisplay.lcdCenterText(""+ this.dimmers[i]['name'] + " " +getDimmerValue(this.dimmers[i]['name'])+"   "));

}
row++;
}
} else {
this.display.clearScreen();
this.display.printText(0, 0, this.parentDisplay.lcdCenterText("Tillbaka"));
}
}

DimmerMenuItem.prototype.onExit = function ()
{
	this.mode = 0;
}


