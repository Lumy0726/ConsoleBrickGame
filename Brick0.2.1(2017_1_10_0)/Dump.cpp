#include "Dump.h"
C_dump::C_dump(){ Dump_ptr = (char *)0; }
C_dump::~C_dump(){ if (Dump_ptr) delete[] Dump_ptr; }
unsigned C_dump::Dump_start(char * ptr){
	unsigned Size = Dump_size();
	ptr = Dump_ptr = new char[Size];
	if (ptr == 0){ return 0; }
	Dump(ptr);
	return Size;
}
