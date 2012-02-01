/* the display object who created the menu item */
DtmfMenuItem.prototype.parentDisplay = null;

DtmfMenuItem.prototype.mode = 0;
DtmfMenuItem.prototype.currentPhoneItem = 0;
DtmfMenuItem.prototype.numberOfItemsInPhonebook = 15;
DtmfMenuItem.prototype.PhoneNumberList = new Array();
DtmfMenuItem.prototype.PhoneNumberListKeys = new Array();
DtmfMenuItem.prototype.PhoneNumberListLatest = -1;


function DtmfMenuItem(parentDisplay, hd44789Object)
{
	var self = this;
	this.parentDisplay = parentDisplay;
	this.display = hd44789Object;
}

/* the display object who created the menu item */
DtmfMenuItem.prototype.parentDisplay = null;
/* the display that we are writing to */
DtmfMenuItem.prototype.display = null;

/* How often the display shall update [ms]*/
DtmfMenuItem.prototype.UpdateTime = 5000;

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
			//Log("currentPhoneItem is now-: "+ this.currentPhoneItem + "\n");
			if (this.currentPhoneItem < 0) {
				this.currentPhoneItem = this.numberOfItemsInPhonebook-1;
			}
		}
		break;

	case "right":
		if (this.mode == 0) {
			this.parentDisplay.changeToRight();
		} else {
			this.currentPhoneItem++;
			//Log("currentPhoneItem is now+: "+ this.currentPhoneItem + "\n");
			if (this.currentPhoneItem >= this.numberOfItemsInPhonebook) {
				this.currentPhoneItem = 0;
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
		//Log("DTMF onEnter\n");
		if (this.mode == 0) {
			this.currentPhoneItem = 0;
			this.mode = 1;
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
	this.PhoneNumberList = Storage_GetParameters("PhoneCalls");
	if (this.PhoneNumberList) 
	{
		for(k in this.PhoneNumberList)
		{
			var tmp  = eval("(" + this.PhoneNumberList[k] + ")");
                        if (tmp["direction"]=="in")
	     			this.PhoneNumberListKeys.push(k);
			//Log("hittade: "+k+"\n");
		}
		this.PhoneNumberListKeys.sort(function(a, b) {return b - a;});
		if (this.PhoneNumberListKeys.length > this.numberOfItemsInPhonebook) {
			this.PhoneNumberListKeys.slice(0, this.numberOfItemsInPhonebook-1);
		}				
	} else {
		//No calls found, clear list?
		Log("hittade inga nummer!\n");
	}
	this.currentPhoneItem = 0;
	this.numberOfItemsInPhonebook = this.PhoneNumberListKeys.length;
	Display_Clear(this.display);
	this.mode = 0;
	//for(k in this.PhoneNumberList)
	//{
	//	Log("hittade: "+k+"\n");
	//}
}
DtmfMenuItem.prototype.update = function ()
{
	if (this.mode == 0) {
		if (this.PhoneNumberListKeys[0] != null) {
			Display_Clear(this.display);
			Display_Print(this.display, 0, 0,"Senaste nummer:");
			var numbers = {};
			numbers  = eval("(" + this.PhoneNumberList[this.PhoneNumberListKeys[0]] + ")");
			if (numbers["direction"]==null)
			  Display_Print(this.display, 0, 0,"Senaste nummer:   - ");
			else if (numbers["direction"]=="in")
			  Display_Print(this.display, 0, 0,"Senaste nummer:   In");
			else 
			  Display_Print(this.display, 0, 0,"Senaste nummer:   Ut");
			var date = new Date(parseInt(this.PhoneNumberListKeys[0])*1000);
			Display_Print(this.display, 0, 1,""+date.getDateFormated()+" "+date.getTimeShortFormated());
			Display_Print(this.display, 0, 2,""+numbers["number"]);
			var phonebook = Storage_GetJsonParamter("PhoneBook",numbers["number"])
			if (phonebook) {
				Display_Print(this.display, 0, 3, phonebook[0]);
			}
		} else {
			Display_Clear(this.display);
			Display_Print(this.display, 0, 0,"SenAste nummer:");
			Display_Print(this.display, 0, 2,"IngA nummer");
			Display_Print(this.display, 0, 3,"i liStan!");
		}
	} else {
		Display_Clear(this.display);
		var numbers = {};
		numbers  = eval("(" + this.PhoneNumberList[this.PhoneNumberListKeys[this.currentPhoneItem]] + ")");
		var date = new Date(parseInt(this.PhoneNumberListKeys[this.currentPhoneItem])*1000);
		if (numbers["direction"]==null)
		  Display_Print(this.display, 0, 0,""+date.getDateFormated()+" "+date.getTimeShortFormated());
		else if (numbers["direction"]=="in")
		  Display_Print(this.display, 0, 0,""+date.getDateFormated()+" "+date.getTimeShortFormated()+"  In");
		else 
		  Display_Print(this.display, 0, 0,""+date.getDateFormated()+" "+date.getTimeShortFormated()+"  Ut");
		Display_Print(this.display, 0, 1,""+numbers["number"]);
		var phonebook = Storage_GetJsonParamter("PhoneBook",numbers["number"])
		if (phonebook) {
			Display_Print(this.display, 0, 2, phonebook[0]);
			if (phonebook.length > 1) {
				Display_Print(this.display, 0, 3, phonebook[1]);
			}
		}
	}
}

DtmfMenuItem.prototype.onExit = function ()
{
	this.mode = 0;
}


