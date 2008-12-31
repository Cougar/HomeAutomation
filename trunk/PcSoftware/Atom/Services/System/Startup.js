
/* Load important scripts */
loadScript("System/Interval.js");
loadScript("System/Socket.js");
loadScript("System/HTTP.js");
loadScript("System/Service.js");
loadScript("System/ServiceManager.js");
loadScript("System/CanMessage.js");
loadScript("System/CanManager.js");
loadScript("System/CanService.js");

loadScript("Other/OnlinePhonebook.js");

/* This function is called when the virtual machine is up and running.
   Put things that needs to start here */
function startup()
{
	if (autostart)
	{
		autostart();
	}

	startOfflineCheck();
}