import mx.utils.Delegate;

class ConfigObj {
	
	//var title:String;
	//var description:String;
	var contentsArr:Array = new Array();
	var timeDate:TimeDateObj;
	var comm:CommunicationObj;
	var confDataXML:XML;
	var mainMovieClip:MovieClip;
	//länka till en menuitem och eller en xmlitem
	
	function ConfigObj (mainMovieClip:MovieClip) {
		confDataXML.load("Flinsh_conf.xml");
		confDataXML.ignoreWhite = true;
		this.mainMovieClip = mainMovieClip;
		//var dummyFnt:TextFormat = new TextFormat();
		//dummyFnt.font = "MenuText";
		confDataXML.onLoad = Delegate.create(this, onLoad);
		//confDataXML.onLoad = onLoad();
		//trace("ba");

		
		
	}		//end constructor
	
	
	private function onLoad(success:Boolean) {
		trace(confDataXML);
			if (success) {
				var menuCnt:Number = 0;
				for (var aNode:XMLNode = confDataXML.firstChild; aNode != null; aNode=aNode.nextSibling) {
					//trace(aNode.nodeName);
					if (aNode.nodeName == "option") {
						switch (aNode.attributes.name) {
							case "backcolor":
							setBG(parseInt(aNode.attributes.value));
							break;
							case "backgroundimage":
								if (aNode.attributes.value.length > 0) {
//									bg_image.loadMovie(aNode.attributes.value);
								}
							break;
							case "host":
								if (aNode.attributes.value.length > 0) {
									comm = new CommunicationObj(aNode.attributes.value);
									
									//gör om, parsa klart config innan commobj skapas, port kommer också
								}
							break;
						}
					}
					if (aNode.nodeName == "item") {
						if (aNode.attributes.name == "type" && aNode.attributes.value == "menu") {
							//menushortname finns hos menyobjektet, skapa funktion som returnerar namn, 
							//flinsh.fla får loopa igenom alla menyer och fråga om vems datan är
							
							//skicka med hela aNode som parameter till menuobj
							var menuConfig:XMLNode = aNode;
							contentsArr[menuCnt] = new MenuObj(mainMovieClip, menuConfig);
			
							//contentsArr[menuCnt] = new MenuObj(mainMovieClip, confArr);
							contentsArr[menuCnt].showHide(true);

							//testDataXML.load("testdata.xml");

							menuCnt++;
						}
						if (aNode.attributes.name == "type" && aNode.attributes.value == "label") {
							//skicka med hela aNode som parameter till menuobj
							var labelConfig:XMLNode = aNode;
//							contentsArr[menuCnt] = new LabelObj(mainMovieClip, labelConfig);
			
//							contentsArr[menuCnt].showHide(true);
//							menuCnt++;
						}
						if (aNode.attributes.name == "type" && aNode.attributes.value == "timedate") {
							//skicka med hela aNode som parameter till timedate
							var timeDateConfig:XMLNode = aNode;
							timeDate = new TimeDateObj(mainMovieClip, timeDateConfig);
						}
					}
				}
				if (comm != undefined) {
					comm.setDataReceiver(contentsArr);
				}
			}
			
			//om inte succes ska log skrivas till fil?
		};
	
	function setBG (col) {
		var rp = -System.capabilities.screenResolutionX;
		with (_level0) {
			clear();
			if (col != undefined) {
				beginFill(col);
				moveTo(rp, rp);
				//let us add 2 px each as flash eats them
				//in the Stage Object from the actual flash dimension
				lineTo(Stage.width*2, rp*2); 
				lineTo(Stage.width*2, Stage.height*2);
				lineTo(rp*2, Stage.height*2);
				endFill();
			}
		}
		mainMovieClip.__bg__ = col;
	};		
		
/*
var testDataXML:XML = new XML();

testDataXML.ignoreWhite = true;
testDataXML.onLoad = function(success:Boolean) {
	if (success) {
		if (testDataXML.firstChild.nodeName == "contentdata") {
			//Data should be sent to menu
			//right now Flinsh have only one menu, send data to it
			menuArr[0].setNewData(testDataXML.firstChild);
		}
		//trace("bingo");
	    //trace("loaded: "+testDataXML.loaded);
	    //trace("status: "+testDataXML.status);
	}
	
	//om inte succes ska log skrivas till fil?
};
*/
		
		
}