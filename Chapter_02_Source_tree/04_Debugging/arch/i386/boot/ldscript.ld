/*! linker script for memory layout */

/* Its parsed as C before used in linking! */

ENTRY(arch_startup)

RAM_START = 0x200000;
STOG = 0x600000;

SECTIONS {
	.code LOAD_ADDR : AT (LOAD_ADDR)
	{
		/* instructions */
		*?/boot/startup.asm.o ( .text* )
		*?/boot/premjesti.o ( .text* ) 	
	}

	size_code = SIZEOF(.code);
	instr_ROM = LOAD_ADDR + size_code;

	.instrukcije RAM_START : AT (instr_ROM)
	{
		/* instructions in RAM */ 
		*( .text* );
		. = ALIGN (4096);
	}

	size_i = SIZEOF(.instrukcije);

	data_ROM = instr_ROM + size_i;
	data_RAM = .;
	
	.data data_RAM : AT (data_ROM)
	{
		/* read only data (constants), initialized global variables */
		* ( .rodata* .data* )
		. = ALIGN (4096);
	}

	size_data = SIZEOF(.data);

	bss_ROM = data_ROM + size_data;
	bss_RAM = .; 

	.bss bss_RAM : AT (bss_ROM)
	{
		/* uninitialized global variables */
		*( .bss* )
		. = ALIGN(4096);
	}

	.stog STOG :
	{
		*( .stog )
		. = ALIGN (4096);
	}
	/*
	 * what with other sections generated with gcc (various versions)?
	 * if optimizing for size discard them with: /DISCARD/ : { *(*) }
	 * if debugging with qemu/gdb they must be included
	 * - they can be included implicitly, without declaring them here
	 * - but they will be put behind "kernel_end_addr" so its best to use
	 *   multiboot loader (QEMU is that)
	 */
/DISCARD/ : { *(.eh_frame*) }
#ifndef DEBUG
	/DISCARD/ : { *(*) }
#endif
	kernel_end_addr = .;
}
