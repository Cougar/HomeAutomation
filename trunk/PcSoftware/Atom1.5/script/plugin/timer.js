
var timer_callbacks = new Array();

function SetTimeout(callback, milliseconds)
{
	var timer_id = StartTimer(milliseconds, false);
	
	timer_callbacks[timer_id] = callback;
	
	return timer_id;
}

function ClearTimeout(timer_id)
{
	return ClearInterval(timer_id);
}

function SetInterval(callback, milliseconds)
{
	var timer_id = StartTimer(milliseconds, true);
	
	timer_callbacks[timer_id] = callback;
	
	return timer_id;
}

function ClearInterval(timer_id)
{
	delete timer_callbacks[timer_id];
	
	ClearTimer(timer_id);
}

function Timer_OnTimeout(timer_id, repeat)
{
	if (timer_callbacks[timer_id])
	{
		timer_callbacks[timer_id]();
		
		if (!repeat)
		{
			delete timer_callbacks[timer_id];
		}
	}
}
