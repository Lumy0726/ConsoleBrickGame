#include "Dump.h"
C_dump::C_dump(){ Dump_ptr = (char *)0; }
C_dump::C_dump(const C_dump& Input){ Dump_ptr = (char *)0; }
C_dump& C_dump::operator=(const C_dump& Input){ return *this; }
C_dump::~C_dump(){ if (Dump_ptr) delete[] Dump_ptr; }
unsigned C_dump::Dump_start(char * &Ptr){
	unsigned Size = Dump_size();
	if (Dump_ptr) delete[] Dump_ptr;
	Ptr = Dump_ptr = new char[Size];
	if (Dump_ptr == 0){ return 0; }
	Dump(Dump_ptr);
	return Size;
}
