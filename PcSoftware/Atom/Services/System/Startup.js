
/* Load important scripts */
loadScript("System/Interval.js");
loadScript("System/Socket.js");
loadScript("System/HTTP.js");
loadScript("System/Service.js");
loadScript("System/ServiceManager.js");
loadScript("System/CanMessage.js");
loadScript("System/CanNMTMessage.js");
loadScript("System/CanManager.js");
loadScript("System/CanService.js");
loadScript("System/CanNode.js");
loadScript("System/Console.js");
loadScript("System/DataStore.js");
loadScript("System/TextTable.js");
loadScript("System/IntelHex.js");
loadScript("System/Base64.js");

loadScript("Other/OnlinePhonebook.js");
loadScript("Other/Spotify.js");
loadScript("Other/ExchangeCalendar.js");
loadScript("Other/EggdropIRCpost.js");

loadScript("System/TempSensors.js");
loadScript("System/Dimmers.js");

/* This function is called when the virtual machine is up and running.
   Put things that needs to start here */
function startup()
{
	if (typeof autostart != "undefined")
	{
		autostart();
	}

	CanOfflineTimer.start();
}
