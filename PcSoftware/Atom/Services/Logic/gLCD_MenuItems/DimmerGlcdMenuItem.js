

/* the display object who created the menu item */
DimmerGlcdMenuItem.prototype.parentDisplay = null;

DimmerGlcdMenuItem.prototype.dimmers = null;
DimmerGlcdMenuItem.prototype.mode = 0;
DimmerGlcdMenuItem.prototype.currentDimmerItem = 0;
DimmerGlcdMenuItem.prototype.window_low = 0;
DimmerGlcdMenuItem.prototype.window_high = 3;

function DimmerGlcdMenuItem(parentDisplay, ks0108Object)
{
	var self = this;
	this.parentDisplay = parentDisplay;
	this.display = ks0108Object;	
}

/* the display object who created the menu item */
DimmerGlcdMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
DimmerGlcdMenuItem.prototype.display = null;

/* How often the display shall update [ms]*/
DimmerGlcdMenuItem.prototype.UpdateTime = 5000;

/* what DimmerGlcdMenuItem is left of this item, if used */
DimmerGlcdMenuItem.prototype.LeftItem = null;
/* what DimmerGlcdMenuItem is right of this item, if used */
DimmerGlcdMenuItem.prototype.RightItem = null;
/* what DimmerGlcdMenuItem is after of this item, if used */
DimmerGlcdMenuItem.prototype.PressEnterItem = null;
/* what DimmerGlcdMenuItem is left of this item, if used */
DimmerGlcdMenuItem.prototype.UpItem = null;
/* what DimmerGlcdMenuItem is right of this item, if used */
DimmerGlcdMenuItem.prototype.DownItem = null;
/* what DimmerGlcdMenuItem is after of this item, if used */
DimmerGlcdMenuItem.prototype.PressBackItem = null;
/*
Standard events can be:
left, right, enter, back, up, down....
*/
DimmerGlcdMenuItem.prototype.processEvent = function (event)
{
	switch (event)
	{
	case "right":
		if (this.mode == 0) {
			this.parentDisplay.changeToRight();
		} else if (this.mode == 1){
			this.currentDimmerItem--;
			if (this.currentDimmerItem < 0) {
				this.currentDimmerItem= this.dimmers.length-1;
if (this.dimmers.length-1 - (this.window_high-this.window_low) >= 0) {
				this.window_low = this.dimmers.length-1 - (this.window_high-this.window_low);
				this.window_high = this.dimmers.length-1;
				//this.display.clearScreen();
this.display.DrawRect(4,17,150,8*(this.window_high-this.window_low+1),"Inverted","Fill",1);
}
			} 
		} 
if (this.currentDimmerItem < this.window_low ) {
				this.window_low--;
				this.window_high--;
				//this.display.clearScreen();
this.display.DrawRect(4,17,150,8*(this.window_high-this.window_low+1),"Inverted","Fill",1);
			}
		break;

	case "left":
		if (this.mode == 0) {
			this.parentDisplay.changeToLeft();
		} else if (this.mode == 1){
			this.currentDimmerItem++;
			if (this.currentDimmerItem >= this.dimmers.length) {
this.currentDimmerItem=0;
				this.window_high = this.window_high-this.window_low;
				this.window_low = 0;
				//this.display.clearScreen();
this.display.DrawRect(4,17,150,8*(this.window_high-this.window_low+1),"Inverted","Fill",1);
			} 
		} 
if (this.currentDimmerItem > this.window_high ) {
				this.window_low++;
				this.window_high++;
				//this.display.clearScreen();
this.display.DrawRect(4,17,150,8*(this.window_high-this.window_low+1),"Inverted","Fill",1);
			} 
		break;
	case "enter":
if (this.mode == 0) {
			if (this.dimmers.length > 0) {
				//this.currentDimmerItem = 0;
				this.mode = 1;
this.display.DrawRect(1,17,3,8*(this.window_high-this.window_low+1),"Standard","Fill",0);
//.DrawRect = function(x, y , width, height, inverted, fill, radius)

			}
		} else {
			this.mode = 0;
this.display.DrawRect(1,17,3,8*(this.window_high-this.window_low+1),"Inverted","Fill",0);
		}
		break;
	case "up":
this.dimmers[this.currentDimmerItem]['service'].relFade(this.dimmers[this.currentDimmerItem]['channel'],132, "Increase", 25);
		//parentDisplay.changeToUp();
		break;
	
	case "down":
this.dimmers[this.currentDimmerItem]['service'].relFade(this.dimmers[this.currentDimmerItem]['channel'],132, "Decrease", 25);
		//parentDisplay.changeToDown();
		break;
	
	case "back":
if (this.dimmers[this.currentDimmerItem]['service'].currentValue != 0)
this.dimmers[this.currentDimmerItem]['service'].absFade(1, 129, 0);
else
this.dimmers[this.currentDimmerItem]['service'].absFade(1, 129, 255);
		//parentDisplay.changeToBack();
		break;
	}
}

DimmerGlcdMenuItem.prototype.onEnter = function ()
{
	this.display.clearScreen("Standard");
	this.display.printText(0,0,"Dimmer menu:","Standard","Standard");
this.dimmers = getDimmerList();
this.mode = 0;
}
DimmerGlcdMenuItem.prototype.update = function ()
{
	//this.display.clearScreen();
row = 0;

	for (var i = this.window_low; i < this.dimmers.length && i <= this.window_high; i++) {
if (this.currentDimmerItem == i) {
this.display.printText(1, row+2, " >"+ this.dimmers[i]['shortName'],"Standard","Standard");


} else {
this.display.printText(1, row+2, "  "+ this.dimmers[i]['shortName'],"Standard","Standard");

}
this.display.printText(16,row+2,""+(getDimmerValue(this.dimmers[i]['name'])/2.55).toFixed(0).toString() + "  " ,"Standard","Standard");
this.display.DrawRect(120,17+row*8,25,5,"Inverted","Fill",0);
this.display.DrawRect(120,17+row*8,getDimmerValue(this.dimmers[i]['name'])/10,5,"Standard","Fill",0);
this.display.DrawRect(120,17+row*8,25,5,"Standard","NoFill",0);


row++;
}
}

DimmerGlcdMenuItem.prototype.onExit = function ()
{
	this.mode = 0;
}





































/*-----------------------------------------------------------------*/
/*
DimmerGlcdMenuItem.prototype.myDimmer230Service = null;
DimmerGlcdMenuItem.prototype.myself;
DimmerGlcdMenuItem.prototype.dimmerServices = new Array();

function DimmerGlcdMenuItem(parentDisplay, ks0108Object)
{
	this.parentDisplay = parentDisplay;
	this.display = ks0108Object;
	// This is used for function declarations like the callbacks below 
	myself = this;
var self = this;
	if (!this.parentDisplay.myInitialArguments["Dimmer230"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, Dimmer230-config missing from config.\n");
		return;
	}
// Get the PWM service that we want from the ServiceManager, it takes type, service name, service id 
	this.myDimmer230Service = ServiceManager.getService("Can", "Dimmer230", parentDisplay.myInitialArguments["Dimmer230"]["Id"]);
	// Add a callback for when the service goes online 
	this.myDimmer230Service.registerEventCallback("online", function(args) { self.Dimmer230Online(); });
this.myDimmer230Service.registerEventCallback("newValue", function(args) { self.NewDimmerValue(); });
	// If the service is already online we should call the handler here
	this.Dimmer230Online();
ServiceManager.addCallback(self.SeviceOnline);

}
DimmerGlcdMenuItem.prototype.DimmerValue = 0;


DimmerGlcdMenuItem.prototype.SeviceOnline = function(service) {
log("Dimmer...0!\n");
if (service.getName() == "Dimmer230") {
log("Dimmer...1!\n");
this.updateDimmerList();
}
}

DimmerGlcdMenuItem.prototype.updateDimmerList = function() {
 
for (var id in ServiceManager.Services)
{if (ServiceManager.Services[id].getName() == "Dimmer230")
{
log("Dimmer...!\n");
this.dimmerServices[dimmerServices.length] = ServiceManager.Services[id];
}}}

DimmerGlcdMenuItem.prototype.Dimmer230Online = function()
{
	// If service is not online do nothing
	if (this.myDimmer230Service.isOnline())
	{
		log("Dimmer is online!\n");
	}
}

DimmerGlcdMenuItem.prototype.NewDimmerValue = function()
{
	if (this.parentDisplay.currentMenuItem == this.myself)
	{// If service is not online do nothing
	if (this.display.isOnline()) {
	this.display.printText(2,2,"Dimmer value: "+getDimmerService('TakSovrum').currentValue + "  " ,"Standard","Standard");
this.display.DrawRect(120,17,25,5,"Inverted","Fill",0);
this.display.DrawRect(120,17,getDimmerService('TakSovrum').currentValue/10,5,"Standard","Fill",0);
this.display.DrawRect(120,17,25,5,"Standard","NoFill",0);


}} else {
this.DimmerValue = getDimmerService('TakSovrum').currentValue
}
}
*/
/* the display object who created the menu item */
//DimmerGlcdMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
//DimmerGlcdMenuItem.prototype.display = null;

/* what DimmerGlcdMenuItem is left of this item, if used */
//DimmerGlcdMenuItem.prototype.LeftItem = null;
/* what DimmerGlcdMenuItem is right of this item, if used */
//DimmerGlcdMenuItem.prototype.RightItem = null;
/* what DimmerGlcdMenuItem is after of this item, if used */
//DimmerGlcdMenuItem.prototype.PressEnterItem = null;
/* what DimmerGlcdMenuItem is before of this item, if used */
//DimmerGlcdMenuItem.prototype.PressBackItem = null;
/* what DimmerGlcdMenuItem is below of this item, if used */
//DimmerGlcdMenuItem.prototype.DownItem = null;
/* what DimmerGlcdMenuItem is abowe of this item, if used */
//DimmerGlcdMenuItem.prototype.UpItem = null;

/*
Standard events can be:
left, right, enter, back, up, down....
*/
/*
DimmerGlcdMenuItem.prototype.processEvent = function (event)
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
this.DimmerValue += 25;
if (this.DimmerValue > 255)
this.DimmerValue = 255;
getDimmerService('TakSovrum').absFade(1, 132, this.DimmerValue);
		//parentDisplay.changeToUp();
		break;
	
	case "down":
this.DimmerValue -= 25;
if (this.DimmerValue < 0)
this.DimmerValue = 0;
getDimmerService('TakSovrum').absFade(1, 132, this.DimmerValue);
		//parentDisplay.changeToDown();
		break;
	
	case "enter":
this.display.printText(3,3,"enter ","Standard","Standard");
		//parentDisplay.changeToEnter();
		break;
	
	case "back":
if (this.DimmerValue != 0)
this.DimmerValue = 0;
else
this.DimmerValue = 255;
getDimmerService('TakSovrum').absFade(1, 129, this.DimmerValue);

		//parentDisplay.changeToBack();
		break;
	}
}

DimmerGlcdMenuItem.prototype.onEnter = function ()
{
	this.display.clearScreen("Standard");
	this.display.printText(0,0,"Dimmer menu:","Standard","Standard");
	getDimmerService('TakSovrum').absFade(1, 132, this.DimmerValue);
for (var n in this.dimmerServices)
{
log("Dimmer: " + this.dimmerServices[n].getName() + " id: " + this.dimmerServices[n].getId() + "\n");
}


}
DimmerGlcdMenuItem.prototype.update = function ()
{

if (getDimmerService('TakSovrum').isOnline()) {
this.display.printText(2,2,"Dimmer value: "+this.DimmerValue + "  " ,"Standard","Standard");
this.display.DrawRect(120,17,25,5,"Inverted","Fill",0);
this.display.DrawRect(120,17,this.DimmerValue/10,5,"Standard","Fill",0);
this.display.DrawRect(120,17,25,5,"Standard","NoFill",0);

}
}
DimmerGlcdMenuItem.prototype.onExit = function ()
{
	
}
*/
