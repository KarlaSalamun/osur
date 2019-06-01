#include "fs.h"
#include "device.h"
#include "memory.h"
#include <api/stdio.h>

list_t fs;

extern device_t _disk;

char bitmap[4096/8];
//unsigned int start_addr;
int id = 10;
int cnt = 0;

void init_fs (void) 
{
	list_init ( &fs );
	
	for (int i=0; i<sizeof(bitmap); i++ ) 
	{	
		bitmap[i] = 0xff;
	}

	bitmap[0] = 0x00;
	bitmap[1] = 0x00;

//	start_addr = (unsigned int) disk.params;
}


file_t *fopen ( char *name, int flags ) 
{
	file_t *file;
	ssize_t size;
	cnt++;

//	file = list_get ( &fs, FIRST );
	
/*	
	while(file) 
	{
		if ( !strcmp ( name, file->name ) ) 
		{
			return file;
			break;
		}
		file = list_get_next ( &file->list );
	}	
*/
	size = cnt<<16;
	size |= 1;

	

	if ( flags & O_CREAT )  { file = kmalloc ( sizeof ( file_t ) ); memcpy (
	file->name, name, strlen(name) ); file->size = 0; file->flags = flags;
	file->id = id; id++; 
	//list_init ( file->list ); 
	 int status = k_device_send ( file, (cnt <<16) | 1,  0, &_disk); 
	 if ( !status ) { return NULL; }
		
	//	file->block = start_addr/512;
	//	start_addr += 512;
	}

	/* TODO: update ostale zastavice */

	return file;	
}

ssize_t file_read ( void *buffer, size_t size, file_t *file ) 
{
	//file_t *file;
	/*
	for ( int i = 0; i<16; i++ ) 
	{
		disk.recv(&file, i<<16 | 0xffff, 0, &disk);
		if ( file->id == id ) 
			break;
	}
	*/
	//return read ( 3, buffer, (file->block << 16) | 1);
	//return disk.recv(&buffer, file->block << 16 | size, 0, &disk);
	return k_device_recv ( buffer, (file->block << 16) | 1, 0, &_disk );		
}

ssize_t file_write ( void *buffer, size_t size, file_t *file ) 
{
	int retval;	

	//file_t *file;
	/*
	for ( int i = 0; i<16; i++ ) 
	{
		disk.recv(&file, i<<16 | size, 0, &disk);
		if ( file->id == id ) 
			break;
	}
	*/
	//retval = write ( 3, buffer, (file->block << 16) | 1 );
	retval = k_device_send ( buffer, (file->block << 16) | 1,  0, &_disk);
	//retval = disk.send(&buffer, file->block << 16 | size, 0, &disk );
	//file->block++;
	return retval;
}
	
	
	
