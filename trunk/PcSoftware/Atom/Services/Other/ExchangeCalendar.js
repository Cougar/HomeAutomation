
function ExchangeCalendar(callback)
{
	this.myHTTP = new HTTP();
	this.myCallback = callback;
}

ExchangeCalendar.prototype.myHTTP = null;
ExchangeCalendar.prototype.myCallback = null;

ExchangeCalendar.prototype.lookup = function(calendarShortName)
{
	var self = this;
//log("\n");
//log("ExchangeCalendar: before lookup\n");
	this.myHTTP.request(function(result, header, content) { self.httpCallback(result, header, content); }, 
						"dev.qrtech.se/exchangeIntegration/exchange.php?function=getMeetingsRestOfDay&shortname=" + calendarShortName);
//log("ExchangeCalendar: called lookup\n");
}

ExchangeCalendar.prototype.book = function(calendarShortName, bookFrom, bookTo)
{
	var self = this;
	this.myHTTP.request(function(result, header, content) { self.httpCallback(result, header, content); }, 
						"dev.qrtech.se/exchangeIntegration/exchange.php?function=bookMeeting&shortname="+calendarShortName+"&from="+bookFrom+"&to="+bookTo);
}

ExchangeCalendar.prototype.httpCallback = function(result, header, content)
{
//log("ExchangeCalendar: httpCallback \n\n");
//return;
	var data = null;
	var shortname = "";
	if (result.indexOf("200 OK") != -1)
	{
		//log("ExchangeCalendar: got data : \n" + content + "\n");
		/*var data = !(/[^,:{}\[\]0-9.\-+Eaeflnr-u \n\r\t]/.test(
		content.replace(/"(\\.|[^"\\])*"/g, ''))) &&
		eval('(' + content + ')');
		*/
		var data = eval('(' + content + ')');
		
		shortname = data['shortname'];
		//log("ExchangeCalendar: did eval go right? \n"+data.meetings[0]["organizer"]+"\n");
	}
	else
	{
		log("ExchangeCalendar: Failed to do calendar lookup, result was: " + result + "\n");
	}
	
	this.myCallback(shortname, data);
}

