/*
BUGGAR
stor description: hackar när den beräknas?
width på timedateobjekt används nog inte nu

MISSTÄNKTA BUGGAR

ATT GÖRA
konfig för flinsh
	vilken server:port skall anropas
	färg/form/ram på markerat objekt
kommentera/dokumentera
cacheasbitmap för bakgrundsbild? bakgrundsbilden ska fixas om helt
menyobjektet skall stödja fade (sätt _alpha för alles till ett parametervärde), ska kunna användas när en meny skall hamna över en annan, den undre skall fadea
menyobjektet skall stödja inga animationer (specas i menuconfig) (jimmys önskan)
menykonfigparser skall göras om, parsa in allt, sen testa

gör om alla felmeddelanden i config, så den lagrar ett meddelande istället för en siffra


ett objekt som scrollats ut har properties:
height=0
alpha=0
inuse=false (=>visible=false)

*/


import mx.transitions.Tween;
import mx.utils.Delegate;

class MenuObj implements CommunicatorIF {

	//screen position and size
	private var menuTop:Number;
	private var menuLeft:Number;
	private var menuWidth:Number;
	private var menuHeight:Number;
	
	private var menuName:String;
	
	//to each rowitem belongs a menuTexts, this is the one that animates up/down and resizes
	private var menuTexts:Array = new Array();
	
	//the maximum number of items or rows that is visible, one extra row/item exist in menuText for the row that should scroll in
	private var	nrOfMenuItem:Number;
	//the position of the item that is marked and show description
	private var middleItemPosition:Number;
	//dataList-index of the object that is placed in middleItemPosition
	private var menuMiddleItem:Number=0;
	//data to list on the menu, title and description
	//private var dataList:XMLNode;
	private var menuData:Array = new Array();
	
	//object to handle the description of the menuMiddleItem
	private var menuItemDesc:MenuDescObj;
	
	//delaytime is time before speeding up to repeattime (first scroll last delaytime and following scroll is repeattime)
	private var scrollDelayTime:Number;
	private var scrollRepeatTime:Number;
	//time for description to fade in
	private var descScrollTime:Number;
	
	//placeholders, makes it easy to place and move menuTexts
	private var placeHolder:Array = new Array();
	
	private var moveTween:Tween;
	
	private var tweenCommand:Number = 0;
		//0 = stop
		//1 = scrollup
		//2 = scrolldown
	
	private var commObj:CommunicationObj;	
	
	private var newDataExists:Boolean;
	private var newData:XMLNode;
	
	private var isScrolling = false;
	
	private var incomDataArr:Array = new Array();
	/*
	lista här alla delar av configen
	index	name							description
	1			menuname					
	2			menudescription		
	3			rowitemfont				
	4			descriptionfont		
	5			left							horizontal placing of this menu
	6			top								vertical placing of this menu
	7			width							horizontal size of this menu, text will be cut if longer than this
	8			height						vertical size of this menu
	9			menurowitems			
	10		sizedrowitems			
	11		scrolldelaytime		
	12		scrollrepeattime	
	13		menucolor
	14		desccolor
	15		descscrolltime		the description fade-in-time
	16		
	17		
	*/

	/**
	* Creates a new instance of MenuObj.
	* @param mainScene Reference to the main MovieClip.
	* @param configArr This is configuration data, like looks and timeing, for this menu.
	*/
	function MenuObj (mainScene:MovieClip, configXML:XMLNode) {
		//anropa parseConfig(configXML), den returnerar en array där index 0 är ett nummer, -1 om datan är ok, annars ett unikt nummer för varje fel
		// om datan inte parsades okej skall inget av nedanstående köras
		//en global variabel skall inte sättas och anropande rutin skall kolla denna flagga, om den inte är satt så bör den signalera fel

		var configArr:Array = parseConfig(configXML);
		if (configArr[0] > -1) {
			trace("Config error: " + configArr[0]);
		} else {
			var menuFont:TextFormat = new TextFormat();
			var descFont:TextFormat = new TextFormat();
			
			menuName = configArr[1];
			
			menuFont.font = configArr[3];
			menuFont.color = configArr[13];
			descFont.font = configArr[4];
			descFont.color = configArr[14];
			descScrollTime = configArr[15];
			
			menuLeft = configArr[5];
			menuTop = configArr[6];
			menuWidth = configArr[7];
			menuHeight = configArr[8];
			nrOfMenuItem = configArr[9];
			var itemsToSize:Number = configArr[10];
			scrollDelayTime = configArr[11];
			scrollRepeatTime = configArr[12];
			// = configArr[13];
			// = configArr[14];
			// = configArr[15];
			// = configArr[16];
	
			middleItemPosition = itemsToSize + 1; //TODO detta måste kontrolleras så itemsToSize är bra i förhållande till nrOfMenuItem
			
			
			//menuFont.size = 16;			//ska inte behöva sättas, men det är så inte den som anropar menuobj har pillat på den
			
			//
			//define the placeholders, give them their y-position and height
			//
			var heightFactor:Number = 1.2;		//1.2;
			//var heightFactorMiddle:Number = 1.2;	
			var heightDivider:Number = 0;
			for (var i:Number=0; i<nrOfMenuItem; i++) {
				if (i<itemsToSize) {
					heightDivider = heightDivider + i + 1;
				} else if (i>nrOfMenuItem-itemsToSize-1) {
					heightDivider = heightDivider + (nrOfMenuItem - i);
			//	} else if (i==itemsToSize+1) {														/
			//		heightDivider = heightDivider + (itemsToSize + 1)*heightFactorMiddle;
				} else {
					heightDivider = heightDivider + (itemsToSize + 1);
				}
			}
			var unitSize = menuHeight / heightDivider;
			//unitsize borde sparas globalt
			for (var i:Number=0; i<nrOfMenuItem; i++) {
				if (i==0) {
					placeHolder[i] = new PlaceHolderObj(menuTop, unitSize*(i+1)*heightFactor);
				} else if (i<itemsToSize) {
					placeHolder[i] = new PlaceHolderObj(placeHolder[i-1].top+placeHolder[i-1].height/heightFactor, unitSize*(i+1)*heightFactor);
				} else if (i>nrOfMenuItem-itemsToSize-1) {
					placeHolder[i] = new PlaceHolderObj(placeHolder[i-1].top+placeHolder[i-1].height/heightFactor, unitSize*(nrOfMenuItem - i)*heightFactor);
			//	} else if (i==itemsToSize+1) {														//TODO !! kolla så detta kan uppfyllas! itemsToSize måste kontrolleras ordentligt
			//		placeHolder[i] = new PlaceHolderObj(placeHolder[i-1].top+placeHolder[i-1].height/heightFactor, unitSize*(itemsToSize+1)*heightFactorMiddle*heightFactor);
				} else {
					placeHolder[i] = new PlaceHolderObj(placeHolder[i-1].top+placeHolder[i-1].height/heightFactor, unitSize*middleItemPosition*heightFactor);
				}
			}	
			for (var i:Number = 0; i < nrOfMenuItem+1; i++) {
				menuTexts[i] = new MenuItemObj(mainScene, menuFont, menuLeft, menuWidth, unitSize*middleItemPosition);
			}

			//description skall placeras under det "valda" objektet (dess top+height)
			var dummytop:Number = placeHolder[middleItemPosition].top+placeHolder[middleItemPosition].height;
			//den skall maximalt sträcka sig ner till det näst sista objektet med "normal" storlek
			var dummymaxheight:Number = placeHolder[nrOfMenuItem - middleItemPosition-1].top+placeHolder[nrOfMenuItem - middleItemPosition-1].height - dummytop;
			menuItemDesc = new MenuDescObj(mainScene, descFont, descScrollTime, menuLeft+menuWidth/15, menuWidth-menuWidth/15, dummytop, dummymaxheight,unitSize*middleItemPosition*3/4);
			//unitSize*middleItemPosition*3/4 avgör storleken på texten för description, *3/4 = tre fjärdedelar så stor som en normal menuitem
	
			
			/*var testItem2:MovieClip = mainScene.createEmptyMovieClip("testItem2",mainScene.getNextHighestDepth());
			testItem2.beginFill(0x229922, 50);
			testItem2.moveTo(menuLeft,dummytop)
			testItem2.lineTo(menuLeft+menuWidth,dummytop)
			testItem2.lineTo(menuLeft+menuWidth,dummymaxheight+dummytop)
			testItem2.lineTo(menuLeft,dummymaxheight+dummytop)
			testItem2.endFill();*/	
	
		//bara tillfälligt så man ser hur ytan man har att jobba med ser ut
		//markerar nu mittitemet
			var testItem:MovieClip = mainScene.createEmptyMovieClip("testItem",mainScene.getNextHighestDepth());
			//testItem.beginFill(0x883333, 40);
			testItem.lineStyle(5, 0x0, 45, false, "none", "round", "round");
			testItem.moveTo(menuLeft,placeHolder[middleItemPosition].top)
			testItem.lineTo(menuLeft+menuWidth,placeHolder[middleItemPosition].top)
			testItem.lineTo(menuLeft+menuWidth,placeHolder[middleItemPosition].top+placeHolder[middleItemPosition].height)
			testItem.lineTo(menuLeft,placeHolder[middleItemPosition].top+placeHolder[middleItemPosition].height)
			//testItem.endFill();
			testItem.lineTo(menuLeft,placeHolder[middleItemPosition].top)
		
		}

	}		//end constructor
	
	function getName():String {
		return menuName;
	}
	
	public function setCommunicator(commObj:CommunicationObj):Void {
		//trace(commObj);
		this.commObj = commObj;
	}
	
	private function sendData(data:XML) {
		//använd commobj som fåtts med setcommunicator ovan, testa om den finns först
		//trace(commObj);
		if (commObj != undefined) {
			commObj.sendToServer(data);
		}
	}
	
	public function showHide (isVisible:Boolean) :Void {
		for (var i:Number = 0; i < nrOfMenuItem+1; i++) {
			menuTexts[i].visible(isVisible);
		}
		menuItemDesc.visible(isVisible);
	}
	
	public function setNewData (data:XMLNode) :Void {
		//trace(data);
		//trace("duration: " + moveTween.duration + " finish: " + moveTween.finish + " FPS: " + moveTween.FPS + " position: " + moveTween.position + " time: " + moveTween.time);

		//trace(data.nodeName);
		
		if (incomDataArr.length > 0) {
			incomDataArr.push(data);
		} else {
			if (data.nodeName == "command") {
				parseCommand(data);
			} else if (data.nodeName == "data") {
				if (!isScrolling) {
					parseMenuData(data);
				} else {
					incomDataArr.push(data);
				}
			}
		}
	}
	
	private function popData () {
		var whileCmd:Boolean = true;
		
		
		while (whileCmd && incomDataArr.length > 0) {
			trace("array size: " + incomDataArr.length);
			var shifted:Object = incomDataArr.shift();
			trace("parsar data, nodenamn: " + shifted.nodeName);
			if (shifted.nodeName == "data") {
				//shifted = incomDataArr.shift();
				parseMenuData(shifted);
				trace("skickat till menudataparsning");
			} else if (shifted.nodeName == "command") {
				//whileCmd = false;
				//trace("avsluta dataparsning");
				trace("kastar kommandon");

			}
		}
		
		/*
		//parsa data tills kommando kommer
		while (whileCmd && incomDataArr.length > 0) {
			var shifted:Object = incomDataArr[0];
			trace("parsar data, nodenamn: " + shifted.nodeName);
			if (shifted.nodeName == "data") {
				shifted = incomDataArr.shift();
				parseMenuData(shifted);
				trace("skickat till menudataparsning");
			} else if (shifted.nodeName == "command") {
				whileCmd = false;
				trace("avsluta dataparsning");

			}
		}
		
		whileCmd = true;
		trace("array size: " + incomDataArr.length);

		//sen parsas kommando tills data kommer
		while (whileCmd && incomDataArr.length > 0) {
			var shifted:Object = incomDataArr[0];
			trace("parsar kommando, nodenamn: " + shifted.nodeName);
			
			if (shifted.nodeName == "command") {
				shifted = incomDataArr.shift();
				//parseCommand(shifted);
				trace("skickat till kommandoparsning");
			} else if (shifted.nodeName == "data") {
				whileCmd = false;
				trace("avsluta kommandoparsning");
			}
		}*/
		
	}
	
	private function parseCommand (data:Object) {
		if (data.nodeName == "command") {
			if (data.firstChild.nodeName == "scroll") {
				if (data.firstChild.firstChild.nodeValue == 0) {
					scrollStop();
				} else if (data.firstChild.firstChild.nodeValue == 1) {
					scrollUp();
				} else if (data.firstChild.firstChild.nodeValue == -1) {
					scrollDown();
				}
			}
		}
	}
		
	private function parseMenuData (data:Object) {
		if (data.nodeName == "data") {
		
		//för tillfället, strunta i att uppdatera med ny data om man håller på att scrolla
		//prio på att fixa detta
		//detta funkar någorlunda, kanske ha en lista med data som läses in här
		//då man läst in data kollar man om listan är tom, annars får man läsa in igen
			//if (!(moveTween.time == undefined || moveTween.time == moveTween.duration)) {
			//	newDataExists = true;
			//	newData = data;
			if (false) {} else {
				menuItemDesc.hide();
				
				var middleItem:Number = 0;
				var middleItemId:String = "";
				var dataList:XMLNode;
				
				//trace(data);
				for (var aNode:XMLNode = data.firstChild; aNode != null; aNode=aNode.nextSibling) {
					//trace(aNode);
					
					if (aNode.nodeName == "middleid") {
						//trace("middle: " + aNode); 
						middleItemId = aNode.firstChild.nodeValue;
						//trace(middleItemId); 
					}
					if (aNode.nodeName == "list") {
						//trace("listan: " + aNode); 
						dataList = aNode;
					}
				}
				if (dataList != null) {
					menuData = new Array();
					var cnt:Number = 0;
					for (var aNode:XMLNode = dataList.firstChild; aNode != null; aNode=aNode.nextSibling) {
						//trace("nästa child: " + aNode);
						var title:String = "";
						var description:String = "";
						var id:String = "";
						title = aNode.attributes.title;
						description = aNode.attributes.desc;
						id = aNode.attributes.id;
						
						if (id == middleItemId) {
							middleItem = cnt;
						}
						
						//trace(id);
						//trace(title);
						//trace(description);
						
						/*for (var bNode:XMLNode = aNode.firstChild; bNode != null; bNode=bNode.nextSibling) {
								//trace("nytt" + bNode.nodeName);
							if (bNode.nodeName == "title") {
								if (bNode.firstChild.nodeValue != undefined) {
									title = bNode.firstChild.nodeValue;
								} else {
									title = "";
								}
							} else if (bNode.nodeName == "description") {
								if (bNode.firstChild.nodeValue != undefined) {
									description = bNode.firstChild.nodeValue;
								} else {
									description = "";
								}
							} else if (bNode.nodeName == "id") {
								if (bNode.firstChild.nodeValue != undefined) {
									id = bNode.firstChild.nodeValue;
									//trace("inkommande id: " + id + " cnt2:");
									if (id == middleItemId) {
										middleItem = cnt;
									}
								} else {
									id = "";
								}
							}
							//trace("nästa child: " + bNode);
						}*/
						menuData[cnt] = new MenuDataObj(id, title, description);
						//trace(menuData[cnt].description);
						cnt++;
					}
				}
	
				//trace(menuTexts.length);
				for (var i:Number = 0; i < menuTexts.length; i++) {
					//menuTexts[i].setText("");
					menuTexts[i].setFree(true);
					menuTexts[i].notInUse();
					menuTexts[i].visible(true);		//göm alla items (inuse är false därmed göms de när man anropa visible)
					menuTexts[i].setAlpha(100);
				}
				
				//dataList = data;							//save global
				menuMiddleItem = middleItem;	//save global
				
				var menuItemCnt:Number = 0;
				for (var i:Number = middleItem-middleItemPosition; i<middleItem+nrOfMenuItem-middleItemPosition; i++) {
					if (i<0) {
						if (menuData.length > nrOfMenuItem) {
							menuTexts[menuItemCnt].setText(menuData[menuData.length + i].title);
							menuTexts[menuItemCnt].setDataIndex(i);
						}
					} else if (i < menuData.length) {
						menuTexts[menuItemCnt].setText(menuData[i].title);
						menuTexts[menuItemCnt].setDataIndex(i);
					} else {		//nu är i > antal noder i listan, vi kanske ska ladda in nod0, nod1 osv nu
						if (menuData.length >= nrOfMenuItem) {
							menuTexts[menuItemCnt].setText(menuData[i - menuData.length].title);
							menuTexts[menuItemCnt].setDataIndex(i);
						} else {
							//trace(menuItemCnt + " " + nrOfMenuItem);
							//trace(menuItemCnt);
						}
					}
					menuItemCnt++;
				}
		
				for (var i:Number = 0; i < nrOfMenuItem; i++) {
					placeHolder[i].menuItemIndex = i;
			
					menuTexts[i].setFree(false);
					menuTexts[i].setTop(placeHolder[i].top);
					menuTexts[i].setHeight(placeHolder[i].height);
					//trace("i: " + i + " top: " + menuTexts[i].getTop() + " text: " + menuTexts[i].getText());
				}
				showDescription();
			}
		}
	}

	
	public function test (Void):Void {
			for (var i:Number = 0; i < nrOfMenuItem + 1; i++) {
				trace(i + " " + menuTexts[i].getTop() + " : " + menuTexts[i].getText());
				trace(menuTexts[i].isInUse());
				//placeHolder[i].menuItemIndex = i;
		
				//trace("i: " + i + " top: " + menuTexts[i].getTop() + " text: " + menuTexts[i].getText());
				//menuTexts[i].setHeight(placeHolder[i].height);
			}
			showDescription();

/*		var testvar:Number;
		for (var i:Number = 0; i < nrOfMenuItem+1; i++) {
			testvar = menuTexts[i].getHeight();
		}*/
	}
	
	private function onMoveTweenDone() {
		/*
	var tweenCommand:Number = 0;
		//0 = stop
		//1 = scrollup
		//2 = scrolldown
		
		*/
		isScrolling = false;
		
		if (tweenCommand == 1) {
			scrollUp();
		} else if (tweenCommand == 2) {
			scrollDown();
		} else if (tweenCommand == 0) {
			if (incomDataArr.length > 0) {
			//if (newDataExists) {
				//detta funkar någorlunda
				//trace(".time: " + moveTween.time + ".duration: " + moveTween.duration);
				//newDataExists = false;
				//setNewData(newData);
				popData();
				
			} else {
			//test();
				showDescription();
				sendData(new XML("<flinshpacket menuid=\"" + menuName + "\" middleid=\"" + menuData[menuMiddleItem].id + "\" />"));
			}
			//sendData(new XML("<data name=\"id\" value=\"" + menuName+ "\"><middleitem>" + menuMiddleItem + "</middleitem></data>"));
		}
	}

	private function showDescription() {
		
			//finns ingen description skall detta inte köras dock (det ser menudescobj till, dess height blir 0)
			menuItemDesc.update(menuData[menuMiddleItem].description);
			//menuItemDesc.update("Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.");
			
			//kör igång animering på de items som ligger ivägen
			var descY:Number = menuItemDesc.getBottom();
			var firstFreePlaceHolder:Number = 0;
			for (var i:Number = middleItemPosition+1; i < nrOfMenuItem; i++) {
				//ta reda på vilken som är första "fria" placeHoldern
				if (descY<placeHolder[i-1].top+placeHolder[i-1].height) {
					firstFreePlaceHolder = i;
					//trace(i + " " + (placeHolder[i-1].top + placeHolder[i-1].height) );
					break;
				}
			}
			if (firstFreePlaceHolder > 0) {
				for (var i:Number = 0; i < nrOfMenuItem + 1 ; i++) {
					menuTexts[i].fforward();
				}
				var offset:Number = firstFreePlaceHolder - middleItemPosition-1;
				for (var i:Number = middleItemPosition+1; i < nrOfMenuItem; i++) {
					//kolla igenom de menuTexts som möjligtvis kan ligga under description
					//dessa skall tweenas i storlek och position, offsetten mellan första upptagna och första fria skall sparas
					//alla ska flyttas med offsetten, hamnar de utanför sista placeholdern skall de läggas där med height = 0
					//placeHolders ska nu bara användas för att flytta och resiza menuTexts, de skall inte ändras
					//trace(i);
					if (i+offset<nrOfMenuItem) {
						moveTween = menuTexts[placeHolder[i].menuItemIndex].scroll(placeHolder[i+offset].top, placeHolder[i+offset].height, descScrollTime/2);
					} else {
						moveTween = menuTexts[placeHolder[i].menuItemIndex].scroll(placeHolder[nrOfMenuItem-1].top + placeHolder[nrOfMenuItem-1].height, 0, descScrollTime/2);
					}
				}
			}
			menuItemDesc.appear();		
	}
		
		
	public function scrollUp (Void) :Void {
		//trace("up");
		
		
		var wrapMenu:Boolean = (menuData.length >= nrOfMenuItem);
		moveTween.onMotionFinished = null;
		if ((menuMiddleItem > 0) || (wrapMenu)) {
			isScrolling = true;
			
			for (var i:Number = 0; i < nrOfMenuItem + 1; i++) {
				menuTexts[i].fforward();
			}

			menuMiddleItem--;
			menuItemDesc.hide();			//Hide the description placed under the marked item (if description is shown)
		
			//makes sure menumiddleitem is correct when wrapping
			if (menuMiddleItem < 0) {
				menuMiddleItem = menuData.length-1;
			}
			
			if (tweenCommand == 0) {		//om vi inte redan är igång så ska den längre scrollen köras
				var timeToScroll:Number = scrollDelayTime;
			} else {										//annars kör vi repeatrate
				var timeToScroll:Number = scrollRepeatTime;
			}
			tweenCommand = 1;
			
			//i placeholdern längst ner finns det nu lediga menuitem
			var dummyFree:Number = placeHolder[nrOfMenuItem-1].menuItemIndex;
			//scrolla detta menuitem så det hamnar under den placeholdern längst ner och scrolla dess height till 0
			moveTween = menuTexts[dummyFree].scroll(placeHolder[nrOfMenuItem-1].top + placeHolder[nrOfMenuItem-1].height, 0, timeToScroll);
		//BUGG löst
		//den beror delvis på att menuitemobj inte scrollar om objektet är på samma y-värde som tidigare
		//fixat längre ner, där man scrollar in den nya
		
			//fadea bort den också
			if (menuTexts[dummyFree].isInUse()) {
				menuTexts[dummyFree].fade(100, 0, timeToScroll);
				//detta buggar, inuse är inte riktigt rätt satt, vilket gör att itemet är visible 
				
			}

			//trace("frigör: " + dummyFree);
			
			for (var i:Number = nrOfMenuItem-1; i > 0; i--) {
				//trace("flyttar " + placeHolder[i-1].menuItemIndex + " till placeHolder[" + i + "]");
				placeHolder[i].menuItemIndex = placeHolder[i-1].menuItemIndex;
				var curPlace:PlaceHolderObj = placeHolder[i];
				moveTween = menuTexts[curPlace.menuItemIndex].scroll(curPlace.top, curPlace.height, timeToScroll);
			}
			
			//find a free menuText
			var freeItem:Number;
			for (var i:Number = 0; i < nrOfMenuItem+1; i++) {
				if (menuTexts[i].isFree()) {
					freeItem = i;
					break;
				}
			}
			
			//frigör den _efter_ valet av ny kontainer
			menuTexts[dummyFree].setFree(true);			
			menuTexts[dummyFree].notInUse();
			
			var curPlace:PlaceHolderObj = placeHolder[0];
			curPlace.menuItemIndex = freeItem;
			
			//newItem is the index of a menudata-entry, the entry that now should enter the menu
			var newItem:Number = menuMiddleItem-middleItemPosition;
			if ((wrapMenu) && (newItem<0)) {
				newItem = newItem + menuData.length;
			}
			if ((newItem>=0) || (wrapMenu)) {
				//trace("använder: " + freeItem + " till: " + menuData[newItem].title);
				menuTexts[curPlace.menuItemIndex].setFree(false);
				menuTexts[curPlace.menuItemIndex].setText(menuData[newItem].title);
				menuTexts[curPlace.menuItemIndex].setDataIndex(newItem);
				menuTexts[curPlace.menuItemIndex].setTop(menuTop+1);
				menuTexts[curPlace.menuItemIndex].setHeight(0);
				menuTexts[curPlace.menuItemIndex].fade(0, 100, timeToScroll);
				moveTween = menuTexts[curPlace.menuItemIndex].scroll(curPlace.top, curPlace.height, timeToScroll);
				//trace("inuse: " + menuTexts[freeItem].isInUse() + " kanal: " + menuData[newItem].title + " visible: " + menuTexts[freeItem].isvisible());
			} else {
				//trace("använder2: " + freeItem);
				menuTexts[curPlace.menuItemIndex].setFree(false);
				
			}

			moveTween.onMotionFinished = Delegate.create(this, onMoveTweenDone);			
		} else {
			showDescription();
			tweenCommand = 0;
		}
	}

	/**
	scrollDown 
	*/
	public function scrollDown () {
		
		var wrapMenu:Boolean = (menuData.length >= nrOfMenuItem);
		moveTween.onMotionFinished = null;
		
		if ((menuMiddleItem < menuData.length-1) || (wrapMenu)) {
			isScrolling = true;
			for (var i:Number = 0; i < nrOfMenuItem + 1 ; i++) {
				menuTexts[i].fforward();
				//fforward()
			}

			menuMiddleItem++;
			menuItemDesc.hide();			//Hide the description placed under the marked item (if description is shown)
			
			//makes sure menumiddleitem is correct when wrapping
			if (menuMiddleItem >= menuData.length) {
				menuMiddleItem = 0;
			}
			
			if (tweenCommand == 0) {		//om vi inte redan är igång så ska den längre scrollen köras
				var timeToScroll:Number = scrollDelayTime;
			} else {										//annars kör vi repeatrate
				var timeToScroll:Number = scrollRepeatTime;
			}
			tweenCommand = 2;
			
			var dummyFree:Number = placeHolder[0].menuItemIndex;
			//menuTexts[dummyFree].fforward();
			moveTween = menuTexts[dummyFree].scroll(menuTexts[dummyFree].getTop, 0, timeToScroll);
			menuTexts[dummyFree].fade(100, 0, timeToScroll);
			//trace("frigör: " + dummyFree);

			for (var i:Number = 0; i < nrOfMenuItem-1; i++) {
				placeHolder[i].menuItemIndex = placeHolder[i+1].menuItemIndex;
				var curPlace:PlaceHolderObj = placeHolder[i];
				moveTween = menuTexts[curPlace.menuItemIndex].scroll(curPlace.top, curPlace.height, timeToScroll);
			}
			
			//find a free menuText
			var freeItem:Number;
			for (var i:Number = 0; i < nrOfMenuItem+1; i++) {
				if (menuTexts[i].isFree()) {
					freeItem = i;
					break;
				}
			}
			//frigör den _efter_ valet av ny kontainer
			menuTexts[dummyFree].setFree(true);
			menuTexts[dummyFree].notInUse();
			
			var curPlace:PlaceHolderObj = placeHolder[nrOfMenuItem-1];
			curPlace.menuItemIndex = freeItem;
			
			//newItem is the index of a menudata-entry, the entry that now should enter the menu
			var newItem:Number = menuMiddleItem+nrOfMenuItem-middleItemPosition-1;
			if ((wrapMenu) && (newItem>=menuData.length)) {
				newItem = newItem - menuData.length;
			}
			if ((newItem<menuData.length) || (wrapMenu)) {
				//trace("använder: " + freeItem);
				menuTexts[curPlace.menuItemIndex].setFree(false);
				menuTexts[curPlace.menuItemIndex].setText(menuData[newItem].title);
				menuTexts[curPlace.menuItemIndex].setDataIndex(newItem);
				menuTexts[curPlace.menuItemIndex].setTop(menuTop+menuHeight);
				menuTexts[curPlace.menuItemIndex].setHeight(0);
				menuTexts[curPlace.menuItemIndex].fade(0, 100, timeToScroll);
				moveTween = menuTexts[curPlace.menuItemIndex].scroll(curPlace.top, curPlace.height, timeToScroll);
			} else {
				//trace("använder2: " + freeItem);
				menuTexts[curPlace.menuItemIndex].setFree(false);
				
			}
			
			moveTween.onMotionFinished = Delegate.create(this, onMoveTweenDone);
		} else {
			showDescription();
			tweenCommand = 0;
		}
		
	}
	
	
	
	public function scrollStop (Void) :Void {
		/*
		description kommer få en max-height, blir den större ska den scrolla :)
		denna kommer få max-height så att det alltid finns en stor item under samt alla de små
		
		var tweenCommand:Number = 0;
		//0 = stop
		//1 = scrollup
		//2 = scrolldown
		*/
		

		tweenCommand = 0;
		
	}

	public function setScrollDelay (value:Number) :Void {
		/*
		*/
		if (value > 0.05) {
			scrollDelayTime = value;
		}
	}
	public function setScrollRepeat (value:Number) :Void {
		/*
		*/
		if (value > 0.05) {
			scrollRepeatTime = value;
		}
	}

	public function getScrollDelay (Void) :Number {
		/*
		*/
			return scrollDelayTime;
	}
	public function getScrollRepeat (Void) :Number {
		/*
		*/
			return scrollRepeatTime;
	}

	private function parseConfig(data:XMLNode): Array {
		var confError:Number = -1;
		var confArr:Array = new Array();
		for (var bNode:XMLNode = data.firstChild; bNode != null && confError == -1; bNode=bNode.nextSibling) {
			//trace(bNode.nodeName);
			if (bNode.nodeName == "option") {
				switch (bNode.attributes.name) {
					case "menushortname":
					confArr[1] = bNode.attributes.value;
					break;
					case "menudescription":
					confArr[2] = bNode.attributes.value;
					break;
					case "rowitemfont":
					if (bNode.attributes.value.length > 0) {	
						confArr[3] = bNode.attributes.value;
					} else {
						confError = 3;
					}
					break;
					case "descriptionfont":
					if (bNode.attributes.value.length > 0) {	
						confArr[4] = bNode.attributes.value;
					} else {
						confError = 4;
					}
					//trace(aNode.attributes.value);
					break;
					case "left":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[5] = parseFloat(bNode.attributes.value) * Stage.width;
					} else {
						confError = 5;
					}
					break;
					case "top":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[6] = parseFloat(bNode.attributes.value) * Stage.height;
					} else {
						confError = 6;
					}
					break;
					case "width":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[7] = parseFloat(bNode.attributes.value) * Stage.width;
					} else {
						confError = 7;
					}
					break;
					case "height":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[8] = parseFloat(bNode.attributes.value) * Stage.height;
					} else {
						confError = 8;
					}
					break;
					case "menurowitems":
					if (parseInt(bNode.attributes.value) > 0) {
						confArr[9] = parseInt(bNode.attributes.value);
					} else {
						confError = 9;
					}
					break;
					case "sizedrowitems":
					if (parseInt(bNode.attributes.value) > 0) {
						confArr[10] = parseInt(bNode.attributes.value);
					} else {
						confError = 10;
					}
					break;
					case "scrolldelaytime":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[11] = parseFloat(bNode.attributes.value);
					} else {
						confError = 11;
					}
					//10
					break;
					case "scrollrepeattime":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[12] = parseFloat(bNode.attributes.value);
					} else {
						confError = 12;
					}
					break;
					case "menucolor":
					if (parseInt(bNode.attributes.value) >= 0) {
						confArr[13] = parseInt(bNode.attributes.value);
						//trace(confArr[13]);
					} else {
						confError = 13;
					}
					break;
					case "desccolor":
					if (parseInt(bNode.attributes.value) >= 0) {
						confArr[14] = parseInt(bNode.attributes.value);
					} else {
						confError = 14;
					}
					break;
					case "descscrolltime":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[15] = parseFloat(bNode.attributes.value);
					} else {
						confError = 15;
					}
					break;
					case "":
					if (true) {
						confArr[16] = parseFloat(bNode.attributes.value);
					} else {
						confError = 16;
					}
					break;				
					case "":
					if (true) {
						confArr[17] = parseFloat(bNode.attributes.value);
					} else {
						confError = 17;
					}
					break;				
					case "":
					if (true) {
						confArr[18] = parseFloat(bNode.attributes.value);
					} else {
						confError = 18;
					}
					break;				
				}
			}
		}
		confArr[0] = confError;
		return confArr;
	}
}
