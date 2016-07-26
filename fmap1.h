#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "tinyxml/tinyxml.h"

//#define DEBUG 1

struct entry_type {
	const char *name;
	int size;
};

struct ioentry {
	ioentry *next;
	struct entry_type *type; // type (0-none, 1-bit, 8-octet)
	char name[32]; // name
	long bit;
	long octet;
};

struct iogroup {
	iogroup *next;
	char name[32]; // name
	char description[64]; // descriprion
	ioentry *ioentry_list;
};

class fmap1 
{
private:
	// Парметры FMAP1
	char device[64];
	long offset;
	long size;

	// Список параметров
	iogroup *iogroup_list;
	// Список типов данных
	 
	struct entry_type entry_type_array[12];
	int entry_type_count;
	
	// Вспомогательные функции чтения данных из XML
	int read_str(TiXmlHandle xml, const char *name, char *val, int size);
	int read_long(TiXmlHandle xml, const char *name, long *val);
	int read_ioentry (TiXmlHandle xml, ioentry *entry);
	int read_iogroup (TiXmlHandle xml, iogroup *group);

	// Вспомогательные функции вычисления и проверки контрольной суммы
	int calculate_verify_code(char *block_data);
	int check_verify_code(char *block_data);

	// Вспомогательные функции чтения данных описанных в XML
	int read_entry(ioentry *ioentry_ptr, long *result);
	int write_entry(ioentry *ioentry_ptr, long value);
public: 
	// Функции инициализации
	fmap1(const char *xmlfile);
        int init(void);
	int load(const char *xmlfile);
	// Отображение конфигурации
	int show_entrytypes (void);
	int show_fmap(void);
	int show_iomap(void);
	// Функции низкоуровневого блочного чтения
	int octet_read(int octet, char *data);
	int octet_write(int octet, char data);
//	int block_read(int block, char *data);
//	int block_write(int block, char *data);
	// Функции чтения октетов через блоки
//	int raw_read(long addr, char *buf);
//	int raw_write(long addr, char buf);
	// Функции чтения параметров описанных в XML
	int show_group (char *name_group);
	int xread (char *name_entry, long *value);
	int xwrite (char *name_entry, long value);

};
