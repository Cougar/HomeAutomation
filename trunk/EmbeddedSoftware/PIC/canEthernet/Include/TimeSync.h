

#ifndef TIMESYNC_H
#define TIMESYNC_H

void timeSync(void);

DWORD getTime(timeClock tc);
DWORD getTimeStr(timeClock tc,char *str);

#define MAX_ARP_TRIES 10
#define MAX_TCP_TRIES 10

#define ServerPort 80

#define SYNC_INTERVAL 300

#define CR '\r'
#define LF '\n'

#endif
