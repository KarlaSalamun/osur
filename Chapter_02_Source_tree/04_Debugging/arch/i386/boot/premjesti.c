#include <types/basic.h>

#define RAM_START 0x200000

void premjesti (void) 
{
	extern size_t size_data;
	extern size_t size_i;

	extern char instr_ROM, data_ROM, data_RAM;

	size_t size_d = (size_t) &size_i;
	size_t i;

	char *from = &instr_ROM;
	char *to = (char *) RAM_START;

	for (i = 0; i< size_d; i++) {
		to[i] = from[i];
	}

	from = (char *) &data_ROM;
	to = (char *) &data_RAM;

	size_d = (size_t) &size_data;

	for (i = 0; i<size_d; i++) {
		to[i] = from[i];
	}
}
