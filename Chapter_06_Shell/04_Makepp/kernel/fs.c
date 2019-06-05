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
int file_id = 10;
int file_cnt = 0;
//int cnt = 0;

static int get_free_block ( void );
static void block_mark_used ( unsigned int block );
static void check_file_from_disk ( char *name, char *tmp, file_t *file );
static file_t *check_open_file ( char *name );

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
	char tmp[512];

	kclock_gettime( CLOCK_REALTIME, &time );

	name += 5;
	
	if (strlen(name)>=FILE_NAME_LEN + 4)
	{
		memcpy(name, name, FILE_NAME_LEN);
		name[FILE_NAME_LEN] = '\0';
		char suffix[] = ".txt\0";
		strcat(name, suffix);
	}

	file = check_open_file ( name );
	if (!file)
	{
		check_file_from_disk ( name, tmp, file );
	}


	if ( flags & O_CREAT )  
	{ 
		if ( file )
		{
			kprintf("Greska: postoji datoteka s tim imenom\n");
			return NULL;
		}

		file = kmalloc ( sizeof ( file_t ) ); 
		strcpy (file->name, name ); 
		file->size = 0; 
		file->flags = FILE_OPEN;
		file->id = file_id; 
		file_cnt++; 
		file_id++;
		file->blocks_used=0;
		list_init(&file->blocks);

		file->block = 31 - msb_index(bitmap[0]);
		bitmap[0] ^= 1 << (31 - file->block%32);

		file->time_created = file->last_used = file->last_modified = time.tv_sec;
	
	 	int status = k_device_send ( file, (file->block <<16) | 1,  0, &_disk); 
	 	if ( !status ) 
	 	{ 
	 		return NULL; 
	 	}
		
		list_append( &files, file, &file->list );
	}

	file->flags |= flags;
	file->last_used = time.tv_sec;
	return file;

}

ssize_t file_read ( void *buffer, size_t size, file_t *file ) 
{
	char tmp_buffer[512]={[0 ... 511] = 0};
	int i=0;
	timespec_t time;

	kclock_gettime( CLOCK_REALTIME, &time );

	file->last_used = time.tv_sec;

	if ( !(file->flags & O_RDONLY) )
	{
		kprintf("file isn't open for reading\n");
		return -1;	
	}

	block_t *block;
	block = list_get( &file->blocks, FIRST );

	while ( block )
	{
		if ( k_device_recv ( tmp_buffer, 
			(block->block_num << 16) | 1, 0, &_disk) == 512 )
		{
			strcat(buffer, tmp_buffer);
			//kprintf(" tekst u read %s\n", tmp_buffer);
		}
		else
			return -1;
		block = list_get_next( &block->list );
		i++;
	}
	return i * 512;
}

ssize_t file_write ( void *buffer, size_t size, file_t *file ) 
{

	if ( !(file->flags & O_RDONLY) ) {
		kprintf("file isn't open for writing\n");
		return -1;
	}

	timespec_t time;
	int retval;	
	start_addr+=512;
	char tmp_buffer[512];
	file->size += size;
	block_t *block;

	//block = kmalloc ( sizeof ( block_t ) ); 
	block = list_get( &file->blocks, FIRST );

	if ( file->size >= BLOCK_SIZE || !block ) 
	{
		block = kmalloc ( sizeof ( block_t ) ); 
		block->block_num = get_free_block();
		list_append(&file->blocks, block, &block->list);
		file->blocks_used++;
		block_mark_used(block->block_num);
	}

	else 
	{
		for (int i=0; i<file->blocks_used; i++) {
			if ( !list_get_next ( &block->list ) )
				break; 
		}
	}

	kprintf("block: %d\n", block->block_num);

	kclock_gettime( CLOCK_REALTIME, &time );
	file->last_modified = time.tv_sec;
	//kprintf("last modified: %d\n", file->last_modified);

	retval = k_device_recv ( tmp_buffer, (block->block_num << 16) | 1,  0, &_disk);
	strcat( tmp_buffer, buffer );

	//kprintf(" tekst u write %s\n", tmp_buffer);

	retval = k_device_send ( tmp_buffer, (block->block_num << 16) | 1,  0, &_disk);
	
	//kprintf("redak: %d, stupac %d\n", block->block_num/32, block->block_num%32);
	kprintf("bitmap: %d %x\n", block->block_num/32, bitmap[block->block_num /32]);
	
	return retval;
}

int file_close ( file_t *file )
{
	//list_remove( &files, 0, &file->list );
	//file->flags ^= FILE_OPEN;
	file->flags = 0;
	//kprintf("close: %s\n", file->name);
	if ( !k_device_send ( file, (file->block <<16) | 1,  0, &_disk) )  {
		return -1;
	}
	else {
	 	return 0;
	}
	 	
}

static int get_free_block ( void )
{
	int i, index;

	for ( i=1; i<128; i++) 
	{
		index = msb_index ( bitmap[i] );
		if (index)
			break;
	}

	//kprintf("block: %d\n", i*32 + 31 - index);
	return i*32 + 31 - index;
}

static void block_mark_used (unsigned int block)
{
	bitmap[block/32] ^= 1 << (31 - block % 32);
}

/* checks if a file with given name exists
	returns NULL if not, pointer to file otherwise
*/

static file_t *check_open_file ( char *name )
{
	file_t *file;
	file = list_get ( &files, FIRST );
	
	while(file) 
	{
		if ( !strcmp(file->name, name) ) 
		{
			return file;
		}
		file = list_get_next ( &file->list );
	}
	return NULL;
}

static void check_file_from_disk ( char *name, char *tmp, file_t *file )
{
	for ( int i=0; i<file_cnt; i++) 
	{
		
		int status = k_device_recv ( tmp, (i <<16) | 1,  0, &_disk );
		file = ( file_t *)tmp;
		//kprintf("file name: %s\n", file->name);
		if (!status)
		{
			break;
		}
		if (  !strcmp ( name, file->name ) ) 
		{
			return;
		}
	}
	file = NULL;
}	
