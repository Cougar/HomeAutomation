
function OnlinePhonebook(callback)
{
	this.myHTTP = new HTTP();
	this.myCallback = callback;
}

OnlinePhonebook.prototype.myHTTP = null;
OnlinePhonebook.prototype.myCallback = null;

OnlinePhonebook.prototype.lookup = function(phonenumber)
{
	var self = this;

	this.myHTTP.request(function(result, header, content) { self.httpCallback(result, header, content); }, "mobil.hitta.se/default.aspx?Who=" + phonenumber + "&Where=&PageAction=White");
}

OnlinePhonebook.prototype.httpCallback = function(result, header, content)
{
	var persons = new Array();

	if (result.indexOf("200 OK") != -1)
	{
		///FIXME: parse html
	}
	
	this.myCallback(persons);
}
