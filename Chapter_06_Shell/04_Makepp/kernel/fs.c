#include "fs.h"
#include "device.h"
#include "memory.h"
#include <api/stdio.h>
#include <kernel/kprint.h>
#include <types/bits.h>

list_t files;

extern device_t _disk;
unsigned int *bitmap;
//unsigned int bitmap[4096/32];
unsigned int start_addr = 15*512;
int id = 10;
int cnt = 0;

void init_fs (void) 
{
	list_init ( &files );
	bitmap = (unsigned int *) kmalloc( 4096/32 );
	
	for (int i=0; i<128; i++ ) 
	{	
		bitmap[i] = 0xffffffff;
	}

	//bitmap[0] = 0x0;
}


file_t *fopen ( char *name, int flags ) 
{
	file_t *file;
	//ssize_t size;
	cnt++;
	//int i, index;

	name += 5;
	
	if (strlen(name)>=FILE_NAME_LEN + 4)
	{
		memcpy(name, name, FILE_NAME_LEN);
		name[FILE_NAME_LEN] = '\0';
		kprintf("%s\n", name);
		char suffix[] = ".txt\0";
		strcat(name, suffix);
	}

	kprintf("%s\n", name);

	file = list_get ( &files, FIRST );
	
	while(file) 
	{
		if ( !strcmp ( name, file->name ) ) 
		{
			if ( flags & O_CREAT ) {
				return NULL;
			}
			file->flags |= flags;
			return file;
			break;
		}
		
		file = list_get_next ( &file->list );
	}	
/*
	for ( i=0; i<sizeof(bitmap); i++) 
	{
		index = msb_index ( bitmap[i] );
		if (index)
			break;
	}
*/
	//file->block = get_free_block();	
	file->block = 31 - msb_index(bitmap[0]);
	bitmap[0] ^= 1 << (31 - file->block%32);

	kprintf("bitmap: %d %x\n", file->block/32, bitmap[0]);

	if ( flags & O_CREAT )  
	{ 
		file = kmalloc ( sizeof ( file_t ) ); 
		memcpy (file->name, name, strlen(name) ); 
		file->size = 0; 
		file->flags = flags | FILE_OPEN;
		file->id = id; 
		id++; 

	 	int status = k_device_send ( file, (file->block <<16) | 1,  0, &_disk); 
	 	if ( !status ) 
	 	{ 
	 		return NULL; 
	 	}
		
		//file->block +=1;
		list_append( &files, file, &file->list );
	}

	/* TODO: update ostale zastavice */
	return file;	
}

ssize_t file_read ( void *buffer, size_t size, file_t *file ) 
{
	if ( !(file->flags & O_RDONLY) )
	{
		kprintf("file not open for reading\n");
		return -1;	
	}


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
	start_addr+=512;

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
	file->block = get_free_block();
	retval = k_device_send ( buffer, (file->block << 16) | 1,  0, &_disk);
	bitmap[file->block /32] ^= 1 << (31 - file->block % 32) ;
	kprintf("redak: %d, stupac %d\n", file->block/32, file->block%32);
	kprintf("bitmap: %d %x\n", file->block/32, bitmap[file->block /32]);
	//retval = disk.send(&buffer, file->block << 16 | size, 0, &disk );
	//file->block++;
	return retval;
}

int get_free_block ( void )
{
	int i, index;

	for ( i=1; i<128; i++) 
	{
		index = msb_index ( bitmap[i] );
		if (index)
			break;
	}
	kprintf("block: %d\n", i*32 + 31 - index);
	return i*32 + 31 - index;
}
	
	
	
