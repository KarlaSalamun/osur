#include <stdio.h>
#include <malloc.h>

void demo (void) 
{
	int i;

	struct req
	{
		void *ptr;
		unsigned int size;
	};

	struct req requests[100];

	for (i = 0; i<100; i++) {
		requests[i].size = (i+1)*16; 
		requests[i].ptr = malloc(requests[i].size);
		printf( "size: %d	address: %x \n", requests[i].size, requests[i].ptr);
	}

	for (i = 0; i<100; i++) {
		free(requests[i].ptr);
	}

	requests[0].ptr = malloc(2457100);
	printf(" address: %x\n ", requests[0].ptr);

}	
