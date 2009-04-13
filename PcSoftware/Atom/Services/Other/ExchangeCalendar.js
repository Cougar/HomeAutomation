
function ExchangeCalendar(callback)
{
	this.myHTTP = new HTTP();
	this.myCallback = callback;
}

ExchangeCalendar.prototype.myHTTP = null;
ExchangeCalendar.prototype.myCallback = null;
//ExchangeCalendar.prototype.calendarShortName;

ExchangeCalendar.prototype.lookup = function(calendarShortName)
{
	var self = this;

	//this.calendarShortName = calendarShortName;

	this.myHTTP.request(function(result, header, content) { self.httpCallback(result, header, content); }, "dev/exchangeIntegration/exchange.php?function=getMeetingsRestOfDay&shortname=" + calendarShortName);
}

ExchangeCalendar.prototype.httpCallback = function(result, header, content)
{
	var data = null;
	var shortname = "";
	if (result.indexOf("200 OK") != -1)
	{
		var data = eval(content);
		shortname = data['shortname'];
	}
	else
	{
		log("ExchangeCalendar: Failed to do calendar lookup, result was: " + result + "\n");
	}
	
	this.myCallback(shortname, data);
}
