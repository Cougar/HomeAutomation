

#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef struct _EVENT
{
	BOOL active;
	DWORD repeat;
	DWORD time;
	BYTE what;
	BYTE value;
} EVENT;

void scheduler(void);

void loadEvents(void);

EVENT getEvent(BYTE id);

BYTE numEvents(void);

#endif



