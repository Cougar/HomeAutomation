
RequireModule("irReceive");
RequireModule("irTransmit");
RequireModule("Dimmer230");

var dimmer_module = GetModule("Dimmer230");
var ir_module = GetModule("irReceive");

function test_press(event, id, channel, data, protocol)
{
	Log("Pressed: protocol=" + protocol + ", channel=" + channel + ", data=" + data);
	
	if (protocol == "NEC" && data == 3877174275)
	{
		Dimmer230_StartFade(2, 0, 135, "Increase");
		Dimmer230_StartFade(1, 0, 135, "Increase");
	}
	else if (protocol == "NEC" && data == 3860462595)
	{
		Dimmer230_StartFade(2, 0, 135, "Decrease");
		Dimmer230_StartFade(1, 0, 135, "Decrease");
	}
	else if (protocol == "NEC" && data == 4144561155)
	{
		irTransmit_SendCode(1, 0, "Burst", "NEC", 3843720570);
	}
	else if (protocol == "NEC" && data == 4211407875)
	{
		irTransmit_SendCode(1, 0, "Burst", "NEC", 3827008890);
	}
}

function test_release(event, id, channel, data, protocol)
{
	Log("Released: protocol=" + protocol + ", channel=" + channel + ", data=" + data);
	
	if (protocol == "NEC" && (data == 3877174275 || data == 3860462595))
	{
		Dimmer230_StopFade(2, 0);
		Dimmer230_StopFade(1, 0);
	}
	else if (protocol == "NEC" && (data == 4144561155 || data == 4211407875))
	{
		//irTransmit_SendCode(1, 0, "Released", "NEC", data);
	}
}

function test_status(event, id, available)
{
	//Log("status change!");
}

ir_module.Bind("onStatusChange", test_status);
ir_module.Bind("onPressed", test_press);
ir_module.Bind("onReleased", test_release);





function media(mode)
{
	if (!mode)
	{
		return "You must specify a mode";
	}
	
	if (mode == "movie")
	{
		irsend("tvbankut", "Yamaha_RAV34", "POWER");
		
		fadeto("bokhylla", 100, 135);
		fadeto("bardisk", 0, 135);

		sleep(1000);
		irsend("tvbankut", "Yamaha_RAV34", "CD");
	}
	else if (mode == "tv")
	{
		irsend("tvbankut", "Yamaha_RAV34", "POWER");
		
		fadeto("bokhylla", 150, 135);
		fadeto("bardisk", 150, 135);

		sleep(1000);
		irsend("tvbankut", "Yamaha_RAV34", "DTV_CBL");
	}
	else if (mode == "off")
	{
		irsend("tvbankut", "Yamaha_RAV34", "STANDBY");
		
		fadeto("bokhylla", 0, 135);
		fadeto("bardisk", 0, 135);
	}
	
	return "OK";
}
RegisterConsoleCommand(media, function(args) { return StandardAutocomplete(args, [ "off", "tv", "movie" ]); });
