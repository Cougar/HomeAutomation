

/* the display object who created the menu item */
SensorGlcdMenuItem.prototype.parentDisplay = null;

SensorGlcdMenuItem.prototype.sensors = null;
SensorGlcdMenuItem.prototype.mode = 0;
SensorGlcdMenuItem.prototype.currentSensorItem = 0;
SensorGlcdMenuItem.prototype.window_low = 0;
SensorGlcdMenuItem.prototype.window_high = 3;

function SensorGlcdMenuItem(parentDisplay, ks0108Object)
{
	var self = this;
	this.parentDisplay = parentDisplay;
	this.display = ks0108Object;	
}

/* the display object who created the menu item */
SensorGlcdMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
SensorGlcdMenuItem.prototype.display = null;

/* How often the display shall update [ms]*/
SensorGlcdMenuItem.prototype.UpdateTime = 5000;

/* what SensorGlcdMenuItem is left of this item, if used */
SensorGlcdMenuItem.prototype.LeftItem = null;
/* what SensorGlcdMenuItem is right of this item, if used */
SensorGlcdMenuItem.prototype.RightItem = null;
/* what SensorGlcdMenuItem is after of this item, if used */
SensorGlcdMenuItem.prototype.PressEnterItem = null;
/* what SensorGlcdMenuItem is left of this item, if used */
SensorGlcdMenuItem.prototype.UpItem = null;
/* what SensorGlcdMenuItem is right of this item, if used */
SensorGlcdMenuItem.prototype.DownItem = null;
/* what SensorGlcdMenuItem is after of this item, if used */
SensorGlcdMenuItem.prototype.PressBackItem = null;
/*
Standard events can be:
left, right, enter, back, up, down....
*/
SensorGlcdMenuItem.prototype.processEvent = function (event)
{
	switch (event)
	{
	case "right":
			this.parentDisplay.changeToRight();
		break;

	case "left":
			this.parentDisplay.changeToLeft();
		break;
	case "enter":
		break;
	case "up":
			this.currentSensorItem++;
			if (this.currentSensorItem >= this.sensors.length) {
this.currentSensorItem=0;
				this.window_high = this.window_high-this.window_low;
				this.window_low = 0;
				//this.display.clearScreen();
this.display.DrawRect(4,17,150,8*(this.window_high-this.window_low+1),"Inverted","Fill",1);
			} 
		
if (this.currentSensorItem > this.window_high ) {
				this.window_low++;
				this.window_high++;
				//this.display.clearScreen();
this.display.DrawRect(4,17,150,8*(this.window_high-this.window_low+1),"Inverted","Fill",1);
}
		//parentDisplay.changeToUp();
		break;
	
	case "down":
			this.currentSensorItem--;
			if (this.currentSensorItem < 0) {
				this.currentSensorItem= this.sensors.length-1;
if (this.sensors.length-1 - (this.window_high-this.window_low) >= 0) {
				this.window_low = this.sensors.length-1 - (this.window_high-this.window_low);
				this.window_high = this.sensors.length-1;
				//this.display.clearScreen();
this.display.DrawRect(4,17,150,8*(this.window_high-this.window_low+1),"Inverted","Fill",1);
}
			} 
		
if (this.currentSensorItem < this.window_low ) {
				this.window_low--;
				this.window_high--;
				//this.display.clearScreen();
this.display.DrawRect(4,17,150,8*(this.window_high-this.window_low+1),"Inverted","Fill",1);
}
		//parentDisplay.changeToDown();
		break;
	
	case "back":
//if (this.sensors[this.currentSensorItem]['service'].currentValue != 0)
//this.sensors[this.currentSensorItem]['service'].absFade(1, 129, 0);
//else
//this.sensors[this.currentSensorItem]['service'].absFade(1, 129, 255);
		//parentDisplay.changeToBack();
		break;
	}
}

SensorGlcdMenuItem.prototype.onEnter = function ()
{
	this.display.clearScreen("Standard");
	this.display.printText(0,0,"Sensor menu:","Standard","Standard");
this.sensors = getSensorList();
this.mode = 0;
}
SensorGlcdMenuItem.prototype.update = function ()
{
	//this.display.clearScreen();
row = 0;

	for (var i = this.window_low; i < this.sensors.length && i <= this.window_high; i++) {
if (this.currentSensorItem == i) {
this.display.printText(1, row+2, " >"+ this.sensors[i]['shortName'],"Standard","Standard");


} else {
this.display.printText(1, row+2, "  "+ this.sensors[i]['shortName'],"Standard","Standard");

}
this.display.printText(16,row+2,""+(getSensorValue(this.sensors[i]['name'])).toFixed(1).toString() + "¤C" ,"Standard","Standard");


row++;
}
}

SensorGlcdMenuItem.prototype.onExit = function ()
{
	this.mode = 0;
}





































/*-----------------------------------------------------------------*/
/*
SensorGlcdMenuItem.prototype.myDimmer230Service = null;
SensorGlcdMenuItem.prototype.myself;
SensorGlcdMenuItem.prototype.dimmerServices = new Array();

function SensorGlcdMenuItem(parentDisplay, ks0108Object)
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
SensorGlcdMenuItem.prototype.DimmerValue = 0;


SensorGlcdMenuItem.prototype.SeviceOnline = function(service) {
log("Dimmer...0!\n");
if (service.getName() == "Dimmer230") {
log("Dimmer...1!\n");
this.updateDimmerList();
}
}

SensorGlcdMenuItem.prototype.updateDimmerList = function() {
 
for (var id in ServiceManager.Services)
{if (ServiceManager.Services[id].getName() == "Dimmer230")
{
log("Dimmer...!\n");
this.dimmerServices[dimmerServices.length] = ServiceManager.Services[id];
}}}

SensorGlcdMenuItem.prototype.Dimmer230Online = function()
{
	// If service is not online do nothing
	if (this.myDimmer230Service.isOnline())
	{
		log("Dimmer is online!\n");
	}
}

SensorGlcdMenuItem.prototype.NewDimmerValue = function()
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
//SensorGlcdMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
//SensorGlcdMenuItem.prototype.display = null;

/* what SensorGlcdMenuItem is left of this item, if used */
//SensorGlcdMenuItem.prototype.LeftItem = null;
/* what SensorGlcdMenuItem is right of this item, if used */
//SensorGlcdMenuItem.prototype.RightItem = null;
/* what SensorGlcdMenuItem is after of this item, if used */
//SensorGlcdMenuItem.prototype.PressEnterItem = null;
/* what SensorGlcdMenuItem is before of this item, if used */
//SensorGlcdMenuItem.prototype.PressBackItem = null;
/* what SensorGlcdMenuItem is below of this item, if used */
//SensorGlcdMenuItem.prototype.DownItem = null;
/* what SensorGlcdMenuItem is abowe of this item, if used */
//SensorGlcdMenuItem.prototype.UpItem = null;

/*
Standard events can be:
left, right, enter, back, up, down....
*/
/*
SensorGlcdMenuItem.prototype.processEvent = function (event)
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

SensorGlcdMenuItem.prototype.onEnter = function ()
{
	this.display.clearScreen("Standard");
	this.display.printText(0,0,"Dimmer menu:","Standard","Standard");
	getDimmerService('TakSovrum').absFade(1, 132, this.DimmerValue);
for (var n in this.dimmerServices)
{
log("Dimmer: " + this.dimmerServices[n].getName() + " id: " + this.dimmerServices[n].getId() + "\n");
}


}
SensorGlcdMenuItem.prototype.update = function ()
{

if (getDimmerService('TakSovrum').isOnline()) {
this.display.printText(2,2,"Dimmer value: "+this.DimmerValue + "  " ,"Standard","Standard");
this.display.DrawRect(120,17,25,5,"Inverted","Fill",0);
this.display.DrawRect(120,17,this.DimmerValue/10,5,"Standard","Fill",0);
this.display.DrawRect(120,17,25,5,"Standard","NoFill",0);

}
}
SensorGlcdMenuItem.prototype.onExit = function ()
{
	
}
*/
