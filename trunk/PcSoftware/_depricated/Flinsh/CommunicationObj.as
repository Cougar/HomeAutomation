import mx.utils.Delegate;

class CommunicationObj {
	
	var isConnected:Boolean;
	var mySocket:XMLSocket;
	var hostname:String;
	var dataReceivers:Array;
	var userName:String;
	var passWd:String
	
	function CommunicationObj (host:String) {
		isConnected = false;
		
		hostname = host;
		
		mySocket = new XMLSocket();
		mySocket.onData = Delegate.create(this, onData);
		mySocket.onClose = Delegate.create(this, onClose);
		mySocket.onConnect = Delegate.create(this, onConnect);
		var intervalID:Number = setInterval(this, "timerConnect", 15000);

		timerConnect();
	}		//end constructor
	
	function setDataReceiver(objs:Array) {
		dataReceivers = objs;
		
		for (var i:Number = 0; i < dataReceivers.length; i++) {
			dataReceivers[i].setCommunicator(this);
		}
		
	}
	
	public function sendToServer(data:XML) {
		if (isConnected) {
			mySocket.send(data + "\n");
		}
		//trace(data);
	}
	
	
	private function onData(msg) {
		//trace("Got data: " + msg.charCodeAt(0) + " " + msg.charCodeAt(1) + " " + msg.charCodeAt(2));
		//ta bort alla linefeed \n och carrage return \r
		msg = replace(msg, "\n", "");
		msg = replace(msg, "\r", "");
		var msgXML:XML = new XML(msg);
		
		//trace(msg);
		
		//trace("nodename: " + msgXML.firstChild.nodeName);
		//trace("id: " + msgXML.firstChild.attributes.id);
		
		
		//trace("blipp");
		if (msgXML.firstChild.nodeName == "packet") {

			var menuName:String = msgXML.firstChild.attributes.menuid;
			var dataList:XMLNode = msgXML.firstChild.firstChild;
			
			//msgXML.firstChild.firstChild, egentligen skulle den rota igenom
			//alla childs, det kanske kan komma flera data i samma paket

			for (var i:Number = 0; i < dataReceivers.length; i++) {
				if (menuName == dataReceivers[i].getName()) {
					dataReceivers[i].setNewData(dataList);
					//mainMenu.setNewData(dataList, middleItem);
				}				
			}

			/*
			//rota igenom dataReceivers, använd deras getName-funktion
			//om dess namn stämmer med namnet i xml-datan så skicka listan dit
			var middleItem:Number = 0;
			var dataList:XMLNode;
			var menuName:String;
			
			dataList = msgXML.firstChild;
			//trace(dataList);
			for (var aNode:XMLNode = msgXML.firstChild.firstChild; aNode != null; aNode=aNode.nextSibling) {
				if (aNode.nodeName == "contentname") {
					menuName = aNode.firstChild.nodeValue;
					for (var i:Number = 0; i < dataReceivers.length; i++) {
						if (menuName == dataReceivers[i].getName()) {
							dataReceivers[i].setNewData(dataList);
							//mainMenu.setNewData(dataList, middleItem);
						}				
					}
				}
			}*/
		}
	}
	
	private function onClose() {
		trace("Server closed connection!");
		//msgArea.htmlText = "<b>Server closed connection</b><br>" + msgArea.htmlText;
		//intervalID = setInterval(this, "timerConnect", 5000);
		isConnected = false;
	}
	
	public function setLogin(username:String, pass:String) {
		passWd = pass;
		userName = username;
		
	}
	
	private function onConnect(success) {
		if (success) {
			trace("Server connection established!");
			//msgArea.htmlText = "<b>Server connection established!</b><br>" + msgArea.htmlText;

			isConnected = true;
			
			//skicka login om det är configurerat
			if (passWd != "" && userName != "") {
				sendToServer(new XML("<flinshpacket login=\"" + userName + "\" pass=\"" + passWd + "\" />"));
			}
			
		} else {
			//msgArea.htmlText = "<b>Server connection failed!</b><br>" + msgArea.htmlText;
			trace("Server connection failed!");
			isConnected = false;
			//intervalID = setInterval(this, "timerConnect", 5000);
			//mySocket.connect("deep.arune.se", 2229);
		}
	}
	
	private function timerConnect() {
		//	intervalID = null;
		if (!isConnected) {
			mySocket.connect(hostname, 2229);
			//msgArea.htmlText = "<b>Connecting...</b><br>" + msgArea.htmlText;
		} else {
			mySocket.send("<ping />\n");
		}
		
	}

	private function replace(holder:String, searchfor:String, replacement:String):String {
		//trace("inte här");
		var temparray:Array = holder.split(searchfor);
		holder = temparray.join(replacement);
		return (holder);
	}
}