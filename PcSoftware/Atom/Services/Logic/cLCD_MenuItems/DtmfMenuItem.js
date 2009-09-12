/* the display object who created the menu item */
DtmfMenuItem.prototype.parentDisplay = null;

DtmfMenuItem.prototype.mode = 0;
DtmfMenuItem.prototype.currentPhoneItem = 0;
const numberOfItemsInPhonebook = 15;
DtmfMenuItem.prototype.PhoneNumberList = new Array();
DtmfMenuItem.prototype.PhoneNumberListLatest = -1;

function DtmfMenuItem(parentDisplay, hd44789Object)
{
	var self = this;
	this.parentDisplay = parentDisplay;
	this.display = hd44789Object;

	if (!this.parentDisplay.myInitialArguments["SimpleDTMF"])
	{
		log(this.myName + ":" + this.myId + "> Failed to initialize, SimpleDTMF-config missing from config.\n");
		return;
	}

	this.myDTMFService = ServiceManager.getService("Can", "SimpleDTMF", this.parentDisplay.myInitialArguments["SimpleDTMF"]["Id"]);
	this.myDTMFService.registerEventCallback("newPhonenumber", function(args) { self.dtmfUpdate(args); });
	//this.myDTMFService.registerEventCallback("online", function(args) { self.dtmfOnline(); });
	this.dtmfOnline();

	this.myOnlinePhonebook = new OnlinePhonebook(function(phonenumber, persons) { self.phonebookLookupCallback(phonenumber, persons); });

	for (var i = 0; i <= numberOfItemsInPhonebook-1; i++)
	{
		this.PhoneNumberList[i] = new Array();
		this.PhoneNumberList[i]['time'] = "";
		this.PhoneNumberList[i]['number'] = "";
		this.PhoneNumberList[i]['persons'] = new Array();
	}
}

/* the display object who created the menu item */
DtmfMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
DtmfMenuItem.prototype.display = null;

/* How often the display shall update [ms]*/
DtmfMenuItem.prototype.UpdateTime = 5000;

/* what DtmfMenuItem is left of this item, if used */
DtmfMenuItem.prototype.LeftItem = null;
/* what DtmfMenuItem is right of this item, if used */
DtmfMenuItem.prototype.RightItem = null;
/* what DtmfMenuItem is after of this item, if used */
DtmfMenuItem.prototype.PressEnterItem = null;
/* what DtmfMenuItem is before of this item, if used */
DtmfMenuItem.prototype.PressBackItem = null;
/* what DtmfMenuItem is below of this item, if used */
DtmfMenuItem.prototype.DownItem = null;
/* what DtmfMenuItem is abowe of this item, if used */
DtmfMenuItem.prototype.UpItem = null;

/*
Standard events can be:
left, right, enter, back, up, down....
*/
DtmfMenuItem.prototype.processEvent = function (event)
{
	switch (event)
	{
	case "left":
		if (this.mode == 0) {
			this.parentDisplay.changeToLeft();
		} else {
			this.currentPhoneItem--;
			if (this.currentPhoneItem < 0) {
				this.currentPhoneItem = numberOfItemsInPhonebook-1;
			}
for (n=0;0<=numberOfItemsInPhonebook;n++) {
if (this.PhoneNumberList[this.currentPhoneItem]['number'] != "")
break;
				this.currentPhoneItem--;
				if (this.currentPhoneItem < 0) {
					this.currentPhoneItem = numberOfItemsInPhonebook-1;
				}				
			}
		}
		break;

	case "right":
		if (this.mode == 0) {
			this.parentDisplay.changeToRight();
		} else {
			this.currentPhoneItem++;
			if (this.currentPhoneItem >= numberOfItemsInPhonebook) {
				this.currentPhoneItem = 0;
			}
for (n=0;0<=numberOfItemsInPhonebook;n++) {
if (this.PhoneNumberList[this.currentPhoneItem]['number'] != "")
break;
			this.currentPhoneItem++;
				if (this.currentPhoneItem  >= numberOfItemsInPhonebook) {
					this.currentPhoneItem = 0;
				}
			}
		}
		break;
	
	case "up":
		//parentDisplay.changeToUp();
		break;
	
	case "down":
		//parentDisplay.changeToDown();
		break;
	
	case "enter":

		if (this.mode == 0) {
			if (this.PhoneNumberList[this.PhoneNumberListLatest] != null) {
				this.currentPhoneItem = this.PhoneNumberListLatest;
				this.mode = 1;
			}
		} else {
			this.mode = 0;
		}
		break;
	
	case "back":
		//parentDisplay.changeToBack();
		break;
	}
}

DtmfMenuItem.prototype.onEnter = function ()
{
	this.display.clearScreen();
}
DtmfMenuItem.prototype.update = function ()
{
	if (this.mode == 0) {
		if (this.PhoneNumberList[this.PhoneNumberListLatest] != null) {
this.display.clearScreen();
			this.display.printText(0, 0, this.parentDisplay.lcdCenterText("Senaste nummer:"));
			this.display.printText(0, 1, this.parentDisplay.lcdCenterText(""+this.PhoneNumberList[this.PhoneNumberListLatest]['time']));
			this.display.printText(0, 2, this.parentDisplay.lcdCenterText(""+this.PhoneNumberList[this.PhoneNumberListLatest]['number']));
if (this.PhoneNumberList[this.PhoneNumberListLatest]['persons'][0] != null) {			
this.display.printText(0, 3, this.parentDisplay.lcdCenterText(""+this.PhoneNumberList[this.PhoneNumberListLatest]['persons'][0]));
}
		} else {
this.display.clearScreen();
			this.display.printText(0, 0, this.parentDisplay.lcdCenterText("Senaste nummer:"));
			this.display.printText(0, 2, this.parentDisplay.lcdCenterText("Inga nummer"));
			this.display.printText(0, 3, this.parentDisplay.lcdCenterText("i listan!"));
		}
	} else {
this.display.clearScreen();
		this.display.printText(0, 0,this.parentDisplay.lcdCenterText(""+this.PhoneNumberList[this.currentPhoneItem]['time']));
		this.display.printText(0, 1, this.parentDisplay.lcdCenterText(""+this.PhoneNumberList[this.currentPhoneItem]['number']));
if (this.PhoneNumberList[this.PhoneNumberListLatest]['persons'][0] != null) {
		this.display.printText(0, 2, this.parentDisplay.lcdCenterText(""+this.PhoneNumberList[this.currentPhoneItem]['persons'][0]));
} 
if (this.PhoneNumberList[this.PhoneNumberListLatest]['persons'][1] != null) {
		this.display.printText(0, 3, this.parentDisplay.lcdCenterText(""+this.PhoneNumberList[this.currentPhoneItem]['persons'][1]));
}
	}
}

DtmfMenuItem.prototype.onExit = function ()
{
	this.mode = 0;
}


DtmfMenuItem.prototype.dtmfOnline = function()
{
}

DtmfMenuItem.prototype.dtmfUpdate = function(args)
{
	this.myOnlinePhonebook.lookup(this.myDTMFService.getLastPhonenumber());
}

DtmfMenuItem.prototype.phonebookLookupCallback = function(phonenumber, persons)
{
	// Update main dtmf menu
	var date = new Date();
	/* Get the current date time on the format YYYY-mm-dd HH.ii.ss */
	var dateAndTime = date.getDateTimeFormated();
this.PhoneNumberListLatest++;
if (this.PhoneNumberListLatest >= numberOfItemsInPhonebook) {
this.PhoneNumberListLatest=0;
}

this.PhoneNumberList[this.PhoneNumberListLatest]['time'] = dateAndTime;
this.PhoneNumberList[this.PhoneNumberListLatest]['number'] = phonenumber;
this.PhoneNumberList[this.PhoneNumberListLatest]['persons'] = persons;	
}
