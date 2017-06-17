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
	virtual void Dump(char * Ptr) const = 0;//Dump��Ģ. ���� �տ� �ڱ��ڽ��� �� ���� ����Ʈ ���� unsigned������ �־�д�(unsigned������ �־�δ� ����Ʈ�������� �����ؼ�).
	virtual bool Restore(char * Ptr) = 0;
	virtual unsigned Dump_start(char * &Ptr);
};

#endif