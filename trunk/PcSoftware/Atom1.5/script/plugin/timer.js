
var Timer_Callbacks = {};

function Timer_SetTimeout(callback, milliseconds)
{
	var timer_id = TimerExport_StartTimer(milliseconds, false);
	
	Timer_Callbacks[timer_id] = callback;
	
	return timer_id;
}

function Timer_SetInterval(callback, milliseconds)
{
	var timer_id = TimerExport_StartTimer(milliseconds, true);
	
	Timer_Callbacks[timer_id] = callback;
	
	return timer_id;
}

function Timer_Cancel(timer_id)
{
	delete Timer_Callbacks[timer_id];
	
	TimerExport_ClearTimer(timer_id);
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
