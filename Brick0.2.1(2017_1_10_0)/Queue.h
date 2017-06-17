//Visual Studio Version
#ifndef QUEUE_HED
#define QUEUE_HED

#include "Dump.h"

//Queue
//지원조건-임의타입 T의 인스턴스가 생성 가능하고 디폴트 생성자,대입 연산자, 복사 생성자가 있는 경우.
//Out, Read의 경우 에러시 타입 T를 디폴트 생성하고 리턴함.
template <typename T>
class C_queue/*: public C_dump*/{
protected:
	unsigned Head, Tail;
	const unsigned Size;
	T* Queue;
public:
	C_queue(unsigned _Size);
	C_queue(const C_queue<T> &Input);
	virtual C_queue<T>& operator=(const C_queue<T> &Input);
	virtual ~C_queue();

	virtual unsigned Num() const ;
	virtual void In(const T &Input);
	virtual const T& Out();
	virtual const T& Read(unsigned _Num) const;
	virtual T* Read_editable(unsigned _Num);

	/*virtual unsigned Dump_size();
	virtual void Dump(char * ptr);
	virtual bool Restore(char * ptr);*/
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
		return T();
	}
}
//큐 Read.
template <typename T>
const T& C_queue<T>::Read(unsigned _Num) const{
	if (_Num < Num()){
		return Queue[(Tail + _Num) % Size];
	}
	else {
		return T();
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
//template <typename T>
//unsigned C_queue<T>::Dump_size(){
//	return T.Dump_size() * Num() + sizeof(unsigned);
//}
//template <typename T>
//void C_queue<T>::Dump(char * ptr){
//	unsigned Loop1;
//	(unsigned *)ptr = Dump_size();
//	(unsigned *)ptr++;
//	for (Loop1 = 0; Loop1 < Num(); Loop1+=T.Dump_size()){
//		Dump(ptr)
//	}
//}
//template <typename T>
//bool C_queue<T>::Restore(char * ptr){
//
//}
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
#endif//QUEUE_HED
//verge











