
function Touch(type, name, id)
{
	this.CanService(type, name, id);
}

extend(Touch, CanService);

Touch.prototype.myLastGesture = null;
Touch.prototype.myLastX = null;
Touch.prototype.myLastY = null;
Touch.prototype.myLastState = "Released";

Touch.prototype.getLastGesture = function()
{
	return this.myLastGesture;
}
Touch.prototype.getLastState = function()
{
	return this.myLastState;
}
Touch.prototype.getLastX = function()
{
	return this.myLastX;
}
Touch.prototype.getLastY = function()
{
	return this.myLastY;
}

Touch.prototype.canMessageHandler = function(canMessage)
{
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{
		case "Gesture":
			this.lookupGesture(canMessage.getData("f1"),canMessage.getData("f2"),canMessage.getData("f3"),canMessage.getData("f4"),canMessage.getData("f5"),canMessage.getData("f6"),canMessage.getData("f7"),canMessage.getData("f8"),canMessage.getData("f9"));
		
			if (this.myLastGesture == null)
			{
				log(this.myName + ":" + this.myId + "> New Gesture, f1: " + canMessage.getData("f1") + ", f2: " + canMessage.getData("f2")+ ", f3: " + canMessage.getData("f3")+ ", f4: " + canMessage.getData("f4")+ ", f5: " + canMessage.getData("f5")+ ", f6: " + canMessage.getData("f6")+ ", f7: " + canMessage.getData("f7")+ ", f8: " + canMessage.getData("f8")+ ", f9: " + canMessage.getData("f9") +"\n");
			}
			else
			{
				log(this.myName + ":" + this.myId + "> New Gesture: " + this.myLastGesture + "\n");
			}
		
			this.callEvent("newGesture", null);
			break;
		case "Raw":
			this.myLastX = canMessage.getData("X");
			this.myLastY = canMessage.getData("Y");
			if (this.myLastState == "Pressed" && canMessage.getData("Status") == "Released")
			{
				this.myLastState = "Released";
				this.callEvent("rawStatus", null);
			}
			this.callEvent("rawTouch", null);
			if (this.myLastState == "Released" && canMessage.getData("Status") == "Pressed")
			{
				this.myLastState = "Pressed";
				this.callEvent("rawStatus", null);
			}
			
			break;
		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}


Touch.prototype.lookupGesture = function(f1, f2, f3, f4, f5, f6, f7, f8, f9)
{
	var gestureStore = DataStore.getStore("Gestures");
	this.myLastGesture = null;
	if (gestureStore)
	{
		for (var n = 0; n < gestureStore['gestures'].length; n++)
		{
			//log(this.myName + ":" + this.myId + "> Checking : " + gestureStore['gestures'][n]['name'] + "\n");
			this.myLastGesture = null;
			var result = 1;
			if (gestureStore['gestures'][n]['masks']['f1_lower'] > f1 || gestureStore['gestures'][n]['masks']['f1_upper'] < f1) 
			{
			//	log("1");
				result = 0;
				continue;
			}
			if (gestureStore['gestures'][n]['masks']['f2'] == 1 && gestureStore['gestures'][n]['functions']['f2'] != f2)
			{
			//	log("2");
				result = 0;
				continue;
			}
			if (gestureStore['gestures'][n]['masks']['f3'] == 1 && gestureStore['gestures'][n]['functions']['f3'] != f3)
			{
			//	log("3");
				result = 0;
				continue;
			}
			if (gestureStore['gestures'][n]['masks']['f4'] == 1 && gestureStore['gestures'][n]['functions']['f4'] != f4)
			{
			//	log("4");
				result = 0;
				continue;
			}
			if (gestureStore['gestures'][n]['masks']['f5'] == 1 && gestureStore['gestures'][n]['functions']['f5'] != f5)
			{
			//	log("5");
				result = 0;
				continue;
			}
			if (gestureStore['gestures'][n]['masks']['f6'] == 1 && gestureStore['gestures'][n]['functions']['f6'] != f6)
			{
			//	log("6");
				result = 0;
				continue;
			}
			if (gestureStore['gestures'][n]['masks']['f7_lower'] > f7 || gestureStore['gestures'][n]['masks']['f7_upper'] < f7) 
			{
			//	log("7");
				result = 0;
				continue;
			}
			if (gestureStore['gestures'][n]['masks']['f8'] == 1 && gestureStore['gestures'][n]['functions']['f8'] != f8)
			{
			//	log("8");
				result = 0;
				continue;
			}
			if (gestureStore['gestures'][n]['masks']['f9'] == 1 && gestureStore['gestures'][n]['functions']['f9'] != f9)
			{
			//	log("9");
				result = 0;
				continue;
			}
			if (result == 1) {
				this.myLastGesture = gestureStore['gestures'][n]['name'];
				return;
			}
		}
	}
	
}

