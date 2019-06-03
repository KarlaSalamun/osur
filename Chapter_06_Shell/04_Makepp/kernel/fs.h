#include <lib/string.h>
#include <lib/list.h>
#include "time.h"

#define FILE_NAME_LEN 15

#define FILE_OPEN ( 1 << 31 )

struct _file_t_;
typedef struct _file_t_ file_t;

struct _file_t_
{
	char name[FILE_NAME_LEN];
	
//	int (* open) (char *pathname, int flags, mode_t mode );
//	int (* close) (int fd );
//	ssize_t (* read) ( int fd, void *buffer, size_t count );
//	ssize_t (* write ) ( int fd, void *buffer, size_t count );
	
	int flags;
	int size;
	int id;	
	
	int date_created;
	int last_modified;
	int last_used;	
	int block;	
	list_h list;

};

void init_fs(void);
file_t *fopen ( char *name, int flags );
ssize_t file_read ( void *buffer, size_t size, file_t *file );
ssize_t file_write ( void *buffer, size_t size, file_t *file );

int get_free_block ( void );
	