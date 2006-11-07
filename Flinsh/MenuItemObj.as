import mx.transitions.Tween;

class MenuItemObj {
	
	private var menuItem:MovieClip;
	private var menuText:TextField;
	private var menuFont:TextFormat;
	
	//ska sättas så fort objektet kopplas till ett index i data
	private var inUse:Boolean = false;
	private var free:Boolean = false;
	//håller en koppling till det index i data vars text nu finns i objektet
	private var dataIndex:Number;
	
	private var menuVisible:Boolean = false;
	
	private var tweenArr:Array;
	
	function MenuItemObj (mainScene:MovieClip, font:TextFormat, left:Number, width:Number, unitHeight:Number) {
		menuItem = mainScene.createEmptyMovieClip("menuItem", mainScene.getNextHighestDepth());
		menuItem._x = left;
		menuText = menuItem.createTextField("menuText", 1, 0, 0, 0, 10);
		menuText.text = "-";
		//font.font = "MenuText";
		menuText.setTextFormat(font);

		//size the textfield, strange way
		menuText.autoSize = "left";
		var fontSize = menuText._height;
		//var fontSize = menuText._height + 2;
		menuText.autoSize = "none";
		menuText._width = width*fontSize / unitHeight;
		menuItem._width = width;
		//dessa bredder är nu beroende av menuItem._height / menuText._height (unitsize), för att få det bra
			
		menuText.selectable = false;
		menuText.type = "dynamic";
		//menuText.textColor = 0xFF0000;
		menuText._height = fontSize;
		menuText.embedFonts = true;
		menuItem._visible = false;
		menuItem._height=0;
		
		menuFont = font;
		
		tweenArr = new Array();
	}		//end constructor
	
	public function setText(text:String) {
		menuText.text = text;
		menuText.setTextFormat(menuFont);
	}
	
	public function isInUse():Boolean {
		return inUse;
	}

	public function isvisible():Boolean {
		return menuItem._visible;
	}
	
	public function notInUse() {
		inUse = false;
	}

	public function isFree():Boolean {
		return free;
	}
	
	public function setFree(freeVar:Boolean) {
		free = freeVar;
	}
	
	public function setDataIndex(index:Number) {
		dataIndex = index;
		inUse = true;
	}
	
	public function fforward() {
		var tweenObj:Object;
		for (; tweenArr.length > 0; tweenObj = tweenArr.pop()) {
			//var testobj:Tween = tweenObj;
			if (tweenObj.time != undefined && tweenObj.time != tweenObj.duration) {
				tweenObj.fforward();
			}
			//tweenObj.stop();
		}
	}
	
	//scroll använder movieclippets nuvarande parametrar för höjd och y-position
	public function scroll(newTop:Number, newHeight:Number, timeToScroll:Number):Tween {
		menuItem._visible = (menuVisible && inUse);
		//menuItem._alpha = 100;
		//trace(menuText.text + " " + menuVisible);
		var tweenObj:Tween;
		if (newHeight != menuItem._height) {
			tweenObj = new Tween(menuItem, "_height", null, menuItem._height, newHeight, timeToScroll, true);
			tweenArr.push(tweenObj);
			/*if (newHeight == 0) {
				trace(menuItem._visible);
			}*/
		}
		if (newTop != menuItem._y) {
			tweenObj = new Tween(menuItem, "_y", null, menuItem._y, newTop, timeToScroll, true);
			tweenArr.push(tweenObj);
		}
		return tweenObj;
	}
	
	public function fade(from:Number, to:Number, timeToFade:Number):Tween {
		var tweenObj:Tween;
		if (from != to) {
			tweenObj = new Tween(menuItem, "_alpha", null, from, to, timeToFade, true);
			tweenArr.push(tweenObj);
		}
		return tweenObj;
		
	}
	
	public function visible(isVisible:Boolean) {
		menuVisible = isVisible;
		menuItem._visible = (menuVisible && inUse);
	}
	
	public function getText():String {
		return menuText.text;
	}
	
	public function getTop():Number {
		return menuItem._y;
	}
	
	public function setAlpha(value:Number) {
		if (value <= 100 && value >= 0) {
			menuItem._alpha = value;
		}
	}
	
	public function getHeight():Number {
		//trace("synlig: " + menuItem._visible + " höjd: " + menuItem._height + " bredd: " + menuItem._width + " y: " +menuItem._y + " bredd: " + menuText._width + " höjd: " + menuText._height);
		return menuItem._height;
	}
	public function setTop(top:Number) {
		//animera senare?
		menuItem._visible = (menuVisible && inUse);
		menuItem._y = top;
	}
	public function setHeight(height:Number) {
		//animera senare?
		menuItem._visible = (menuVisible && inUse);
		menuItem._height = height;
	}
	
}