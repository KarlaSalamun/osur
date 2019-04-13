/*! Memory management */
#pragma once

#include <kernel/memory.h>

/*! Kernel dynamic memory --------------------------------------------------- */
#include <lib/ff_simple3.h>
#include <lib/gma.h>

#if MEM_ALLOCATOR == FIRST_FIT

#define MEM_ALLOC_T ffs_mpool_t
#define	K_MEM_INIT(segment, size)	ffs3_init ( segment, size )
#define	KMALLOC(size)			ffs3_alloc ( size )
#define	KFREE(addr)			ffs3_free ( addr )

#define HEAP_START_S (void *) 0x0010C000
#define HEAP_START_M (void *) 0x00364000
#define HEAP_START_L (void *) 0x005A8000

#define HEAP_SIZE_S 0x00258000
#define HEAP_SIZE_M 0x00244000
#define HEAP_SIZE_L 0x00258000

#elif MEM_ALLOCATOR == GMA

#define MEM_ALLOC_T gma_t
#define	K_MEM_INIT(segment, size)	gma_init ( segment, size, 32, 0 )
#define	KMALLOC(size)			gma_alloc ( k_mpool, size )
#define	KFREE(addr)			gma_free ( k_mpool,  addr )

#else /* memory allocator not selected! */

#error	Dynamic memory manager not defined!

#endif



/*! Kernel memory layout ---------------------------------------------------- */
#include <types/basic.h>
#include <lib/list.h>
#include <api/prog_info.h>
#include <arch/memory.h>


extern MEM_ALLOC_T *k_mpool; /* defined in memory.c */
extern MEM_ALLOC_T *k_mpool_s, *k_mpool_m, *k_mpool_l;
extern list_t kobjects;

void k_memory_init ();
void k_memory_info ();

/*! Object referenced in programs (kernel object reference + additional info) */
struct _kobject_t_
{
	void	*kobject;
		 /* pointer to kernel object, e.g. device */
	uint	 flags;
		 /* various flags */
	void	*ptr;
		 /* pointer for extra per process info */

	list_h	 spec;
		 /* list for object purposes */

	list_h	 list;
};

/* -------------------------------------------------------------------------- */
/*! kernel ids, objects */

id_t k_new_id ();
void k_free_id ( id_t id );
int k_check_id ( id_t id );

void k_memory_fault (); /* memory fault handler */

void *kmalloc_kobject ( size_t obj_size );
void *kfree_kobject ( kobject_t *kobj );
