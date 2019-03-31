#include <types/basic.h>

void premjesti (void) 
{
	extern size_t size_data;
	extern size_t size_i;

	extern char code_ROM;
//	extern char data_ROM, data_RAM;
	size_t size_d = (size_t) &size_i;
	size_t i;

	char *from = &code_ROM;
//	char *from = (char *) 0x50000;
//	char *to = (char *) 0x500000;
	char *to = (char *) 0x200000;

	for (i = 0; i< size_d; i++) {
		to[i] = from[i];
	}

	from = (char *) 0x150000;
	to = (char *) 0x250000;

//	from = &data_ROM;
//	to = &data_RAM;	

	size_d = (size_t) &size_data;

	for (i = 0; i<size_d; i++) {
		to[i] = from[i];
	}

}
