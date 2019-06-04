/*! Startup function - initialize kernel subsystem */
#define _K_STARTUP_C_

#include "time.h"
#include "device.h"
#include "memory.h"
#include "fs.h"
#include <kernel/errno.h>
#include <kernel/features.h>
#include <arch/interrupt.h>
#include <arch/processor.h>
#include <lib/string.h>
#include <api/prog_info.h>
#include <api/stdio.h>

char system_info[] = 	OS_NAME ": " NAME_MAJOR ":" NAME_MINOR ", "
			"Version: " VERSION " (" ARCH ")";

/* state of kernel features */
uint kernel_features = FEATURE_SUPPORTED; /* initially set all to "on" state */

/*!
 * First kernel function (after boot loader loads it to memory)
 */
void k_startup ()
{
	extern void *k_stdout; /* console for kernel messages */
	extern prog_info_t pi;

	/* set initial stdout */
	kdevice_set_initial_stdout ();

	/* initialize memory subsystem (needed for boot) */
	k_memory_init ();

	/*! start with regular initialization */

	/* interrupts */
	arch_init_interrupts ();

	/* detect memory faults (qemu do not detect segment violations!) */
	arch_register_interrupt_handler ( INT_MEM_FAULT, k_memory_fault, NULL );
	arch_register_interrupt_handler ( INT_UNDEF_FAULT, k_memory_fault, NULL );

	/* timer subsystem */
	k_time_init ();

	/* devices */
	k_devices_init ();

	init_fs();

	/* switch to default 'stdout' for kernel */
	k_stdout = k_device_open ( K_STDOUT, O_WRONLY );

	kprintf ( "%s\n", system_info );

	pi.heap = kmalloc ( PROG_HEAP_SIZE );
	pi.heap_size = PROG_HEAP_SIZE;

	/* enable interrupts */
	enable_interrupts ();

	prog_init ( NULL );
	/* example using disk */
	int fd = open ("DISK", 0, 0 ), ok = 0;
	int status;
	//int test = 0xdeadbeef;
	//int test_read = 0;
	if ( fd > 0 ) {
		char data_out[2048] = {[0 ... 2047] = 5};
		size_t block = 55, blocks = 2048/512;
		status = write ( fd, data_out, (block << 16) | blocks );
		if ( status == blocks * 512 ) {
			/* + provjera status! */
			char data_in[512];
			status = read ( fd, data_in, (block << 16) | blocks);
			if ( status == blocks * 512 ) {
				if ( memcmp ( data_out, data_in, 2048 ) == 0 ) {
					kprintf ( "DISK working OK! %d\n", fd);
					ok = 1;
				}
			}
		}
	}
	if (!ok)
		kprintf ( "DISK NOT working!\n");

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
	kprintf ( "\nSystem halted!\n" );
	halt ();
}

/*! Turn kernel feature on/off */
uint sys__feature ( uint features, int cmd, int enable )
{
	uint prev_state = kernel_features & features;

	ASSERT ( !( features & ~FEATURE_SUPPORTED ) );

	if ( cmd == FEATURE_GET )
		return prev_state;

	/* update state */
	if ( enable )
		kernel_features |= features;
	else
		kernel_features &= ~features;

	/* action required? */

	if ( ( features & FEATURE_INTERRUPTS ) )
	{
		if ( enable )
			enable_interrupts ();
		else
			disable_interrupts ();
	}

	return prev_state;
}
