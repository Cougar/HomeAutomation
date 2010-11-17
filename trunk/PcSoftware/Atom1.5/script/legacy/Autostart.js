
var d1 = null;
var d2 = null;

function autostart()
{
	ServiceManager.startService("Logic", "SensorPrint", 1, {	HD44789 : { Id : 1 },
									DS18x20 : { Id : 1 },
									BusVoltage : { Id : 2 },
									SimpleDTMF : { Id : 1 } });

	d1 = ServiceManager.getService("Can", "Dimmer230", 1);
	d2 = ServiceManager.getService("Can", "Dimmer230", 2);
}

function dmax()
{
	d1.absFade(0, 130, 255);
	d2.absFade(0, 130, 255);
}

function dmin()
{
	d1.absFade(0, 130, 0);
	d2.absFade(0, 130, 0);
}

