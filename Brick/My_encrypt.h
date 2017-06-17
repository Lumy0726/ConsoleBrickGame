#ifndef ENCRYPT_HED
#define ENCRYPT_HED
namespace My_encrypt{
	unsigned char Movebit(unsigned char Input, int Num);
	unsigned char Reverse(unsigned char Input);
	bool Myencrypt(unsigned char * Ptr, unsigned Size, unsigned char Key);
	bool Mydecrypt(unsigned char * Ptr, unsigned Size, unsigned char Key);
}
#endif