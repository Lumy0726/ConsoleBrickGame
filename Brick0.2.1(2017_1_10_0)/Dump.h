#ifndef DUMP_HED
#define DUMP_HED

class C_dump{
protected:
	char * Dump_ptr;
public:
	C_dump();
	~C_dump();
	virtual unsigned Dump_size() = 0;
	virtual void Dump(char * ptr) = 0;
	virtual bool Restore(char * ptr) = 0;
	virtual unsigned Dump_start(char * ptr);
};

#endif