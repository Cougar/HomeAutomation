
var global_legacy = false;

LoadScript("common/json.js");

function Start(legacy)
{
	global_legacy = legacy;
	
	if (global_legacy)
	{
		LoadScript("legacy/System/Base.js");
		LoadScript("legacy/System/Startup.js");
		LoadScript("legacy/Autostart.js");
		
		autostart();
	}
	else
	{
		LoadScript("user/autostart.js");
	}
}

function Extend(descendant, parent)
{
	var s_constructor = parent.toString();
	var a_match = s_constructor.match(/\s*function (.*)\(/);
	
	if (a_match != null)
	{
		descendant.prototype[a_match[1]] = parent;
	}
	
	for (var m in parent.prototype)
	{
		descendant.prototype[m] = parent.prototype[m];
	}
}

function ArrayContains(array, item)
{
	for (var n in array)
	{
		if (array[n] == item)
		{
			return true;
		}
	}
	
	return false;
}

String.prototype.ltrim = function(charlist)
{
	charlist = !charlist ? ' \s\xA0' : charlist.replace(/([\[\]\(\)\.\?\/\*\{\}\+\$\^\:])/g, '\$1');
	var re = new RegExp('^[' + charlist + ']+', 'g');
	return this.replace(re, '');
}

String.prototype.rtrim = function(charlist)
{
	charlist = !charlist ? ' \s\xA0' : charlist.replace(/([\[\]\(\)\.\?\/\*\{\}\+\$\^\:])/g, '\$1');
	var re = new RegExp('[' + charlist + ']+$', 'g');
	return this.replace(re, '');
}

String.prototype.trim = function(charlist)
{
	var str = this.ltrim(charlist);
	return str.rtrim(charlist);
}

function EventBase()
{
	this.callbacks_ = new Array();
}

EventBase.prototype.callbacks_ = null;

EventBase.prototype.Bind = function(event_name, callback)
{
	if (typeof this.callbacks_[event_name] == 'undefined')
	{
		this.callbacks_[event_name] = new Array();
	}
	
	this.callbacks_[event_name].push(callback);
}

EventBase.prototype.Trigger = function(event_name)
{
	if (typeof this.callbacks_[event_name] != 'undefined')
	{
		for (var n = 0; n < this.callbacks_[event_name].length; n++)
		{
			this.callbacks_[event_name][n].apply(null, Array.prototype.slice.call(arguments, 0));
		}
	}
}
