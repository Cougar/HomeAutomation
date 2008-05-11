#ifndef ACTIONS_H_
#define ACTIONS_H_

#include "interface.h"

typedef enum {
	upload,
	reset,
	start,
	dump,
	send
} action_t;

typedef struct {
	int node_id;
	action_t action;
	char* parameter;
} actions_t;

int do_upload(actions_t* action);
int do_reset(actions_t* action);
int do_start(actions_t* action);
int do_dump(actions_t* action);
int do_send(actions_t* action);

#endif /*ACTIONS_H_*/
