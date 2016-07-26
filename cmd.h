#ifndef _CMD_H_

#include <string.h>
#include <stdio.h>

#include "tinyxml/tinyxml.h"
#include "fmap1.h"

#define P140_SUCCESS	1
#define P140_FAIL	0

#define prg_copyright	"(c) 'SKTB SKIT' http://skitlab.ru/"
#define prg_autors	"Yakovlev U.V. <krotos139@gmail.com>"
#define prg_name	"vim3u3_fpga_cli"
#define prg_version	"0.1"
#define prg_description	"fmap1 client."

//typedef int (*void)(char **args) cmd_func;

typedef struct {
	char		cmd[32];
	int		(*function)(char **argv);
	int		count_argument;
	char		arguments[16][16];
	char		description[256];

} cmd_arg;

#define commands_count 11

extern cmd_arg commands[commands_count];

int cmd_help(char **argv);
int cmd_list_arg(char **argv);
int cmd_version(char **argv);
int cmd_list_cmd(char **argv);

int cmd_show_entry_types(char **argv);

int cmd_fmap(char **argv);
int cmd_octet_read(char **argv);
int cmd_octet_write(char **argv);

int cmd_group(char **argv);
int cmd_read(char **argv);
int cmd_write(char **argv);

int cmd_nocommand(); 


#endif
