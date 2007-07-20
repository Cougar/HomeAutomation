class TimeDateObj {
	
	private var timeDateMC:MovieClip;
	private var timeDateTF:TextField;
	
	private var timeDateFont:TextFormat;
	//variabeldeklaration
	//screen position and size
	private var timeDateTop:Number;
	private var timeDateLeft:Number;
	private var timeDateWidth:Number;
	private var timeDateHeight:Number;
	
	private var timeDateConfFormat:String;
	
	function TimeDateObj (mainScene:MovieClip, configXML:XMLNode) {
		//TODO
		//objektet lyder inte width
		var configArr:Array = parseConfig(configXML);
		
		if (configArr[0] > -1) {
			trace("Config error: " + configArr[0]);
		} else {
			timeDateFont = new TextFormat();
			
			timeDateFont.font = configArr[1];
			timeDateFont.color = configArr[6];
			timeDateLeft = configArr[2];
			timeDateTop = configArr[3];
			timeDateWidth = configArr[4];
			timeDateHeight = configArr[5];
			timeDateConfFormat = configArr[7];
			
			timeDateMC = mainScene.createEmptyMovieClip("timeDateMC", mainScene.getNextHighestDepth());
			timeDateMC._x=timeDateLeft;
			timeDateMC._y=timeDateTop;
			timeDateTF = timeDateMC.createTextField("timeDateTF", 1, 0, 0, 0, 10);
			timeDateTF.embedFonts = true;
		
		
			timeDateTF.text = "Bla bla";
			timeDateTF.setTextFormat(timeDateFont);

			timeDateTF.autoSize = "left";
			timeDateMC._width = timeDateWidth;
			timeDateMC._height = timeDateHeight;
			
			timeDateTF.selectable = false;
			timeDateTF.type = "dynamic";
			timeDateMC._visible = true;
			
			
			updateTimer();
			//timeDateTF._rotation = 45;
			timeDateMC._rotation = configArr[8];
			
			var intervalID:Number = setInterval(this, "updateTimer", 200);
			//trace(timeDateTF.text);
		}
	}
	
	
	 private function updateTimer():Void {
		/*var my_date:Date = new Date();
		var minStr:String = my_date.getMinutes().toString();
		var div:String = ":";
		if (minStr.length == 1) {
			var div:String = ":0";
		}*/
		//trace(timeDateConfFormat);
		timeDateTF.text = parseFormat(timeDateConfFormat);
		//timeDateTF.text = my_date.getHours() + div + minStr;
		timeDateTF.setTextFormat(timeDateFont);
	}
	
	private function parseFormat(timeDateFormat:String): String {
		//TODO:
		//fixa så escapeade tecken inte parsas om till tid/datum
		var dayOfWeek_array:Array = new Array("Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday");
		var shortDayOfWeek_array:Array = new Array("Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat");
		var month_array:Array = new Array("January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December");
		var shortMonth_array:Array = new Array("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec");
		var daysInMonth_array:Array = new Array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
		
		var today_date:Date = new Date();
		var dummyInt:Number = today_date.getDate();
		var date_d:String = "" + dummyInt;
		if (dummyInt < 10) {
			date_d = "0" + date_d;
		}

		var date_D:String = shortDayOfWeek_array[today_date.getDay()];
		var date_j:String = "" + today_date.getDate();
		var date_l:String = dayOfWeek_array[today_date.getDay()];
		
		var date_N:String = "" + today_date.getDay();
		if (today_date.getDay() == 0) {
			date_N = "7";
		}
		
		if (today_date.getDate() == 1 || today_date.getDate() == 21 || today_date.getDate() == 31) {
			var date_S:String = "st";
		} else if (today_date.getDate() == 2 || today_date.getDate() == 22) {
			var date_S:String = "nd";
		} else if (today_date.getDate() == 3 || today_date.getDate() == 23) {
			var date_S:String = "rd";
		} else {
			var date_S:String = "th";
		}
		
		var date_w:String = "" + today_date.getDay();
		var date_F:String = month_array[today_date.getMonth()];
		
		var dummyInt:Number = today_date.getMonth()+1;
		var date_m:String = "" + dummyInt;
		if (dummyInt < 10) {
			date_m = "0" + date_m;
		}
		
		var date_M:String = shortMonth_array[today_date.getMonth()];
		var date_n:String = "" + today_date.getMonth();
		var date_t:String = daysInMonth_array[today_date.getMonth()];
		var date_Y:String = "" + today_date.getFullYear();
		var date_y:String = date_Y.substr(2, 2);
		
		dummyInt = today_date.getHours();
		var date_a:String = (dummyInt<12) ? "am" : "pm";
		var date_A:String = (dummyInt<12) ? "AM" : "PM";

    dummyInt = dummyInt%12;
    if (dummyInt == 0) {
    	dummyInt = 12;
    }
		var date_g:String = "" + dummyInt;
		var date_G:String = "" + today_date.getHours();
		var date_h:String = "" + dummyInt;
		var date_H:String = "" + today_date.getHours();
		if (dummyInt < 10) {
			date_h = "0" + date_h;
		}
		dummyInt = today_date.getHours();
		if (dummyInt < 10) {
			date_H = "0" + date_H;
		}
		
		dummyInt = today_date.getMinutes();
		var date_i:String = "" + dummyInt;
		if (dummyInt < 10) {
			date_i = "0" + date_i;
		}
		dummyInt = today_date.getSeconds();
		var date_s:String = "" + dummyInt;
		if (dummyInt < 10) {
			date_s = "0" + date_s;
		}
		
		var dummyInt2:Number = dummyInt2.parseInt(today_date.valueOf()/1000)
		var date_U:String = "" + dummyInt;
		
		var returnstr:String = replace(timeDateFormat, "dd", date_d);
		returnstr = replace(returnstr, "d", date_d);
		returnstr = replace(returnstr, "D", date_D);
		returnstr = replace(returnstr, "j", date_j);
		returnstr = replace(returnstr, "l", date_l);
		returnstr = replace(returnstr, "N", date_N);
		returnstr = replace(returnstr, "S", date_S);
		returnstr = replace(returnstr, "w", date_w);
		returnstr = replace(returnstr, "F", date_F);
		returnstr = replace(returnstr, "mm", date_m);
		returnstr = replace(returnstr, "m", date_m);
		returnstr = replace(returnstr, "M", date_M);
		returnstr = replace(returnstr, "n", date_n);
		returnstr = replace(returnstr, "t", date_t);
		returnstr = replace(returnstr, "YYYY", date_Y);
		returnstr = replace(returnstr, "Y", date_Y);
		returnstr = replace(returnstr, "yy", date_y);
		returnstr = replace(returnstr, "y", date_y);
		returnstr = replace(returnstr, "a", date_a);
		returnstr = replace(returnstr, "A", date_A);
		returnstr = replace(returnstr, "g", date_g);
		returnstr = replace(returnstr, "G", date_G);
		returnstr = replace(returnstr, "hh", date_h);
		returnstr = replace(returnstr, "h", date_h);
		returnstr = replace(returnstr, "HH", date_H);
		returnstr = replace(returnstr, "H", date_H);
		returnstr = replace(returnstr, "ii", date_i);
		returnstr = replace(returnstr, "i", date_i);
		returnstr = replace(returnstr, "ss", date_s);
		returnstr = replace(returnstr, "s", date_s);
		returnstr = replace(returnstr, "U", date_U);
//		returnstr = replace(returnstr, "", );

		/*
	not implemented: 
	Timezone (e, I (capital i), O, T, Z)
	Full Date/Time (c, r)
	Anything with leap year (L, z)
	Week (W)
	and o
		*/
	
		return returnstr;
	}
	
	private function replace(holder:String, searchfor:String, replacement:String):String {
		var temparray:Array = holder.split(searchfor);
		holder = temparray.join(replacement);
		return (holder);
	}
	
	private function parseConfig(data:XMLNode): Array {
		var confError:Number = -1;
		var confArr:Array = new Array();
		for (var bNode:XMLNode = data.firstChild; bNode != null && confError == -1; bNode=bNode.nextSibling) {
			//trace(bNode.nodeName);
			if (bNode.nodeName == "option") {
				switch (bNode.attributes.name) {
					case "font":
					if (bNode.attributes.value.length > 0) {	
						confArr[1] = bNode.attributes.value;
					} else {
						confError = 1;
					}
					break;
					case "left":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[2] = parseFloat(bNode.attributes.value) * Stage.width;
					} else {
						confError = 2;
					}
					break;
					case "top":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[3] = parseFloat(bNode.attributes.value) * Stage.height;
					} else {
						confError = 3;
					}
					break;
					case "width":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[4] = parseFloat(bNode.attributes.value) * Stage.width;
					} else {
						confError = 4;
					}
					break;
					case "height":
					if (parseFloat(bNode.attributes.value) >= 0) {
						confArr[5] = parseFloat(bNode.attributes.value) * Stage.height;
					} else {
						confError = 5;
					}
					break;
					case "fontcolor":
					if (parseInt(bNode.attributes.value) >= 0) {
						confArr[6] = parseInt(bNode.attributes.value);
						//trace(confArr[6]);
					} else {
						confError = 6;
					}
					break;
					case "format":
					if (bNode.attributes.value.length > 0) {
						confArr[7] = bNode.attributes.value;
						//trace(confArr[6]);
					} else {
						confError = 7;
					}
					break;
					case "rotation":
					if (parseInt(bNode.attributes.value) >= -180 && parseInt(bNode.attributes.value) <= 180) {
						confArr[8] = parseInt(bNode.attributes.value);
						//trace(confArr[6]);
					} else {
						confError = 8;
					}
					break;
				
				}
			}
		}
		confArr[0] = confError;
		return confArr;
	}
	
}