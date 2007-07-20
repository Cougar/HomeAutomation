#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "ihex.h"
#include "actions.h"
#include "interface.h"
#include <can.h>

unsigned char buf[8192];
long start_addr = 0, len = 0;

/* Flag set by `--verbose'. */
static int verbosity;

int (*actionfuncs[5])(actions_t* action) ;

#define MAXACTIONS 16

actions_t actions[MAXACTIONS+1];

const char* usage = "Usage:\n\n"
					"canman <options> <actions>\n\n"
					"  -v\n"
					"  --verbose               Increase verbosity\n\n"
					"  -q\n"
					"  --quiet                 Decrease verbosity\n\n"
					"  -i <interface>\n"
					"  --interface=<interface> Select canManager interface\n\n"
					"  -p <device>\n"
					"  --port=<device>         Set interface port\n"
					;

char* const default_port = "/dev/ttyS0";

int main (int argc, char* argv[]) {
	int c;
	int n = 0;
	interface_t interface = unset;
	int node_id = 0;
	char* port = default_port;
	Can_Message_t txMsg;
	txMsg.DataLength = 3;
	
	actionfuncs[upload] = do_upload;
	actionfuncs[reset] = do_reset;
	actionfuncs[start] = do_start;
	actionfuncs[dump] = do_dump;
	actionfuncs[send] = do_send;
	
	while (1) {
		static struct option long_options[] =
		{
			{"verbose",   no_argument,       0, 'v'},
			{"quiet",     no_argument,       0, 'q'},
			{"interface", required_argument, 0, 'i'},
			{"port",      required_argument, 0, 'p'},
			{"node",      required_argument, 0, 'n'},
			/* These options are actions */
			{"upload", required_argument, 0, 'u'},
			{"reset",  no_argument,       0, 'r'},
			{"start",  no_argument,       0, 's'},
			{"dump",   no_argument,       0, 'd'},
			{"send",   required_argument, 0, 'x'},
			/* End of options */
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "vqi:p:n:u:rsdx:",
		                 long_options, &option_index);
		
		/* Detect the end of the options. */
		if (c == -1)
			break;
		
		switch (c) {
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				//printf ("option %s", long_options[option_index].name);
				//if (optarg)
				//	printf (" with arg %s", optarg);
				//printf ("\n");
				break;
				
			case 'v': // --verbose
				verbosity++;
				break;
				
			case 'q': // --quiet
				verbosity--;
				break;
				
			case 'i': // --interface <type>
				if (interface == unset)
					if (strcasecmp(optarg, "udp") == 0)
						interface = udp;
					else if (strcasecmp(optarg, "can2serial") == 0)
						interface = can2serial;
					else {
						fprintf (stderr, "interface %s not supported.\n", optarg);
						exit (0);
					}
				else {
					fprintf (stderr, "multiple -i options not allowed.\n");
					exit (0);
				}
				break;
				
			case 'p': // --port <device>
				port = optarg;
				break;
				
			case 'n': // --node <id>
				sscanf (optarg, "%i", &node_id);
				break;
				
			case 'u': // --upload <hexfile>
			case 'r': // --reset
			case 's': // --start
			case 'd': // --dump
			case 'x': // --send
				if (node_id>0)
					actions[n].node_id = node_id;
				else {
					puts ("no node id selected for action");
					exit (1);
				}
				
				actions[n].parameter = optarg; // null if not used

				if (c=='u')
					actions[n].action = upload;
				else if (c=='r')
					actions[n].action = reset;
				else if (c=='s')
					actions[n].action = start;
				else if (c=='d')
					actions[n].action = dump;
				else if (c=='x')
					actions[n].action = send;
					
				if (n++ >= MAXACTIONS) {
					puts ("too many actions specified");
					exit (1);
				}
				break;
				
			case '?':
				/* getopt_long already printed an error message. */
				puts (usage);
				exit (1);
				break;
				
			default:
				abort ();
		}
	}
	
	/* Print any remaining command line arguments (not options). */
	if (optind < argc) {
		printf ("invalid argument: %s\n", argv[optind]);
		exit (1);
	}
	printf ("interface = %d, port = %s\n", interface, port);
	int i;
	for (i=0; i<n; i++) {
		printf ("node = %d, action = %d, parameter = %s\n",
				actions[i].node_id,
				actions[i].action,
				actions[i].parameter);
		actionfuncs[actions[i].action](&actions[i]);
	}
	exit (0);
	
}
