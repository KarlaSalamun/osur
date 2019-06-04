#include "fs.h"
#include "device.h"
#include "memory.h"
#include <api/stdio.h>
#include <kernel/kprint.h>
#include <types/bits.h>
#include "time.h"

list_t files;

extern device_t _disk;
unsigned int *bitmap;
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
}


file_t *fopen ( char *name, int flags ) 
{
	timespec_t time;
	file_t *file;
	cnt++;

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
	
	kclock_gettime( CLOCK_REALTIME, &time );
	file->last_used = time.tv_sec;
	kprintf("last used: %d\n", file->last_used);

	file->block = 31 - msb_index(bitmap[0]);
	bitmap[0] ^= 1 << (31 - file->block%32);

	kprintf("bitmap: %d %x\n", file->block/32, bitmap[0]);

	if ( flags & O_CREAT )  
	{ 
		file = kmalloc ( sizeof ( file_t ) );
		//block_t *block;
		//block = kmalloc ( sizeof ( block_t ) ); 
		memcpy (file->name, name, strlen(name) ); 
		file->size = 0; 
		file->flags = flags | FILE_OPEN;
		file->id = id; 
		id++; 
		file->blocks_used=0;
		list_init(&file->blocks);

	 	int status = k_device_send ( file, (file->block <<16) | 1,  0, &_disk); 
	 	if ( !status ) 
	 	{ 
	 		return NULL; 
	 	}
		
		list_append( &files, file, &file->list );
		//list_append( &file->blocks, block, &block->list);

		//int day = time.tv_sec / (3600*24);
		int tmp = 40348800;
		int year = tmp / (3600*24*365);
		int month = (tmp % (3600*24*365)) / (3600*24*30);
		int day = (tmp % (3600*24*365)) / (month*30*24*3600);
		kprintf("time: %d %d %d\n", year, month, day);
	}

	/* TODO: update ostale zastavice */
	return file;	
}

ssize_t file_read ( void *buffer, size_t size, file_t *file ) 
{
	char tmp_buffer[512]={[0 ... 511] = 0};
	if ( !(file->flags & O_RDONLY) )
	{
		kprintf("file not open for reading\n");
		return -1;	
	}

	block_t *block;
	block = list_get( &file->blocks, FIRST );
	for (int i = 0; i<file->blocks_used; i++) 
	{
		block = list_get_next ( &block->list );
		if ( k_device_recv ( tmp_buffer, 
			(block->block_num << 16) | 1, 0, &_disk) == 512 )
		{
			strcat(buffer, tmp_buffer);
		}
		else {
			return -1;
		} 
	}
	
	return file->blocks_used * 512;
		
}

ssize_t file_write ( void *buffer, size_t size, file_t *file ) 
{
	timespec_t time;
	int retval;	
	start_addr+=512;
	char tmp_buffer[512];
	file->size += size;
	block_t *block;

	if ( file->size >= BLOCK_SIZE ) 
	{
		block = kmalloc ( sizeof ( block_t ) ); 
		block->block_num = get_free_block();
		list_append(&file->blocks, block, &block->list);
		file->blocks_used++;
	}

	else 
	{
		block = list_get( &file->blocks, FIRST );
		if (!block)
		{
			block = kmalloc ( sizeof (block_t) );
			block->block_num = get_free_block();
			list_append(&file->blocks, block, &block->list);
		}
		else 
		{
		for (int i=0; i<file->blocks_used; i++)
			block = list_get_next ( &block->list );
		}
	}

	kprintf("block: %d\n", block->block_num);
/*
	block_t *block;
	block = kmalloc ( sizeof ( block_t ) ); 
	block->block_num = get_free_block();
	list_append(&file->blocks, block, &block->list);
	file->blocks_used++;
*/
	kclock_gettime( CLOCK_REALTIME, &time );
	file->last_modified = time.tv_sec;
	kprintf("last modified: %d\n", file->last_modified);

	retval = k_device_recv ( tmp_buffer, (block->block_num << 16) | 1,  0, &_disk);
	strcat( tmp_buffer, buffer );

	retval = k_device_send ( tmp_buffer, (block->block_num << 16) | 1,  0, &_disk);
	
	block_mark_used(block->block_num);
	kprintf("redak: %d, stupac %d\n", block->block_num/32, block->block_num%32);
	kprintf("bitmap: %d %x\n", block->block_num/32, bitmap[block->block_num /32]);
	
	return retval;
}

int file_close ( file_t *file )
{
	int status = k_device_send ( file, (file->block <<16) | 1,  0, &_disk); 
	 	if ( !status )  
	 		return -1; 
	 	else
	 		return 0;
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

void block_mark_used (unsigned int block)
{
	bitmap[block/32] ^= 1 << (31 - block % 32);
}
