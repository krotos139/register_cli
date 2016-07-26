#include "fmap1.h"

int fmap1::read_str (TiXmlHandle xml, const char *name, char *val, int size) {
	TiXmlElement* pElem;

	pElem = xml.FirstChild( name ).Element();
	if (pElem!=NULL) {
		snprintf(val, size, "%s", pElem->GetText());
//		sscanf(pElem->GetText(),"%s",val);
		return 0;
	} else {
#ifdef DEBUG
		printf("ERROR load string value \"%s\" \n",name);
#endif
		return -2;
	}
	return 0;
}
int fmap1::read_long (TiXmlHandle xml, const char *name, long *val) {
	TiXmlElement* pElem;

	pElem = xml.FirstChild( name ).Element();
	if (pElem!=NULL) {
		sscanf(pElem->GetText(),"%ld",val);
		return 0;
	} else {
#ifdef DEBUG
		printf("ERROR load long value \"%s\" \n",name);
#endif
		return -2;
	}
	return 0;
}
int fmap1::read_ioentry (TiXmlHandle xml, ioentry *ioentry_ptr) {
	int ret,i;
	char type[32];

	ret = read_str(xml, "name", ioentry_ptr->name, 31);
	if (ret!=0) return ret;
	ret = read_long(xml, "bit", &ioentry_ptr->bit);
	if (ret!=0) {
		ioentry_ptr->bit = 0;
	//	return ret-10;
	}
	ret = read_long(xml, "octet", &ioentry_ptr->octet);
	if (ret!=0) return ret-20;
	ret = read_str(xml, "type", type, 31);
	if (ret!=0) return ret-30;
	for (i=0 ; i<this->entry_type_count ; i++) 
		if (strcmp(type , entry_type_array[i].name)==0) {
			ioentry_ptr->type = entry_type_array + i;
			return 0;
		}
	return -30;
}
int fmap1::read_iogroup (TiXmlHandle xml, iogroup *iogroup_ptr) {
	int ret;
	char type[32];
	TiXmlElement* xmlentry;
	ioentry *ioentry_ptr;

	ret = read_str(xml, "name", iogroup_ptr->name, 31);
	if (ret!=0) return ret;
	ret = read_str(xml, "description", iogroup_ptr->description, 63);
	if (ret!=0) return ret-10;

	iogroup_ptr->ioentry_list = NULL;
	xmlentry = xml.FirstChild( "entry" ).ToElement();
	for( xmlentry; xmlentry; xmlentry=xmlentry->NextSiblingElement() )
	{
		// Связывание в список
		ioentry_ptr = new ioentry;
		if (iogroup_ptr->ioentry_list == NULL) {
			ioentry_ptr->next = NULL;
		} else {
			ioentry_ptr->next = iogroup_ptr->ioentry_list;
		}
		iogroup_ptr->ioentry_list = ioentry_ptr;
		// Заполнение записи списка
		read_ioentry(TiXmlHandle(xmlentry), ioentry_ptr);
	}

	return 0;
}


fmap1::fmap1 (const char *xmlfile) {
	int ret;
	this->init();
	ret = this->load(xmlfile);
	if (ret!=0) {
		printf("ERROR: load (%d) \n" , ret);
		return;
	}
}

int fmap1::init (void) {
	int i;

	strcpy(this->device,"./fmap.bin");
	this->offset =0;
	this->size=255;

	this->iogroup_list = NULL;

/*	this->entry_type_array[0].name = "none";
	this->entry_type_array[0].size = -1;*/

	this->entry_type_array = {
		{"none" , 0},
		{"bit", 1},
		{"nibble", 4},
		{"octet",8},
		{"1b",8},
		{"2b",16},
		{"4b",32}
	};
	entry_type_count =7;

	return 0;
}

int fmap1::load (const char *xmlfile) {

	int group_id, entry_id;
	TiXmlElement* xmlgroup;
	iogroup *iogroup_ptr;


	TiXmlDocument doc(xmlfile);
	if (!doc.LoadFile()) return -1;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle h_fmap(0);
	TiXmlHandle h_io(0);
	TiXmlHandle h_proc(0);

	h_fmap=TiXmlHandle(hDoc.FirstChild( "fmap" ).Element());
	h_io=TiXmlHandle(hDoc.FirstChild( "io" ).Element());
	h_proc=TiXmlHandle(hDoc.FirstChild( "proc" ).Element());
	// Загрузка секции fmap с параметрами карты памяти
	read_str(h_fmap, "device", this->device, 63);
	read_long(h_fmap, "offset", &this->offset);
	read_long(h_fmap, "size", &this->size);
	// Загрузка секции io с параметрами полей в памяти
	iogroup_list = NULL;
	xmlgroup = h_io.FirstChild( "group" ).ToElement();
	for( xmlgroup; xmlgroup; xmlgroup=xmlgroup->NextSiblingElement() )
	{
		// Связывание в список
		iogroup_ptr = new iogroup;
		if (iogroup_list == NULL) {
			iogroup_ptr->next = NULL;
		} else {
			iogroup_ptr->next = iogroup_list;
		}
		iogroup_list = iogroup_ptr;
		// Заполнение записи списка
		read_iogroup(TiXmlHandle(xmlgroup), iogroup_ptr);
	}


	return 0;
}

int fmap1::show_entrytypes (void) {
	int i;

	printf("Entry types:\n");
	printf(" %3s | %10s | %10s \n", "#", "name", "size");
	printf("------------------------------\n");
	for (i=0 ; i<this->entry_type_count ; i++) 
		printf(" %3d | %10s | %10d \n", i, entry_type_array[i].name, entry_type_array[i].size);
	printf("\n");

	return 0;
}

int fmap1::show_fmap (void) {
	printf("Flat MAP config:\n");
	printf("file device = %s \n", this->device);
	printf("Address : %ld .. %ld (%ld octets)\n",this->offset, this->offset + this->size-1, this->size);
	printf("\n");
	return 0;
}

int fmap1::show_iomap () {
	int group_id,entry_id;
	iogroup *iogroup_ptr;
	ioentry *ioentry_ptr;

	printf("Flat MAP parametrs:\n\n");
	
	group_id = 1;
	iogroup_ptr = iogroup_list;
	for (iogroup_ptr ; iogroup_ptr ; iogroup_ptr = iogroup_ptr->next ) {
		printf(" Group %d \"%s\"  \n" , group_id , iogroup_ptr->name );
		printf(" %s  \n" , iogroup_ptr->description );
		if (iogroup_ptr->ioentry_list==NULL) {
			printf("group in empty\n");
			continue;
		}

		entry_id = 1;
		ioentry_ptr = iogroup_ptr->ioentry_list;
		for (ioentry_ptr ; ioentry_ptr ; ioentry_ptr = ioentry_ptr->next ) {
			printf("entry %d \"%s\" octet=0x%lx bit=0x%lx size=%d bit \n" , 
					entry_id , 
					ioentry_ptr->name , 
					ioentry_ptr->octet ,
					ioentry_ptr->bit ,
					ioentry_ptr->type->size );
			entry_id++;
		}
		printf("----------------------------------------------------------------\n");
		group_id++;
	}
}

int fmap1::show_group (char *name_group) {
	iogroup *iogroup_ptr;
	ioentry *ioentry_ptr;
	long value;
	int res;

	iogroup_ptr = iogroup_list;
	for (iogroup_ptr ; iogroup_ptr ; iogroup_ptr = iogroup_ptr->next ) {
		if (strcmp(iogroup_ptr->name,name_group)!=0) continue;
		printf(" Group \"%s\"  \n" , iogroup_ptr->name );
		printf(" %s  \n" , iogroup_ptr->description );

		if (iogroup_ptr->ioentry_list==NULL) {
			printf("group in empty\n");
			continue;
		}

		ioentry_ptr = iogroup_ptr->ioentry_list;
		for (ioentry_ptr ; ioentry_ptr ; ioentry_ptr = ioentry_ptr->next ) {
			res = read_entry(ioentry_ptr, &value);
			if (res==0) {
				printf("entry \"%s\" octet=%ld bit=%ld value=%ld \n" , 
						ioentry_ptr->name , 
						ioentry_ptr->octet , 
						ioentry_ptr->bit ,
						value );
			} else {
				fprintf(stderr, "error read entry \"%s\" \n" , ioentry_ptr->name);
			}
		}
		return 0;
	}
	return -1;
}

int fmap1::xread (char *name_entry, long *value) {
	iogroup *iogroup_ptr;
	ioentry *ioentry_ptr;
//	long value;
	int res;

	iogroup_ptr = iogroup_list;
	for (iogroup_ptr ; iogroup_ptr ; iogroup_ptr = iogroup_ptr->next ) {

		ioentry_ptr = iogroup_ptr->ioentry_list;
		for (ioentry_ptr ; ioentry_ptr ; ioentry_ptr = ioentry_ptr->next ) {
			if (strcmp(ioentry_ptr->name,name_entry)!=0) continue;

			res = read_entry(ioentry_ptr, value);
			if (res!=0) {
				fprintf(stderr, "Error read_entry %d \n",res);
				return -4;
			}

//			printf("%ld\n" , value );
			return 0;
		}
	}
	fprintf(stderr, "Error entry not found \n");
	return -1;
}
int fmap1::xwrite (char *name_entry, long value) {
	iogroup *iogroup_ptr;
	ioentry *ioentry_ptr;
	int res;

	iogroup_ptr = iogroup_list;
	for (iogroup_ptr ; iogroup_ptr ; iogroup_ptr = iogroup_ptr->next ) {

		ioentry_ptr = iogroup_ptr->ioentry_list;
		for (ioentry_ptr ; ioentry_ptr ; ioentry_ptr = ioentry_ptr->next ) {
			if (strcmp(ioentry_ptr->name,name_entry)!=0) continue;
			res = write_entry(ioentry_ptr, value);
			if (res!=0) {
				fprintf(stderr, "Error write_entry %d \n",res);
				return -4;
			}

//			printf("Ok\n");
			return 0;
		}
	}
	fprintf(stderr, "Error entry not found \n");
	return -1;
}


int fmap1::octet_read(int octet, char *data) {
/*	FILE * pFile;
	int res; */

	int ret;

	int fd;
	mode_t mode = S_IRUSR | S_IWUSR;
	int flags = O_RDONLY ;

#ifdef DEBUG
	printf("DBG: octet_read: octet=0x%x \n",octet);
#endif
//	if (octet > (this->offset + this->size)) return -1;

	fd = open (this->device, flags, mode);
	if (fd < 0) return 1;
	
	ret = lseek (fd, octet, SEEK_SET);
	if (ret != octet) return 2;

	ret = read (fd, data, 1);
	if (ret != 1) return 3;

	ret = close (fd);
	if (ret != 0) return 4;

/*
	pFile = fopen ( this->device , "r" );
	if (pFile==NULL) return -2;

	fseek (pFile , octet , SEEK_SET);

	res = fread (data, 1, 1, pFile);
	if (res != 1) return -3;

	fclose (pFile);
*/
	return 0;
}

int fmap1::octet_write(int octet, char data) {
/*	FILE * pFile;
	int res; */

	int ret;

	int fd;
	mode_t mode = S_IRUSR | S_IWUSR;
	int flags = O_WRONLY ;

#ifdef DEBUG 
	printf("DBG: block_write: octet=0x%x data=0x%x\n", octet, data);
#endif

//	if (octet > (this->offset + this->size)) return -1;

	fd = open (this->device, flags, mode);
	if (fd < 0) return 1;
	
	ret = lseek (fd, octet, SEEK_SET);
	if (ret != octet) return 2;

	ret = write (fd, &data, 1);
	if (ret != 1) return 3;

	ret = close (fd);
	if (ret != 0) return 4;

/*
	pFile = fopen ( this->device , "r+" );
	if (pFile==NULL) return -2;

	fseek (pFile , octet , SEEK_SET);

	res = fwrite (&data, 1, 1, pFile);
	if (res != 1) return -3;
	
	fclose (pFile);
*/
	return 0;
}

/**
 * чтение 1 параметра по его описанию (Длиной от 0 до 32 бит)
 */
int fmap1::read_entry(ioentry *ioentry_ptr, long *result) {
	char data;
	int i,res;
	long bitmask;
	long rvalue,value;
#ifdef DEBUG
	printf("DBG: read_entry: ioentry{name=%s, octet=0x%lx, bit=0x%lx size=%d}\n",
			ioentry_ptr->name, 
			ioentry_ptr->octet,
			ioentry_ptr->bit,
			ioentry_ptr->type->size);
#endif
	// Проверка на неадекватные параметры
	if (ioentry_ptr->type->size > 32) return -1;
	if (ioentry_ptr->type->size + ioentry_ptr->bit > 32) return -2;
	if (ioentry_ptr->octet > this->size) return -3;
	
	// Вычислим битовую маску
	bitmask = 0;
	for (i=0 ; i<ioentry_ptr->type->size ; i++) {
		bitmask = bitmask << 1;
		bitmask |= 1;
	}
	bitmask = bitmask << ioentry_ptr->bit;
	// Считаем значение
	value = 0;
	for (i=0 ; i <= ioentry_ptr->type->size/8 ; i++) { // читаем несколько бит
		res = this->octet_read(ioentry_ptr->octet+i, &data);
		if (res!=0) return res-10;
		rvalue = data;
		value |= rvalue << 8*i;
	}
	// Получим результат
	*result = (value & bitmask) >> ioentry_ptr->bit;
	return 0;
}

/**
 * чтение 1 параметра по его описанию (Длиной от 0 до 32 бит)
 */
int fmap1::write_entry(ioentry *ioentry_ptr, long value) {
	char data;
	int i,res;
	long bitmask;
	long rvalue,wvalue,devvalue;

	// Проверка на неадекватные параметры
	if (ioentry_ptr->type->size > 32) return -1;
	if (ioentry_ptr->type->size + ioentry_ptr->bit > 32) return -2;
	if (ioentry_ptr->octet > this->size) return -3;
	
	// Вычислим битовую маску
	bitmask = 0;
	for (i=0 ; i<ioentry_ptr->type->size ; i++) {
		bitmask = bitmask << 1;
		bitmask |= 1;
	}
	bitmask = bitmask << ioentry_ptr->bit;
	// Считаем значение
	devvalue = 0;
	for (i=0 ; i <= ioentry_ptr->type->size/8 ; i++) { // читаем несколько бит
		res = this->octet_read(ioentry_ptr->octet+i, &data);
		if (res!=0) {
			fprintf(stderr, "Error raw_read (%d)\n", res);
			return res-10;
		}
		rvalue = data;
		devvalue |= rvalue << 8*i;
	}
	// Вычислим новое значение	
	wvalue = (devvalue & (~bitmask)) | (( value << ioentry_ptr->bit ) & bitmask);
#ifdef DEBUG
	printf("0x%lx = (0x%lx & (~0x%lx)) | ((0x%lx << 0x%lx) & 0x%lx)\n",
			wvalue, devvalue, bitmask, value, ioentry_ptr->bit, bitmask);
#endif
	// Записываем новое значение
	for (i=0 ; i <= ioentry_ptr->type->size/8 ; i++) { // читаем несколько бит
		data = (wvalue >> 8*i) & 0xFF;
		res = this->octet_write(ioentry_ptr->octet+i, data);
		if (res!=0) {
			fprintf(stderr, "Error raw_write (%d)\n", res);
			return res-20;
		}
	}
	return 0;
}




