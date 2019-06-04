#include <stdio.h>
#include <lib/string.h>

char PROG_HELP[] = "Test filesystem functions";

int fs_test ( char *args[] )
{
	printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		 PROG_HELP );

	char file_out[] = "deadbeef";
	int status;
	int fd;
	//int test = 0xdeadbeef;
	char file_in[512];
	fd = open ( "file:test.txt", O_CREAT | O_RDWR, 0);
	if ( fd == -1 )
	{	
		printf("file exists\n");
	}
	printf ( "file descriptor: %d\n", fd);
	printf("pozivam write\n");
	status = write ( fd, file_out, sizeof(file_out));
	printf("pozivam write\n");
	status = write ( fd, file_out, sizeof(file_out));
	if ( status == 512) {
		status = read ( fd, file_in, 512);
	}
	printf("read function %d\n", status);
	printf("%s\n", file_in);

	strcpy(file_out, "ffffffff");
	memset(file_in, 0, 512);

	close(fd);

	/*
	fd = open ( "file:datotekajakodugaimena.txt", O_CREAT | O_RDWR, 0);
	if ( fd == -1 ) {
		kprintf("file exists\n");
		halt();
	}
	kprintf ( "file descriptor: %d\n", fd);
	status = write ( fd, file_out, sizeof(file_out) );
	if ( status == 512) {
		status = read ( fd, file_in, 512);
	}
	kprintf("read function\n");
	kprintf("%s\n", file_in);
*/
	return 0;

}