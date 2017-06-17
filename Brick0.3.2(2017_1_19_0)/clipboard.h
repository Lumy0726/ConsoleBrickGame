/*
─ │ ┌ ┐ ┘ └ ├ ┬ ┤ ┴ ┼
━ ┃ ┏ ┓ ┛ ┗ ┣ ┳ ┫ ┻ ╋
┠ ┯ ┨ ┷ ┿
┝ ┰ ┥ ┸ ╂
┒ ┑ ┚ ┙ ┖ ┕ ┎ ┍
┞ ┟ ┡ ┢ ┦ ┧ ┩ ┪
┭ ┮ ┱ ┲ ┵ ┶ ┹ ┺
┽ ┾ ╀ ╁ ╃ ╄ ╅ ╆ ╇ ╈ ╉ ╊
■●□→←↑↓▲▼≪≫○△▽◁▷▲▼◀▶◈
*/
#include <vector>
std::vector<int> s;
typedef std::vector<int> asdfasdf;
//처음에 블럭은 312개임.

typedef enum{
BLACK,
D_BLUE,//1
D_GREEN,
D_SKYBLUE,
D_RED,
D_VIOLET,//5
D_YELLOW,
GREY,
D_GREY,
BLUE,
GREEN,//10
SKYBLUE,
RED,
VIOLET,
YELLOW,
WHITE,//15
	} T_color;

enum{
	BOARD_VERGE = 18,//화면 위의 여백 값.
	OUTPUT_TICK = 30,//화면 출력간격.(단위 ms).
	BRICK_SIZE = 5,//"하나의 벽돌이 몇개의 문자인가?".
	//5는 출력지연를 고려한 수(저사양모드). 그러나 벽돌이 작고 디자인이 힘듬.
	//6은 짝수여서 디자인이 왜곡될 가능성이 있음.
	//7은 홀수이면서 커서 디자인이 좋으나 출력지연이 심할 것.
	INTERNAL_RESOLUTION = 100,//짝수 이어야함.
	//필수조건. BRICK_SIZE*INTERNAL_RESOLUTON >= VECTOR_LENGTH.
	BALL_MAX_NUM = 6,//공 최대 갯수.
	LEVELUP_SEC = 20,//레벨업 주기.
	PLAYBRICK_SPEED_TICK_START = 300,
	PLAYBRICK_SPEED_TICK_END = 160,
};

enum{
	SHADOW_NUM = 50,//그림자수.
	DIRECTION_NUM = 40,//백터 방향 수.
};

template <typename T>
void C_queue_dump<T>::Dump(char * Ptr) const{
	unsigned Loop1;
	*(unsigned *)Ptr = Dump_size();
	Ptr += sizeof(unsigned);
	for (Loop1 = 0; Loop1 < Num(); Loop1++){
		Read(Loop1).Dump(Ptr);
		Ptr += Read(Loop1).Dump_size();
	}
}
template <typename T>
bool C_queue_dump<T>::Restore(char * Ptr){
	char * Ptr_E1, *Ptr_E2;
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