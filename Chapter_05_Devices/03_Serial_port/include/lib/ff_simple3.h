/*! Dynamic memory allocator - first fit (simple)
 *
 * In this implementation double linked list are used.
 * Free list is not sorted. Search is started from first element until chunk
 * with adequate size is found (same or greater than required).
 * When chunk is freed, first join is tried with left and right neighbor chunk
 * (by address). If not joined, chunk is marked as free and put at list start.
 */

#pragma once

#ifdef MEM_TEST
#include "test/test.h"
#endif
#include <types/basic.h>

#ifndef _FF_SIMPLE_C_

typedef void ffs_mpool_t;

/*! interface */
void *ffs3_init ( void *mem_segm, size_t size );
void *ffs3_alloc ( ffs_mpool_t *mpool, size_t size );
int ffs3_free ( ffs_mpool_t *mpool, void *chunk_to_be_freed );
void *ffs3_segment_init ( ffs_mpool_t *mpool, size_t start, size_t size ); 

/*! rest is only for first_fit.c */
#else /* _FF_SIMPLE_C_ */

/* free chunk header (in use chunk header is just 'size') */
typedef struct _ffs_hdr_t_
{
	size_t		     size;
			     /* chunk size, including head and tail headers */
	struct _ffs_hdr_t_  *prev;
			     /* previous free in list */
	struct _ffs_hdr_t_  *next;
			     /* next free in list */
}
ffs_hdr_t;

/* chunk tail (and header for in use chunks) */
typedef struct _ffs_tail_t_
{
	size_t  size;
		/* chunk size, including head and tail headers */
}
ffs_tail_t;

typedef struct _ffs_mpool_t_
{
	ffs_hdr_t *first;
}
ffs_mpool_t;

#define HEADER_SIZE ( sizeof (ffs_hdr_t) + sizeof (ffs_tail_t) )

/* use LSB of 'size' to mark chunk as used (otherwise size is always even) */
#define MARK_USED(HDR)	do { (HDR)->size |= 1;  } while(0)
#define MARK_FREE(HDR)	do { (HDR)->size &= ~1; } while(0)

#define CHECK_USED(HDR)	((HDR)->size & 1)
#define CHECK_FREE(HDR)	!CHECK_USED(HDR)

#define GET_SIZE(HDR)	((HDR)->size & ~1)

#define GET_AFTER(HDR)	(((void *) (HDR)) +  GET_SIZE(HDR))
#define GET_TAIL(HDR)	(GET_AFTER(HDR) - sizeof (ffs_tail_t))
#define GET_HDR(TAIL)	(((void *)(TAIL)) - GET_SIZE(TAIL) + sizeof(ffs_tail_t))

#define CLONE_SIZE_TO_TAIL(HDR)	\
	do { ( (ffs_tail_t *) GET_TAIL(HDR) )->size = (HDR)->size; } while(0)

#define ALIGN_VAL	( (size_t) sizeof(size_t) )
#define ALIGN_MASK	( ~( ALIGN_VAL - 1 ) )
#define ALIGN(P)	\
	do { (P) = ALIGN_MASK & ( (size_t) (P) ); } while(0)
#define ALIGN_FW(P)	\
	do { (P) = ALIGN_MASK & (((size_t) (P)) + (ALIGN_VAL - 1)) ; } while(0)

#define BLOCK_MIN_M 65
#define BLOCK_MIN_L 513

extern ffs_mpool_t *k_mpool_s, *k_mpool_m, *k_mpool_l;

void *ffs3_init ( void *mem_segm, size_t size );
void *ffs3_alloc ( ffs_mpool_t *mpool, size_t size );
int ffs3_free ( ffs_mpool_t *mpool, void *chunk_to_be_freed );
void *ffs3_segment_init ( ffs_mpool_t *mpool, size_t start, size_t size ); 

static void ffs3_remove_chunk ( ffs_mpool_t *mpool, ffs_hdr_t *chunk );
static void ffs3_insert_chunk ( ffs_mpool_t *mpool, ffs_hdr_t *chunk );

#endif /* _FF_SIMPLE_C_ */
