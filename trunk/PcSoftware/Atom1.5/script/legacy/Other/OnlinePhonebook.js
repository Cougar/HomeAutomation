
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

	this.myHTTP.request(function(result, header, content) { self.httpCallback(result, header, content); }, "wap.hitta.se/default.aspx?Who=" + phonenumber + "&Where=&PageAction=White");
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
				persons[persons.length] = line.substr(pos, line.length-pos-nameEndString.length).html_entity_decode().replace("  ", " ");
				log("OnlinePhonebook: Caller name is " + persons[persons.length-1]);
			}
		}
	}
	else
	{
		log("OnlinePhonebook: Failed to do name lookup, result was: " + result + "\n");
	}
/* Uncomment to enable output to file
	// Update main dtmf menu
	var date = new Date();
	// Get the current date time on the format YYYY-mm-dd HH.ii.ss 
	var dateAndTime = date.getDateTimeFormated();
	var filename = "/dev/shm/can/lastPhoneCall";
	var content = getFileContents(filename);
	if (!content) {
		content = "";
	}
	content += dateAndTime +": ";
	content += this.myPhonenumber+"\t";
	var i = 0;
	var separator = "";
	while (persons[i] != null) {
		content += separator + persons[i];
		i++;
		separator = ", ";
	}
	
	content += "\n";
	var filename = "/dev/shm/can/lastPhoneCall";
	setFileContents(filename, content);
*/
	this.myCallback(this.myPhonenumber, persons);
}
