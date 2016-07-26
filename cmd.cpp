
#include "cmd.h"


cmd_arg commands[] = {
	{"help",	cmd_help,	0,	{},	"Show help"},
	{"list_cmd",    cmd_list_cmd,   0,      {},     "Show list commands"},
	{"list_arg",    cmd_list_arg,   1,      {"cmd"},        "Show list arguments cmd"},
	{"version",	cmd_version,	0,	{},	"Show version"},

	{"show_entry_types",	cmd_show_entry_types,	0,	{},	"Show table supported entry types"},

	{"fmap",	cmd_fmap,		0,	{},			"Show info organisation fmap1"},
	{"group",	cmd_group,		1,	{"group_name"},			"Read all entries from group"},

	{"octet_read",	cmd_octet_read,		1,	{"octet"},		"read one octet data"},
	{"octet_write",	cmd_octet_write,	2,	{"octet","value"},	"write one octet data"},

	{"read",	cmd_read,		1,	{"entry_name"},			"Read entry (output only value; need for scripts)"},
	{"write",	cmd_write,		2,	{"entry_name", "value"},	"Write entry (need for scripts)"},


};

fmap1 fpga("fmap1.xml");

int cmd_nocommand() {
	cmd_help(NULL);
	return 0;
};

int cmd_version(char **argv) {
	int i;

	printf("%s v%s\n",prg_name,prg_version);

	return 0;
}



int cmd_help(char **argv) {
	int i;

	printf(" %s \n",prg_copyright);
	printf("Autor: %s \n",prg_autors);
	printf("%s v%s\n",prg_name,prg_version);
	printf("\nUsage: %s <cmd> \n",prg_name);
	printf("Example: %s list_cmd\n",prg_name);
	printf("\n %s \n",prg_description);

	printf("\n Commands:\n");
	cmd_list_cmd(argv);

	return 0;

}

int cmd_list_arg(char **argv) {
	char cmd[32];
	int i,n;

	strcpy(cmd,argv[0]);
	for (i=0; i<commands_count; i++) {
		if (strcmp(commands[i].cmd,cmd)!=0) continue;
		printf("Usage: %s %s ",prg_name,cmd);
		for (n=0;n<commands[i].count_argument;n++) {
			printf("<%s> ",commands[i].arguments[n]);
		}
		printf("\n");
		return 0;
	}
	fprintf(stderr,"Command not found.");

	return -1;
	
}


int cmd_list_cmd(char **argv) {
	int i;

	for (i=0;i<commands_count;i++) {
		printf("%s - %s \n",commands[i].cmd,commands[i].description);
	}

	return 0;
}

int cmd_show_entry_types(char **argv) {
	fpga.show_entrytypes();
	return 0;
}

int cmd_fmap(char **argv) {

	printf("*****************************************************************\n"	\
	       " FMAP1 \n"	\
	       "*****************************************************************\n");
	fpga.show_fmap();
	printf("*****************************************************************\n");
	fpga.show_iomap();
	return 0;

}

int cmd_octet_read(char **argv) {
	char data;
	int i, ret;
	int octet_id;

	sscanf(argv[0],"%d",&octet_id);

	ret = fpga.octet_read(octet_id, &data);
	if (ret!=0) {
		printf("Error read (%d)\n", ret);
		return ret;
	}

	printf("octet %d = 0x%02hhx \n", octet_id, data);

	return 0;

}

int cmd_octet_write(char **argv) {
	char data;
	int ret;
	int octet_id;

	sscanf(argv[0], "%d", &octet_id);
	sscanf(argv[1], "%hhx", &data);

	ret = fpga.octet_write(octet_id, data);
	if (ret!=0) {
		printf("Error write (%d)\n", ret);
		return ret;
	}

	printf("write to octet %d OK\n", octet_id);

	return 0;

}

int cmd_group(char **argv) {
	int res;
	res = fpga.show_group(argv[0]);
	return res;
}

int cmd_read(char **argv) {
	long value;
	int res;

	res = fpga.xread(argv[0], &value);
	if (res==0) {
		printf("%ld\n", value);
	} else {
		printf("ERROR\n");
	}
	return res;

}
int cmd_write(char **argv) {
	long value;
	int res;

	sscanf(argv[1], "%ld", &value);
	res = fpga.xwrite(argv[0], value);
	if (res==0) {
		printf("OK\n");
	} else {
		printf("ERROR\n");
	}
	return res;
}

