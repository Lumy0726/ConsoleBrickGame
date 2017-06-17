#ifndef DUMP_HED
#define DUMP_HED

class C_dump{
protected:
	char * Dump_ptr;
public:
	C_dump();
	C_dump(const C_dump& Input);
	virtual C_dump& operator=(const C_dump& Input);
	virtual ~C_dump();
	virtual unsigned Dump_size() const = 0;
	virtual void Dump(char * Ptr) const = 0;//Dump규칙. 제일 앞에 자기자신의 총 덤프 바이트 수를 unsigned형으로 넣어둔다(unsigned형으로 넣어두는 바이트수까지도 포함해서).
	virtual bool Restore(char * Ptr) = 0;
	virtual unsigned Dump_start(char * &Ptr);
};

#endif