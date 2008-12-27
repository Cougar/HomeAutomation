
function OnlinePhonebook(callback)
{
	this.myHTTP = new HTTP();
	this.myCallback = callback;
}

OnlinePhonebook.prototype.myHTTP = null;
OnlinePhonebook.prototype.myCallback = null;
OnlinePhonebook.prototype.myPhonenumber;

OnlinePhonebook.prototype.lookup = function(phonenumber)
{
	var self = this;

	this.myPhonenumber = phonenumber;

	this.myHTTP.request(function(result, header, content) { self.httpCallback(result, header, content); }, "mobil.hitta.se/default.aspx?Who=" + phonenumber + "&Where=&PageAction=White");
}

OnlinePhonebook.prototype.httpCallback = function(result, header, content)
{
	var persons = new Array();

	if (result.indexOf("200 OK") != -1)
	{
		var endString = "<anchor>Tillbaka<prev/></anchor>";
		var nameStartString = " title=\"Link\">";
		var nameEndString = "</a>";
	
		var lines = content.split("<br/>");
		
		for (var n = 1; n < lines.length; n++)
		{
			var line = lines[n].trim(" \n");
			
			if (line.indexOf(endString) != -1)
			{
				break;
			}
		
			var pos = line.indexOf(nameStartString + (persons.length+1) + ".");
			
			if (pos != -1)
			{
				pos += nameStartString.length + 2
				persons[persons.length] = line.substr(pos, line.length-pos-nameEndString.length).replace("  ", " ");
			}
		}
	}
	
	this.myCallback(this.myPhonenumber, persons);
}
