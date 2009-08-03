#define THIS_IS_SCHEDULER

#include "..\Include\Helpers.h"
#include "..\Include\UART.h"
#include "..\Include\StackTsk.h"
#include "..\Include\TimeSync.h"
#include "..\Include\Scheduler.h"

#if defined(STACK_USE_SCHEDULER)




#define MAX_EVENTS 2

EVENT events[MAX_EVENTS];


void scheduler(void)
{
	DWORD ct;
	BYTE i;

	if (ct=getTime(CURRENT))
	{
		for(i=0;i<MAX_EVENTS;i++)
		{
			if (events[i].active==TRUE && ct>events[i].time)
			{
				setVariable(events[i].what,events[i].value);
				events[i].active=FALSE;
			}
		}
	}

}

void loadEvents(void)
{
	events[0].active=TRUE;
	events[0].repeat=0;
	events[0].time=1159170615;
	events[0].what=VAR_LED1;
	events[0].value=1;

	events[1].active=TRUE;
	events[1].repeat=0;
	events[1].time=events[0].time+20;
	events[1].what=VAR_LED1;
	events[1].value=0;

}

EVENT getEvent(BYTE id)
{
	EVENT event;

	if (id<0 || id>(MAX_EVENTS-1)) return event;

	return events[id];
}


BYTE numEvents(void)
{
	return MAX_EVENTS;
}

#endif
