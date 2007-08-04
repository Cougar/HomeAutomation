import mx.transitions.Tween;

class MenuDescObj {
	
	private var menuItem:MovieClip;
	private var menuText:TextField;
	private var menuFont:TextFormat;
	
	private var menuVisible:Boolean = false;
	private var inUse:Boolean = false;
	
	private var textWidth:Number;
	private var fontSize:Number;
	private var unitHeight:Number;
	private var maxHeight:Number;
	
	private var scrollTime:Number;
	
	function MenuDescObj (mainScene:MovieClip, font:TextFormat, scrollTime:Number, left:Number, width:Number, top:Number, maxHeight:Number, unitHeight:Number) {
		this.scrollTime = scrollTime;
		menuItem = mainScene.createEmptyMovieClip("menuItem", mainScene.getNextHighestDepth());
		menuItem._x = left;
		menuItem._y = top;
		menuFont = font;
		this.unitHeight = unitHeight;
		this.maxHeight = maxHeight;
		menuText = menuItem.createTextField("menuText", 1, 0, 0, 0, 10);
		menuText.embedFonts = true;
		
		//size the textfield, strange way
		menuText.text = "-";
		menuText.setTextFormat(menuFont);
		menuText.autoSize = "left";
		fontSize = menuText._height;
		menuText.autoSize = "none";

		menuText._width = width*fontSize / unitHeight * 1.2;	//(1.2 to fit more text)
		menuItem._width = width;
		
		menuText.multiline = true;
		//menuText.html = true;
		menuText.wordWrap = true;
		menuText.selectable = false;
		//menuText.textColor = 0xFF0000;
		menuText.type = "dynamic";
		menuItem._visible = false;
		
		menuText._height = fontSize;
		menuItem._height = unitHeight;

		
	}		//end constructor
	
	public function update(text:String) {
		//rutinen skall skrivas om, då objektet laddas skall man testa raderna vs height
		//så det görs EN gång
		
		menuText.text = text;
		//menuText.htmlText
		menuText.setTextFormat(menuFont);
		menuText._height = fontSize;
		menuItem._height = unitHeight;
		
		//trace("bottom: " + menuText.bottomScroll + " max: " + menuText.maxscroll);
		if (text.length > 0) {
			
			for (var i:Number = fontSize; true; i = i+fontSize/5) {
				menuText._height = i*fontSize/unitHeight;
				menuItem._height = i;
				menuText.text = text;
				menuText.setTextFormat(menuFont);
				if (menuText.maxscroll == 1) {
					break;
				}
				if (menuItem._height > maxHeight) {
					menuText._height = (i-fontSize/5)*fontSize/unitHeight;
					menuItem._height = (i-fontSize/5);
					
					break;
				}
				
				//TODO: ta bort denna!
				if (i > 500) {
					trace(menuItem._height + " " + maxHeight);
					break;
				}
				//trace(menuText.maxscroll + " " + menuText.scroll + " " + menuText.bottomScroll);
				//trace(menuText.bottomScroll + " fontsize: " + fontSize + " i: " + i + " menutext-height: " + menuText._height);
			}
		} else {
			menuItem._height = 0;
		}

	}
	
	//public function getSize
	
	public function appear() {
		inUse = true;
		menuItem._visible = (menuVisible && inUse);
		var test:Tween = new Tween(menuText, "_alpha", null, 5, 100, scrollTime, true);
		
	}

	public function hide() {
		inUse = false;
		menuItem._visible = (menuVisible && inUse);
		//var test:Tween = new Tween(menuText, "_alpha", null, 5, 100, 1, true);
		
	}
	
	public function setText(text:String) {
		menuText.text = text;
		menuText.setTextFormat(menuFont);
	}

	public function visible(isVisible:Boolean) {
		menuVisible = isVisible;
		menuItem._visible = (menuVisible && inUse);
	}	
	
	public function getTop():Number {
		return menuItem._y;
	}
	public function getBottom():Number {
		return menuItem._y + menuItem._height;
	}
	/*public function setTop(top:Number) {
		//animera senare?
		//menuItem._visible = (menuVisible && inUse);
		menuItem._y = top;
	}*/
	public function setHeight(height:Number) {
		//animera senare?
		//menuItem._visible = (menuVisible && inUse);
		menuItem._height = height;
	}
	
}