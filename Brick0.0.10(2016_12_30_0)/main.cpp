#include <iostream>
using namespace std;
#include <Windows.h>
#include <assert.h>
#include "Coordinate_Outputstream.h"
#include "Queue.h"
#include "Fraction.h"
#include "Keyboard.h"

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

//type, class
class C_ball;
class C_board{
private:
	C_board(const C_board& Input);
	C_board& operator=(const C_board& Input);
public:
//protected:
	enum{
		OUTPUT_TICK = 30,//화면 출력간격.(단위 ms).
		BRICK_SIZE = 6,//"하나의 벽돌이 몇개의 문자인가?" 6권장.(굳이 바꾸면 이상해보임.)
		INTERNAL_RESOLUTION = 100,//짝수 권장.
		//필수조건. BRICK_SIZE*INTERNAL_RESOLUTON mod 4 == 0, BRICK_SIZE*INTERNAL_RESOLUTON >= VECTOR_LENGTH.
		//권장. (BRICK_SIZE*INTERNAL_RESOLUTON == 2 * VECTOR_LENGTH).
		BALL_MAX_NUM = 10,//공 최대 갯수.
	};
	const unsigned BASE_SIZE;//사용자의 공간.(이 공간에 벽돌이 들어오면 게임오버)(벽돌수 기준).
	const unsigned FIELD_SIZE;//HITTING_ZONE으로부터 벽돌까지 기본거리.(벽돌수 기준).
	const unsigned BRICK_HEIGHT;
	const unsigned BOARD_SIZE;// FIELD + (HEIGHT+HITTING) * 2
	const unsigned BOARD_SIZE_IN;//BOARD_SIZE*BRICK_SIZE*INTERNAL_RESOLUTION. IN(TERANL BOARD)(for great quality).
	const C_screen_color::T_color BACK_COLOR;

	unsigned Playbrick_speed_tick;
	unsigned Playbrick_tick;

	class C_brick{
	public:
		unsigned Health;
		enum Brick_state{//NONE은 벽돌일수 없는 곳(공이 진입할 수 없음). NORMAL이고 Health==0일때 빈 공간을 의미.
			NONE = 0, NORMAL=1,
		} State;
		C_brick(){ Health = 1; State = NONE; }//NONE도 반사성을 지녀야 하기 때문에 체력이 0이 아님.
		C_brick(int H){ if (H >= 0){ Health = H; State = NORMAL; } else { Health = 0; State = NONE; } }
		C_brick(unsigned H, Brick_state _State){ Health = H; State = _State; }
	};
	friend class C_board::C_brick;

	C_brick **Board;
	C_queue<C_ball> Balls;
	C_screen_color Screen;
	enum Direction{
		R = 0, DR = 5, D = 10, DL = 15, L = 20, UL = 25, U = 30, UR = 35,
	};
	struct T_reflector {
		int X, Y;
		unsigned Position;
		unsigned Move_length;//Reflector의 한칸 이동 길이(단위길이: INTERNAL_RESOLUTION).
		unsigned Length;//Reflector길이의 절반(단위길이: VECTOR_LENGTH).
	} Reflector;

	//function.
	void Game_init();
	void Print_ball(unsigned Ball_num, bool Print = true);
	void Print_Reflector(bool Print=true);
	void Print_brick(unsigned X, unsigned Y);
	void Print_hitting_zone();
	void Get_reflector_edge(int * X1, int * Y1, int * X2, int * Y2);//Reflector의 사이즈 정보도 포함.
	int Dead_verge();
	bool Play_brick();//true는 게임오버.
	bool Play_ball(unsigned Ball_num);//공이 사라져야 할 경우 true반환.
	bool Play_ball_ref(C_ball& Ball);
	void Play_ball_brick(C_ball&Ball);
	void Move_reflector(Direction Input, unsigned Speed=1);
	void Arrange_reflector();
	void Brick_add(unsigned X,unsigned Y);
	void Brick_hit(unsigned X, unsigned Y);
	void Screen_recorvery();
	void Print_line(int X1, int Y1, int X2, int Y2, const char* Letter, int X_ref=-1, int Y_ref=-1, char Ref_color=0);

public:
	C_board(unsigned Base_size = 5, unsigned Field_size = 4, unsigned Brick_height = 6);
	~C_board(){
		int Temp1;
		for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
			delete[] Board[Temp1];
		}
		delete[] Board;
	}

	void PLAY();

	static bool Box_in_line(int X1, int Y1, int X2, int Y2, int Box_X1, int Box_Y1, int Box_X2, int Box_Y2);//line은 직선 취급.
	static bool Line_in_line(int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4, C_fraction* T = NULL);//line은 선분 취급.매개변수직선과 직선의 교점의 t값을 T로 리턴.
	static bool Dot_is_right_side(int X1, int Y1, int X2, int Y2, int X3, int Y3);//(X1,Y1)에서 (X2,Y2)로 가는 백터를 그을 때, (X3,Y3)점이 이 백터의 오른쪽에 있는가?(경계선도 포함).
	template <typename T>
	static T HIGH(T a, T b){ return a > b ? a : b; }
	template <typename T>
	static T LOW(T a, T b){ return a < b ? a : b; }
	static int FLOOR(int a, unsigned b){ return a - (a%b); }//내림.
	static int CEIL(int a, unsigned b){ return (a%b == 0) ? a : a + b - (a%b); }//올림.
	static int ROUND(int a, unsigned b){ return ((a%b) * 2<b) ? a - (a%b) : a + b - (a%b); }//반올림.

};

class C_ball{
	friend class C_board;

protected:
	static const int Vector[40][2];//Vector[DIRECTION_NUM][2].
	enum{
		SHADOW_NUM=50,//그림자수.
		DIRECTION_NUM = 40,//백터 방향 수.
	};
	class C_ball_vector{
	public:
		int X, Y;
		unsigned Vector_num;
		C_ball_vector(){ X = Y = Vector_num = 0; }
		C_ball_vector(int _X, int _Y, unsigned V_num){ X = _X; Y = _Y; Vector_num = V_num; }
		C_ball_vector(const C_ball_vector& Input){ X = Input.X; Y = Input.Y; Vector_num = Input.Vector_num; }
		C_ball_vector& operator=(const C_ball_vector& Input){ X = Input.X; Y = Input.Y; Vector_num = Input.Vector_num; return *this; }
		int Vector_X() const{ return C_ball::Vector[Vector_num][0]; }
		int Vector_Y() const{ return C_ball::Vector[Vector_num][1]; }
	};

	C_queue<C_ball_vector> Ball_rec;
	unsigned Speed_tick;
	unsigned Speed_movenum;
	//속력=진동수*파장=파장/주기. Tick이 주기 효과이고, movenum이 파장 효과.
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
};

//variable
#define VECTOR_LENGTH 60
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

//function

//main
int main(){
	C_board Test;
	Test.PLAY();
	return 0;
}

//function define
C_board::C_board(unsigned Base_size, unsigned Field_size, unsigned Brick_height)
	:BASE_SIZE(Base_size), FIELD_SIZE(Field_size), BRICK_HEIGHT(Brick_height),
	BOARD_SIZE(BASE_SIZE + (BRICK_HEIGHT + FIELD_SIZE) * 2), BOARD_SIZE_IN(BOARD_SIZE * BRICK_SIZE * INTERNAL_RESOLUTION),
	BACK_COLOR(C_screen_color::T_color::GREY),
	Balls(BALL_MAX_NUM),
	Screen(BOARD_SIZE*BRICK_SIZE * 2, BOARD_SIZE*BRICK_SIZE, C_screen_color::BLACK, BACK_COLOR){
	int Temp1, Temp2;
	//Board할당.
	Board = new C_brick*[BOARD_SIZE];
	for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
		Board[Temp1] = new C_brick[BOARD_SIZE];
	}

	//게임 시작 전 정보 소개.
	system("color f0");
	cout << "4방향 벽돌깨기"<<endl;
	cout << "제작자: 이명진" << endl;
	cout << "if you can't read above letter, you need EUC-KR format. (Also required in game)"<<endl;
	cout << "반사판 움직이기: 방향키."<<endl;
	cout << "반사판 돌리기: W,D (왼쪽 쉬프트를 같이누르면 2배 가속.)"<<endl;
	cout << "블럭 추가하기: E" << endl;
	cout << "첫 공 발사: 스페이스키를 길게 눌렀다 떼기." << endl;
	cout << "만약 플레이중에 도스창의 크기변경으로 인한 화면깨짐이 일어났다면, 스페이스키로 복구해주세요." << endl;
	cout << "정상적인 플레이를 위해서, 도스창을 우클릭하여 속성에서 폰트크기를 최소(5)로 설정해주세요."<<endl;
	cout << "설정이 완료되었다면, 스페이스키로 진행해주세요.";
	C_keyboard::Check(C_keyboard::SPACE);
	while (C_keyboard::Check_out(C_keyboard::SPACE) == false){ ; }//스페이스를 떼면 콘솔크기변경.
	Screen.Setcolor(C_screen_color::BLACK, C_screen_color::WHITE);//출력 과정에서 검정색화면 뜨는것 방지.
	Screen.Set_consol();
}
void C_board::Game_init(){
	int Temp1, Temp2, H1, H2, Vector_num;
	C_time Time;

	//Screen초기화.
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	Screen.Clrscr();
	//Board초기화.
	for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
		for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){
			if ((BRICK_HEIGHT <= Temp1&&Temp1 < BOARD_SIZE - BRICK_HEIGHT) ^
				(BRICK_HEIGHT <= Temp2&&Temp2 < BOARD_SIZE - BRICK_HEIGHT)){
				if (random(40)){
					if (random(30)){
						if (random(20)){
							if (random(10)){
								Board[Temp1][Temp2].Health = 1;
							}
							else { Board[Temp1][Temp2].Health = 2; }
						}
						else { Board[Temp1][Temp2].Health = 3; }
					}
					else { Board[Temp1][Temp2].Health = 4; }
				}
				else { Board[Temp1][Temp2].Health = 5; }
				Board[Temp1][Temp2].State = C_brick::NORMAL;
			}
			else if ((BRICK_HEIGHT <= Temp1&&Temp1 < BOARD_SIZE - BRICK_HEIGHT) &&
				(BRICK_HEIGHT <= Temp2&&Temp2 < BOARD_SIZE - BRICK_HEIGHT)){
				Board[Temp1][Temp2].Health = 0;
				Board[Temp1][Temp2].State = C_brick::NORMAL;
			}
		}
	}
	Playbrick_speed_tick = 300;
	Playbrick_tick = 1;
	//공 초기화.
	while (Balls.Num()){ Balls.Out(); }
	//Reflector 초기화.
	Reflector.X = Reflector.Y = FLOOR(BOARD_SIZE_IN / 2, INTERNAL_RESOLUTION) + INTERNAL_RESOLUTION / 2;
	Reflector.Move_length = 3;
	Reflector.Length = CEIL(BRICK_SIZE * INTERNAL_RESOLUTION * 4 / (unsigned)3, VECTOR_LENGTH) / VECTOR_LENGTH;
	Reflector.Position = C_ball::DIRECTION_NUM/4;
	//Screen 셋팅
	for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
		for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){
			Print_brick(Temp2, Temp1);
		}
	}
	H1 = BRICK_HEIGHT * BRICK_SIZE - 1;
	H2 = (BOARD_SIZE - BRICK_HEIGHT) * BRICK_SIZE;
	Screen.Setcolor(C_screen_color::RED, C_screen_color::BLACK);
	for (Temp1 = 0; Temp1 <= H1; Temp1++){
		Screen << Coord(H1 * 2, Temp1) << "  " << Coord(H2 * 2, Temp1) << "  " << Coord(Temp1 * 2, H1) << "  " << Coord(Temp1 * 2, H2) << "  ";
	}
	for (Temp1 = H2; Temp1 < BOARD_SIZE*BRICK_SIZE; Temp1++){
		Screen << Coord(H1 * 2, Temp1) << "  " << Coord(H2 * 2, Temp1) << "  " << Coord(Temp1 * 2, H1) << "  " << Coord(Temp1 * 2, H2) << "  ";
	}
	Screen.Show_opt_kr();

	//공 발사.
	C_keyboard::Check(C_keyboard::SPACE);
	while (C_keyboard::Check_press(C_keyboard::SPACE) == false){ ; }//스페이스를 누르면 공 조준 시작.
	Vector_num = random(C_ball::DIRECTION_NUM);
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	while (1){
		Time.Reset_time();
		Print_line(
			BOARD_SIZE_IN / 2,
			BOARD_SIZE_IN / 2,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][0] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][1] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			"  "
			);
		if (C_keyboard::Check_state(C_keyboard::SPACE) == false){ break; }//스페이스를 떼면 발사.
		Vector_num = (Vector_num + 1) % C_ball::DIRECTION_NUM;
		Print_line(
			BOARD_SIZE_IN / 2,
			BOARD_SIZE_IN / 2,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][0] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][1] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			"■"
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
void C_board::PLAY(){
	bool Check=false;
	unsigned Temp1, Temp2;
	C_time Time;
	int Lag_check;
	while (1){
		//게임의 시작. 공 쏘기.
		Game_init();
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

		while (Check){//한 프레임.
			Time.Reset_time();

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
			for (Temp1 = 0; Temp1 < Screen.GetL(); Temp1+=2){
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
#endif

			//게임의 플레이. 루프 및 지속적 Reflector조작
			//Some Object Delete Screen(for optimization)
			for (Temp1 = 0; Temp1 < BALL_MAX_NUM; Temp1++){
				Print_ball(Temp1, false);
			}
			Print_Reflector(false);
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
				Move_reflector(UL);//시계반대방향 회전.
			}
			if (C_keyboard::Check_state(C_keyboard::D)){
				Move_reflector(UR);//시계방향 회전.
			}
			if (C_keyboard::Check_press(C_keyboard::SPACE)){
				Screen_recorvery();
			}
			if (C_keyboard::Check_press(C_keyboard::E)){ if (Play_brick()){ Check = false; } }
#ifdef MYDEBUG
			while (C_keyboard::Check_state(C_keyboard::A)){ ; }
#endif
			//brick Play
			if (Playbrick_tick == Playbrick_speed_tick){
				Playbrick_tick = 1;
				if (Play_brick()){ Check = false; }
			}
			else { Playbrick_tick++; }
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
				Check = false;
			}
			//화면 출력.
			Print_hitting_zone();
			Print_Reflector();
			for (Temp1 = 0; Temp1 < Balls.Num(); Temp1++){
				Print_ball(Temp1);
			}
			Screen.Show_opt_kr();

			Lag_check = (int)OUTPUT_TICK - Time.Get_time();//잔여 시간.
#ifdef MYDEBUG
			if (Lag_check<0){//시간 지연 발생.
				Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
				for (Temp1 = 0; Temp1 < BRICK_HEIGHT*BRICK_SIZE*INTERNAL_RESOLUTION*2; Temp1 += 2){
					if ((int)Temp1 * 3 < -1*Lag_check){//네모 칸수*6이 지연 시간(ms)
						Screen << Coord(Temp1, Screen.GetH()) << "■";
					}
					else {
						Screen << Coord(Temp1, Screen.GetH()) << "  ";
					}
				}
			}
			else {
				Screen.Setcolor(C_screen_color::GREEN, BACK_COLOR);
				for (Temp1 = 0; Temp1 <BRICK_HEIGHT*BRICK_SIZE*INTERNAL_RESOLUTION * 2; Temp1 += 2){
					if ((int)Temp1 * 3 < Lag_check){
						Screen << Coord(Temp1, Screen.GetH()) << "■";
					}
					else {
						Screen << Coord(Temp1, Screen.GetH()) << "  ";
					}
				}
			}
#endif
			while (Time.Get_time() < OUTPUT_TICK){ ; }//속도조절구문.
		}
		//게임 오버.

		"";//재시도 및 종료 점검.
		if (Check==false){ break; }//종료.
		//종료 아니면 재시도.
	}
}

void C_board::Print_ball(unsigned Ball_num, bool Print){
	int Temp1;
	int X_ref, Y_ref;
	if (Ball_num >= Balls.Num()){ return; }
	char *Str;
	if (Print){ Str = "■"; }
	else { Str = "  "; }
	C_queue<C_ball::C_ball_vector> Ball_rec = Balls.Read(Ball_num).Ball_rec;

	for (Temp1 = 0; Temp1 < Ball_rec.Num() - 1; Temp1++){//공 그림자.(무반사).
		Screen.Setcolor(C_screen_color::D_GREY, BACK_COLOR);
		if (Temp1 == Ball_rec.Num() - 2){
			Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
		}
		if (Ball_rec.Read(Temp1).X == -1){ ; }//출력 필요 없음.(Reflector반사표식).
		else if (Ball_rec.Read(Temp1 + 1).X == -1){ ; }//Reflector반사.(생략).
		else if (Ball_rec.Read(Temp1).Vector_num == Ball_rec.Read(Temp1 + 1).Vector_num){//무반사 경우.
			Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1 + 1).X, Ball_rec.Read(Temp1 + 1).Y, Str);
		}
	}
	for (Temp1 = 0; Temp1 < Ball_rec.Num() - 1; Temp1++){//공 그림자.(반사.)
		Screen.Setcolor(C_screen_color::D_GREY, BACK_COLOR);
		if (Temp1 == Ball_rec.Num() - 2){
			Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
		}
		if (Ball_rec.Read(Temp1).X == -1){ ; }//출력 필요 없음.(Reflector반사표식).
		else if (Ball_rec.Read(Temp1 + 1).X == -1){//Reflector반사.
			Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1 + 2).X, Ball_rec.Read(Temp1 + 2).Y, Str);
			Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
			Screen << Coord(
				2 * ((Ball_rec.Read(Temp1).Vector_X() > 0) ? CEIL(Ball_rec.Read(Temp1 + 2).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION - 1 : FLOOR(Ball_rec.Read(Temp1 + 2).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION),
				(Ball_rec.Read(Temp1).Vector_Y() > 0) ? CEIL(Ball_rec.Read(Temp1 + 2).Y, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION - 1 : FLOOR(Ball_rec.Read(Temp1 + 2).Y, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION
				) << Str;
		}
		else if (Ball_rec.Read(Temp1).Vector_num == Ball_rec.Read(Temp1 + 1).Vector_num){ ; }//무반사 경우(생략.).
		else {//벽돌 반사.
			if (Ball_rec.Read(Temp1).Vector_X() != Ball_rec.Read(Temp1 + 1).Vector_X()){
				X_ref = (Ball_rec.Read(Temp1).X + Ball_rec.Read(Temp1).Vector_X() + Ball_rec.Read(Temp1 + 1).X) / 2;
			}
			else { X_ref = -1; }
			if (Ball_rec.Read(Temp1).Vector_Y() != Ball_rec.Read(Temp1 + 1).Vector_Y()){
				Y_ref = (Ball_rec.Read(Temp1).Y + Ball_rec.Read(Temp1).Vector_Y() + Ball_rec.Read(Temp1 + 1).Y) / 2;
			}
			else { Y_ref = -1; }
			Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1).X + Ball_rec.Read(Temp1).Vector_X(), Ball_rec.Read(Temp1).Y + Ball_rec.Read(Temp1).Vector_Y(), Str, X_ref, Y_ref, 0xff & (BACK_COLOR * 16 + C_screen_color::RED));
		}
	}
	//현재 공.
	Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
	Screen << Coord(
		((Ball_rec.Read(Temp1).Vector_X() >= 0) ? FLOOR(Ball_rec.Read(Temp1).X, INTERNAL_RESOLUTION) : CEIL(Ball_rec.Read(Temp1).X, INTERNAL_RESOLUTION) - INTERNAL_RESOLUTION) * 2 / INTERNAL_RESOLUTION,
		((Ball_rec.Read(Temp1).Vector_Y() >= 0) ? FLOOR(Ball_rec.Read(Temp1).Y, INTERNAL_RESOLUTION) : CEIL(Ball_rec.Read(Temp1).Y, INTERNAL_RESOLUTION) - INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION
		) << Str;
}
void C_board::Print_Reflector(bool Print){
	int X1, Y1, X2, Y2;
	char *Str;
	if (Print){ Str = "■"; }
	else { Str = "  "; }
	Get_reflector_edge(&X1, &Y1, &X2, &Y2);
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	Print_line(X1, Y1, X2, Y2, Str);
}
void C_board::Print_brick(unsigned X, unsigned Y){
	int Temp1, Temp2;
	C_screen_color::T_color col;
	if (X >= BOARD_SIZE){ return; }
	if (Y >= BOARD_SIZE){ return; }
	if (Board[Y][X].State == C_brick::NONE){ return; }

	switch (Board[Y][X].State){
	case C_brick::NORMAL:
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
			for (Temp1 = 0; Temp1 < BRICK_SIZE; Temp1++){
				for (Temp2 = 0; Temp2 < BRICK_SIZE; Temp2++){
					Screen << Coord((X*BRICK_SIZE + Temp1) * 2, Y*BRICK_SIZE + Temp2) << "  ";
				}
			}
		}
		else if (Board[Y][X].Health > 0){
			for (Temp1 = 0; Temp1 < BRICK_SIZE; Temp1++){
				Screen << Coord((X*BRICK_SIZE + Temp1) * 2, Y*BRICK_SIZE) << "■";
				Screen << Coord((X*BRICK_SIZE + Temp1) * 2, Y*BRICK_SIZE + BRICK_SIZE - 1) << "■";
			}
			for (Temp1 = 0; Temp1 < BRICK_SIZE - 2; Temp1++){
				Screen << Coord(X*BRICK_SIZE * 2, Y*BRICK_SIZE + 1 + Temp1) << "■";
				Screen << Coord((X*BRICK_SIZE + BRICK_SIZE - 1) * 2, Y*BRICK_SIZE + 1 + Temp1) << "■";
				for (Temp2 = 0; Temp2 < BRICK_SIZE - 2; Temp2++){
					Screen << Coord((X*BRICK_SIZE + 1 + Temp2) * 2, Y*BRICK_SIZE + 1 + Temp1) << "  ";
				}
			}
		}
		break;
	default:
		"";//벽돌속성추가.
		break;
	}
}
void C_board::Print_hitting_zone(){
	static int Verge = Dead_verge();
	int H[4];
	int Temp1;
	if (Verge != Dead_verge()){//Verge달라짐. 지우기 필요.
		H[0] = (BRICK_HEIGHT + FIELD_SIZE - Verge + 1) * BRICK_SIZE;
		H[1] = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge - 1) * BRICK_SIZE - 1;
		for (Temp1 = H[0]; Temp1 <= H[1]; Temp1++){
			Screen << Coord(Temp1 * 2, H[0]) << "  " << Coord(Temp1 * 2, H[1]) << "  ";
			Screen << Coord(H[0] * 2, Temp1) << "  " << Coord(H[1] * 2, Temp1) << "  ";
		}
	}
	//출력.
	Verge = Dead_verge();
	H[0] = (BRICK_HEIGHT + FIELD_SIZE - Verge + 1) * BRICK_SIZE;
	H[1] = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge - 1) * BRICK_SIZE - 1;
	H[2] = (BRICK_HEIGHT + FIELD_SIZE) * BRICK_SIZE;
	H[3] = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE) * BRICK_SIZE - 1;
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	for (Temp1 = H[0]; Temp1 <= H[1]; Temp1++){
		Screen << Coord(Temp1 * 2, H[0]) << "━" << Coord(Temp1 * 2, H[1]) << "━";
		Screen << Coord(H[0] * 2, Temp1) << "┃" << Coord(H[1] * 2, Temp1) << "┃";
	}
	Screen << Coord(H[0] * 2, H[0]) << "┏" << Coord(H[0] * 2, H[1]) << "┗" << Coord(H[1] * 2, H[1]) << "┛" << Coord(H[1] * 2, H[0]) << "┓";
	Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
	for (Temp1 = H[2]; Temp1 <= H[3]; Temp1++){
		Screen << Coord(Temp1 * 2, H[2]) << "━" << Coord(Temp1 * 2, H[3]) << "━";
		Screen << Coord(H[2] * 2, Temp1) << "┃" << Coord(H[3] * 2, Temp1) << "┃";
	}
	Screen << Coord(H[2] * 2, H[2]) << "┏" << Coord(H[2] * 2, H[3]) << "┗" << Coord(H[3] * 2, H[3]) << "┛" << Coord(H[3] * 2, H[2]) << "┓";
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
		for (Loop2 = BRICK_HEIGHT + FIELD_SIZE - 1 - Loop1; Loop2 <= BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Loop1; Loop2++){
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
	static unsigned Way = 0;
	int Temp1, Temp2, Temp3, Temp4;
	unsigned X, Y;
	int X_in, Y_in;
	C_ball::C_ball_vector* Ball;
	switch (Way){
	case 0://위
		for (Temp1 = 0; Temp1 < BASE_SIZE + 2 * FIELD_SIZE; Temp1++){//한 줄.
			X = BRICK_HEIGHT + Temp1;
			for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){//줄 내부 처리.
				if (Board[Temp2][X].Health == 0){//블럭 존재 안함.-->밀리기 가능.
					for (Temp3 = Temp2; Temp3 > 0; Temp3--){
						Board[Temp3][X] = Board[Temp3 - 1][X];
						Print_brick(X, Temp3);
					}
					Brick_add(X, 0);
					Print_brick(X, 0);
					//공이 포함되면 밀기.
					for (Temp3 = 0; Temp3 < Balls.Num(); Temp3++){//하나의 공.
						for (Temp4 = 0; Temp4 < Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//공의 그림자중 하나.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = X*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->X &&
								Ball->X <= X_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in <= Ball->Y &&
								Ball->Y <= Y_in + BRICK_SIZE*INTERNAL_RESOLUTION
								){//공의 그림자중 하나가 벽돌 내부 및 좌우에 존재하는 경우.
								Ball->Y += BRICK_SIZE*INTERNAL_RESOLUTION;
							}
						}
					}
					break;
				}
			}
		}
		break;
	case 1://오른쪽
		for (Temp1 = 0; Temp1 < BASE_SIZE + 2 * FIELD_SIZE; Temp1++){//한 줄.
			Y = BRICK_HEIGHT + Temp1;
			for (Temp2 = BOARD_SIZE - 1; Temp2 >= 0; Temp2--){//줄 내부 처리.
				if (Board[Y][Temp2].Health == 0){//블럭 존재 안함.-->밀리기 가능.
					for (Temp3 = Temp2; Temp3 < BOARD_SIZE - 1; Temp3++){
						Board[Y][Temp3] = Board[Y][Temp3 + 1];
						Print_brick(Temp3, Y);
					}
					Brick_add(BOARD_SIZE - 1, Y);
					Print_brick(BOARD_SIZE - 1, Y);
					//공이 포함되면 밀기.
					for (Temp3 = 0; Temp3 < Balls.Num(); Temp3++){//하나의 공.
						for (Temp4 = 0; Temp4 < Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//공의 그림자중 하나.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Y*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in <= Ball->X &&
								Ball->X <= X_in + BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->Y &&
								Ball->Y <= Y_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION
								){//공의 그림자중 하나가 벽돌 내부 및 좌우에 존재하는 경우.
								Ball->X -= BRICK_SIZE*INTERNAL_RESOLUTION;
							}
						}
					}
					break;
				}
			}
		}
		break;
	case 2://아래
		for (Temp1 = 0; Temp1 < BASE_SIZE + 2 * FIELD_SIZE; Temp1++){//한 줄.
			X = BRICK_HEIGHT + Temp1;
			for (Temp2 = BOARD_SIZE - 1; Temp2 >= 0; Temp2--){//줄 내부 처리.
				if (Board[Temp2][X].Health == 0){//블럭 존재 안함.-->밀리기 가능.
					for (Temp3 = Temp2; Temp3 < BOARD_SIZE - 1; Temp3++){
						Board[Temp3][X] = Board[Temp3 + 1][X];
						Print_brick(X, Temp3);
					}
					Brick_add(X, BOARD_SIZE - 1);
					Print_brick(X, BOARD_SIZE - 1);
					//공이 포함되면 밀기.
					for (Temp3 = 0; Temp3 < Balls.Num(); Temp3++){//하나의 공.
						for (Temp4 = 0; Temp4 < Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//공의 그림자중 하나.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = X*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->X &&
								Ball->X <= X_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in <= Ball->Y &&
								Ball->Y <= Y_in + BRICK_SIZE*INTERNAL_RESOLUTION
								){//공의 그림자중 하나가 벽돌 내부 및 좌우에 존재하는 경우.
								Ball->Y -= BRICK_SIZE*INTERNAL_RESOLUTION;
							}
						}
					}
					break;
				}
			}
		}
		break;
	case 3://왼쪽
		for (Temp1 = 0; Temp1 < BASE_SIZE + 2 * FIELD_SIZE; Temp1++){//한 줄.
			Y = BRICK_HEIGHT + Temp1;
			for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){//줄 내부 처리.
				if (Board[Y][Temp2].Health == 0){//블럭 존재 안함.-->밀리기 가능.
					for (Temp3 = Temp2; Temp3 > 0; Temp3--){
						Board[Y][Temp3] = Board[Y][Temp3 - 1];
						Print_brick(Temp3, Y);
					}
					Brick_add(0, Y);
					Print_brick(0, Y);
					//공이 포함되면 밀기.
					for (Temp3 = 0; Temp3 < Balls.Num(); Temp3++){//하나의 공.
						for (Temp4 = 0; Temp4 < Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//공의 그림자중 하나.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Y*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in <= Ball->X &&
								Ball->X <= X_in + BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->Y &&
								Ball->Y <= Y_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION
								){//공의 그림자중 하나가 벽돌 내부 및 좌우에 존재하는 경우.
								Ball->X += BRICK_SIZE*INTERNAL_RESOLUTION;
							}
						}
					}
					break;
				}
			}
		}
		break;
	}
	Arrange_reflector();
	Way = (Way + 1) % 4;
	if (Dead_verge() == -1){
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
				if (Ball.Basezone_check((BRICK_HEIGHT + FIELD_SIZE)*BRICK_SIZE*INTERNAL_RESOLUTION, (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE)*BRICK_SIZE*INTERNAL_RESOLUTION)
					== false){//공이 감속불가로 사라져야됨.
					return true;
				}
			}
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
		if (Ball.Ball_rec.Num() == Ball.SHADOW_NUM){ Ball.Ball_rec.Out(); Ball.Ball_rec.Out(); }//Ball_rec공간 확보.
		else if (Ball.Ball_rec.Num() == Ball.SHADOW_NUM - 1){ Ball.Ball_rec.Out(); }
		Ball.Ball_rec.In(C_ball::C_ball_vector(-1, -1, 0));//Reflector반사되었다는 표식.
		Ball.Ball_rec.In(
			C_ball::C_ball_vector(
			int(Lastball.X + Lastball.Vector_X()*(Ref_t.Get_value()) + 3 * Ball_is_right * C_ball::Vector[Reflector.Position][0] / (double)VECTOR_LENGTH),
			int(Lastball.Y + Lastball.Vector_Y()*(Ref_t.Get_value()) + 3 * Ball_is_right * C_ball::Vector[Reflector.Position][1] / (double)VECTOR_LENGTH),
			(2 * Reflector.Position + C_ball::DIRECTION_NUM * 3 / 2 - Lastball.Vector_num) % C_ball::DIRECTION_NUM)
			);
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
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X - 1, Brick_Y) : 0;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y) : 0;
				}
				else if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y :
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y) : 0;
				}
			}
			else {//Y축 비경계선.
				if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y :
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y) : 0;
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
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y - 1) : 0;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y) : 0;
				}
				else if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_X() > 0) ?
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X :
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X;
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y) : 0;
				}
			}
			else {//Y축 비경계선.
				if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_X() > 0) ?
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X :
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X;
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y) : 0;
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
					(Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y) : 0;
				}
				else { Brick_X--; }
			}
			if (Ref_X1) {
				if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
					Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y) : 0;
				}
				else { Brick_X++; }
			}
			if (Ref_Y0){
				if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y) ? Brick_hit(Brick_X, Brick_Y - 1) : 0;
				}
				else { Brick_Y--; }
			}
			if (Ref_Y1){
				if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X, Brick_Y + 1) : 0;
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
						(Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y) : 0;
					}
				}
				if (Temp == Ref_tX1){
					if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
						Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						(Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y) : 0;
					}
				}
				if (Temp == Ref_tY0){
					if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
						Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						(Brick_Y) ? Brick_hit(Brick_X, Brick_Y - 1) : 0;
					}
				}
				if (Temp == Ref_tY1){
					if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
						Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
						(Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X, Brick_Y + 1) : 0;
					}
				}
			}
		}
		else if (Check == 2){//모서리로 가는 반사.
			if (Ref_X0&&Ref_Y0){
				if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){
					Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y) : 0;
					Check--;
				}
				if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y) ? Brick_hit(Brick_X, Brick_Y - 1) : 0;
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
						(Brick_X&&Brick_Y) ? Brick_hit(Brick_X - 1, Brick_Y - 1) : 0;
					}
				}
				else if (Check == 1){//옆 블럭 1개에서 반사된 경우.
					(Brick_X&&Brick_Y) ? Brick_hit(Brick_X - 1, Brick_Y - 1) : 0;
				}
			}
			else if (Ref_Y0&&Ref_X1){
				if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y) ? Brick_hit(Brick_X, Brick_Y - 1) : 0;
					Check--;
				}
				if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
					Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y) : 0;
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
						(Brick_Y&&Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y - 1) : 0;
					}
				}
				else if (Check == 1){
					(Brick_Y&&Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y - 1) : 0;
				}
			}
			else if (Ref_X1&&Ref_Y1){
				if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
					Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y) : 0;
					Check--;
				}
				if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X, Brick_Y + 1) : 0;
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
						(Brick_X != BOARD_SIZE - 1 && Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y + 1) : 0;
					}
				}
				else if (Check == 1){
					(Brick_X != BOARD_SIZE - 1 && Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X + 1, Brick_Y + 1) : 0;
				}
			}
			else if (Ref_Y1&&Ref_X0){
				if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_Y != BOARD_SIZE - 1) ? Brick_hit(Brick_X, Brick_Y + 1) : 0;
					Check--;
				}
				if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){
					Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM * 3 / 2 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y) : 0;
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
						(Brick_Y != BOARD_SIZE - 1 && Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y + 1) : 0;
					}
				}
				else if (Check == 1){
					(Brick_Y != BOARD_SIZE - 1 && Brick_X) ? Brick_hit(Brick_X - 1, Brick_Y + 1) : 0;
				}
			}
		}
	}

	//공 반사 결과 삽입.
	Lastball_X_temp = Lastball_X_temp + C_ball::Vector[Lastball_V_num_temp][0];
	Lastball_Y_temp = Lastball_Y_temp + C_ball::Vector[Lastball_V_num_temp][1];
	if (Ball.Ball_rec.Num() == Ball.SHADOW_NUM){ Ball.Ball_rec.Out(); }//Ball_rec공간 확보.
	Ball.Ball_rec.In(C_ball::C_ball_vector(Lastball_X_temp, Lastball_Y_temp, Lastball_V_num_temp));
}
void C_board::Move_reflector(Direction Input, unsigned Speed){
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
	for (Loop1 = 0; Loop1 < Balls.Num(); Loop1++){
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
	if (X_small < (BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION){
		Reflector.X += CEIL((BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION - X_small, INTERNAL_RESOLUTION);
	}
	if (Y_small < (BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION){
		Reflector.Y += CEIL((BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION - Y_small, INTERNAL_RESOLUTION);
	}
	if (X_large >(BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION - 1){
		Reflector.X -= CEIL(X_large - (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION + 1, INTERNAL_RESOLUTION);
	}
	if (Y_large >(BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION - 1){
		Reflector.Y -= CEIL(Y_large - (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION + 1, INTERNAL_RESOLUTION);
	}
}
void C_board::Brick_add(unsigned X, unsigned Y){
	"";//벽돌속성추가.
	if (random(40)){
		if (random(30)){
			if (random(20)){
				if (random(10)){
					Board[Y][X].Health = 1;
				}
				else { Board[Y][X].Health = 2; }
			}
			else { Board[Y][X].Health = 3; }
		}
		else { Board[Y][X].Health = 4; }
	}
	else { Board[Y][X].Health = 5; }
	Board[Y][X].State = C_brick::NORMAL;
}
void C_board::Brick_hit(unsigned X, unsigned Y){
	"";//벽돌속성추가.
	if (Board[Y][X].State == C_brick::NONE){
		return;
	}
	if (Board[Y][X].Health > 0){
		if (Board[Y][X].State == C_brick::NORMAL){
			Board[Y][X].Health--;
			Print_brick(X, Y);
		}
	}
}

void C_board::Screen_recorvery(){
	Screen.Set_consol();
	Screen.Show_opt_kr();
}
void C_board::Print_line(int X1, int Y1, int X2, int Y2, const char* Letter, int X_ref, int Y_ref, char Ref_color){
	int Temp1, Temp2;
	unsigned Screen_X, Screen_Y;
	char Str[3] = { Letter[0], Letter[1], '\0' };
	char Color = Screen.Get_color();
	ERROR_DEBUGSTOP(("Reflection point must be on INTERNAL_RESOLUTION", (X_ref%INTERNAL_RESOLUTION) != 0 && X_ref != -1));
	ERROR_DEBUGSTOP(("Reflection point must be on INTERNAL_RESOLUTION", (Y_ref%INTERNAL_RESOLUTION) != 0 && Y_ref != -1));

	Temp1 = FLOOR(LOW(X1, X2), INTERNAL_RESOLUTION);
	do{
		Temp2 = FLOOR(LOW(Y1, Y2), INTERNAL_RESOLUTION);
		do{

			if (Box_in_line(X1, Y1, X2, Y2,
				Temp1, Temp2, Temp1 + INTERNAL_RESOLUTION, Temp2 + INTERNAL_RESOLUTION)){
				Screen.Setcolor(Color);
				Screen_X = Temp1 * 2 / INTERNAL_RESOLUTION;
				Screen_Y = Temp2 / INTERNAL_RESOLUTION;
				if (X_ref != -1){
					if ((X2 - X_ref)*(Temp1 + (INTERNAL_RESOLUTION / 2) - X_ref) > 0){
						Screen_X = (2 * X_ref - Temp1 - INTERNAL_RESOLUTION) * 2 / INTERNAL_RESOLUTION;
					}
					if (Temp1 == X_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							Temp1, Temp2, Temp1, Temp2 + INTERNAL_RESOLUTION)){
							Screen.Setcolor(Ref_color);
						}
					}
					else if (Temp1 + INTERNAL_RESOLUTION == X_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							Temp1 + INTERNAL_RESOLUTION, Temp2, Temp1 + INTERNAL_RESOLUTION, Temp2 + INTERNAL_RESOLUTION)){
							Screen.Setcolor(Ref_color);
						}
					}
				}
				if (Y_ref != -1){
					if ((Y2 - Y_ref)*(Temp2 + (INTERNAL_RESOLUTION / 2) - Y_ref) > 0){
						Screen_Y = (2 * Y_ref - Temp2 - INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION;
					}
					if (Temp2 == Y_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							Temp1, Temp2, Temp1 + INTERNAL_RESOLUTION, Temp2)){
							Screen.Setcolor(Ref_color);
						}
					}
					else if (Temp2 + INTERNAL_RESOLUTION == Y_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							Temp1, Temp2 + INTERNAL_RESOLUTION, Temp1 + INTERNAL_RESOLUTION, Temp2 + INTERNAL_RESOLUTION)){
							Screen.Setcolor(Ref_color);
						}
					}
				}
				Screen << Coord(Screen_X, Screen_Y) << Str;
			}

			Temp2 += INTERNAL_RESOLUTION;
		} while (Temp2 < CEIL(HIGH(Y1, Y2), INTERNAL_RESOLUTION));
		Temp1 += INTERNAL_RESOLUTION;
	} while (Temp1 < CEIL(HIGH(X1, X2), INTERNAL_RESOLUTION));
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
	if ((T1 >= 0 &&T1 <= 1) && (T2 >= 0 &&T2 <= 1)){
		return true;
	}
	else return false;
}
bool C_board::Dot_is_right_side(int X1, int Y1, int X2, int Y2, int X3, int Y3){
	return (X3 - X1)*(Y1 - Y2) <= (Y3 - Y1)*(X1 - X2);
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
bool C_ball::Basezone_check(int H1, int H2){
	int X = Ball_rec.Read(Ball_rec.Num() - 1).X;
	int Y = Ball_rec.Read(Ball_rec.Num() - 1).Y;
	if (
		(H1 <= X) && (X <= H2) &&
		(H1 <= Y) && (Y <= H2)
		){//Basezone안에 있는 경우.
		if (Basezone_state == BASEZONE_OUT){//basezone으로 들어온경우.
			Basezone_state = BASEZONE_IN;
		}
		if (Basezone_state == START){//시작부터 basezone이면 반사되었다고 침.
			Basezone_state = REF;
		}
	}
	else {//Basezone밖에 있는 경우.
		if (Basezone_state == BASEZONE_IN){//basezone밖으로 나간 경우(반사판에 닿지 않고).
			if (Speed_edit(0, 1) == false){
				return false;
			}
		}
		Basezone_state = BASEZONE_OUT;
	}
	return true;
}
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







