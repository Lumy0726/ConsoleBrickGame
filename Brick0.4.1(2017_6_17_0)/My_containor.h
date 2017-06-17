//Visual Studio VersionMY_CONTAINOR_HED
#ifndef MY_CONTAINOR_HED
#define MY_CONTAINOR_HED

#include <vector>
#include "Dump.h"

//메모리 덤프 가능 vector. T는 C_dump의 파생 클래스이고 순수가상함수를 재정의 해두어야함.
template <typename T>
class C_vector_dump:public vector<T>, public C_dump{
public:
	C_vector_dump();
	C_vector_dump(unsigned Size);
	C_vector_dump(const C_vector_dump<T>& Input);
	//대입 연산자는 디폴트 활용.
	bool operator==(const C_vector_dump<T>& Input) const;
	T& operator[](unsigned Offset);
	const T& operator[](unsigned Offset) const;

	virtual unsigned Dump_size() const;
	virtual void Dump(char * Ptr) const;//Dump규칙. 제일 앞에 자기자신의 총 덤프 바이트 수를 unsigned형으로 넣어둔다(unsigned형으로 넣어두는 바이트수까지도 포함해서).
	virtual bool Restore(char * Ptr);
};
//function.
template<typename T>
C_vector_dump<T>::C_vector_dump() :vector<T>(){ ; }
template<typename T>
C_vector_dump<T>::C_vector_dump(unsigned Size) : vector<T>(Size){ ; }
template<typename T>
C_vector_dump<T>::C_vector_dump(const C_vector_dump<T>& Input) : vector<T>(Input){ ; }
template<typename T>
bool C_vector_dump<T>::operator==(const C_vector_dump<T>& Input) const{
	return this->vector<T>::operator==(Input);
}
template<typename T>
T& C_vector_dump<T>::operator[](unsigned Offset){
	return this->vector<T>::operator[](Offset);
}
template<typename T>
const T& C_vector_dump<T>::operator[](unsigned Offset) const{
	return this->vector<T>::operator[](Offset);
}
template<typename T>
unsigned C_vector_dump<T>::Dump_size() const{
	unsigned Size = 0, Loop1;
	for (Loop1 = 0; Loop1 < size(); Loop1++){
		Size += at(Loop1).Dump_size();
	}
	return sizeof(unsigned) + Size;
}
template<typename T>
void C_vector_dump<T>::Dump(char * Ptr) const{
	unsigned Loop1;
	*(unsigned *)Ptr = Dump_size();
	Ptr += sizeof(unsigned);
	for (Loop1 = 0; Loop1 < size(); Loop1++){
		at(Loop1).Dump(Ptr);
		Ptr += at(Loop1).Dump_size();
	}
}
template<typename T>
bool C_vector_dump<T>::Restore(char * Ptr){
	char * Ptr_E1, *Ptr_E2;
	unsigned Loop1;
	Ptr_E1 = Ptr + *(unsigned *)Ptr;
	Ptr_E2 = Ptr + sizeof(unsigned);
	while (size() != 0){ pop_back(); }
	for (Loop1 = 0; true; Loop1++){
		push_back(T());
		if (!(at(Loop1).Restore(Ptr_E2))){
			return false;
		}
		Ptr_E2 = Ptr_E2 + *(unsigned *)Ptr_E2;
		if (Ptr_E1 == Ptr_E2){
			break;
		}
		else if (Ptr_E1 < Ptr_E2){
			return false;
		}
	}
	return true;
}


//Queue...이지만 중간값을 읽을 수 있는 무서운 큐.
//지원조건-임의타입 T의 인스턴스가 생성 가능하고 디폴트 생성자,대입 연산자, 복사 생성자가 있는 경우.
//Out, Read의 경우 에러시 타입 T를 디폴트 생성하고 리턴함.
template <typename T>
class C_queue{
protected:
	unsigned Head, Tail;
	const unsigned Size;
	T* Queue;
public:
	C_queue(unsigned _Size);
	C_queue(const C_queue<T> &Input);
	virtual C_queue<T>& operator=(const C_queue<T> &Input);
	virtual ~C_queue();

	virtual unsigned Num() const;
	virtual void In(const T &Input);
	virtual const T& Out();
	virtual const T& Read(unsigned _Num) const;
	virtual T* Read_editable(unsigned _Num);
};
//function
//생성자.
template <typename T>
C_queue<T>::C_queue(unsigned _Size) :Size(_Size + 1){
	Head = Tail = 0;
	Queue = new T[Size];
}
//복사 생성자.
template <typename T>
C_queue<T>::C_queue(const C_queue<T> &Input) :Size(Input.Size){
	Head = Input.Head;
	Tail = Input.Tail;
	int Loop1;
	Queue = new T[Size];
	for (Loop1 = 0; Loop1 < (int)
		Size; Loop1++){
		Queue[Loop1] = Input.Queue[Loop1];
	}
}
//대입 연산자.
template <typename T>
C_queue<T>& C_queue<T>::operator=(const C_queue<T> &Input){
	unsigned Loop1;
	if (Size == Input.Size){
		Head = Input.Head;
		Tail = Input.Tail;
		for (Loop1 = 0; Loop1 < Size; Loop1++){
			Queue[Loop1] = Input.Queue[Loop1];
		}
	}
	return *this;
}
//파괴자.
template <typename T>
C_queue<T>::~C_queue(){
	delete[] Queue;
}
//큐 자료 갯수.
template <typename T>
unsigned C_queue<T>::Num() const {
	return ((Head + Size) - Tail) % Size;
}
//큐 인.
template <typename T>
void C_queue<T>::In(const T &Input){
	if (Size - Num() != 1){
		Queue[Head] = Input;
		Head = (Head + 1) % Size;
	}
}
//큐 아웃.
template <typename T>
const T& C_queue<T>::Out(){
	unsigned Temp;
	if (Head != Tail){
		Temp = Tail;
		Tail = (Tail + 1) % Size;
		return Queue[Temp];
	}
	else {
		return *(T*)NULL;
	}
}
//큐 Read.
template <typename T>
const T& C_queue<T>::Read(unsigned _Num) const{
	if (_Num < Num()){
		return Queue[(Tail + _Num) % Size];
	}
	else {
		return *(T*)NULL;
	}
}
//큐 Read with pointer.
template <typename T>
T* C_queue<T>::Read_editable(unsigned _Num){
	if (_Num < Num()){
		return Queue + ((Tail + _Num) % Size);
	}
	else {
		return NULL;
	}
}

//메모리 덤프 가능 큐. T는 C_dump의 파생 클래스이고 순수가상함수를 재정의 해두어야함.
template <typename T>
class C_queue_dump: public C_queue<T>, public C_dump{
public:
	C_queue_dump(unsigned _Size);
	C_queue_dump(const C_queue_dump<T>& Input);
	virtual C_queue_dump<T>& operator=(const C_queue_dump<T>& Input);
	virtual unsigned Dump_size() const;
	virtual void Dump(char * Ptr) const;
	virtual bool Restore(char * Ptr);
};
template <typename T>
C_queue_dump<T>::C_queue_dump(unsigned _Size) : C_queue<T>(_Size){ ; }
template <typename T>
C_queue_dump<T>::C_queue_dump(const C_queue_dump<T>& Input) : C_queue<T>(Input){ ; }
template <typename T>
C_queue_dump<T>& C_queue_dump<T>::operator=(const C_queue_dump<T>& Input){ C_queue<T>::operator=(Input); return *this; }
template <typename T>
unsigned C_queue_dump<T>::Dump_size() const{
	unsigned Size = 0, Loop1;
	for (Loop1 = 0; Loop1 < Num(); Loop1++){
		Size += Read(Loop1).Dump_size();
	}
	return sizeof(unsigned) + Size;
}
template <typename T>
void C_queue_dump<T>::Dump(char * Ptr) const{
	unsigned Loop1;
	*(unsigned *)Ptr = Dump_size();
	Ptr += sizeof(unsigned);
	for (Loop1 = 0; Loop1 < Num();Loop1++){
		Read(Loop1).Dump(Ptr);
		Ptr += Read(Loop1).Dump_size();
	}
}
template <typename T>
bool C_queue_dump<T>::Restore(char * Ptr){
	char * Ptr_E1, * Ptr_E2;
	unsigned Loop1;
	Ptr_E1 = Ptr + *(unsigned *)Ptr;
	Ptr_E2 = Ptr + sizeof(unsigned);
	while (Num() != 0){ Out(); }
	for (Loop1 = 0; true; Loop1++){
		In(T());
		if (!(Read_editable(Loop1)->Restore(Ptr_E2))){
			return false;
		}
		Ptr_E2 = Ptr_E2 + *(unsigned *)Ptr_E2;
		if (Ptr_E1 == Ptr_E2){
			break;
		}
		else if (Ptr_E1 < Ptr_E2){
			return false;
		}
		if (Num() == Size - 1){ return false; }
	}
	return true;
}
#if 0
//Queue - inherit class
//지원조건 - 임의의 클래스 T의 하위 클래스(type T2)들의 인스턴스가 생성 가능하고 복사 생성자가 있는 경우.
//클래스 T는 추상 클래스 일수도 있음.
//T*를 Size만큼 동적 할당하고 각각의 T*는 동적 할당된 T2을 가리킨다.
//이 클래스 탬플릿은 상속이 불가능하다.
template <typename T>
class C_queue_inherit{
protected:
	unsigned Head, Tail;
	const unsigned Size;
	T** Queue;

	C_queue_inherit(C_queue_inherit<T> &Input);//복사생성 금지
	C_queue_inherit<T>& operator=(const C_queue_inherit<T> &Input);//대입연산 금지
	//Queue가 최종으로 가리키는 동적 할당된 클래스들의 타입을 알 수 없기 때문.

public:
	C_queue_inherit(unsigned _Size);
	virtual ~C_queue_inherit();

	virtual unsigned Num();
	template <typename T2>
	void In(const T2 &Input){
		if (((Tail + Size) - Head) % Size != 1){
			Queue[Head] = new T2(Input);
			Head = (Head + 1) % Size;
		}
	}
	virtual void Out();
	virtual const T* Read(unsigned Num) const;
};
//function
//생성자.
template <typename T>
C_queue_inherit<T>::C_queue_inherit(unsigned _Size) : Size(_Size + 1){

	Head = Tail = 0;
	Queue = new T*[Size];
}
//파괴자.
template <typename T>
C_queue_inherit<T>::~C_queue_inherit(){
	int Loop1, Num = ((Head + Size) - Tail) % Size;
	for (Loop1 = 0; Loop1 < Num; Loop1++){
		Out();
	}
	delete[] Queue;
}
//큐 자료 갯수.
template <typename T>
C_queue_inherit<T>::Num(){
	return ((Head + Size) - Tail) % Size;
}
//큐 인.
//template <typename T, typename T2>
//void C_queue_inherit<T>::In(const T2 &Input){
//	if (((Tail + Size) - Head) % Size != 1){
//		Queue[Head] = new T2(Input);
//		Head = (Head + 1) % Size;
//	}
//}
//큐 아웃.
template <typename T>
void C_queue_inherit<T>::Out(){
	if (Head != Tail){
		delete Queue[Tail];
		Tail = (Tail + 1) % Size;
	}
}
//큐 Read.
template <typename T>
const T* C_queue_inherit<T>::Read(unsigned Num) const{
	if (Num < ((Head + Size) - Tail) % Size){
		return Queue[(Tail + Num) % Size];
	}
	else {
		return (const T*)0;
	}
}

#endif//
#endif//MY_CONTAINOR_HED
//verge
