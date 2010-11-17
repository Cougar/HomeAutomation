

/*
String.pad(length: Integer, [substring: String = " "], [type: Integer = 0]): String
    	Returns the string with a substring padded on the left, right or both sides.
    length
        amount of characters that the string must have
    substring
        string that will be concatenated
    type
        specifies the side where the concatenation will happen, where: 0 = left, 1 = right and 2 = both sides
*/
String.prototype.pad = function(l, s, t)
{
	return	s || (s = " "), (l -= this.length) > 0 ? (s = new Array(Math.ceil(l / s.length)
		+ 1).join(s)).substr(0, t = !t ? l : t == 1 ? 0 : Math.ceil(l / 2))
		+ this + s.substr(0, l - t) : this;
}

String.prototype.html_entity_decode = function()
{
	var str = this.replace(/&#197;/g, "Ä");
	str = str.replace(/&Auml;/g, "Ä");
	
	str = str.replace(/&#196;/g, "Å");
	str = str.replace(/&Aring;/g, "Å");
	
	str = str.replace(/&#214;/g, "Ö");
	str = str.replace(/&Ouml;/g, "Ö");
	
	str = str.replace(/&#228;/g, "ä");
	str = str.replace(/&auml;/g, "ä");
	
	str = str.replace(/&#229;/g, "å");
	str = str.replace(/&aring;/g, "å");
	
	str = str.replace(/&#246;/g, "ö");
	str = str.replace(/&ouml;/g, "ö");
	
	//FIXME: Add more characters
	
	return str;
}

Date.prototype.getTimestamp = function()
{
	return this.getTime()/1000;
}

Date.prototype.getDateFormated = function()
{
	return this.getFullYear() + "-" + (this.getMonth()+1).toString().pad(2, "0", 0) + "-" + this.getDate().toString().pad(2, "0", 0);
}

Date.prototype.getTimeFormated = function()
{
	return this.getHours().toString().pad(2, "0", 0) + ":" + this.getMinutes().toString().pad(2, "0", 0) + ":" + this.getSeconds().toString().pad(2, "0", 0);
}

Date.prototype.getTimeShortFormated = function()
{
	return this.getHours().toString().pad(2, "0", 0) + ":" + this.getMinutes().toString().pad(2, "0", 0);
}

Date.prototype.getDateTimeFormated = function()
{
	return this.getDateFormated() + " " + this.getTimeFormated();
}

var ClientId = 0;

function setClientId(clientId)
{
	ClientId = clientId;
}
