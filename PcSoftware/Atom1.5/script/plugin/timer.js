
var Timer_Callbacks = {};

function Timer_SetTimer(callback, milliseconds, repeat)
{
	var timer_id = TimerExport_SetTimer(milliseconds, repeat);
	
	Timer_Callbacks[timer_id] = callback;
	
	return timer_id;
}

function Timer_SetAlarm(callback, time)
{
	var timer_id = TimerExport_SetAlarm(time);
	
	Timer_Callbacks[timer_id] = callback;
	
	return timer_id;
}

function Timer_Cancel(timer_id)
{
	delete Timer_Callbacks[timer_id];
	
	//TimerExport_Cancel(timer_id);
}

function Timer_OnTimeout(timer_id, repeat)
{
	if (Timer_Callbacks[timer_id])
	{
		Timer_Callbacks[timer_id](timer_id);
		
		if (!repeat)
		{
			delete Timer_Callbacks[timer_id];
		}
	}
}
