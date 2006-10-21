#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <inttypes.h>

#define COMMANDSHELL_MODEINIT   (0)
#define COMMANDSHELL_MODERUN    (1<<0)
#define COMMANDSHELL_MODEEXIT   (1<<1)
#define COMMANDSHELL_MODEMASK   ((1<<0)|(1<<1))
#define COMMANDSHELL_REDRAW (1<<7)

void CommandShell(uint8_t *state);

extern uint8_t gTimedSend;

#endif
