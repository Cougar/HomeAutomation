
/* Load important scripts */
LoadScript("legacy/System/Interval.js");
LoadScript("legacy/System/Socket.js");
LoadScript("legacy/System/HTTP.js");
LoadScript("legacy/System/Service.js");
LoadScript("legacy/System/ServiceManager.js");
LoadScript("legacy/System/CanMessage.js");
LoadScript("legacy/System/CanManager.js");
LoadScript("legacy/System/CanService.js");
LoadScript("legacy/System/Console.js");
LoadScript("legacy/System/DataStore.js");
LoadScript("legacy/System/TextTable.js");
LoadScript("legacy/System/IntelHex.js");
LoadScript("legacy/System/Base64.js");

LoadScript("legacy/Other/OnlinePhonebook.js");
LoadScript("legacy/Other/Spotify.js");
LoadScript("legacy/Other/ExchangeCalendar.js");
LoadScript("legacy/Other/EggdropIRCpost.js");

LoadScript("legacy/System/TempSensors.js");
LoadScript("legacy/System/Dimmers.js");

/* This function is called when the virtual machine is up and running.
   Put things that needs to start here */
function startup()
{
	if (typeof autostart != "undefined")
	{
		autostart();
	}
}

function log(data)
{
	Log(data.trim('\n'));
}

function extend(a, b)
{
	Extend(a, b);
}
