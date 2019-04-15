#include <api/stdio.h>
#include <api/malloc.h>
#include <lib/ff_simple3.h>

#define HEAP_START_S (void *) 0x0010C000

extern size_t heap_size;
extern ffs_mpool_t *mpool_s, *mpool_m, *mpool_l;

int demo (void) 
{
	int i;
	size_t segment_size = heap_size/3;

	struct req
	{
		void *ptr;
		unsigned int size;
	};

	struct req requests[21];

	printf("heap size: %d\n", heap_size);
	printf("segment size: %d\n", segment_size);
	printf("segment start:\n small blocks: %x\n medium blocks: %x\n large blocks: %x\n\n",
			mpool_s, mpool_m, mpool_l);

	
	for (i = 0; i<5; i++) {
		requests[i].size = (i+1)*16; 
		if ( ( requests[i].ptr = malloc(requests[i].size ) ) == NULL ) {
			printf("malloc returned NULL\n");
			return 1;
		}
		printf( "size: %d	address: %x\n", requests[i].size, requests[i].ptr);
	}

	for (i = 0; i<5; i++) {
		requests[i+5].size = (i+1)*256;
		if ( ( requests[i+5].ptr = malloc(requests[i+5].size ) ) == NULL ) {
			printf("malloc returned NULL\n");
			return 1;
		}
		printf( "size: %d	address: %x\n", requests[i+5].size, requests[i+5].ptr);
	}

	for (i = 0; i<5; i++) {
		requests[i+10].size = segment_size/256*(i+1);
		if ( ( requests[i+10].ptr = malloc(requests[i+10].size ) ) == NULL ) {
			printf("malloc returned NULL\n");
			return 1;
		}
		printf( "size: %d	address: %x\n", requests[i+10].size, requests[i+10].ptr);
	}

	for (i = 0; i<5; i++) {
		requests[i+15].size = segment_size/16*(i+1);
		if ( ( requests[i+15].ptr = malloc(requests[i+15].size ) ) == NULL ) {
			printf("malloc returned NULL\n");
			return 1;
		}
		printf( "size: %d	address: %x\n", requests[i+15].size, requests[i+15].ptr);
	}
	for (i = 0; i<20; i++) {
		if(free(requests[i].ptr))
			printf("failed to free memory\n");
	}

	requests[20].size = 2428900;
	if ( ( requests[20].ptr = malloc(requests[20].size) ) == NULL ) {
		printf("malloc returned NULL\n");
		return 1;
	}
	printf("size: %d	address: %x\n", requests[20].size, requests[20].ptr);
	
	free(requests[20].ptr);

	return 0;
}	
