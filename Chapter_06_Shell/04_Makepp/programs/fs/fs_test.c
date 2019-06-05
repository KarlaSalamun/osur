#include <stdio.h>
#include <lib/string.h>

char PROG_HELP[] = "Test filesystem functions";

int fs_test ( char *args[] )
{
	printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		 PROG_HELP );

	char file_out[] = "deadbeef";
	int status;
	int fd1, fd2;
	char file_in[512];

	printf("Otvaram datoteku file1.txt sa zastavicama O_CREAT | O_RDWR\n");
	fd1 = open ( "file:file1.txt", O_CREAT | O_RDWR, 0);
	if ( fd1 == -1 )
	{	
		printf("greska\n");
		return 1;
	}
	printf ( "file descriptor: %d\n", fd1);

	printf("Upisujem u datoteku:	%s\n", file_out);
	status = write ( fd1, file_out, sizeof(file_out));
	printf("Upisujem u datoteku:	%s\n", file_out);
	status = write ( fd1, file_out, sizeof(file_out));

	printf("\n\nOtvaram datoteku file2.txt sa zastavicama O_RDWR\n");
	fd2 = open ( "file:file2.txt", O_RDWR, 0);
	if ( fd2 == -1 ) {	
		printf("open vratio -1, ocekivano\n");
	}
	else {
		return -1;
	}

	printf("\n\nOtvaram datoteku file2.txt sa zastavicama O_CREAT\n");
	fd2 = open ( "file:file2.txt", O_CREAT | O_RDWR, 0);
	if ( fd2 == -1 )
	{	
		printf("greska\n");
		return 1;
	}
	printf ( "file descriptor: %d\n", fd2);

	printf("Upisujem u datoteku:	%s\n", file_out);
	status = write ( fd2, file_out, sizeof(file_out));
	if ( status < 1) {
		printf("ocekivana greska\n");
	}

	close(fd1);
	close(fd2);
/*
	printf("\n\nOtvaram datoteku file1.txt sa zastavicama O_WRONLY\n");
	fd1 = open ( "file:file1.txt", O_WRONLY, 0);
	if ( fd1 == -1 )
	{	
		printf("greska\n");
		return 1;
	}
	//printf ( "file descriptor 3: %d\n\n", fd1);

	memset(file_in, 0, sizeof(file_in));
	status = read ( fd1, file_in, 512);
	if ( status <= 0 ) {
		printf("ocekivana greska u citanju\n");
	}

	printf("\n\nOtvaram datoteku file1.txt sa zastavicama O_RDONLY\n");
	fd1 = open ( "file:file1.txt", O_RDONLY, 0);
	if ( fd1 == -1 )
	{	
		printf("greska\n");
		return 1;
	}
	//printf ( "file descriptor 3: %d\n", fd1);

	status = read ( fd1, file_in, 512);
	if ( status <= 0 ) {
		printf("greska\n");
		return 1;
	}

	printf("Citam iz datoteke file1.txt:	%s\n", file_in);

	printf("\n\nOtvaram datoteku file2.txt sa zastavicama O_RDWR\n");
	fd2 = open ( "file:file2.txt", O_RDWR, 0);
	if ( fd2 == -1 )
	{	
		printf("greska\n");
		return 1;
	}
	//printf ( "file descriptor: %d\n", fd2);

	memset(file_in, 0, sizeof(file_in));
	status = read ( fd2, file_in, 512);
	if ( status <= 0 ) {
		printf("greska\n");
		return 1;
	}

	printf("Citam iz datoteke file2.txt:	%s\n\n\n", file_in);
*/
	fd1 = open ( "file:file1.txt", O_RDWR, 0);
	if ( fd1 == -1 )
	{	
		printf("greska\n");
		return 1;
	}
	printf ( "file descriptor 3: %d\n\n", fd1);

	memset(file_in, 0, sizeof(file_in));
	status = read ( fd1, file_in, 512);
	if ( status <= 0 ) {
		printf("greska\n");
		return 1;
	}

	printf("Citam iz datoteke file1.txt:	%s\n", file_in);

	fd2 = open ( "file:file2.txt", O_RDWR, 0);
	if ( fd2 == -1 )
	{	
		printf("greska\n");
		return 1;
	}
	printf ( "file descriptor 4: %d\n\n", fd2);

	memset(file_in, 0, sizeof(file_in));
	status = read ( fd2, file_in, 512);
	if ( status <= 0 ) {
		printf("greska\n");
		return 1;
	}

	printf("Citam iz datoteke file2.txt:	%s\n", file_in);

	return 0;

}