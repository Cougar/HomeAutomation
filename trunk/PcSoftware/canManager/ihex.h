#ifndef IHEX_H_
#define IHEX_H_

int ihexInit(int file);
int ihexLoadSegment(unsigned char* buf, long* start_addr, long* len);
int ihexLoadFile(char* file, unsigned char* buf, long* start_addr, long* len);

#endif /*IHEX_H_*/
