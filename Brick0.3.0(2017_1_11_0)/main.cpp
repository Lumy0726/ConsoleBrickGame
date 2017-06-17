#include <iostream>
#include <fstream>
using namespace std;
#include <Windows.h>
#include <conio.h>
#include <assert.h>
#include "Coordinate_Outputstream.h"
#include "Queue.h"
#include "Fraction.h"
#include "Keyboard.h"
#include "Dump.h"

//mecro
//#define MYDEBUG
#ifdef MYDEBUG
#define ERROR_DEBUGSTOP(exp) if (exp){STOP();}
void STOP(){
	int Temp = 0;
	assert(false);
	Temp = 0;
}
#else
#define ERROR_DEBUGSTOP(exp) (void)0
#endif

#define VECTOR_LENGTH 60//상수 백터들의 내부길이.

//type, class
class C_ball;
class C_board: public C_dump{
private:
	C_board(const C_board& Input);
	C_board& operator=(const C_board& Input);
public:
	//protected:
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
	const unsigned BASE_SIZE;//사용자의 공간.(이 공간에 벽돌이 들어오면 게임오버)(벽돌수 기준).
	const unsigned FIELD_SIZE;//HITTING_ZONE으로부터 벽돌까지 기본거리.(벽돌수 기준).
	const unsigned BRICK_HEIGHT;
	const unsigned BOARD_SIZE;// BASE_SIZE + (BRICK_HEIGHT + FIELD_SIZE) * 2
	const unsigned BOARD_SIZE_IN;//BOARD_SIZE*BRICK_SIZE*INTERNAL_RESOLUTION. IN(TERANL BOARD)(for great quality).
	const C_screen_color::T_color BACK_COLOR;

	class C_brick{
	public:
		unsigned Health;
		enum Brick_state{
			NONE = 0,//NONE은 벽돌일수 없는 곳(공이 진입할 수 없음).
			NORMAL = 1,//NORMAL이고 Health==0일때 빈 공간을 의미.
			BALLOUT,//깨지면 공이 3개 나옴.
		} State;
		C_brick(){ Health = 1; State = NONE; }//NONE도 반사성을 지녀야 하기 때문에 체력이 0이 아님.
		C_brick(int H){ if (H >= 0){ Health = H; State = NORMAL; } else { Health = 0; State = NONE; } }
		C_brick(unsigned H, Brick_state _State){ Health = H; State = _State; }
	};
	friend class C_board::C_brick;

	C_brick **Board;
	C_queue_dump<C_ball> Balls;
	C_screen_color Screen;
	static const char Letter[][5];
	unsigned Playbrick_speed_tick;
	unsigned Playbrick_tick;
	unsigned Level;
	unsigned Levelup_tick;
	unsigned Score;
	unsigned Bestscore;
	const unsigned int Damageup_speed;//공의 최대 속도를 5등분.
	enum T_direction{
		R = 0, DR = 5, D = 10, DL = 15, L = 20, UL = 25, U = 30, UR = 35,
	};
	T_direction Play_brick_way;
	struct T_reflector {
		int X, Y;
		unsigned Position;
		unsigned Move_length;//Reflector의 한칸 이동 길이(단위길이: INTERNAL_RESOLUTION).
		unsigned Length;//Reflector길이의 절반(단위길이: VECTOR_LENGTH).
	} Reflector;

	//function.
	void Screen_init();
	void Game_init();
	void PLAY(bool Loadstate);
	void Gamestop();
	void Countdown();
	bool Save_state();
	void Save_score();
	bool Load();
	static void Myencrypt(char * Ptr, unsigned Size);//간단하게 뒤섞기만 하는 암호화(소스코드 보안 필요).
	static void Mydecrypt(char * Ptr, unsigned Size);
	void Print_ball(unsigned Ball_num, bool Print = true);
	void Print_Reflector(bool Print = true);
	void Print_brick(unsigned X, unsigned Y);
	void Print_hitting_zone();
	void Print_score();
	void Get_reflector_edge(int * X1, int * Y1, int * X2, int * Y2);//Reflector의 사이즈 정보도 포함.
	int Dead_verge();
	bool Play_brick();//true는 게임오버.
	bool Play_ball(unsigned Ball_num);//공이 사라져야 할 경우 true반환.
	bool Play_ball_ref(C_ball& Ball);
	void Play_ball_brick(C_ball&Ball);
	bool Play_ball_basezone_check(unsigned Ball_num);
	void Move_reflector(T_direction Input, unsigned Speed = 1);
	void Arrange_reflector();
	void Arrange_ball_in_brick(unsigned Ball_num, T_direction Way);//공이 벽돌에 끼는 경우를 정리.
	void Arrange_ball_zoneout(unsigned Ball_num);//영영 Hitting_zone으로 들어오지 못하는 공을 정리함.
	void Brick_add(unsigned X, unsigned Y);//벽돌의 난이도 설정값이 이 함수안에 있음.
	void Brick_hit(unsigned X, unsigned Y, unsigned Speed);
	void Screen_recorvery();
	void Print_line(int X1, int Y1, int X2, int Y2, const char* Letter, char Color, int X_ref = -1, int Y_ref = -1, char Ref_color = 0);
	static C_fraction Level_func(unsigned Input, C_fraction Start, C_fraction End, unsigned High_level, unsigned Speed);//레벨구간 1~High_level까지는 난이도가 Start에서 End + (Start-End)/SPEED까지 일차함수로 변화하며, 그 이후로는 End에 근사해져감.
	static const char* Get_letter(char Input);
	void Draw_letter(unsigned X, unsigned Y, const char * Input, const char * Str, char color);//BOARD_VERGE포함좌표.(Screen객체상 좌표).
	static void Draw_letter_setscreen(C_screen_color& Screen, unsigned X, unsigned Y, const char * Input, const char * Str, char color);//BOARD_VERGE포함좌표.(Screen객체상 좌표).

public:
	C_board(unsigned Base_size = 5, unsigned Field_size = 4, unsigned Brick_height = 6);
	~C_board(){
		int Temp1;
		for (Temp1 = 0; Temp1 < (int)BOARD_SIZE; Temp1++){
			delete[] Board[Temp1];
		}
		delete[] Board;
	}

	unsigned Dump_size() const;
	void Dump(char * Ptr) const;
	bool Restore(char * Ptr);

	static bool Box_in_line(int X1, int Y1, int X2, int Y2, int Box_X1, int Box_Y1, int Box_X2, int Box_Y2);//line은 직선 취급.
	static bool Line_in_line(int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4, C_fraction* T = NULL);//line은 선분 취급.매개변수직선과 직선의 교점의 t값을 T로 리턴.
	static bool Dot_is_right_side(int X1, int Y1, int X2, int Y2, int X3, int Y3);//(X1,Y1)에서 (X2,Y2)로 가는 백터를 그을 때, (X3,Y3)점이 이 백터의 오른쪽에 있는가?(경계선도 포함).
};

class C_ball: public C_dump{
	friend class C_board;

protected:
	static const int Vector[40][2];//Vector[DIRECTION_NUM][2].
	enum{
		SHADOW_NUM = 50,//그림자수.
		DIRECTION_NUM = 40,//백터 방향 수.
	};
	class C_ball_vector: C_dump{
	public:
		int X, Y;
		unsigned Vector_num;
		enum T_state{
			NONE, BRICK_REF, REF_REF,
		}State;
		C_ball_vector(){ X = Y = Vector_num = 0; State = NONE; }
		C_ball_vector(int _X, int _Y, unsigned V_num){ X = _X; Y = _Y; Vector_num = V_num; State = NONE; }
		C_ball_vector(const C_ball_vector& Input){ X = Input.X; Y = Input.Y; Vector_num = Input.Vector_num; State = Input.State; }
		int Vector_X() const{ return C_ball::Vector[Vector_num][0]; }
		int Vector_Y() const{ return C_ball::Vector[Vector_num][1]; }

		virtual unsigned Dump_size() const{ return sizeof(unsigned) + sizeof(C_ball_vector); }
		virtual void Dump(char * Ptr) const{ *(unsigned *)Ptr = Dump_size(); Ptr += sizeof(unsigned); *(C_ball_vector*)Ptr = *this; }
		virtual bool Restore(char * Ptr){
			char * Ptr_E = Ptr + *(unsigned *)Ptr;
			Ptr += sizeof(unsigned);
			*this = *(C_ball_vector*)Ptr;
			Ptr += sizeof(C_ball_vector);
			if (Ptr == Ptr_E){ return true; }
			else { return false; }
		}
	};

	C_queue_dump<C_ball_vector> Ball_rec;
	unsigned Speed_tick;
	unsigned Speed_movenum;
	//속력=진동수*파장=파장/주기. Tick이 주기 효과이고, movenum이 파장 효과.
	//현재 Speed_tick은 1로 고정되어 있음(현재 쓸모없음).
	unsigned Tick;//틱이 0이면 정지, 틱은 1부터 Speed_tick까지 순환.
	enum T_basezone_state{
		BASEZONE_OUT, BASEZONE_IN, REF, START
	}Basezone_state;

	bool Speed_edit(int Offset, unsigned Set = 0);//감속 요구에 감속 실패시 false반환.
	bool Basezone_check(int H1, int H2);//감속 요구에 감속 실패시 false반환.


public:
	C_ball(int _X = 0, int _Y = 0, unsigned V_num = 0) :Ball_rec(SHADOW_NUM){
		C_ball_vector Temp(_X, _Y, V_num);
		Ball_rec.In(Temp); Speed_tick = 1; Speed_movenum = 1; Tick = 1; Basezone_state = START; "";//공 속도 조절.
	}
	C_ball(const C_ball& Input) :Ball_rec(Input.Ball_rec){
		Speed_tick = Input.Speed_tick; Speed_movenum = Input.Speed_movenum; Tick = Input.Tick; Basezone_state = Input.Basezone_state;
	}
	C_ball& operator=(const C_ball& Input){
		Ball_rec = Input.Ball_rec; Speed_tick = Input.Speed_tick; Speed_movenum = Input.Speed_movenum; Tick = Input.Tick; Basezone_state = Input.Basezone_state; return *this;
	}

	virtual unsigned Dump_size() const;
	virtual void Dump(char * Ptr) const;
	virtual bool Restore(char * Ptr);
};

//variable

//function

//main
C_board Game;
int main(){
	/*char Test[3] = "■";
	unsigned Loop1;
	for (Loop1 = 0; Loop1 < 10; Loop1++){
		gotoxy(Loop1, 0);
		cout << Loop1;
	}
	gotoxy(0, 0);
	cout << Test[0];
	C_time::Sleep_sec(2);
	gotoxy(1, 0);
	cout << Test[1];
	gotoxy(0, 5);*/
	return 0;
}

//function define
C_board::C_board(unsigned Base_size, unsigned Field_size, unsigned Brick_height)
	:BASE_SIZE(Base_size), FIELD_SIZE(Field_size), BRICK_HEIGHT(Brick_height),
	BOARD_SIZE(BASE_SIZE + (BRICK_HEIGHT + FIELD_SIZE) * 2), BOARD_SIZE_IN(BOARD_SIZE * BRICK_SIZE * INTERNAL_RESOLUTION),
	BACK_COLOR(C_screen_color::T_color::GREY),
	Balls(BALL_MAX_NUM),
	Screen(BOARD_SIZE*BRICK_SIZE * 2, BOARD_SIZE*BRICK_SIZE + BOARD_VERGE + 1, C_screen_color::BLACK, BACK_COLOR), 
	Damageup_speed((2 * C_board::BRICK_SIZE * C_board::INTERNAL_RESOLUTION / 5) / VECTOR_LENGTH){
	int Temp1;
	bool Loadstate;
	//Board할당.
	Board = new C_brick*[BOARD_SIZE];
	for (Temp1 = 0; Temp1 < (int)BOARD_SIZE; Temp1++){
		Board[Temp1] = new C_brick[BOARD_SIZE];
	}

	//게임 시작 전 정보 소개.
	system("color f0");
	system("title 4Way Brick");
	cout << "4방향 벽돌깨기" << endl;
	cout << "제작자: 이명진" << endl;
	cout << "if you can't read above letter, you need EUC-KR format. (Also required in game)" << endl << endl;

	cout << "반사판 움직이기: 방향키.(왼쪽 쉬프트를 같이누르면 3배 가속.)" << endl;
	cout << "반사판 돌리기: W,D (왼쪽 쉬프트를 같이누르면 2배 가속.)" << endl;
	cout << "벽돌 직접 추가하기: E" << endl;
	cout << "일시중지: ENTER" << endl;
	cout << "첫 공 발사: 스페이스키를 길게 눌렀다 떼기." << endl << endl;

	cout << "게임 룰." << endl;
	cout << "일정 시간마다 사방에서 벽돌이 나옵니다. 만약 그 벽돌이 중앙의 빨간 영역에 닿게 되면 게임오버입니다." << endl;
	cout << "공은 공마다 각각의 속도를 가지며, 반사판으로 공을 밀어서 속도를 증가시킬수 있습니다." << endl;
	cout << "공의 속도가 빠른 상태에서 벽돌을 깨면, 데미지와 점수가 높습니다." << endl;
	cout << "공이 반사판에 닿지 않고 중앙의 빨간 영역을 완전히 통과하면, 공의 속도가 최저로 떨어지며(공의 꼬리가 검정색), 이미 최저였던 경우에는 공이 사라집니다." << endl;
	cout << "만약에 모든 공이 다 사라지면, 그것도 게임오버입니다." << endl;
	cout << "벽돌 중에는 공을 생성하는 벽돌도 있습니다. 잘 활용하세요." << endl << endl;

	cout << "만약 플레이중에 도스창의 크기변경으로 인한 화면깨짐이 일어났다면, 스페이스키로 복구해주세요." << endl;
	cout << "정상적인 플레이를 위해서, 도스창을 우클릭하여 속성에서 폰트크기를 매우작게(7 정도) 설정해주세요." << endl;
	cout << "설정이 완료되었다면, 스페이스키로 진행해주세요.";

	Bestscore = 0;
	Loadstate = Load();
	C_keyboard::Check(C_keyboard::SPACE);
	while (C_keyboard::Check_out(C_keyboard::SPACE) == false){ ; }//스페이스를 떼면 콘솔크기변경.
	Screen.Setcolor(C_screen_color::BLACK, C_screen_color::WHITE);//출력 과정에서 검정색화면 뜨는것 방지.
	Screen.Set_consol();
	PLAY(Loadstate);
}
void C_board::Screen_init(){
	int Temp1, Temp2, H1, H2;
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	Screen.Clrscr();
	Draw_letter(0, 0, "Best Score:", "■", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
	Draw_letter(0, 6, "Score:", "■", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
	Draw_letter(0, 12, "Level:", "■", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
	Screen.Setcolor(BACK_COLOR, C_screen_color::BLACK);
	for (Temp1 = 0; Temp1 < int((2 * FIELD_SIZE + BASE_SIZE) * BRICK_SIZE) + 2; Temp1++){
		Screen << Coord((BRICK_HEIGHT * BRICK_SIZE + Temp1 - 1) * 2, BOARD_VERGE - 1) << "  ";
		Screen << Coord((BRICK_HEIGHT * BRICK_SIZE + Temp1 - 1) * 2, BOARD_VERGE + (2 * (FIELD_SIZE + BRICK_HEIGHT) + BASE_SIZE) * BRICK_SIZE) << "  ";
	}
	for (Temp1 = 0; Temp1 < (int)BOARD_SIZE; Temp1++){
		for (Temp2 = 0; Temp2 < (int)BOARD_SIZE; Temp2++){
			Print_brick(Temp2, Temp1);
		}
	}
	H1 = BRICK_HEIGHT * BRICK_SIZE - 1;
	H2 = (BOARD_SIZE - BRICK_HEIGHT) * BRICK_SIZE;
	Screen.Setcolor(C_screen_color::RED, C_screen_color::BLACK);
	for (Temp1 = 0; Temp1 <= H1; Temp1++){
		Screen << Coord(H1 * 2, Temp1 + BOARD_VERGE) << "  " << Coord(H2 * 2, Temp1 + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H1 + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H2 + BOARD_VERGE) << "  ";
	}
	for (Temp1 = H2; Temp1 < int(BOARD_SIZE*BRICK_SIZE); Temp1++){
		Screen << Coord(H1 * 2, Temp1 + BOARD_VERGE) << "  " << Coord(H2 * 2, Temp1 + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H1 + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H2 + BOARD_VERGE) << "  ";
	}
}
void C_board::Game_init(){
	int Temp1, Temp2, Vector_num;
	C_time Time;

	Level = 1; Score = 0, Play_brick_way = U, Levelup_tick = 1;
	//Board초기화.
	for (Temp1 = 0; Temp1 < (int)BOARD_SIZE; Temp1++){
		for (Temp2 = 0; Temp2 < (int)BOARD_SIZE; Temp2++){
			if ((int(BRICK_HEIGHT) <= Temp1&&Temp1 < int(BOARD_SIZE - BRICK_HEIGHT)) ^
				(int(BRICK_HEIGHT) <= Temp2&&Temp2 < int(BOARD_SIZE - BRICK_HEIGHT))){
				Brick_add(Temp2, Temp1);
			}
			else if ((int(BRICK_HEIGHT) <= Temp1&&Temp1 < int(BOARD_SIZE - BRICK_HEIGHT)) &&
				(int(BRICK_HEIGHT) <= Temp2&&Temp2 < int(BOARD_SIZE - BRICK_HEIGHT))){
				Board[Temp1][Temp2].Health = 0;
				Board[Temp1][Temp2].State = C_brick::NORMAL;
			}
		}
	}
	Board[BRICK_HEIGHT][BRICK_HEIGHT - 1].State = C_brick::BALLOUT;
	Board[BOARD_SIZE - BRICK_HEIGHT - 1][BOARD_SIZE - BRICK_HEIGHT].State = C_brick::BALLOUT;
	Playbrick_speed_tick = PLAYBRICK_SPEED_TICK_START;
	Playbrick_tick = 1;
	//공 초기화.
	while (Balls.Num()){ Balls.Out(); }
	//Reflector 초기화.
	Reflector.X = Reflector.Y = FLOOR(BOARD_SIZE_IN / 2, INTERNAL_RESOLUTION) + INTERNAL_RESOLUTION / 2;
	Reflector.Move_length = 3;
	Reflector.Length = CEIL(BRICK_SIZE * INTERNAL_RESOLUTION * 4 / (unsigned)3, VECTOR_LENGTH) / VECTOR_LENGTH;
	Reflector.Position = C_ball::DIRECTION_NUM / 4;
	//Screen 셋팅
	Screen_init();
	Print_score();
	Screen.Show_opt_kr();

	//공 발사.
	C_keyboard::Check(C_keyboard::SPACE);
	while (C_keyboard::Check_press(C_keyboard::SPACE) == false){ ; }//스페이스를 누르면 공 조준 시작.
	Vector_num = random(C_ball::DIRECTION_NUM);
	while (1){
		Time.Reset_time();
		Print_line(
			BOARD_SIZE_IN / 2,
			BOARD_SIZE_IN / 2,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][0] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][1] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			"  ",
			(BACK_COLOR * 16 + C_screen_color::BLACK) & 0xff
			);
		if (C_keyboard::Check_state(C_keyboard::SPACE) == false){ break; }//스페이스를 떼면 발사.
		Vector_num = (Vector_num + 1) % C_ball::DIRECTION_NUM;
		Print_line(
			BOARD_SIZE_IN / 2,
			BOARD_SIZE_IN / 2,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][0] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][1] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			"■", 
			(BACK_COLOR * 16 + C_screen_color::BLACK) & 0xff
			);
		Screen.Show_opt_kr();
		while (Time.Get_time() < OUTPUT_TICK){ ; }
	}
	Balls.In(
		C_ball(
		(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][0] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
		(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][1] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
		Vector_num
		)
		);
	Balls.In(
		C_ball(
		(int)BOARD_SIZE_IN / 2 - (C_ball::Vector[Vector_num][0] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
		(int)BOARD_SIZE_IN / 2 - (C_ball::Vector[Vector_num][1] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
		(Vector_num + C_ball::DIRECTION_NUM / 2) % C_ball::DIRECTION_NUM
		)
		);

	Print_Reflector();
	Print_hitting_zone();
}
void C_board::PLAY(bool Loadstate){
	bool Check = false;
	unsigned Temp1, Temp2;
	C_time Time;
	C_time Stime;
	int Lag_check;

#if MYDEBUG
	char * TTTest = NULL;
#endif
#ifdef MYDEBUG
	C_time Time_test;
	unsigned Time1 = 0, Time2 = 0, Time3 = 0, Time4 = 0, Time5 = 0, Time6 = 0;
#endif

	while (1){
		//게임의 시작. 공 쏘기.
		if (!Loadstate){
			Game_init();
		}
		else {
			//게임 상태 로드 경우.
			//화면 출력.
			Screen_init();
			for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
				for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){
					Print_brick(Temp2, Temp1);
				}
			}
			Print_hitting_zone();
			Print_Reflector();
			for (Temp1 = 0; Temp1 < Balls.Num(); Temp1++){
				Print_ball(Temp1);
			}
			Print_score();
			Screen_recorvery();
			//카운트다운.
			Countdown();
			Loadstate = false;
		}
		Check = true;
		C_keyboard::Check(C_keyboard::D);
		C_keyboard::Check(C_keyboard::W);
		C_keyboard::Check(C_keyboard::RIGHT);
		C_keyboard::Check(C_keyboard::LEFT);
		C_keyboard::Check(C_keyboard::UP);
		C_keyboard::Check(C_keyboard::DOWN);
		C_keyboard::Check(C_keyboard::SPACE);
		C_keyboard::Check(C_keyboard::LSHIFT);
		C_keyboard::Check(C_keyboard::E);
		C_keyboard::Check(C_keyboard::ENTER);
#if MYDEBUG
		C_keyboard::Check(C_keyboard::U);
		C_keyboard::Check(C_keyboard::I);
#endif

#ifdef MYDEBUG
		C_keyboard::Check(C_keyboard::NUM1);
		C_keyboard::Check(C_keyboard::NUM2);
		C_keyboard::Check(C_keyboard::NUM3);
		C_keyboard::Check(C_keyboard::NUM4);
		C_keyboard::Check(C_keyboard::NUM5);
		C_keyboard::Check(C_keyboard::NUM6);
		C_keyboard::Check(C_keyboard::NUM7);
		C_keyboard::Check(C_keyboard::NUM8);
		C_keyboard::Check(C_keyboard::A);
#endif
		Stime.Reset_time();
		Time.Reset_time();
		while (Check){//한 프레임.
			Time.Reset_time();//시간 초기화.

#ifdef MYDEBUG
			if (C_keyboard::Check_press(C_keyboard::NUM4)){
				Balls.Read_editable(0)->Speed_tick++;
			}
			if (C_keyboard::Check_press(C_keyboard::NUM3)){
				Balls.Read_editable(0)->Speed_tick--;
				if (Balls.Read(0).Speed_tick == 0){
					Balls.Read_editable(0)->Speed_tick = 1;
				}
				if (Balls.Read(0).Tick > Balls.Read(0).Speed_tick){
					Balls.Read_editable(0)->Tick = Balls.Read(0).Speed_tick;
				}
			}
			if (C_keyboard::Check_press(C_keyboard::NUM1)){
				Balls.Read_editable(0)->Speed_movenum--;
				if (Balls.Read(0).Speed_movenum == 0){
					Balls.Read_editable(0)->Speed_movenum = 1;
				}
			}
			if (C_keyboard::Check_press(C_keyboard::NUM2)){
				Balls.Read_editable(0)->Speed_movenum++;
			}
			Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
			for (Temp1 = 0; Temp1 < Screen.GetL(); Temp1 += 2){
				if (Temp1 < (Balls.Read(0).Speed_tick) * 2){
					Screen << Coord(Temp1, 1) << "■";
				}
				else {
					Screen << Coord(Temp1, 1) << "  ";
				}
			}
			for (Temp1 = 0; Temp1 < Screen.GetL(); Temp1 += 2){
				if (Temp1 < (Balls.Read(0).Speed_movenum) * 2){
					Screen << Coord(Temp1, 0) << "■";
				}
				else {
					Screen << Coord(Temp1, 0) << "  ";
				}
			}
			if (Balls.Num() >= 2){
				if (C_keyboard::Check_press(C_keyboard::NUM8)){
					Balls.Read_editable(1)->Speed_tick++;
				}
				if (C_keyboard::Check_press(C_keyboard::NUM7)){
					Balls.Read_editable(1)->Speed_tick--;
					if (Balls.Read(1).Speed_tick == 0){
						Balls.Read_editable(1)->Speed_tick = 1;
					}
					if (Balls.Read(1).Tick > Balls.Read(1).Speed_tick){
						Balls.Read_editable(1)->Tick = Balls.Read(1).Speed_tick;
					}
				}
				if (C_keyboard::Check_press(C_keyboard::NUM5)){
					Balls.Read_editable(1)->Speed_movenum--;
					if (Balls.Read(1).Speed_movenum == 0){
						Balls.Read_editable(1)->Speed_movenum = 1;
					}
				}
				if (C_keyboard::Check_press(C_keyboard::NUM6)){
					Balls.Read_editable(1)->Speed_movenum++;
				}
				Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
				for (Temp1 = 0; Temp1 < Screen.GetL(); Temp1 += 2){
					if (Temp1 < (Balls.Read(1).Speed_tick) * 2){
						Screen << Coord(Temp1, 4) << "■";
					}
					else {
						Screen << Coord(Temp1, 4) << "  ";
					}
				}
				for (Temp1 = 0; Temp1 < Screen.GetL(); Temp1 += 2){
					if (Temp1 < (Balls.Read(1).Speed_movenum) * 2){
						Screen << Coord(Temp1, 3) << "■";
					}
					else {
						Screen << Coord(Temp1, 3) << "  ";
					}
				}
			}
#endif

			//게임의 플레이. 루프 및 지속적 Reflector조작
			if (Levelup_tick >= (LEVELUP_SEC * 1000) / OUTPUT_TICK){
				Levelup_tick = 1;
				Level++;
			}
			else { Levelup_tick++; }
			//Some Object Delete Screen(for optimization)
#ifdef MYDEBUG
			Time_test.Reset_time();
#endif
			for (Temp1 = 0; Temp1 < BALL_MAX_NUM; Temp1++){
				Print_ball(Temp1, false);
			}
			Print_Reflector(false);
#ifdef MYDEBUG
			Time1 += Time_test.Get_time();
#endif

#ifdef MYDEBUG
			Time_test.Reset_time();
#endif
			//Key Input.
			if (C_keyboard::Check_state(C_keyboard::RIGHT)){
				if (C_keyboard::Check_state(C_keyboard::LSHIFT)){
					Move_reflector(R, 3);
				}
				else { Move_reflector(R); }
			}
			if (C_keyboard::Check_state(C_keyboard::LEFT)){
				if (C_keyboard::Check_state(C_keyboard::LSHIFT)){
					Move_reflector(L, 3);
				}
				else { Move_reflector(L); }
			}
			if (C_keyboard::Check_state(C_keyboard::UP)){
				if (C_keyboard::Check_state(C_keyboard::LSHIFT)){
					Move_reflector(U, 3);
				}
				else { Move_reflector(U); }
			}
			if (C_keyboard::Check_state(C_keyboard::DOWN)){
				if (C_keyboard::Check_state(C_keyboard::LSHIFT)){
					Move_reflector(D, 3);
				}
				else { Move_reflector(D); }
			}
			if (C_keyboard::Check_state(C_keyboard::W)){
				if (C_keyboard::Check_state(C_keyboard::LSHIFT)){
					Move_reflector(UL);//시계반대방향 회전.
				}
				Move_reflector(UL);//시계반대방향 회전.
			}
			if (C_keyboard::Check_state(C_keyboard::D)){
				if (C_keyboard::Check_state(C_keyboard::LSHIFT)){
					Move_reflector(UR);//시계방향 회전.
				}
				Move_reflector(UR);//시계방향 회전.
			}
			if (C_keyboard::Check_press(C_keyboard::SPACE)){
				Screen_recorvery();
			}
			if (C_keyboard::Check_press(C_keyboard::E)){
				if (Play_brick()){
					Check = false;//게임오버.
				}
			}
			if (C_keyboard::Check_state(C_keyboard::ENTER)){//게임 일시중지.
				Gamestop();
				Time.Reset_time();
			}
#if MYDEBUG
			if (C_keyboard::Check_state(C_keyboard::U)){//게임 벡업(테스트 코드).
				this->Dump_start(TTTest);
			}
			if (C_keyboard::Check_state(C_keyboard::I)){//게임 복원(테스트 코드).
				this->Restore(TTTest);
				Time.Reset_time();
				Screen_init();
				for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
					for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){
						Print_brick(Temp2, Temp1);
					}
				}
			}
#endif
#ifdef MYDEBUG
			Time2+=Time_test.Get_time();
#endif
#ifdef MYDEBUG
			if (C_keyboard::Check_state(C_keyboard::A)){//누르면 멈춤.
				while (!C_keyboard::Check_press(C_keyboard::A)){ ; }//다시 누르고.
				while (C_keyboard::Check_state(C_keyboard::A)){ ; }//떼면 시작.
			}
#endif

#ifdef MYDEBUG
			Time_test.Reset_time();
#endif
			//brick Play
			if (Playbrick_tick == Playbrick_speed_tick){
				Playbrick_tick = 1;
				Playbrick_speed_tick = int(Level_func(Level, PLAYBRICK_SPEED_TICK_START, PLAYBRICK_SPEED_TICK_END, 600 / LEVELUP_SEC, 100).Get_value());
				if (Play_brick()){
					Check = false;
				}//게임오버.
			}
			else { Playbrick_tick++; }
#ifdef MYDEBUG
			Time3 += Time_test.Get_time();
#endif

#ifdef MYDEBUG
			Time_test.Reset_time();
#endif
			//Ball Play.
			for (Temp1 = 0; Temp1 < Balls.Num(); Temp1++){
				if (Play_ball(Temp1)){
					//공 삭제 필요.
					if (Temp1 == 0){//첫번째공인경우.
						Balls.Out();//첫번째공 삭제.
					}
					else {
						*(Balls.Read_editable(Temp1)) = Balls.Read(0);//제일 첫번째공으로 덮어씌움.
						Balls.Out();//첫번째공 삭제.
						Temp1--;
					}
				}
			}
			if (Balls.Num() == 0){
				Check = false;//게임오버.
			}
#ifdef MYDEBUG
			Time4+=Time_test.Get_time();
#endif

#ifdef MYDEBUG
			Time_test.Reset_time();
#endif
			//화면 출력.
			Print_hitting_zone();
			Print_Reflector();
			for (Temp1 = 0; Temp1 < Balls.Num(); Temp1++){
				Print_ball(Temp1);
			}
			Print_score();

#ifdef MYDEBUG
			Time5+=Time_test.Get_time();
#endif
#ifdef MYDEBUG
			Time_test.Reset_time();
#endif
			Lag_check = (int)OUTPUT_TICK - Time.Get_time() - 5;
			if (Lag_check>0){
				Screen.Show_opt_kr(Lag_check);
			}
			//Screen.Show_opt_kr();
#ifdef MYDEBUG
			Time6 += Time_test.Get_time();
#endif

			Lag_check = (int)OUTPUT_TICK - Time.Get_time();//잔여 시간.
			if (Lag_check < 0){//시간 지연 발생.
				Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
				for (Temp1 = 0; Temp1 < BRICK_HEIGHT*BRICK_SIZE * 2 - 2; Temp1 += 2){
					if ((int)Temp1 * 3 < -1 * Lag_check){//네모 칸수*6이 지연 시간(ms)
						Screen << Coord(Temp1, Screen.GetH() - 1) << "■";
					}
					else {
						Screen << Coord(Temp1, Screen.GetH() - 1) << "  ";
					}
				}
			}
			else {
				Screen.Setcolor(C_screen_color::GREEN, BACK_COLOR);
				for (Temp1 = 0; Temp1 < BRICK_HEIGHT*BRICK_SIZE * 2 - 2; Temp1 += 2){
					if ((int)Temp1 * 3 < Lag_check){
						Screen << Coord(Temp1, Screen.GetH() - 1) << "■";
					}
					else {
						Screen << Coord(Temp1, Screen.GetH() - 1) << "  ";
					}
				}
			}

			while (Time.Get_time() < OUTPUT_TICK){ ; }//속도조절구문.
		}
		//게임 오버.
		Screen.Show_opt_kr();
		if (Score > Bestscore){ Bestscore = Score; }
		Save_score();
		//재시도 및 종료 점검.
		Screen.Setcolor(C_screen_color::BLACK, C_screen_color::WHITE);
		for (Temp1 = BOARD_VERGE; Temp1 < BOARD_VERGE + 6 * 3; Temp1++){
			Screen << Coord(0, Temp1);
			for (Temp2 = 0; Temp2 < Screen.GetL(); Temp2++){
				Screen << ' ';
			}
		}
		Draw_letter(ROUND(Screen.GetL() / 2, 2) - 48, BOARD_VERGE, "game over!!", "■", char((16 * C_screen_color::WHITE + C_screen_color::RED) & 0xff));
		Draw_letter(ROUND(Screen.GetL() / 2, 2) - 60, BOARD_VERGE + 6, "Retry:ENTER KEY", "■", char((16 * C_screen_color::WHITE + C_screen_color::BLACK) & 0xff));
		Draw_letter(ROUND(Screen.GetL() / 2, 2) - 36, BOARD_VERGE + 12, "END:E KEY", "■", char((16 * C_screen_color::WHITE + C_screen_color::BLACK) & 0xff));
		Screen.Show_opt_kr();
		while (1){
			if (C_keyboard::Check_state(C_keyboard::ENTER)){
				Check = true;
				break;
			}
			if (C_keyboard::Check_state(C_keyboard::E)){
				Check = false;
				break;
			}
		}
		if (Check == false){ break; }//종료.
		//종료 아니면 재시도.
	}
}
void C_board::Gamestop(){
	unsigned Temp1, Temp2;
	bool Check;

	//오브젝트 제거.
	for (Temp1 = 0; Temp1 < BALL_MAX_NUM; Temp1++){
		Print_ball(Temp1, false);
	}
	Print_Reflector(false);

	C_screen_color Screen_temp(Screen);//임시 화면.
	Print_hitting_zone();
	Screen_temp = Screen;
	Screen_temp.Setcolor(C_screen_color::BLACK, C_screen_color::WHITE);
	for (Temp1 = BOARD_VERGE; Temp1 < BOARD_VERGE + 12; Temp1++){
		Screen_temp << Coord(0, Temp1);
		for (Temp2 = 0; Temp2 < Screen.GetL(); Temp2++){
			Screen_temp << ' ';
		}
	}
	Draw_letter_setscreen(Screen_temp, ROUND(Screen.GetL() / 2, 2) - 64, BOARD_VERGE, "ENTER to continue", "■", char((16 * C_screen_color::WHITE + C_screen_color::BLACK) & 0xff));
	Draw_letter_setscreen(Screen_temp, ROUND(Screen.GetL() / 2, 2) - 72, BOARD_VERGE + 6, "S to save and exit", "■", char((16 * C_screen_color::WHITE + C_screen_color::BLACK) & 0xff));
	Screen_temp.Show_opt_kr();
	while (C_keyboard::Check_state(C_keyboard::ENTER)){ ; }//엔터키를 뗄때까지 기다림.
	while (_kbhit()){ _getch(); }//키보드 버퍼 리셋.
	while (1){
		if (_kbhit()){//사용자가 이 도스창에 키를 넣을 때.
			if (C_keyboard::Check_state(C_keyboard::ENTER)){//엔터이면.
				while (C_keyboard::Check_state(C_keyboard::ENTER)){ ; }//엔터키를 뗄때까지 기다림.
				break;
			}
			else if (C_keyboard::Check_state(C_keyboard::S)){//S키이면.
				Check = Save_state();//게임 상태 저장.
				Screen_temp.Setcolor(C_screen_color::BLACK, C_screen_color::WHITE);
				for (Temp1 = BOARD_VERGE; Temp1 < BOARD_VERGE + 12; Temp1++){
					Screen_temp << Coord(0, Temp1);
					for (Temp2 = 0; Temp2 < Screen.GetL(); Temp2++){
						Screen_temp << ' ';
					}
				}
				if (Check){
					Draw_letter_setscreen(Screen_temp, ROUND(Screen.GetL() / 2, 2) - 56, BOARD_VERGE, "Save complate", "■", char((16 * C_screen_color::WHITE + C_screen_color::BLACK) & 0xff));
				}
				else {
					Draw_letter_setscreen(Screen_temp, ROUND(Screen.GetL() / 2, 2) - 32, BOARD_VERGE, "Save Fail", "■", char((16 * C_screen_color::WHITE + C_screen_color::RED) & 0xff));
				}
				Draw_letter_setscreen(Screen_temp, ROUND(Screen.GetL() / 2, 2) - 64, BOARD_VERGE + 6, "ENTER to continue", "■", char((16 * C_screen_color::WHITE + C_screen_color::BLACK) & 0xff));
				Screen_temp.Show_opt_kr();
				while (_kbhit()){ _getch(); }//키보드 버퍼 리셋.
				while (1){
					if (_kbhit()){//사용자가 이 도스창에 키를 넣을 때.
						if (C_keyboard::Check_state(C_keyboard::ENTER)){//엔터이면.
							if (Check){
								exit(0);
							}
							else {
								break;
							}
						}
					}
				}
			}
			else {
				while (_kbhit()){ _getch(); }//키보드 버퍼 리셋.
			}
		}
	}
	//오브젝트 출력.
	Print_hitting_zone();
	Print_Reflector();
	for (Temp1 = 0; Temp1 < Balls.Num(); Temp1++){
		Print_ball(Temp1);
	}
	Screen.Show();
	Countdown();
}
void C_board::Countdown(){
	C_time Time;
	char Buf[50];
	int Loop1;
	Draw_letter(ROUND(Screen.GetL() / 2, 2) - 4, (Screen.GetH() + BOARD_VERGE) / 2 - 2, "4", "●", ((BACK_COLOR)* 16 + C_screen_color::RED) & 0xff);
	for (Loop1 = 3; Loop1 >= 0; Loop1--){
		Time.Reset_time();
		Screen.Show_opt_kr();
		if (Loop1){
			sprintf_s(Buf, "%d", Loop1);
			Draw_letter(ROUND(Screen.GetL() / 2, 2) - 4, (Screen.GetH() + BOARD_VERGE) / 2 - 2, Buf, "●", ((BACK_COLOR)* 16 + C_screen_color::RED) & 0xff);
		}
		else {
			Draw_letter(ROUND(Screen.GetL() / 2, 2) - 4, (Screen.GetH() + BOARD_VERGE) / 2 - 2, " ", "●", ((BACK_COLOR)* 16 + C_screen_color::RED) & 0xff);
		}
		while (Time.Get_time() < 1000){ ; }
	}
}
bool C_board::Save_state(){
	ofstream Savefile("Savestate", ios_base::out | ios_base::trunc | ios_base::binary);
	char * Dump_ptr = NULL;
	unsigned Size;

	if (!Savefile.is_open()){ return false; }//파일 열기 실패.
	Size = Dump_start(Dump_ptr);
	if (Size){
		//덤프 성공.
		Myencrypt(Dump_ptr, Size);
		Savefile.write(Dump_ptr, Size);
		Savefile.close();
		return true;
	}
	else {//덤프 실패.
		Savefile.close();
		return false;
	}
}
void C_board::Save_score(){
	ofstream Savefile("Savescore", ios_base::out | ios_base::trunc | ios_base::binary);
	if (!Savefile.is_open()){ return; }//파일 열기 실패.
	char * const Str = new char[sizeof(unsigned) + 2];
	char * Ptr = Str;
	if (Str == NULL){//동적할당실패..할일없겠지만.
		Savefile.close();
		return;
	}

	*(unsigned *)Ptr = Bestscore;//점수 저장.
	Ptr += sizeof(unsigned);
	*Ptr = (char)0xab;//암호 코드.
	Ptr += sizeof(char);
	*Ptr = (char)0xdd;//암호 코드.

	Ptr = Str;
	Myencrypt(Ptr, sizeof(unsigned) + 2);//Xor암호화.

	Savefile.write(Str, sizeof(unsigned) + 2);
	Savefile.close();
	delete[] Str;
}
bool C_board::Load(){
	char * Str;
	unsigned Size;
	bool Loadstate = false;

	//점수 파일.
	ifstream Scorefile("Savescore", ios_base::in | ios_base::binary);
	if (Scorefile.is_open()){
		Size = sizeof(unsigned) + 2;
		Str = new char[Size];
		if (Str != NULL){

			//정상 파일오픈, 동적할당 코드.
			Scorefile.read(Str, Size);
			if (Scorefile.gcount() == Size){//정상 길이로 읽은 경우.
				Mydecrypt(Str, Size);
				if ((*(Str + Size - 2) & 0xff)== 0xab && (*(Str + Size - 1) & 0xff) == 0xdd){//암호검사코드.
					Bestscore = *(unsigned *)Str;
				}
			}

			delete[] Str;
		}
		Scorefile.close();
	}

	//상태 파일.
	ifstream Statefile("Savestate", ios_base::in | ios_base::binary);
	if (Statefile.is_open()){
		Statefile.seekg(0, ios_base::end);
		Size = (unsigned)Statefile.tellg().seekpos();
		Statefile.seekg(0, ios_base::beg);
		if (Size >= sizeof(unsigned)){//파일 길이 존재.
			Str = new char[Size];
			if (Str != NULL){

				//정상 파일오픈, 동적할당 코드.
				Statefile.read(Str, Size);
				if (Statefile.gcount() == Size){//정상 길이로 읽은 경우.
					Mydecrypt(Str, Size);
					if (*(unsigned *)Str == Size){//덤프파일에 기록된 길이와도 일치해야함.
						Loadstate = Restore(Str);
					}
				}

				delete[] Str;
			}
		}
		Statefile.close();
		//기존 파일 삭제 코드.
		ofstream Delete_file("Savestate", ios_base::out | ios_base::trunc | ios_base::binary);
		if (Delete_file.is_open()){
			Delete_file.close();
		}
	}
	return Loadstate;
}

void C_board::Myencrypt(char * Ptr, unsigned Size){
	unsigned Loop1;
	for (Loop1 = 0; Loop1 < Size; Loop1++){
		if (Loop1 == Size - 1){
			*Ptr = *(Ptr + Loop1) ^ *Ptr;
		}
		else {
			*(Ptr + Loop1 + 1) = *(Ptr + Loop1) ^ *(Ptr + Loop1 + 1);
		}
	}
}
void C_board::Mydecrypt(char * Ptr, unsigned Size){
	unsigned Loop1;
	*Ptr = *Ptr ^ *(Ptr + Size - 1);
	for (Loop1 = Size - 2; true; Loop1--){
		*(Ptr + Loop1 + 1) = *(Ptr + Loop1) ^ *(Ptr + Loop1 + 1);
		if (Loop1 == 0){ break; }
	}
}

void C_board::Print_ball(unsigned Ball_num, bool Print){
	int Temp1;
	int X_ref, Y_ref;
	char Color;
	char Color_speed[6] = {
		(BACK_COLOR * 16 + C_screen_color::BLACK) & 0xff,
		(BACK_COLOR * 16 + C_screen_color::D_SKYBLUE) & 0xff,
		(BACK_COLOR * 16 + C_screen_color::BLUE) & 0xff,
		(BACK_COLOR * 16 + C_screen_color::D_BLUE) & 0xff,
		(BACK_COLOR * 16 + C_screen_color::VIOLET) & 0xff,
		(BACK_COLOR * 16 + C_screen_color::RED) & 0xff,
	};
	char *Str;
	if (Print){ Str = "■"; }
	else { Str = "  "; }
	if (Ball_num >= Balls.Num()){ return; }
	C_queue_dump<C_ball::C_ball_vector> Ball_rec = Balls.Read(Ball_num).Ball_rec;

	for (Temp1 = 0; Temp1 < (int)Ball_rec.Num() - 1; Temp1++){//공 그림자.(무반사).
		Color = (BACK_COLOR * 16 + C_screen_color::D_GREY) & 0xff;
		if (Temp1 >= (int)Ball_rec.Num() - (int)C_ball::SHADOW_NUM / 5){
			if (Balls.Read(Ball_num).Speed_movenum <= Balls.Read(Ball_num).Speed_tick){
				Color = Color_speed[0];
			}
			else {
				Color = Color_speed[int(CEIL(Balls.Read(Ball_num).Speed_movenum / Balls.Read(Ball_num).Speed_tick, Damageup_speed) / Damageup_speed)];
			}
		}
		if (Ball_rec.Read(Temp1 + 1).State == C_ball::C_ball_vector::NONE){//무반사 경우.
			Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1 + 1).X, Ball_rec.Read(Temp1 + 1).Y, Str, Color);
		}
	}
	for (Temp1 = 0; Temp1 < (int)Ball_rec.Num() - 1; Temp1++){//공 그림자.(반사.)
		Color = (BACK_COLOR * 16 + C_screen_color::D_GREY) & 0xff;
		if (Temp1 >= (int)Ball_rec.Num() - (int)C_ball::SHADOW_NUM / 5){
			if (Balls.Read(Ball_num).Speed_movenum <= Balls.Read(Ball_num).Speed_tick){
				Color = Color_speed[0];
			}
			else {
				Color = Color_speed[int(CEIL(Balls.Read(Ball_num).Speed_movenum / Balls.Read(Ball_num).Speed_tick, Damageup_speed) / Damageup_speed)];
			}
		}
		if (Ball_rec.Read(Temp1 + 1).State == C_ball::C_ball_vector::REF_REF){//Reflector반사.
			Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1 + 1).X, Ball_rec.Read(Temp1 + 1).Y, Str, Color);
			Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
			Screen << Coord(
				2 * ((Ball_rec.Read(Temp1).Vector_X() > 0) ? CEIL(Ball_rec.Read(Temp1 + 1).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION - 1 : FLOOR(Ball_rec.Read(Temp1 + 1).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION),
				((Ball_rec.Read(Temp1).Vector_Y() > 0) ? CEIL(Ball_rec.Read(Temp1 + 1).Y, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION - 1 : FLOOR(Ball_rec.Read(Temp1 + 1).Y, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION) + BOARD_VERGE
				) << Str;
		}
		else if (Ball_rec.Read(Temp1 + 1).State == C_ball::C_ball_vector::BRICK_REF){//벽돌 반사.
			if (Ball_rec.Read(Temp1).Vector_X() != Ball_rec.Read(Temp1 + 1).Vector_X()){
				X_ref = (Ball_rec.Read(Temp1).X + Ball_rec.Read(Temp1).Vector_X() + Ball_rec.Read(Temp1 + 1).X) / 2;
			}
			else { X_ref = -1; }
			if (Ball_rec.Read(Temp1).Vector_Y() != Ball_rec.Read(Temp1 + 1).Vector_Y()){
				Y_ref = (Ball_rec.Read(Temp1).Y + Ball_rec.Read(Temp1).Vector_Y() + Ball_rec.Read(Temp1 + 1).Y) / 2;
			}
			else { Y_ref = -1; }
			Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1).X + Ball_rec.Read(Temp1).Vector_X(), Ball_rec.Read(Temp1).Y + Ball_rec.Read(Temp1).Vector_Y(), Str, Color, X_ref, Y_ref, 0xff & (BACK_COLOR * 16 + C_screen_color::RED));
		}
	}
	//현재 공.
	Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
	Screen << Coord(
		((Ball_rec.Read(Temp1).Vector_X() >= 0) ? FLOOR(Ball_rec.Read(Temp1).X, INTERNAL_RESOLUTION) : CEIL(Ball_rec.Read(Temp1).X, INTERNAL_RESOLUTION) - INTERNAL_RESOLUTION) * 2 / INTERNAL_RESOLUTION,
		(((Ball_rec.Read(Temp1).Vector_Y() >= 0) ? FLOOR(Ball_rec.Read(Temp1).Y, INTERNAL_RESOLUTION) : CEIL(Ball_rec.Read(Temp1).Y, INTERNAL_RESOLUTION) - INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION) + BOARD_VERGE
		) << Str;
}
void C_board::Print_Reflector(bool Print){
	int X1, Y1, X2, Y2;
	char *Str;
	if (Print){ Str = "■"; }
	else { Str = "  "; }
	Get_reflector_edge(&X1, &Y1, &X2, &Y2);
	Print_line(X1, Y1, X2, Y2, Str, (BACK_COLOR * 16 + C_screen_color::BLACK) & 0xff);
}
void C_board::Print_brick(unsigned X, unsigned Y){
	if (X >= BOARD_SIZE){ return; }
	if (Y >= BOARD_SIZE){ return; }

	int Temp1, Temp2;
	C_screen_color::T_color col;
	unsigned X1 = X * BRICK_SIZE;
	unsigned X2 = (X + 1) * BRICK_SIZE - 1;
	unsigned Y1 = Y * BRICK_SIZE;
	unsigned Y2 = (Y + 1) * BRICK_SIZE - 1;
	if (Board[Y][X].State == C_brick::NONE){ return; }

	"";//벽돌속성추가.
	switch (Board[Y][X].State){
	case C_brick::NORMAL:
	case C_brick::BALLOUT:
		if (Board[Y][X].Health >= 5){
			col = C_screen_color::RED;
		}
		else {
			switch (Board[Y][X].Health){
			case 4: col = C_screen_color::VIOLET; break;
			case 3: col = C_screen_color::D_BLUE; break;
			case 2: col = C_screen_color::BLUE; break;
			case 1: col = C_screen_color::D_SKYBLUE; break;
			case 0: col = C_screen_color::BLACK; break;
			}
		}
		Screen.Setcolor(col, BACK_COLOR);
		if (Board[Y][X].Health == 0){
			for (Temp1 = X1; Temp1 <= (int)X2; Temp1++){
				for (Temp2 = Y1; Temp2 <= (int)Y2; Temp2++){
					Screen << Coord(Temp1 * 2, Temp2 + BOARD_VERGE) << "  ";
				}
			}
		}
		else if (Board[Y][X].Health > 0){
			//블럭 테두리.
			Screen << Coord(X1 * 2, Y1 + BOARD_VERGE) << "■";
			Screen << Coord(X1 * 2, Y2 + BOARD_VERGE) << "■";
			Screen << Coord(X2 * 2, Y1 + BOARD_VERGE) << "■";
			Screen << Coord(X2 * 2, Y2 + BOARD_VERGE) << "■";
			for (Temp1 = 0; Temp1 < BRICK_SIZE - 2; Temp1++){
				Screen << Coord(X1 * 2, Y1 + Temp1 + 1 + BOARD_VERGE) << "■";
				Screen << Coord(X2 * 2, Y1 + Temp1 + 1 + BOARD_VERGE) << "■";
				Screen << Coord((X1 + Temp1 + 1) * 2, Y1 + BOARD_VERGE) << "■";
				Screen << Coord((X1 + Temp1 + 1) * 2, Y2 + BOARD_VERGE) << "■";
			}
			//블럭 내부.
			if (Board[Y][X].State == C_brick::NORMAL){
				for (Temp1 = 0; Temp1 < BRICK_SIZE - 2; Temp1++){
					for (Temp2 = 0; Temp2 < BRICK_SIZE - 2; Temp2++){
						Screen << Coord((X1 + Temp1 + 1) * 2, Y1 + Temp2 + 1 + BOARD_VERGE) << "  ";
					}
				}
			}
			if (Board[Y][X].State == C_brick::BALLOUT){
				for (Temp1 = 0; Temp1 < BRICK_SIZE - 2; Temp1++){
					for (Temp2 = 0; Temp2 < BRICK_SIZE - 2; Temp2++){
						if (Temp1 == Temp2 || Temp1 + Temp2 == BRICK_SIZE - 3){
							Screen.Setcolor(BACK_COLOR, col);
						}
						else {
							Screen.Setcolor(col, BACK_COLOR);
						}
						Screen << Coord((X1 + Temp1 + 1) * 2, Y1 + Temp2 + 1 + BOARD_VERGE) << "  ";
					}
				}
			}
		}
		break;
	default:
		break;
	}
}
void C_board::Print_hitting_zone(){
	static int Verge = Dead_verge();
	int H[4];
	int Temp1;
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	if (Verge != Dead_verge()){//Verge달라짐. 지우기 필요.
		H[0] = (BRICK_HEIGHT + FIELD_SIZE - Verge + 1) * BRICK_SIZE;
		H[1] = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge - 1) * BRICK_SIZE - 1;
		for (Temp1 = H[0]; Temp1 <= H[1]; Temp1++){
			Screen << Coord(Temp1 * 2, H[0] + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H[1] + BOARD_VERGE) << "  ";
			Screen << Coord(H[0] * 2, Temp1 + BOARD_VERGE) << "  " << Coord(H[1] * 2, Temp1 + BOARD_VERGE) << "  ";
		}
	}
	//출력.
	Verge = Dead_verge();
	H[0] = (BRICK_HEIGHT + FIELD_SIZE - Verge + 1) * BRICK_SIZE;
	H[1] = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge - 1) * BRICK_SIZE - 1;
	H[2] = (BRICK_HEIGHT + FIELD_SIZE) * BRICK_SIZE;
	H[3] = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE) * BRICK_SIZE - 1;
	for (Temp1 = H[0]; Temp1 <= H[1]; Temp1++){
		Screen << Coord(Temp1 * 2, H[0] + BOARD_VERGE) << "━" << Coord(Temp1 * 2, H[1] + BOARD_VERGE) << "━";
		Screen << Coord(H[0] * 2, Temp1 + BOARD_VERGE) << "┃" << Coord(H[1] * 2, Temp1 + BOARD_VERGE) << "┃";
	}
	Screen << Coord(H[0] * 2, H[0] + BOARD_VERGE) << "┏" << Coord(H[0] * 2, H[1] + BOARD_VERGE) << "┗" << Coord(H[1] * 2, H[1] + BOARD_VERGE) << "┛" << Coord(H[1] * 2, H[0] + BOARD_VERGE) << "┓";
	Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
	if (Verge <= 1){ Screen.Reverse_color(); }
	for (Temp1 = H[2]; Temp1 <= H[3]; Temp1++){
		if (Verge <= 1){
			Screen << Coord(Temp1 * 2, H[2] + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H[3] + BOARD_VERGE) << "  ";
			Screen << Coord(H[2] * 2, Temp1 + BOARD_VERGE) << "  " << Coord(H[3] * 2, Temp1 + BOARD_VERGE) << "  ";
		}
		else {
			Screen << Coord(Temp1 * 2, H[2] + BOARD_VERGE) << "━" << Coord(Temp1 * 2, H[3] + BOARD_VERGE) << "━";
			Screen << Coord(H[2] * 2, Temp1 + BOARD_VERGE) << "┃" << Coord(H[3] * 2, Temp1 + BOARD_VERGE) << "┃";
		}
	}
	if (Verge <= 1){
		Screen << Coord(H[2] * 2, H[2] + BOARD_VERGE) << "  " << Coord(H[2] * 2, H[3] + BOARD_VERGE) << "  " << Coord(H[3] * 2, H[3] + BOARD_VERGE) << "  " << Coord(H[3] * 2, H[2] + BOARD_VERGE) << "  ";
	}
	else {
		Screen << Coord(H[2] * 2, H[2] + BOARD_VERGE) << "┏" << Coord(H[2] * 2, H[3] + BOARD_VERGE) << "┗" << Coord(H[3] * 2, H[3] + BOARD_VERGE) << "┛" << Coord(H[3] * 2, H[2] + BOARD_VERGE) << "┓";
	}
}
void C_board::Print_score(){
	char Buf[50];
	sprintf_s(Buf, "%9d", Bestscore);
	Draw_letter(88, 0, Buf, "●", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
	sprintf_s(Buf, "%9d", Score);
	Draw_letter(48, 6, Buf, "●", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
	sprintf_s(Buf, "%9d", Level);
	Draw_letter(48, 12, Buf, "●", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
}
void C_board::Get_reflector_edge(int * X1, int * Y1, int * X2, int * Y2){
	*X1 = Reflector.X + (int)Reflector.Length * C_ball::Vector[(Reflector.Position + C_ball::DIRECTION_NUM / 4) % C_ball::DIRECTION_NUM][0];
	*Y1 = Reflector.Y + (int)Reflector.Length  * C_ball::Vector[(Reflector.Position + C_ball::DIRECTION_NUM / 4) % C_ball::DIRECTION_NUM][1];
	*X2 = Reflector.X + (int)Reflector.Length  * C_ball::Vector[(Reflector.Position + C_ball::DIRECTION_NUM * 3 / 4) % C_ball::DIRECTION_NUM][0];
	*Y2 = Reflector.Y + (int)Reflector.Length  * C_ball::Vector[(Reflector.Position + C_ball::DIRECTION_NUM * 3 / 4) % C_ball::DIRECTION_NUM][1];
}
int C_board::Dead_verge(){
	int Loop1, Loop2;
	bool Check = false;
	for (Loop1 = -1; Loop1 < (int)FIELD_SIZE; Loop1++){
		for (Loop2 = BRICK_HEIGHT + FIELD_SIZE - 1 - Loop1; Loop2 <= int(BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE) + Loop1; Loop2++){
			(Board[BRICK_HEIGHT + FIELD_SIZE - 1 - Loop1][Loop2].Health > 0) ? Check = true : 0;
			(Board[Loop2][BRICK_HEIGHT + FIELD_SIZE - 1 - Loop1].Health > 0) ? Check = true : 0;
			(Board[BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Loop1][Loop2].Health > 0) ? Check = true : 0;
			(Board[Loop2][BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Loop1].Health > 0) ? Check = true : 0;
			if (Check){ return Loop1; }
		}
	}
	return Loop1;
}

bool C_board::Play_brick(){
	int Temp1, Temp2, Temp3, Temp4;
	unsigned X, Y;
	int X_in, Y_in;
	C_ball::C_ball_vector* Ball;
	"";
	switch (Play_brick_way){
	case U://위
		for (Temp1 = 0; Temp1 < int(BASE_SIZE + 2 * FIELD_SIZE); Temp1++){//한 줄.
			X = BRICK_HEIGHT + Temp1;
			for (Temp2 = 0; Temp2 < (int)BOARD_SIZE; Temp2++){//줄 내부 처리.
				if (Board[Temp2][X].Health == 0){//블럭 존재 안함.-->밀리기 가능.
					for (Temp3 = Temp2; Temp3 > 0; Temp3--){
						Board[Temp3][X] = Board[Temp3 - 1][X];
						Print_brick(X, Temp3);
					}
					Brick_add(X, 0);
					Print_brick(X, 0);
					//공이 포함되면 밀기.
					for (Temp3 = 0; Temp3 < (int)Balls.Num(); Temp3++){//하나의 공.
						for (Temp4 = 0; Temp4 < (int)Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//공의 그림자중 하나.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = X*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->X &&
								Ball->X <= X_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in <= Ball->Y &&
								Ball->Y <= Y_in + BRICK_SIZE*INTERNAL_RESOLUTION
								){//공의 그림자중 하나가 벽돌 내부 및 좌우에 존재하는 경우.
								Arrange_ball_in_brick(Temp3, U);
							}
						}
					}
					break;
				}
			}
		}
		Play_brick_way = R;
		break;
	case R://오른쪽
		for (Temp1 = 0; Temp1 < int(BASE_SIZE + 2 * FIELD_SIZE); Temp1++){//한 줄.
			Y = BRICK_HEIGHT + Temp1;
			for (Temp2 = BOARD_SIZE - 1; Temp2 >= 0; Temp2--){//줄 내부 처리.
				if (Board[Y][Temp2].Health == 0){//블럭 존재 안함.-->밀리기 가능.
					for (Temp3 = Temp2; Temp3 < (int)BOARD_SIZE - 1; Temp3++){
						Board[Y][Temp3] = Board[Y][Temp3 + 1];
						Print_brick(Temp3, Y);
					}
					Brick_add(BOARD_SIZE - 1, Y);
					Print_brick(BOARD_SIZE - 1, Y);
					//공이 포함되면 밀기.
					for (Temp3 = 0; Temp3 < (int)Balls.Num(); Temp3++){//하나의 공.
						for (Temp4 = 0; Temp4 < (int)Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//공의 그림자중 하나.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Y*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in <= Ball->X &&
								Ball->X <= X_in + BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->Y &&
								Ball->Y <= Y_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION
								){//공의 그림자중 하나가 벽돌 내부 및 좌우에 존재하는 경우.
								Arrange_ball_in_brick(Temp3, R);
							}
						}
					}
					break;
				}
			}
		}
		Play_brick_way = D;
		break;
	case D://아래
		for (Temp1 = 0; Temp1 < int(BASE_SIZE + 2 * FIELD_SIZE); Temp1++){//한 줄.
			X = BRICK_HEIGHT + Temp1;
			for (Temp2 = BOARD_SIZE - 1; Temp2 >= 0; Temp2--){//줄 내부 처리.
				if (Board[Temp2][X].Health == 0){//블럭 존재 안함.-->밀리기 가능.
					for (Temp3 = Temp2; Temp3 < (int)BOARD_SIZE - 1; Temp3++){
						Board[Temp3][X] = Board[Temp3 + 1][X];
						Print_brick(X, Temp3);
					}
					Brick_add(X, BOARD_SIZE - 1);
					Print_brick(X, BOARD_SIZE - 1);
					//공이 포함되면 밀기.
					for (Temp3 = 0; Temp3 < (int)Balls.Num(); Temp3++){//하나의 공.
						for (Temp4 = 0; Temp4 < (int)Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//공의 그림자중 하나.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = X*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->X &&
								Ball->X <= X_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in <= Ball->Y &&
								Ball->Y <= Y_in + BRICK_SIZE*INTERNAL_RESOLUTION
								){//공의 그림자중 하나가 벽돌 내부 및 좌우에 존재하는 경우.
								Arrange_ball_in_brick(Temp3, D);
							}
						}
					}
					break;
				}
			}
		}
		Play_brick_way = L;
		break;
	case L://왼쪽
		for (Temp1 = 0; Temp1 < int(BASE_SIZE + 2 * FIELD_SIZE); Temp1++){//한 줄.
			Y = BRICK_HEIGHT + Temp1;
			for (Temp2 = 0; Temp2 < (int)BOARD_SIZE; Temp2++){//줄 내부 처리.
				if (Board[Y][Temp2].Health == 0){//블럭 존재 안함.-->밀리기 가능.
					for (Temp3 = Temp2; Temp3 > 0; Temp3--){
						Board[Y][Temp3] = Board[Y][Temp3 - 1];
						Print_brick(Temp3, Y);
					}
					Brick_add(0, Y);
					Print_brick(0, Y);
					//공이 포함되면 밀기.
					for (Temp3 = 0; Temp3 < (int)Balls.Num(); Temp3++){//하나의 공.
						for (Temp4 = 0; Temp4 < (int)Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//공의 그림자중 하나.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Y*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in <= Ball->X &&
								Ball->X <= X_in + BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->Y &&
								Ball->Y <= Y_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION
								){//공의 그림자중 하나가 벽돌 내부 및 좌우에 존재하는 경우.
								Arrange_ball_in_brick(Temp3, L);
							}
						}
					}
					break;
				}
			}
		}
		Play_brick_way = U;
		break;
	}
	Arrange_reflector();
	if (Dead_verge() == 0){
		return true;
	}
	return false;//게임오버 아님.
}
bool C_board::Play_ball(unsigned Ball_num){
	if (Ball_num >= Balls.Num()){ return false; }
	C_ball& Ball = *(Balls.Read_editable(Ball_num));
	unsigned Temp1;
	if (Ball.Tick > 0){//정지 아님.
		if (Ball.Tick == Ball.Speed_tick){
			Ball.Tick = 1;
			//Play
			//"Reflector점검(및 반사)";
			for (Temp1 = 0; Temp1 < Ball.Speed_movenum; Temp1++){
				if (Play_ball_ref(Ball)){ Ball.Basezone_state = C_ball::REF; }//결과가 참일시 '공 상태 반사됨' 으로 변경.
				else {//"결과가 거짓일시 Brick점검";
					Play_ball_brick(Ball);
				}
				if (Play_ball_basezone_check(Ball_num) == false){//공이 감속불가로 사라져야됨.
					return true;
				}
			}
			Arrange_ball_zoneout(Ball_num);
		}
		else { Ball.Tick++; }
	}
	return false;
}
bool C_board::Play_ball_ref(C_ball& Ball){
	/*
	매개변수 직선 (a+(c-a)t,b+(d-b)t)과 점 (X1, Y1)(X2, Y2)로 이루어진 교점의 t값은, [(a-X1)(Y1-Y2)+(b-Y1)(X2-X1)] / [(a-c)(Y1-Y2)+(b-d)(X2-X1)]
	*/
	C_ball::C_ball_vector Lastball = Ball.Ball_rec.Read(Ball.Ball_rec.Num() - 1);//현재 공.(마지막공).
	int Ball_X, Ball_Y, Ref_X1, Ref_Y1, Ref_X2, Ref_Y2;
	C_fraction Ref_t = 2;
	bool Check;
	int Ball_is_right;

	Get_reflector_edge(&Ref_X1, &Ref_Y1, &Ref_X2, &Ref_Y2);
	Ball_X = Lastball.X + Lastball.Vector_X(); Ball_Y = Lastball.Y + Lastball.Vector_Y();

	Check = Line_in_line(Lastball.X, Lastball.Y, Ball_X, Ball_Y, Ref_X1, Ref_Y1, Ref_X2, Ref_Y2, &Ref_t);
	if (Check){//반사 일어남.
		if (Dot_is_right_side(Ref_X2, Ref_Y2, Ref_X1, Ref_Y1, Lastball.X, Lastball.Y)){ Ball_is_right = 1; }//반사되는 쪽이 법선백터쪽인가?.
		else { Ball_is_right = -1; }
		if (Ball.Ball_rec.Num() == Ball.SHADOW_NUM){ Ball.Ball_rec.Out(); }//Ball_rec공간 확보.
		Ball.Ball_rec.In(
			C_ball::C_ball_vector(
			int(Lastball.X + Lastball.Vector_X()*(Ref_t.Get_value()) + 3 * Ball_is_right * C_ball::Vector[Reflector.Position][0] / (double)VECTOR_LENGTH),
			int(Lastball.Y + Lastball.Vector_Y()*(Ref_t.Get_value()) + 3 * Ball_is_right * C_ball::Vector[Reflector.Position][1] / (double)VECTOR_LENGTH),
			(2 * Reflector.Position + C_ball::DIRECTION_NUM * 3 / 2 - Lastball.Vector_num) % C_ball::DIRECTION_NUM)
			);
		Ball.Ball_rec.Read_editable(Ball.Ball_rec.Num() - 1)->State = C_ball::C_ball_vector::REF_REF;//반사되었다는 표식.
		return true;
	}
	else return false;//반사가 일어나지 않음.
}
void C_board::Play_ball_brick(C_ball& Ball){
	C_ball::C_ball_vector Lastball = Ball.Ball_rec.Read(Ball.Ball_rec.Num() - 1);//현재 공.(마지막공).
	unsigned Brick_X, Brick_Y;
	int Lastball_X_temp = Lastball.X, Lastball_Y_temp = Lastball.Y, Lastball_V_num_temp = Lastball.Vector_num;
	bool Ref_X0, Ref_X1, Ref_Y0, Ref_Y1;
	C_fraction Ref_tX0, Ref_tX1, Ref_tY0, Ref_tY1, Temp;
	unsigned Check = 0;
	bool Is_ref = false;
	//현재 공이 속한 Brick의 좌표를 구함.
	if ((Lastball.X % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0) && (Lastball.Vector_X() < 0)){ Brick_X = Lastball.X / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1; }//현재 공이 경계선에서 낮은 좌표 쪽으로 가는 경우.
	else { Brick_X = FLOOR(Lastball.X, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION); }
	if ((Lastball.Y % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0) && (Lastball.Vector_Y() < 0)){ Brick_Y = Lastball.Y / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1; }//현재 공이 경계선에서 낮은 좌표 쪽으로 가는 경우.
	else { Brick_Y = FLOOR(Lastball.Y, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION); }

	if (Lastball.Vector_X() == 0 || Lastball.Vector_Y() == 0){//공이 수직 진행중.
		if (Lastball.Vector_X() == 0){//Y축 움직임.
			(Lastball.Vector_Y() > 0) ?
				Brick_Y = FLOOR(Lastball.Y + Lastball.Vector_Y(), BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION) :
				Brick_Y = CEIL(Lastball.Y + Lastball.Vector_Y(), BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1;
			if (Lastball.X % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0){//Y축 경계선.
				if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){//반사됨.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y :
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X - 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
				else if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y :
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
			else {//Y축 비경계선.
				if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y :
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
		}
		else {//X축 움직임.
			(Lastball.Vector_X() > 0) ?
				Brick_X = FLOOR(Lastball.X + Lastball.Vector_X(), BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION) :
				Brick_X = CEIL(Lastball.X + Lastball.Vector_X(), BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1;
			if (Lastball.Y % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0){//X축 경계선.
				if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_X() > 0) ?
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X :
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X;
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
				else if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_X() > 0) ?
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X :
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X;
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
			else {//Y축 비경계선.
				if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_X() > 0) ?
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X :
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X;
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
		}
	}
	else {//공 대각선.
		Ref_X0 = Line_in_line(
			Lastball.X, Lastball.Y,
			Lastball.X + Lastball.Vector_X(), Lastball.Y + Lastball.Vector_Y(),
			Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION, Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION,
			Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION, (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, &Ref_tX0);
		Ref_X1 = Line_in_line(
			Lastball.X, Lastball.Y,
			Lastball.X + Lastball.Vector_X(), Lastball.Y + Lastball.Vector_Y(),
			(Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION,
			(Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, &Ref_tX1);
		Ref_Y0 = Line_in_line(
			Lastball.X, Lastball.Y,
			Lastball.X + Lastball.Vector_X(), Lastball.Y + Lastball.Vector_Y(),
			Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION, Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION,
			(Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION, &Ref_tY0);
		Ref_Y1 = Line_in_line(
			Lastball.X, Lastball.Y,
			Lastball.X + Lastball.Vector_X(), Lastball.Y + Lastball.Vector_Y(),
			Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION, (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION,
			(Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, &Ref_tY1);
		if (Ref_X0 && (Ref_tX0 != 0)){ Check++; }
		else { Ref_X0 = false; }
		if (Ref_X1 && (Ref_tX1 != 0)){ Check++; }
		else { Ref_X1 = false; }
		if (Ref_Y0 && (Ref_tY0 != 0)){ Check++; }
		else { Ref_Y0 = false; }
		if (Ref_Y1 && (Ref_tY1 != 0)){ Check++; }
		else { Ref_Y1 = false; }

		if (Check == 1){//벽면 반사가능성.
			//1차반사검사.
			if (Ref_X0) {

				if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){//반사한 경우.
					Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
				else { Brick_X--; }
			}
			if (Ref_X1) {
				if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
					Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
				else { Brick_X++; }
			}
			if (Ref_Y0){
				if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y) ? Brick_hit(Brick_X, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
				else { Brick_Y--; }
			}
			if (Ref_Y1){
				if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X, Brick_Y + 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
				else { Brick_Y++; }
			}
			//2차반사검사.
			Ref_X0 = Line_in_line(
				Lastball_X_temp, Lastball_Y_temp,
				Lastball_X_temp + C_ball::Vector[Lastball_V_num_temp][0], Lastball_Y_temp + C_ball::Vector[Lastball_V_num_temp][1],
				Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION, Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION,
				Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION, (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, &Ref_tX0);
			Ref_X1 = Line_in_line(
				Lastball_X_temp, Lastball_Y_temp,
				Lastball_X_temp + C_ball::Vector[Lastball_V_num_temp][0], Lastball_Y_temp + C_ball::Vector[Lastball_V_num_temp][1],
				(Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION,
				(Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, &Ref_tX1);
			Ref_Y0 = Line_in_line(
				Lastball_X_temp, Lastball_Y_temp,
				Lastball_X_temp + C_ball::Vector[Lastball_V_num_temp][0], Lastball_Y_temp + C_ball::Vector[Lastball_V_num_temp][1],
				Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION, Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION,
				(Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION, &Ref_tY0);
			Ref_Y1 = Line_in_line(
				Lastball_X_temp, Lastball_Y_temp,
				Lastball_X_temp + C_ball::Vector[Lastball_V_num_temp][0], Lastball_Y_temp + C_ball::Vector[Lastball_V_num_temp][1],
				Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION, (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION,
				(Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION, &Ref_tY1);
			Temp = Check = 0;
			if (Ref_X0){ Check++; Temp = Ref_tX0; }
			if (Ref_X1){ Check++; Temp = HIGH(Temp, Ref_tX1); }
			if (Ref_Y0){ Check++; Temp = HIGH(Temp, Ref_tY0); }
			if (Ref_Y1){ Check++; Temp = HIGH(Temp, Ref_tY1); }
			if (Check == 2){//반사된 경우.
				if (Temp == Ref_tX0){
					if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){//반사한 경우.
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
						Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						(Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
						Is_ref = true;
					}
				}
				if (Temp == Ref_tX1){
					if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
						Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						(Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
						Is_ref = true;
					}
				}
				if (Temp == Ref_tY0){
					if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
						Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						(Brick_Y) ? Brick_hit(Brick_X, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
						Is_ref = true;
					}
				}
				if (Temp == Ref_tY1){
					if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
						Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						(Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X, Brick_Y + 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
						Is_ref = true;
					}
				}
			}
		}
		else if (Check == 2){//모서리로 가는 반사.
			if (Ref_X0&&Ref_Y0){
				if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){
					Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
					Check--;
				}
				if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y) ? Brick_hit(Brick_X, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
					Check--;
				}
				if (Check == 2){//옆 블럭에서 반사안된 경우.(대각선 블럭 검사).
					if ((Brick_X&&Brick_Y) ? Board[Brick_Y - 1][Brick_X - 1].Health != 0 : true){
						if (random(2)){
							Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
							Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						}
						else {
							Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
							Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						}
						(Brick_X&&Brick_Y) ? Brick_hit(Brick_X - 1, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
						Is_ref = true;
					}
				}
				else if (Check == 1){//옆 블럭 1개에서 반사된 경우.
					(Brick_X&&Brick_Y) ? Brick_hit(Brick_X - 1, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
			else if (Ref_Y0&&Ref_X1){
				if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y) ? Brick_hit(Brick_X, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
					Check--;
				}
				if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
					Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
					Check--;
				}
				if (Check == 2){
					if ((Brick_Y&&Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y - 1][Brick_X + 1].Health != 0 : true){
						if (random(2)){
							Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
							Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						}
						else {
							Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
							Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						}
						(Brick_Y&&Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
						Is_ref = true;
					}
				}
				else if (Check == 1){
					(Brick_Y&&Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
			else if (Ref_X1&&Ref_Y1){
				if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
					Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
					Check--;
				}
				if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X, Brick_Y + 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
					Check--;
				}
				if (Check == 2){
					if ((Brick_X != BOARD_SIZE - 1 && Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X + 1].Health != 0 : true){
						if (random(2)){
							Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
							Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						}
						else {
							Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
							Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						}
						(Brick_X != BOARD_SIZE - 1 && Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y + 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
						Is_ref = true;
					}
				}
				else if (Check == 1){
					(Brick_X != BOARD_SIZE - 1 && Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y + 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
			else if (Ref_Y1&&Ref_X0){
				if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X, Brick_Y + 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
					Check--;
				}
				if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){
					Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
					Check--;
				}
				if (Check == 2){
					if ((Brick_Y != BOARD_SIZE - 1 && Brick_X) ? Board[Brick_Y + 1][Brick_X - 1].Health != 0 : true){
						if (random(2)){
							Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
							Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						}
						else {
							Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
							Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						}
						(Brick_Y != BOARD_SIZE - 1 && Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y + 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
						Is_ref = true;
					}
				}
				else if (Check == 1){
					(Brick_Y != BOARD_SIZE - 1 && Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y + 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
		}
	}

	//공 반사 결과 삽입.
	Lastball_X_temp = Lastball_X_temp + C_ball::Vector[Lastball_V_num_temp][0];
	Lastball_Y_temp = Lastball_Y_temp + C_ball::Vector[Lastball_V_num_temp][1];
	if (Ball.Ball_rec.Num() == Ball.SHADOW_NUM){ Ball.Ball_rec.Out(); }//Ball_rec공간 확보.
	Ball.Ball_rec.In(C_ball::C_ball_vector(Lastball_X_temp, Lastball_Y_temp, Lastball_V_num_temp));
	if (Is_ref){
		Ball.Ball_rec.Read_editable(Ball.Ball_rec.Num() - 1)->State = C_ball::C_ball_vector::BRICK_REF;
	}
}
bool C_board::Play_ball_basezone_check(unsigned Ball_num){
	if (Ball_num >= Balls.Num()){ return true; }
	C_ball& Ball = *(Balls.Read_editable(Ball_num));
	int X = Ball.Ball_rec.Read(Ball.Ball_rec.Num() - 1).X;
	int Y = Ball.Ball_rec.Read(Ball.Ball_rec.Num() - 1).Y;
	int H1 = ((BRICK_HEIGHT + FIELD_SIZE)*BRICK_SIZE + 1)*INTERNAL_RESOLUTION;
	int H2 = ((BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE)*BRICK_SIZE - 1)*INTERNAL_RESOLUTION;
	if (
		(H1 <= X) && (X <= H2) &&
		(H1 <= Y) && (Y <= H2)
		){//Basezone안에 있는 경우.
		if (Ball.Basezone_state == C_ball::BASEZONE_OUT){//basezone으로 들어온경우.
			Ball.Basezone_state = C_ball::BASEZONE_IN;
		}
		if (Ball.Basezone_state == C_ball::START){//시작부터 basezone이면 반사되었다고 침.
			Ball.Basezone_state = C_ball::REF;
		}
	}
	else {//Basezone밖에 있는 경우.
		if (Ball.Basezone_state == C_ball::BASEZONE_IN){//basezone밖으로 나간 경우(반사판에 닿지 않고).
			if (Ball.Speed_edit(0, 1) == false){
				return false;
			}
		}
		Ball.Basezone_state = C_ball::BASEZONE_OUT;
	}
	return true;
}
void C_board::Move_reflector(T_direction Input, unsigned Speed){
	int X1, Y1, X2, Y2;
	int Ref_X1, Ref_Y1, Ref_X2, Ref_Y2;
	C_ball::C_ball_vector* Lastball;
	bool Temp, Ball_speedup;
	int Loop1;

	Get_reflector_edge(&Ref_X1, &Ref_Y1, &Ref_X2, &Ref_Y2);//Reflector이동 전의 위치 저장.
	switch (Input){
	case R:
		Reflector.X += Reflector.Move_length * INTERNAL_RESOLUTION * Speed;
		break;
	case D:
		Reflector.Y += Reflector.Move_length * INTERNAL_RESOLUTION * Speed;
		break;
	case L:
		Reflector.X -= Reflector.Move_length * INTERNAL_RESOLUTION * Speed;
		break;
	case U:
		Reflector.Y -= Reflector.Move_length * INTERNAL_RESOLUTION * Speed;
		break;
	case UL://시계반대방향 회전.(데카르트좌표계 시계반대방향).
		Reflector.Position = (C_ball::DIRECTION_NUM + Reflector.Position - 1) % C_ball::DIRECTION_NUM;
		break;
	case UR://시계방향 회전.(데카르트좌표계 시계방향).
		Reflector.Position = (Reflector.Position + 1) % C_ball::DIRECTION_NUM;
		break;
	default:
		break;
	}
	//Hitting Zone바깥일 경우 안쪽으로 재이동.
	Arrange_reflector();

	//Reflector안에 공이 끼는 경우를 정리.
	Get_reflector_edge(&X1, &Y1, &X2, &Y2);
	if ((X1 == Ref_X1) && (Y1 == Ref_Y1) && (X2 == Ref_X2) && (Y2 == Ref_Y2)){//Reflector움직임 없음.
		return;
	}
	for (Loop1 = 0; Loop1 < (int)Balls.Num(); Loop1++){
		Ball_speedup = false;
		Lastball = Balls.Read_editable(Loop1)->Ball_rec.Read_editable((Balls.Read(Loop1).Ball_rec.Num() - 1));//각 공(Loop1번)의 현재 위치(마지막 공).
		switch (Input){
		case R:
		case D:
		case L:
		case U:
			Temp = Dot_is_right_side(X1, Y1, X2, Y2, Lastball->X, Lastball->Y);
			for (;
				(Temp == Dot_is_right_side(X2, Y2, Ref_X2, Ref_Y2, Lastball->X, Lastball->Y)) &&
				(Temp == Dot_is_right_side(Ref_X2, Ref_Y2, Ref_X1, Ref_Y1, Lastball->X, Lastball->Y)) &&
				(Temp == Dot_is_right_side(Ref_X1, Ref_Y1, X1, Y1, Lastball->X, Lastball->Y))
				;){//공이 Reflector의 움직임에 닿은 경우.
				Ball_speedup = true;
				switch (Input){
				case R:
					Lastball->X += Reflector.Move_length * INTERNAL_RESOLUTION * Speed;
					break;
				case D:
					Lastball->Y += Reflector.Move_length * INTERNAL_RESOLUTION * Speed;
					break;
				case L:
					Lastball->X -= Reflector.Move_length * INTERNAL_RESOLUTION * Speed;
					break;
				case U:
					Lastball->Y -= Reflector.Move_length * INTERNAL_RESOLUTION * Speed;
					break;
				}
				Temp = Dot_is_right_side(X1, Y1, X2, Y2, Lastball->X, Lastball->Y);
			}
			break;
		case UL://시계반대방향 회전.(데카르트좌표계 시계반대방향).
		case UR://시계방향 회전.(데카르트좌표계 시계방향).
			for (;
				(Dot_is_right_side(Reflector.X, Reflector.Y, X1, Y1, Lastball->X, Lastball->Y) == Dot_is_right_side(X1, Y1, Ref_X1, Ref_Y1, Lastball->X, Lastball->Y)) &&
				(Dot_is_right_side(X1, Y1, Ref_X1, Ref_Y1, Lastball->X, Lastball->Y) == Dot_is_right_side(Ref_X1, Ref_Y1, Reflector.X, Reflector.Y, Lastball->X, Lastball->Y))
				;){//공이 Reflector의 움직임에 닿은 경우.(1번점들은 Position방향에서 시계방향에 있음(데카르트))
				Ball_speedup = true;
				switch (Input){
				case UL://시계반대방향 회전.(데카르트좌표계 시계반대방향).
					Lastball->X += C_ball::Vector[Reflector.Position][0];
					Lastball->Y += C_ball::Vector[Reflector.Position][1];
					break;
				case UR://시계방향 회전.(데카르트좌표계 시계방향).
					Lastball->X -= C_ball::Vector[Reflector.Position][0];
					Lastball->Y -= C_ball::Vector[Reflector.Position][1];
					break;
				}
			}
			for (;
				(Dot_is_right_side(Reflector.X, Reflector.Y, X2, Y2, Lastball->X, Lastball->Y) == Dot_is_right_side(X2, Y2, Ref_X2, Ref_Y2, Lastball->X, Lastball->Y)) &&
				(Dot_is_right_side(X2, Y2, Ref_X2, Ref_Y2, Lastball->X, Lastball->Y) == Dot_is_right_side(Ref_X2, Ref_Y2, Reflector.X, Reflector.Y, Lastball->X, Lastball->Y))
				;){//공이 Reflector의 움직임에 닿은 경우.
				Ball_speedup = true;
				switch (Input){
				case UL://시계반대방향 회전.(데카르트좌표계 시계반대방향).
					Lastball->X -= C_ball::Vector[Reflector.Position][0];
					Lastball->Y -= C_ball::Vector[Reflector.Position][1];
					break;
				case UR://시계방향 회전.(데카르트좌표계 시계방향).
					Lastball->X += C_ball::Vector[Reflector.Position][0];
					Lastball->Y += C_ball::Vector[Reflector.Position][1];
					break;
				}
			}
		}
		if (Ball_speedup){//공이 끼인적이 있는 경우.
			Balls.Read_editable(Loop1)->Speed_edit(1);//속도 증가.
			Balls.Read_editable(Loop1)->Basezone_state = C_ball::REF;
		}
	}
}
void C_board::Arrange_reflector(){
	int X1, Y1, X2, Y2, X_small, X_large, Y_small, Y_large;
	int Verge = Dead_verge() - 1;//Reflector는 벽돌에서 한칸 떨어져 있어야 함.그렇지 않을 경우에는 반사 알고리즘에서 많은 오류가 있음.
	Get_reflector_edge(&X1, &Y1, &X2, &Y2);
	X_small = LOW(X1, X2);
	X_large = HIGH(X1, X2);
	Y_small = LOW(Y1, Y2);
	Y_large = HIGH(Y1, Y2);
	if (X_small < int((BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION)){
		Reflector.X += CEIL((BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION - X_small, INTERNAL_RESOLUTION);
	}
	if (Y_small < int((BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION)){
		Reflector.Y += CEIL((BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION - Y_small, INTERNAL_RESOLUTION);
	}
	if (X_large > int((BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION) - 1){
		Reflector.X -= CEIL(X_large - (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION + 1, INTERNAL_RESOLUTION);
	}
	if (Y_large > int((BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION) - 1){
		Reflector.Y -= CEIL(Y_large - (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION + 1, INTERNAL_RESOLUTION);
	}
}
void C_board::Arrange_ball_in_brick(unsigned Ball_num, T_direction Way){//공이 벽돌에 끼는 경우를 정리.
	if (Ball_num >= Balls.Num()){ return; }
	C_ball& Ball = *Balls.Read_editable(Ball_num);
	C_ball::C_ball_vector * Lastball;
	int Brick_X, Brick_Y;
	int Brick_X_find[2], Brick_Y_find[2];
	int Dis, Dis_X, Dis_Y;
	int Loop1, Loop2;

	while (Ball.Ball_rec.Num() != 1){//공 그림자 제거.
		Ball.Ball_rec.Out();
	}
	Lastball = Ball.Ball_rec.Read_editable(0);//마지막 공.

	if (Lastball->X % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0){ Lastball->X++; }//공이 경계선에 있는경우를제거(미세하게 공이 달라지지만..큰 영향 없음).
	if (Lastball->X > (int)BOARD_SIZE_IN){ Lastball->X -= 2; }
	if (Lastball->Y % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0){ Lastball->Y++; }
	if (Lastball->Y > (int)BOARD_SIZE_IN){ Lastball->Y -= 2; }
	Brick_X = FLOOR(Lastball->X, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION);
	Brick_Y = FLOOR(Lastball->Y, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION);

	if (Board[Brick_Y][Brick_X].Health > 0){//공이 블럭 속에 있음.
		//일단 밀린 방향으로 1칸 밀림.
		switch (Way){
		case U:
			Lastball->Y += BRICK_SIZE*INTERNAL_RESOLUTION;
			break;
		case D:
			Lastball->Y -= BRICK_SIZE*INTERNAL_RESOLUTION;
			break;
		case R:
			Lastball->X -= BRICK_SIZE*INTERNAL_RESOLUTION;
			break;
		case L:
			Lastball->X += BRICK_SIZE*INTERNAL_RESOLUTION;
			break;
		default:
			break;
		}
		Brick_X = FLOOR(Lastball->X, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION);
		Brick_Y = FLOOR(Lastball->Y, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION);
		//주변 블럭 탐색.
		for (Dis = 1; true; Dis++){//Dis==중앙 블럭으로부터의 거리.
			for (Dis_X = 0; Dis_X <= Dis; Dis_X++){
				Dis_Y = Dis - Dis_X;
				Brick_X_find[0] = Brick_X + Dis_X;
				Brick_X_find[1] = Brick_X - Dis_X;
				Brick_Y_find[0] = Brick_Y + Dis_Y;
				Brick_Y_find[1] = Brick_Y - Dis_Y;
				for (Loop1 = 0; Loop1 < 2; Loop1++){
					for (Loop2 = 0; Loop2 < 2; Loop2++){
						if (0 <= Brick_X_find[Loop1] && Brick_X_find[Loop1] < (int)BOARD_SIZE && 0 <= Brick_Y_find[Loop2] && Brick_Y_find[Loop2] < (int)BOARD_SIZE){
							if (Board[Brick_Y_find[Loop2]][Brick_X_find[Loop1]].Health == 0){
								Lastball->X += (Loop1 ? -Dis_X : Dis_X)*int(BRICK_SIZE)*int(INTERNAL_RESOLUTION);
								Lastball->Y += (Loop2 ? -Dis_Y : Dis_Y)*int(BRICK_SIZE)*int(INTERNAL_RESOLUTION);
								Lastball->State = C_ball::C_ball_vector::NONE;
								return;
							}
						}
					}
				}
			}
		}
	}
}
void C_board::Arrange_ball_zoneout(unsigned Ball_num){//영영 Hitting_zone으로 들어오지 못하는 공을 정리함.
	if (Ball_num >= Balls.Num()){ return; }
	C_ball::C_ball_vector& Lastball = *(Balls.Read_editable(Ball_num)->Ball_rec.Read_editable(Balls.Read(Ball_num).Ball_rec.Num() - 1));
	int Verge = Dead_verge() - 1;//Reflector의 경계선 정보.
	int H1 = (BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION;
	int H2 = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION - 1;
	bool Check = false;
	if (Lastball.State == C_ball::C_ball_vector::NONE){//공이 다른 반사 요인이 없을 때에만 정리함. 공이 이상하게 틀어져 출력에러가 나는것을 방지.
		if (Lastball.Vector_X() == 0){
			if (Lastball.X < H1){
				Check = true;
			}
			else if (H2 < Lastball.X){
				Check = true;
			}
		}
		if (Lastball.Vector_Y() == 0){
			if (Lastball.Y < H1){
				Check = true;
			}
			else if (H2 < Lastball.Y){
				Check = true;
			}
		}
		if (Check){
			Lastball.Vector_num = (Lastball.Vector_num + 1) % C_ball::DIRECTION_NUM;
		}
	}
}
void C_board::Brick_add(unsigned X, unsigned Y){
	"";//벽돌속성추가.
	C_fraction Temp;
	unsigned Loop1;
	static const unsigned High_level = 600 / LEVELUP_SEC;//10분 후의 레벨.
	static const unsigned Diff_speed = 100;//난이도 상승 속도.

	static const C_fraction H_probability[4][2] = {
		{ C_fraction(9, 10), C_fraction(1, 20) },//Health==1이 나올 확률. 시작확률과 마지막확률(점근선).
		{ C_fraction(1, 3), C_fraction(1, 3) },//Health==1이 아닐때 Health==2가 나올 확률. 시작확률과 마지막확률(점근선).
		{ C_fraction(1, 3), C_fraction(1, 3) },//Health==2이 아닐때 Health==3가 나올 확률.
		{ C_fraction(1, 3), C_fraction(1, 2) },//Health==3이 아닐때 Health==4가 나올 확률.
		//그 외 Health==5.
	};

	//Health
	for (Loop1 = 0; Loop1 < 5; Loop1++){
		if (Loop1 == 4){
			Board[Y][X].Health = 5;
		}
		else {
			if (Temp = Level_func(Level, H_probability[Loop1][0], H_probability[Loop1][1], High_level, Diff_speed), Probability(Temp.Get_numer(), Temp.Get_denom())){
				Board[Y][X].Health = Loop1 + 1;
				break;
			}
		}
	}

	//속성.
	Board[Y][X].State = C_brick::NORMAL;
	if (random(Balls.Num() * 20) == 0){
		Board[Y][X].State = C_brick::BALLOUT;
	}
//#ifdef MYDEBUG
//	Board[Y][X].State = C_brick::BALLOUT;
//#endif
}
void C_board::Brick_hit(unsigned X, unsigned Y, unsigned Speed){
	int Temp1;
	"";//벽돌속성추가.
	if (Board[Y][X].State == C_brick::NONE){
		return;
	}
	if (Board[Y][X].Health > 0){
		if (Board[Y][X].State == C_brick::NORMAL){
			for (Temp1 = 0; Temp1 < int(CEIL(Speed, Damageup_speed) / Damageup_speed); Temp1++){
				if (Board[Y][X].Health > 0){
					Board[Y][X].Health--;
				}
			}
			Print_brick(X, Y);
			Score += 10 * Speed;
		}
		if (Board[Y][X].State == C_brick::BALLOUT){
			for (Temp1 = 0; Temp1 < int(CEIL(Speed, Damageup_speed) / Damageup_speed); Temp1++){
				if (Board[Y][X].Health > 0){
					Board[Y][X].Health--;
				}
			}
			Print_brick(X, Y);
			Score += 10 * Speed;
			if (Board[Y][X].Health == 0){
				for (Temp1 = 0; Temp1 < 3; Temp1++){
					if (Balls.Num() < BALL_MAX_NUM){
						Balls.In(C_ball(
							X * BRICK_SIZE * INTERNAL_RESOLUTION + BRICK_SIZE * INTERNAL_RESOLUTION / 2,
							Y * BRICK_SIZE * INTERNAL_RESOLUTION + BRICK_SIZE * INTERNAL_RESOLUTION / 2,
							random(C_ball::DIRECTION_NUM)
							));
						Balls.Read_editable(Balls.Num() - 1)->Speed_movenum = Speed;
					}
				}
			}
		}
	}
}

void C_board::Screen_recorvery(){
	Screen.Set_consol();
	Screen.Show_opt_kr();
}
void C_board::Print_line(int X1, int Y1, int X2, int Y2, const char* Letter, char Color, int X_ref, int Y_ref, char Ref_color){
	int Temp1, Temp2, Temp3, Temp4;
	unsigned Screen_X, Screen_Y;
	char Str[3] = { Letter[0], Letter[1], '\0' };
	ERROR_DEBUGSTOP(("Reflection point must be on INTERNAL_RESOLUTION", (X_ref%INTERNAL_RESOLUTION) != 0 && X_ref != -1));
	ERROR_DEBUGSTOP(("Reflection point must be on INTERNAL_RESOLUTION", (Y_ref%INTERNAL_RESOLUTION) != 0 && Y_ref != -1));

	if (X_ref != -1){
		X_ref /= INTERNAL_RESOLUTION;
	}
	if (Y_ref != -1){
		Y_ref /= INTERNAL_RESOLUTION;
	}
	Temp3 = CEIL(HIGH(X1, X2), INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION;
	Temp4 = CEIL(HIGH(Y1, Y2), INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION;

	Temp1 = FLOOR(LOW(X1, X2), INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION;
	do{
		Temp2 = FLOOR(LOW(Y1, Y2), INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION;
		do{

			if (Box_in_line(X1, Y1, X2, Y2,
				Temp1 * INTERNAL_RESOLUTION, Temp2 * INTERNAL_RESOLUTION, (Temp1 + 1) * INTERNAL_RESOLUTION, (Temp2 + 1) * INTERNAL_RESOLUTION)){
				Screen.Setcolor(Color);
				Screen_X = Temp1 * 2;
				Screen_Y = Temp2;
				if (X_ref != -1){
					if ((X2 - X_ref * INTERNAL_RESOLUTION)*((Temp1 - X_ref >= 0) ? 1 : -1) > 0){
						Screen_X = (2 * X_ref - Temp1 - 1) * 2;
					}
					if (Temp1 == X_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							Temp1 * INTERNAL_RESOLUTION, Temp2 * INTERNAL_RESOLUTION, Temp1 * INTERNAL_RESOLUTION, (Temp2 + 1) * INTERNAL_RESOLUTION)){
							Screen.Setcolor(Ref_color);
						}
					}
					else if (Temp1 + 1 == X_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							(Temp1 + 1) * INTERNAL_RESOLUTION, Temp2 * INTERNAL_RESOLUTION, (Temp1 + 1) * INTERNAL_RESOLUTION, (Temp2 + 1) * INTERNAL_RESOLUTION)){
							Screen.Setcolor(Ref_color);
						}
					}
				}
				if (Y_ref != -1){
					if ((Y2 - Y_ref * INTERNAL_RESOLUTION)*((Temp2 - Y_ref >= 0) ? 1 : -1) > 0){
						Screen_Y = (2 * Y_ref - Temp2 - 1);
					}
					if (Temp2 == Y_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							Temp1 * INTERNAL_RESOLUTION, Temp2 * INTERNAL_RESOLUTION, (Temp1 + 1) * INTERNAL_RESOLUTION, Temp2 * INTERNAL_RESOLUTION)){
							Screen.Setcolor(Ref_color);
						}
					}
					else if (Temp2 + 1 == Y_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							Temp1 * INTERNAL_RESOLUTION, (Temp2 + 1) * INTERNAL_RESOLUTION, (Temp1 + 1) * INTERNAL_RESOLUTION, (Temp2 + 1) * INTERNAL_RESOLUTION)){
							Screen.Setcolor(Ref_color);
						}
					}
				}
				Screen << Coord(Screen_X, Screen_Y + BOARD_VERGE) << Str;
			}

			Temp2++;
		} while (Temp2 < Temp4);
		Temp1++;
	} while (Temp1 < Temp3);
}
C_fraction C_board::Level_func(unsigned Input, C_fraction Start, C_fraction End, unsigned High_level, unsigned Speed){
	//레벨구간 1~High_level까지는 난이도가 Start에서 End + (Start-End)/Speed까지 일차함수로 변화하며, 그 이후로는 End에 근사해져감.
	if (Input <= High_level){
		return
			(C_fraction(1 - (int)Speed) * (Start - End) * ((int)Input - 1)) /
			C_fraction(int(Speed) * ((int)High_level - 1))
			+ Start;
	}
	else {
		return
			(C_fraction(Start - End) * ((int)High_level + 1)) /
			C_fraction((int(Speed) + 1) * int(Input) + int(High_level) - int(Speed))
			+ End;
	}
}

unsigned C_board::Dump_size() const{
	return sizeof(unsigned) + sizeof(unsigned) * 5 + sizeof(C_screen_color::T_color) + sizeof(C_brick) * BOARD_SIZE * BOARD_SIZE + Balls.Dump_size()
		+ sizeof(unsigned) * 6 + sizeof(T_direction) + sizeof(T_reflector);
}
void C_board::Dump(char * Ptr) const{
	unsigned Loop1, Loop2;
	*(unsigned *)Ptr = Dump_size();
	Ptr += sizeof(unsigned);

	*(unsigned *)Ptr = BASE_SIZE;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = FIELD_SIZE;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = BRICK_HEIGHT;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = BOARD_SIZE;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = BOARD_SIZE_IN;
	Ptr += sizeof(unsigned);
	*(C_screen_color::T_color *)Ptr = BACK_COLOR;
	Ptr += sizeof(C_screen_color::T_color);

	for (Loop1 = 0; Loop1 < BOARD_SIZE; Loop1++){
		for (Loop2 = 0; Loop2 < BOARD_SIZE; Loop2++){
			*(C_brick *)Ptr = Board[Loop1][Loop2];
			Ptr += sizeof(C_brick);
		}
	}
	Balls.Dump(Ptr);
	Ptr += Balls.Dump_size();

	*(unsigned *)Ptr = Playbrick_speed_tick;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = Playbrick_tick;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = Level;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = Levelup_tick;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = Score;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = Damageup_speed;
	Ptr += sizeof(unsigned);
	*(T_direction *)Ptr = Play_brick_way;
	Ptr += sizeof(T_direction);
	*(T_reflector *)Ptr = Reflector;
	Ptr += sizeof(T_reflector);
}
bool C_board::Restore(char * Ptr){
	unsigned Loop1, Loop2;
	char* Ptr_E = Ptr + *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);

	if (*(unsigned *)Ptr != BASE_SIZE){ return false; }
	Ptr += sizeof(unsigned);
	if (*(unsigned *)Ptr != FIELD_SIZE){ return false; }
	Ptr += sizeof(unsigned);
	if (*(unsigned *)Ptr != BRICK_HEIGHT){ return false; }
	Ptr += sizeof(unsigned);
	if (*(unsigned *)Ptr != BOARD_SIZE){ return false; }
	Ptr += sizeof(unsigned);
	if (*(unsigned *)Ptr != BOARD_SIZE_IN){ return false; }
	Ptr += sizeof(unsigned);
	if (*(C_screen_color::T_color*)Ptr != BACK_COLOR){ return false; }
	Ptr += sizeof(C_screen_color::T_color);

	for (Loop1 = 0; Loop1 < BOARD_SIZE; Loop1++){
		for (Loop2 = 0; Loop2 < BOARD_SIZE; Loop2++){
			Board[Loop1][Loop2] = *(C_brick *)Ptr;
			Ptr += sizeof(C_brick);
		}
	}
	if (!(Balls.Restore(Ptr))){ return false; }
	Ptr += Balls.Dump_size();

	Playbrick_speed_tick= *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);
	Playbrick_tick= *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);
	Level= *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);
	Levelup_tick = *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);
	Score= *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);
	if (*(unsigned *)Ptr != Damageup_speed){ return false; }
	Ptr += sizeof(unsigned);
	Play_brick_way = *(T_direction *)Ptr;
	Ptr += sizeof(T_direction);
	Reflector = *(T_reflector *)Ptr;
	Ptr += sizeof(T_reflector);
	if (Ptr == Ptr_E){
		return true;
	}
	else { return false; }
}

bool C_board::Box_in_line(int X1, int Y1, int X2, int Y2, int Box_X1, int Box_Y1, int Box_X2, int Box_Y2){
	//직선--- (Y1 - Y2)x + (X2 - X1)y + (Y2 - Y1)X1 + (X1 - X2)Y1 = 0;
	int T1, T2, T3, T4;
	T1 = (Y1 - Y2)*Box_X1 + (X2 - X1)*Box_Y1 + (Y2 - Y1)*X1 + (X1 - X2)*Y1;
	T2 = (Y1 - Y2)*Box_X1 + (X2 - X1)*Box_Y2 + (Y2 - Y1)*X1 + (X1 - X2)*Y1;
	T3 = (Y1 - Y2)*Box_X2 + (X2 - X1)*Box_Y1 + (Y2 - Y1)*X1 + (X1 - X2)*Y1;
	T4 = (Y1 - Y2)*Box_X2 + (X2 - X1)*Box_Y2 + (Y2 - Y1)*X1 + (X1 - X2)*Y1;
	if ((T1 > 0 || T2 > 0 || T3 > 0 || T4 > 0) && (T1 < 0 || T2 < 0 || T3 < 0 || T4 < 0)){
		return true;
	}
	else if (((int)(T1 == 0) + (int)(T2 == 0) + (int)(T3 == 0) + (int)(T4 == 0)) == 2){
		return true;
	}
	else return false;
}
bool C_board::Line_in_line(int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4, C_fraction* T){
	C_fraction T1, T2;
	//((X1 - X3)*(Y3 - Y4) + (Y1 - Y3)*(X4 - X3)) / ((X1 - X2)*(Y3 - Y4) + (Y1 - Y2)*(X4 - X3));
	//((X3 - X1)*(Y1 - Y2) + (Y3 - Y1)*(X2 - X1)) / ((X3 - X4)*(Y1 - Y2) + (Y3 - Y4)*(X2 - X1));
	if (((X1 - X2)*(Y3 - Y4) + (Y1 - Y2)*(X4 - X3))){
		T1 = C_fraction((X1 - X3)*(Y3 - Y4) + (Y1 - Y3)*(X4 - X3)) / ((X1 - X2)*(Y3 - Y4) + (Y1 - Y2)*(X4 - X3));
		T2 = C_fraction((X3 - X1)*(Y1 - Y2) + (Y3 - Y1)*(X2 - X1)) / ((X3 - X4)*(Y1 - Y2) + (Y3 - Y4)*(X2 - X1));
	}
	else {
		if ((X1 - X3)*(Y3 - Y4) + (Y1 - Y3)*(X4 - X3)){//평행. T존재하지 않음.
			T1 = -1; T2 = -1;
		}
		else {//직선이 일치함.(4점과 3점을 약간 틀어서 처음으로만나는 지점과 마지막으로 만나는 지점 중 가장 작은 T값을 고름.)
			T1 = LOW(C_fraction((X1 - X3)*(Y3 - (Y4 + 1)) + (Y1 - Y3)*(X4 - X3)) / ((X1 - X2)*(Y3 - (Y4 + 1)) + (Y1 - Y2)*(X4 - X3)),
				C_fraction((X1 - X3)*((Y3 + 1) - Y4) + (Y1 - (Y3 + 1))*(X4 - X3)) / ((X1 - X2)*((Y3 + 1) - Y4) + (Y1 - Y2)*(X4 - X3)));
			T2 = 0;
		}
	}
	if (T != NULL){
		*T = T1;
	}
	if ((T1 >= 0 && T1 <= 1) && (T2 >= 0 && T2 <= 1)){
		return true;
	}
	else return false;
}
bool C_board::Dot_is_right_side(int X1, int Y1, int X2, int Y2, int X3, int Y3){
	return (X3 - X1)*(Y1 - Y2) <= (Y3 - Y1)*(X1 - X2);
}

const char* C_board::Get_letter(char Input){
	if ('a' <= Input&&Input <= 'z'){
		return Letter[(int)Input - (int)'a' + 11];
	}
	else if ('A' <= Input&&Input <= 'Z'){
		return Letter[(int)Input - (int)'A' + 11];
	}
	else if ('0' <= Input&&Input <= '9'){
		return Letter[(int)Input - (int)'0' + 1];
	}
	else if (Input == '!'){
		return Letter[37];
	}
	else if (Input == ':'){
		return Letter[38];
	}
	else if (Input == '.'){
		return Letter[39];
	}
	else if (Input == ','){
		return Letter[40];
	}
	else {
		return Letter[0];
	}
}
void C_board::Draw_letter(unsigned X, unsigned Y, const char * Input, const char * Str, char color){//BOARD_VERGE포함좌표.(Screen객체상 좌표).
	const char * Letter;
	const char L[3] = { Str[0], Str[1], '\0' };
	unsigned Loop1, Loop2, Loop3;
	Loop1 = 0;
	Screen.Setcolor(color);
	for (; *Input != '\0'; Input++){//한 글자.
		Letter = Get_letter(*Input);
		for (Loop2 = 0; Loop2 < 5; Loop2++){//한 줄.
			Screen << Coord(X + Loop1, Y + Loop2);
			for (Loop3 = 0; Loop3 < 3; Loop3++){//한 문자.
				if (Letter[Loop2] & (0x04 >> (unsigned)Loop3)){
					if (L[0] == ' '&& L[1] == ' '){
						Screen.Reverse_color();
						Screen << L;
						Screen.Reverse_color();
					}
					else {
						Screen << L;
					}
				}
				else {
					Screen << "  ";
				}
			}
			Screen << "  ";
		}
		Loop1 += 8;
	}
}
void C_board::Draw_letter_setscreen(C_screen_color& Screen, unsigned X, unsigned Y, const char * Input, const char * Str, char color){
	const char * Letter;
	const char L[3] = { Str[0], Str[1], '\0' };
	unsigned Loop1, Loop2, Loop3;
	Loop1 = 0;
	Screen.Setcolor(color);
	for (; *Input != '\0'; Input++){//한 글자.
		Letter = Get_letter(*Input);
		for (Loop2 = 0; Loop2 < 5; Loop2++){//한 줄.
			Screen << Coord(X + Loop1, Y + Loop2);
			for (Loop3 = 0; Loop3 < 3; Loop3++){//한 문자.
				if (Letter[Loop2] & (0x04 >> (unsigned)Loop3)){
					if (L[0] == ' '&& L[1] == ' '){
						Screen.Reverse_color();
						Screen << L;
						Screen.Reverse_color();
					}
					else {
						Screen << L;
					}
				}
				else {
					Screen << "  ";
				}
			}
			Screen << "  ";
		}
		Loop1 += 8;
	}
}

bool C_ball::Speed_edit(int Offset, unsigned Set){
	static const unsigned int Max_movenum = 2 * C_board::BRICK_SIZE*C_board::INTERNAL_RESOLUTION / VECTOR_LENGTH;
	bool Check = true;
	if (Set){
		if (Speed_movenum == 1 && Set == 1 && Offset <= 0){//속도를 최저로 요구하였으나 이미 최저인 경우.
			Check = false;
		}
		Speed_movenum = unsigned(
			((int)Set + Offset) ?
			(((int)Set + Offset <= Max_movenum) ? (int)Set + Offset : Max_movenum)
			: 1
			);
	}
	else {
		if (Speed_movenum == 1 && Offset < 0){//속도를 낮추라고 요구하였으나 이미 최저인 경우.
			Check = false;
		}
		Speed_movenum = unsigned(
			((int)Speed_movenum + Offset) ?
			(((int)Speed_movenum + Offset <= Max_movenum) ? (int)Speed_movenum + Offset : Max_movenum)
			: 1
			);
	}
	return Check;
}

unsigned C_ball::Dump_size() const{
	return sizeof(unsigned) + Ball_rec.Dump_size() + sizeof(unsigned) * 3 + sizeof(T_basezone_state);
}
void C_ball::Dump(char * Ptr) const{
	*(unsigned *)Ptr = Dump_size();
	Ptr += sizeof(unsigned);
	Ball_rec.Dump(Ptr);
	Ptr += Ball_rec.Dump_size();
	*(unsigned *)Ptr = Speed_tick;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr =Speed_movenum;
	Ptr += sizeof(unsigned);
	*(unsigned *)Ptr = Tick;
	Ptr += sizeof(unsigned);
	*(T_basezone_state *)Ptr = Basezone_state;
	Ptr += sizeof(T_basezone_state);
}
bool C_ball::Restore(char * Ptr){
	char * Ptr_E = Ptr + *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);
	if (!Ball_rec.Restore(Ptr)){ return false; }
	else { Ptr += Ball_rec.Dump_size(); }
	Speed_tick = *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);
	Speed_movenum = *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);
	Tick = *(unsigned *)Ptr;
	Ptr += sizeof(unsigned);
	Basezone_state = *(T_basezone_state *)Ptr;
	Ptr += sizeof(T_basezone_state);
	if (Ptr == Ptr_E){ return true; }
	else { return false; }
}

//const array variable
const int C_ball::Vector[40][2] = {//좌표 출력 특징 때문에, y좌표는 아래쪽이 양수이므로, 각은 동경에서 시계 방향으로 잰다.
	{ 60, 0 },
	{ 59, 9 },
	{ 57, 19 },
	{ 53, 27 },
	{ 49, 35 },
	{ 42, 42 },
	{ 35, 49 },
	{ 27, 53 },
	{ 19, 57 },
	{ 9, 59 },

	{ 0, 60 },
	{ -9, 59 },
	{ -19, 57 },
	{ -27, 53 },
	{ -35, 49 },
	{ -42, 42 },
	{ -49, 35 },
	{ -53, 27 },
	{ -57, 19 },
	{ -59, 9 },

	{ -60, 0 },
	{ -59, -9 },
	{ -57, -19 },
	{ -53, -27 },
	{ -49, -35 },
	{ -42, -42 },
	{ -35, -49 },
	{ -27, -53 },
	{ -19, -57 },
	{ -9, -59 },

	{ 0, -60 },
	{ 9, -59 },
	{ 19, -57 },
	{ 27, -53 },
	{ 35, -49 },
	{ 42, -42 },
	{ 49, -35 },
	{ 53, -27 },
	{ 57, -19 },
	{ 59, -9 },
};
const char C_board::Letter[][5] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },//공백.
	{ 0x07, 0x05, 0x05, 0x05, 0x07 },//0
	{ 0x02, 0x06, 0x02, 0x02, 0x07 },//1
	{ 0x07, 0x01, 0x07, 0x04, 0x07 },//2
	{ 0x07, 0x01, 0x07, 0x01, 0x07 },//3
	{ 0x05, 0x05, 0x07, 0x01, 0x01 },//4
	{ 0x07, 0x04, 0x07, 0x01, 0x07 },//5
	{ 0x07, 0x04, 0x07, 0x05, 0x07 },//6
	{ 0x07, 0x01, 0x01, 0x01, 0x01 },//7
	{ 0x07, 0x05, 0x07, 0x05, 0x07 },//8
	{ 0x07, 0x05, 0x07, 0x01, 0x07 },//9
	{ 0x07, 0x05, 0x07, 0x05, 0x05 },//A
	{ 0x07, 0x05, 0x06, 0x05, 0x07 },//B
	{ 0x07, 0x04, 0x04, 0x04, 0x07 },//C
	{ 0x06, 0x05, 0x05, 0x05, 0x06 },//D
	{ 0x07, 0x04, 0x07, 0x04, 0x07 },//E
	{ 0x07, 0x04, 0x07, 0x04, 0x04 },//F
	{ 0x07, 0x04, 0x05, 0x05, 0x07 },//G
	{ 0x05, 0x05, 0x07, 0x05, 0x05 },//H
	{ 0x07, 0x02, 0x02, 0x02, 0x07 },//I
	{ 0x03, 0x01, 0x01, 0x01, 0x06 },//J
	{ 0x05, 0x05, 0x06, 0x05, 0x05 },//K
	{ 0x04, 0x04, 0x04, 0x04, 0x07 },//L
	{ 0x05, 0x07, 0x05, 0x05, 0x05 },//M
	{ 0x01, 0x05, 0x07, 0x05, 0x04 },//N
	{ 0x07, 0x05, 0x05, 0x05, 0x07 },//O
	{ 0x07, 0x05, 0x07, 0x04, 0x04 },//P
	{ 0x07, 0x05, 0x05, 0x06, 0x01 },//Q
	{ 0x07, 0x05, 0x06, 0x05, 0x05 },//R
	{ 0x07, 0x04, 0x07, 0x01, 0x07 },//S
	{ 0x07, 0x02, 0x02, 0x02, 0x02 },//T
	{ 0x05, 0x05, 0x05, 0x05, 0x07 },//U
	{ 0x05, 0x05, 0x05, 0x05, 0x02 },//V
	{ 0x05, 0x05, 0x05, 0x07, 0x05 },//W
	{ 0x05, 0x05, 0x02, 0x05, 0x05 },//X
	{ 0x05, 0x05, 0x02, 0x02, 0x02 },//Y
	{ 0x07, 0x01, 0x02, 0x04, 0x07 },//Z
	{ 0x02, 0x02, 0x02, 0x00, 0x02 },//(!)(37)
	{ 0x00, 0x02, 0x00, 0x02, 0x00 },//(:)(38)
	{ 0x00, 0x00, 0x00, 0x00, 0x02 },//(.)(39)
	{ 0x00, 0x00, 0x00, 0x02, 0x04 },//(,)(40)
};
//verge
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







