#include <stdio.h>
#include <string.h>

#include "cmd.h"

int main(int argc, char **argv) {
	int i;
	char cmd[32];

	/********************************************
	 *	Get command
	 *******************************************/
	if (argc>=2) {
		strncpy(cmd,argv[1],32);
	} else {
		cmd_nocommand();
		return 0;
	}

	/********************************************
	 *	Command processor
	 *******************************************/
	for (i=0;i<commands_count;i++) {
		if (strcmp(commands[i].cmd,cmd)!=0) continue;
		if (argc-2!=commands[i].count_argument) {
			cmd_list_arg(&(argv[1]));
			return -1;
		}
		if (commands[i].function==NULL) {
			fprintf(stderr,"Function not executed.\n");
			return -1;
		}
		return (*commands[i].function)(&(argv[2]));
	}

	fprintf(stderr,"Command not found.\n");
	return -1;
	
}

