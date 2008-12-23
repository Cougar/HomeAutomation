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
    return s || (s = " "), (l -= this.length) > 0 ? (s = new Array(Math.ceil(l / s.length)
        + 1).join(s)).substr(0, t = !t ? l : t == 1 ? 0 : Math.ceil(l / 2))
        + this + s.substr(0, l - t) : this;
}

function extend(descendant, parent)
{
	var sConstructor = parent.toString();
	var aMatch = sConstructor.match(/\s*function (.*)\(/);
	
	if (aMatch != null)
	{
		descendant.prototype[aMatch[1]] = parent;
	}
	
	for (var m in parent.prototype)
	{
		descendant.prototype[m] = parent.prototype[m];
	}
}

function time(d)
{
	if (!d)
		d = new Date();
		
	return d.getTime()/1000;
}

function formatDate(d)
{
	if (!d)
		d = new Date();
		
	return d.getFullYear() + "-" + (d.getMonth()+1).toString().pad(2, "0", 0) + "-" + d.getDate().toString().pad(2, "0", 0);
}

function formatTime(d)
{
	if (!d)
		d = new Date();
		
	return d.getHours().toString().pad(2, "0", 0) + "." + d.getMinutes().toString().pad(2, "0", 0) + "." + d.getSeconds().toString().pad(2, "0", 0);
}

function formatDateTime(d)
{
	if (!d)
		d = new Date();
		
	return formatDate(d) + " " + formatTime(d);
}

function ltrim(str, charlist)
{
	charlist = !charlist ? ' \s\xA0' : charlist.replace(/([\[\]\(\)\.\?\/\*\{\}\+\$\^\:])/g, '\$1');
	var re = new RegExp('^[' + charlist + ']+', 'g');
	return str.replace(re, '');
}

function rtrim(str, charlist)
{
	charlist = !charlist ? ' \s\xA0' : charlist.replace(/([\[\]\(\)\.\?\/\*\{\}\+\$\^\:])/g, '\$1');
	var re = new RegExp('[' + charlist + ']+$', 'g');
	return str.replace(re, '');
}

function trim(str, charlist)
{
	str = ltrim(str, charlist);
	return rtrim(str, charlist);
}
