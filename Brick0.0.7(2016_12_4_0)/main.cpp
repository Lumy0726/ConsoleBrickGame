#include <iostream>
using namespace std;
#include <Windows.h>
#include <assert.h>
#include "Coordinate_Outputstream.h"
#include "Queue.h"
#include "Fraction.h"
#include "Keyboard.h"
//mecro

//type, class
class C_ball;
class C_board{
private:
	C_board(const C_board& Input);
	C_board& operator=(const C_board& Input);
public:
//protected:
	enum{
		OUTPUT_FRAME = 30,//화면 출력간격.(단위 ms).
		BRICK_SIZE = 6,//"하나의 벽돌이 몇개의 문자인가?" BRICK_SIZE*INTERNAL_RESOLUTION의 길이에 근사한 정수 방향백터 필요.
		INTERNAL_RESOLUTION = 20,//짝수 권장. BRICK_SIZE*INTERNAL_RESOLUTON mod 4 == 0, BRICK_SIZE*INTERNAL_RESOLUTON >= 60.
		BALL_MAX_NUM = 10,//공 최대 갯수.
	};
	const unsigned HITTING_SIZE;//사용자가 공을 튀기는 공간(벽돌수 기준).
	const unsigned FIELD_SIZE;//HITTING_ZONE으로부터 벽돌까지 기본거리.(벽돌수 기준).
	const unsigned BRICK_HEIGHT;
	const unsigned BOARD_SIZE;// FIELD + (HEIGHT+HITTING) * 2
	const unsigned BOARD_SIZE_IN;//BOARD_SIZE*BRICK_SIZE*INTERNAL_RESOLUTION. IN(TERANL BOARD)(for great quality).
	const C_screen_color::T_color BACK_COLOR;

	class C_brick{
	public:
		unsigned Health;
		enum Brick_state{//NONE은 벽돌일수 없는 곳(공이 진입할 수 없음). NORMAL이고 Health==0일때 빈 공간을 의미.
			NONE = 0, NORMAL=1,
		} State;
		C_brick(){ Health = 1; State = NONE; }//NONE도 반사성을 지녀야 하기 때문에 채력이 0이 아님.
		C_brick(int H){ if (H >= 0){ Health = H; State = NORMAL; } else { Health = 0; State = NONE; } }
		C_brick(unsigned H, Brick_state _State){ Health = H; State = _State; }
		void Brick_hit(C_board& Board);
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
		Direction Position;//Reflector의 법선벡터 방향(12면). 0,5,10---35.
	} Reflector;
	//Please add reflector.

	void Print_brick(unsigned X, unsigned Y);
	void Get_reflector_edge(int * X1, int * Y1, int * X2, int * Y2);//Reflector의 사이즈 정보도 포함.
	void Print_Reflector();
	//Balls function.
	void Print_ball(unsigned Ball_num);
	void Print_hitting_zone();
	void Print_line(int X1, int Y1, int X2, int Y2, int X_ref=-1, int Y_ref=-1);
	bool Play_ball_ref(C_ball& Ball);
	void Play_ball_brick(C_ball&Ball);
	void Play_ball(unsigned Ball_num);
	void Move_reflector(Direction Input);

public:
	C_board(unsigned H_size = 5, unsigned F_size = 4, unsigned B_height = 6);
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
	static const int Vector[40][2];
	enum{
		SHADOW_NUM=10,//그림자수.
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
	unsigned Speed_frame;
	unsigned Tick;//틱이 0이면 정지, 틱은 1부터 Speed_frame까지 순환.

public:
	C_ball(int _X = 0, int _Y = 0, unsigned V_num = 0) :Ball_rec(SHADOW_NUM){
		C_ball_vector Temp(_X, _Y, V_num);
		Ball_rec.In(Temp); Speed_frame = 1; Tick = 1; "공 속도 조절.";
	}
	C_ball(const C_ball& Input) :Ball_rec(Input.Ball_rec){
		Speed_frame = Input.Speed_frame; Tick = Input.Tick;
	}
	C_ball& operator=(const C_ball& Input){
		Ball_rec = Input.Ball_rec; Speed_frame = Input.Speed_frame; Tick = Input.Tick; return *this;
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
C_board::C_board(unsigned H_size, unsigned F_size, unsigned B_height)
	:HITTING_SIZE(H_size), FIELD_SIZE(F_size), BRICK_HEIGHT(B_height),
	BOARD_SIZE(HITTING_SIZE + (BRICK_HEIGHT + FIELD_SIZE) * 2), BOARD_SIZE_IN(BOARD_SIZE * BRICK_SIZE * INTERNAL_RESOLUTION),
	BACK_COLOR(C_screen_color::T_color::GREY),
	Balls(BALL_MAX_NUM),
	Screen(BOARD_SIZE*BRICK_SIZE * 2, BOARD_SIZE*BRICK_SIZE, C_screen_color::BLACK, BACK_COLOR){
	int Temp1, Temp2, H1, H2;
	//Board할당.
	Board = new C_brick*[BOARD_SIZE];
	for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
		Board[Temp1] = new C_brick[BOARD_SIZE];
	}
	//Board초기화.
	for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
		for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){
			if ((BRICK_HEIGHT <= Temp1&&Temp1 < BOARD_SIZE - BRICK_HEIGHT) ^
				(BRICK_HEIGHT <= Temp2&&Temp2 < BOARD_SIZE - BRICK_HEIGHT)){
				Board[Temp1][Temp2].Health = random(5) + 1;
				Board[Temp1][Temp2].State = C_brick::NORMAL;
			}
			else if ((BRICK_HEIGHT <= Temp1&&Temp1 < BOARD_SIZE - BRICK_HEIGHT) &&
				(BRICK_HEIGHT <= Temp2&&Temp2 < BOARD_SIZE - BRICK_HEIGHT)){
				Board[Temp1][Temp2].Health = 0;
				Board[Temp1][Temp2].State = C_brick::NORMAL;
			}
		}
	}
	Reflector.X = Reflector.Y = FLOOR(BOARD_SIZE_IN / 2, INTERNAL_RESOLUTION) + INTERNAL_RESOLUTION / 2;//Reflector.
	Reflector.Position = U;

	//게임 시작 전 정보 소개.
	system("color f0");
	cout << "4방향 벽돌깨기"<<endl;
	cout << "제작자: 이명진" << endl;
	cout << "if you can't read above letter, you need EUC-KR format. (Also required in game)"<<endl;
	cout << "반사판 움직이기: 방향키."<<endl;
	cout << "반사판 돌리기: W,D"<<endl;
	cout << "정상적인 플레이를 위해서, 도스창을 우클릭하여 속성에서 폰트크기를 최소(5)로 설정해주세요."<<endl;
	cout << "스페이스바로 시작합니다.";

	//Screen 셋팅
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
		for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){
			Print_brick(Temp2, Temp1);
		}
	}
	Print_Reflector();
	//경계선들
	Print_hitting_zone();
	H1 = BRICK_HEIGHT * BRICK_SIZE - 1;
	H2 = (BOARD_SIZE - BRICK_HEIGHT) * BRICK_SIZE;
	Screen.Setcolor(C_screen_color::RED, C_screen_color::BLACK);
	for (Temp1 = 0; Temp1 <= H1; Temp1++){
		Screen << Coord(H1*2, Temp1) << "  " << Coord(H2*2, Temp1) << "  " << Coord(Temp1*2, H1) << "  " << Coord(Temp1*2, H2) << "  ";
	}
	for (Temp1 = H2; Temp1 < BOARD_SIZE*BRICK_SIZE; Temp1++){
		Screen << Coord(H1 * 2, Temp1) << "  " << Coord(H2 * 2, Temp1) << "  " << Coord(Temp1 * 2, H1) << "  " << Coord(Temp1 * 2, H2) << "  ";
	}
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
			case 3: col = C_screen_color::BLUE; break;
			case 2: col = C_screen_color::GREEN; break;
			case 1: col = C_screen_color::SKYBLUE; break;
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
		"";
		break;
	}
	if (Board[Y][X].Health >= 5){
		col = C_screen_color::RED;
	}
	else {
		switch (Board[Y][X].Health){
		case 4: col = C_screen_color::VIOLET; break;
		case 3: col = C_screen_color::BLUE; break;
		case 2: col = C_screen_color::GREEN; break;
		case 1: col = C_screen_color::SKYBLUE; break;
		default: col = C_screen_color::BLACK; break;
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
}
void C_board::C_brick::Brick_hit(C_board& Board){
	if (Health>0){
		if (State == NORMAL){
			Health--;
		}
	}
}
void C_board::PLAY(){
	bool Check=false;
	unsigned Loop1, Loop2;
	C_time Time;

	C_keyboard::Check(C_keyboard::SPACE);
	while (!(C_keyboard::Check_press(C_keyboard::SPACE))){ ; }//스페이스 눌리면 시작.

	Screen.Setcolor(C_screen_color::BLACK, C_screen_color::WHITE);//출력 과정에서 검정색화면 뜨는것 방지.
	Screen.Set_consol();
	Screen.Show_opt_kr();
	while (1){

		//게임의 시작. 공 쏘기.
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 0));
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 1));
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 2));
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 3));
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 4));
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 5));
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 6));
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 7));
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 8));
		Balls.In(C_ball(BOARD_SIZE_IN * 9 / 30, BOARD_SIZE_IN * 8 / 30, 9));
		"";

		Check = true;
		C_keyboard::Check(C_keyboard::D);
		C_keyboard::Check(C_keyboard::W);
		C_keyboard::Check(C_keyboard::RIGHT);
		C_keyboard::Check(C_keyboard::LEFT);
		C_keyboard::Check(C_keyboard::UP);
		C_keyboard::Check(C_keyboard::DOWN);
		while (Check){//한 프레임.
			Time.Reset_time();

			//게임의 플레이. 루프 및 지속적 Reflector조작
			//Key Input.
			if (C_keyboard::Check_press(C_keyboard::RIGHT)){
				Move_reflector(R);
			}
			if (C_keyboard::Check_press(C_keyboard::LEFT)){
				Move_reflector(L);
			}
			if (C_keyboard::Check_press(C_keyboard::UP)){
				Move_reflector(U);
			}
			if (C_keyboard::Check_press(C_keyboard::DOWN)){
				Move_reflector(D);
			}
			if (C_keyboard::Check_press(C_keyboard::W)){
				Move_reflector(UL);//시계반대방향 회전.
			}
			if (C_keyboard::Check_press(C_keyboard::D)){
				Move_reflector(UR);//시계방향 회전.
			}
			//Ball Play.
			for (Loop1 = 0; Loop1 < BALL_MAX_NUM; Loop1++){
				Play_ball(Loop1);
			}
			//화면 출력.
			for (Loop1 = 0; Loop1 < BOARD_SIZE; Loop1++){
				for (Loop2 = 0; Loop2 < BOARD_SIZE; Loop2++){
					Print_brick(Loop1, Loop2);
				}
			}
			Print_hitting_zone();
			Print_Reflector();
			for (Loop1 = 0; Loop1 < BALL_MAX_NUM; Loop1++){
				Print_ball(Loop1);
			}
			Screen.Show_opt_kr();
			"";
			//Check = false;//게임오버 검사구문 필요.
			gotoxy(0, Screen.GetH()); cout << "                ";
			while (Time.Get_time() < OUTPUT_FRAME){ gotoxy(0, Screen.GetH()); cout << "정상 속도."; }
		}
		//게임 오버.

		//재시도 및 종료 점검.
		"";
		if (Check==false){ break; }//종료.
		else {
			//재시도시 초기화하고 처음으로.
		}
	}
}
void C_board::Get_reflector_edge(int * X1, int * Y1, int * X2, int * Y2){
	static const double Sin45 = 0.707106781187;
	static const int Dx_R = int(BRICK_SIZE * INTERNAL_RESOLUTION * 2 / (unsigned)3);//Reflector사이즈 결정.
	static const int Dy_R = 2 * Dx_R;
	static const int Dx_DR = -ROUND((int((Sin45)*((double)(Dx_R)))), INTERNAL_RESOLUTION);//DR은 +45도 회전변환, 근사값.
	static const int Dy_DR = -3 * Dx_DR;
	switch (Reflector.Position){//콘솔의 방향은 y좌표가 클수록 아래쪽이기 때문에, 일반적인 데키르트 좌표계에서 위아래가 뒤집힌다는 것에 주의.
	case R:
		*X1 = Reflector.X + Dx_R; *Y1 = Reflector.Y + Dy_R; *X2 = Reflector.X + Dx_R; *Y2 = Reflector.Y - Dy_R;
		break;
	case DR:
		*X1 = Reflector.X + Dx_DR; *Y1 = Reflector.Y + Dy_DR; *X2 = Reflector.X + Dy_DR; *Y2 = Reflector.Y + Dx_DR;
		break;
	case D:
		*X1 = Reflector.X - Dy_R; *Y1 = Reflector.Y + Dx_R; *X2 = Reflector.X + Dy_R; *Y2 = Reflector.Y + Dx_R;
		break;
	case DL:
		*X1 = Reflector.X - Dy_DR; *Y1 = Reflector.Y + Dx_DR; *X2 = Reflector.X - Dx_DR; *Y2 = Reflector.Y + Dy_DR;
		break;
	case L:
		*X1 = Reflector.X - Dx_R; *Y1 = Reflector.Y - Dy_R; *X2 = Reflector.X - Dx_R; *Y2 = Reflector.Y + Dy_R;
		break;
	case UL:
		*X1 = Reflector.X - Dx_DR; *Y1 = Reflector.Y - Dy_DR; *X2 = Reflector.X - Dy_DR; *Y2 = Reflector.Y - Dx_DR;
		break;
	case U:
		*X1 = Reflector.X + Dy_R; *Y1 = Reflector.Y - Dx_R; *X2 = Reflector.X - Dy_R; *Y2 = Reflector.Y - Dx_R;
		break;
	case UR:
		*X1 = Reflector.X + Dy_DR; *Y1 = Reflector.Y - Dx_DR; *X2 = Reflector.X + Dx_DR; *Y2 = Reflector.Y - Dy_DR;
		break;
	}
}
void C_board::Print_Reflector(){
	int X1, Y1, X2, Y2;
	Get_reflector_edge(&X1, &Y1, &X2, &Y2);
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	Print_line(Reflector.X, Reflector.Y, X1, Y1);
	Print_line(Reflector.X, Reflector.Y, X2, Y2);
	Print_line(X1, Y1, X2, Y2);
}
void C_board::Print_ball(unsigned Ball_num){
int Temp1;
int X_ref, Y_ref;
if (Ball_num >= Balls.Num()){ return; }
C_queue<C_ball::C_ball_vector> Ball_rec = Balls.Read(Ball_num).Ball_rec;

for (Temp1 = 0; Temp1 < Ball_rec.Num() - 1; Temp1++){//공 그림자.(무반사).
	Screen.Setcolor(C_screen_color::D_GREY, BACK_COLOR);
	if (Temp1 == Ball_rec.Num() - 2){
		Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	}
	if (Ball_rec.Read(Temp1).X == -1){ ; }//출력 필요 없음.(Reflector반사표식).
	else if (Ball_rec.Read(Temp1 + 1).X == -1){ ; }//Reflector반사.(생략).
	else if (Ball_rec.Read(Temp1).Vector_num == Ball_rec.Read(Temp1 + 1).Vector_num){//무반사 경우.
		Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1 + 1).X, Ball_rec.Read(Temp1 + 1).Y);
	}
}
for (Temp1 = 0; Temp1 < Ball_rec.Num() - 1; Temp1++){//공 그림자.(반사.)
	Screen.Setcolor(C_screen_color::D_GREY, BACK_COLOR);
	if (Temp1 == Ball_rec.Num() - 2){
		Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	}
	if (Ball_rec.Read(Temp1).X == -1){ ; }//출력 필요 없음.(Reflector반사표식).
	else if (Ball_rec.Read(Temp1 + 1).X == -1){//Reflector반사.
		Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1 + 2).X, Ball_rec.Read(Temp1 + 2).Y);
		if (Ball_rec.Read(Temp1).Vector_X() <= 0){
			FLOOR(Ball_rec.Read(Temp1 + 2).X, INTERNAL_RESOLUTION)/INTERNAL_RESOLUTION;
		}
		else{
			CEIL(Ball_rec.Read(Temp1 + 2).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION - 1;
		}
		Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
		Screen << Coord(
			2 * ((Ball_rec.Read(Temp1).Vector_X() > 0) ? CEIL(Ball_rec.Read(Temp1 + 2).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION - 1 : FLOOR(Ball_rec.Read(Temp1 + 2).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION),
			(Ball_rec.Read(Temp1).Vector_Y() > 0) ? CEIL(Ball_rec.Read(Temp1 + 2).Y, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION - 1 : FLOOR(Ball_rec.Read(Temp1 + 2).Y, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION
			) << "■";
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
		Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1).X + Ball_rec.Read(Temp1).Vector_X(), Ball_rec.Read(Temp1).Y + Ball_rec.Read(Temp1).Vector_Y(), X_ref, Y_ref);
	}
}
//현재 공.
Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
Screen << Coord((FLOOR(Ball_rec.Read(Temp1).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION)*2, FLOOR(Ball_rec.Read(Temp1).Y, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION) << "■";
}
void C_board::Print_line(int X1, int Y1, int X2, int Y2, int X_ref, int Y_ref){
	int Temp1, Temp2;
	unsigned Screen_X, Screen_Y;
	char Color = Screen.Get_color();
	assert((/*"Reflection point must be on INTERNAL_RESOLUTION", */(X_ref%INTERNAL_RESOLUTION) == 0 || X_ref == -1));
	assert((/*"Reflection point must be on INTERNAL_RESOLUTION", */(Y_ref%INTERNAL_RESOLUTION) == 0 || Y_ref == -1));

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
							Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
						}
					}
					else if (Temp1 + INTERNAL_RESOLUTION == X_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							Temp1 + INTERNAL_RESOLUTION, Temp2, Temp1 + INTERNAL_RESOLUTION, Temp2 + INTERNAL_RESOLUTION)){
							Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
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
							Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
						}
					}
					else if (Temp2 + INTERNAL_RESOLUTION == Y_ref){
						if (Box_in_line(X1, Y1, X2, Y2,
							Temp1, Temp2 + INTERNAL_RESOLUTION, Temp1 + INTERNAL_RESOLUTION, Temp2 + INTERNAL_RESOLUTION)){
							Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
						}
					}
				}
				Screen << Coord(Screen_X, Screen_Y) << "■";
			}

			Temp2 += INTERNAL_RESOLUTION;
		} while (Temp2 < CEIL(HIGH(Y1, Y2), INTERNAL_RESOLUTION));
		Temp1 += INTERNAL_RESOLUTION;
	} while (Temp1 < CEIL(HIGH(X1, X2), INTERNAL_RESOLUTION));
}
bool C_board::Play_ball_ref(C_ball& Ball){
	/*
	매개변수 직선 (a+(c-a)t,b+(d-b)t)과 점 (X1, Y1)(X2, Y2)로 이루어진 교점의 t값은, [(a-X1)(Y1-Y2)+(b-Y1)(X2-X1)] / [(a-c)(Y1-Y2)+(b-d)(X2-X1)] 
	*/
	C_ball::C_ball_vector Lastball = Ball.Ball_rec.Read(Ball.Ball_rec.Num() - 1);//현재 공.(마지막공).
	int Ball_X, Ball_Y, Ref_X1, Ref_Y1, Ref_X2, Ref_Y2;
	C_fraction Ref_t12 = 2, Ref_t01 = 2, Ref_t02 = 2, Ref_t = 2;
	bool Ref_12, Ref_01, Ref_02;
	unsigned Vector_n;

	Get_reflector_edge(&Ref_X1, &Ref_Y1, &Ref_X2, &Ref_Y2);
	Ball_X = Lastball.X + Lastball.Vector_X(); Ball_Y = Lastball.Y + Lastball.Vector_Y();

	Ref_01 = Line_in_line(Lastball.X, Lastball.Y, Ball_X, Ball_Y, Reflector.X, Reflector.Y, Ref_X1, Ref_Y1, &Ref_t01);//Reflector의 01면에서 반사되었는지를 확인, 매개변수직선 T값 얻음.
	if (!Ref_01){ Ref_t01 = 2; }//만약 반사되지 않았다면 T값 다시 리셋.
	Ref_02 = Line_in_line(Lastball.X, Lastball.Y, Ball_X, Ball_Y, Reflector.X, Reflector.Y, Ref_X2, Ref_Y2, &Ref_t02);
	if (!Ref_02){ Ref_t02 = 2; }
	Ref_12 = Line_in_line(Lastball.X, Lastball.Y, Ball_X, Ball_Y, Ref_X1, Ref_Y1, Ref_X2, Ref_Y2, &Ref_t12);
	if (!Ref_12){ Ref_t12 = 2; }

	Ref_t = LOW(Ref_t01, LOW(Ref_t02, Ref_t12));//반사된 T값중 가장 작은 값 얻음.(가장 작은 값이 진짜로 반사된 위치이니까).
	if (Ref_t == 2){ return false; }//반사가 일어나지 않은 경우 종료.
	if (Ref_t != Ref_t01){ Ref_t01 = 2; }//반사된 T값중 진짜로 반사되지 않은 값을 제거.
	if (Ref_t != Ref_t02){ Ref_t02 = 2; }
	if (Ref_t != Ref_t12){ Ref_t12 = 2; }

	if (Ref_t12 == Ref_t&&Ref_t01==2&&Ref_t02==2){//12면 반사.
		Vector_n = (unsigned)Reflector.Position;
	}
	else if (Ref_t01 == Ref_t&&Ref_t02 == 2 && Ref_t12 == 2){//01면 반사.
		Vector_n = (17 + (unsigned)Reflector.Position) % 40;
	}
	else if (Ref_t02 == Ref_t&&Ref_t01 == 2 && Ref_t12 == 2){//02면 반사.
		Vector_n = (23 + (unsigned)Reflector.Position) % 40;
	}
	else if (Ref_t01 == Ref_t&&Ref_t02 == Ref_t){//0모서리.
		Vector_n = (20 + (unsigned)Reflector.Position) % 40;
	}
	else if (Ref_t01 == Ref_t&&Ref_t12 == Ref_t){//1모서리.
		Vector_n = (9 + (unsigned)Reflector.Position) % 40;
	}
	else if (Ref_t02 == Ref_t&&Ref_t12 == Ref_t){//2모서리.
		Vector_n = (31 + (unsigned)Reflector.Position) % 40;
	}
	//반사되는 법선백터를 Vector_n에 대입 완료.

	if (Ball.Ball_rec.Num() == Ball.SHADOW_NUM){ Ball.Ball_rec.Out(); Ball.Ball_rec.Out(); }//Ball_rec공간 확보.
	else if (Ball.Ball_rec.Num() == Ball.SHADOW_NUM - 1){ Ball.Ball_rec.Out();}
	Ball.Ball_rec.In(C_ball::C_ball_vector(-1,-1,0));//Reflector반사되었다는 표식.
	Ball.Ball_rec.In(
		C_ball::C_ball_vector(
		int(Lastball.X + Lastball.Vector_X()*(Ref_t.Get_value()) + 3 * C_ball::Vector[Vector_n][0] / (double)VECTOR_LENGTH),
		int(Lastball.Y + Lastball.Vector_Y()*(Ref_t.Get_value()) + 3 * C_ball::Vector[Vector_n][1] / (double)VECTOR_LENGTH),
		(2 * Vector_n +60 - Lastball.Vector_num) % 40)
		);
	return true;
}
void C_board::Play_ball_brick(C_ball& Ball){
	C_ball::C_ball_vector Lastball = Ball.Ball_rec.Read(Ball.Ball_rec.Num() - 1);//현재 공.(마지막공).
	unsigned Brick_X, Brick_Y;
	int Lastball_X_temp = Lastball.X, Lastball_Y_temp = Lastball.Y, Lastball_V_num_temp = Lastball.Vector_num;
	bool Ref_X0, Ref_X1, Ref_Y0, Ref_Y1;
	C_fraction Ref_tX0, Ref_tX1, Ref_tY0, Ref_tY1, Temp;
	unsigned Check = 0;
	//현재 공이 속한 Brick의 좌표를 구함.
	if ((Lastball.X%(BRICK_SIZE*INTERNAL_RESOLUTION) == 0) && (Lastball.Vector_X() < 0)){ Brick_X = Lastball.X / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1; }//현재 공이 경계선에서 낮은 좌표 쪽으로 가는 경우.
	else { Brick_X = FLOOR(Lastball.X, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION); }
	if ((Lastball.Y%(BRICK_SIZE*INTERNAL_RESOLUTION) == 0) && (Lastball.Vector_Y() < 0)){ Brick_Y = Lastball.Y / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1; }//현재 공이 경계선에서 낮은 좌표 쪽으로 가는 경우.
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
					Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X - 1].Brick_hit(*this) : 0;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Brick_hit(*this) : 0;
				}
				else if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y :
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Brick_hit(*this) : 0;
				}
			}
			else {//Y축 비경계선.
				if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0: true){//반사됨.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y:
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Brick_hit(*this) : 0;
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
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % 40;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y - 1][Brick_X].Brick_hit(*this) : 0;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Brick_hit(*this) : 0;
				}
				else if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_X() > 0) ?
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X :
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X;
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % 40;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Brick_hit(*this) : 0;
				}
			}
			else {//Y축 비경계선.
				if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//반사됨.
					(Lastball.Vector_X() > 0) ?
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X :
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X;
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % 40;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Brick_hit(*this) : 0;
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
		if (Ref_X0 && (Ref_tX0 != 0)){ Check++; } else { Ref_X0 = false; }
		if (Ref_X1 && (Ref_tX1 != 0)){ Check++; } else { Ref_X1 = false; }
		if (Ref_Y0 && (Ref_tY0 != 0)){ Check++; } else { Ref_Y0 = false; }
		if (Ref_Y1 && (Ref_tY1 != 0)){ Check++; } else { Ref_Y1 = false; }

		if (Check == 1){//벽면 반사가능성.
			//1차반사검사.
			if (Ref_X0) {

				if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0:true){//반사한 경우.
					Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
					(Brick_X) ? Board[Brick_Y][Brick_X - 1].Brick_hit(*this) : 0;
				}
				else { Brick_X--; }
			}
			if (Ref_X1) {
				if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
					Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
					(Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Brick_hit(*this) : 0;
				}
				else { Brick_X++; }
			}
			if (Ref_Y0){
				if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
					(Brick_Y) ? Board[Brick_Y - 1][Brick_X].Brick_hit(*this) : 0;
				}
				else { Brick_Y--; }
			}
			if (Ref_Y1){
				if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
					(Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Brick_hit(*this) : 0;
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
						Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
						(Brick_X) ? Board[Brick_Y][Brick_X - 1].Brick_hit(*this) : 0;
					}
				}
				if (Temp == Ref_tX1){
					if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
						Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
						(Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Brick_hit(*this) : 0;
					}
				}
				if (Temp == Ref_tY0){
					if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
						Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
						(Brick_Y) ? Board[Brick_Y - 1][Brick_X].Brick_hit(*this) : 0;
					}
				}
				if (Temp == Ref_tY1){
					if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
						Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
						(Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Brick_hit(*this) : 0;
					}
				}
			}
		}
		else if (Check == 2){//모서리로 가는 반사.
			if (Ref_X0&&Ref_Y0){
				if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){
					Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
					(Brick_X) ? Board[Brick_Y][Brick_X - 1].Brick_hit(*this) : 0;
				}
				else Check--;
				if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
					(Brick_Y) ? Board[Brick_Y - 1][Brick_X].Brick_hit(*this) : 0;
				}
				else Check--;
				if (Check == 2){//옆 블럭에서 반사안된 경우.(대각선 블럭 검사).
					if ((Brick_X&&Brick_Y) ? Board[Brick_Y - 1][Brick_X - 1].Health != 0 : true){
						if (random(2)){
							Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
							Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
						}
						else {
							Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
							Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
						}
						(Brick_X&&Brick_Y) ? Board[Brick_Y - 1][Brick_X - 1].Brick_hit(*this) : 0;
					}
				}
				else if (Check==1){//옆 블럭 1개에서 반사된 경우.
					(Brick_X&&Brick_Y) ? Board[Brick_Y - 1][Brick_X - 1].Brick_hit(*this) : 0;
				}
			}
			else if (Ref_Y0&&Ref_X1){
				if ((Brick_Y) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
					(Brick_Y) ? Board[Brick_Y - 1][Brick_X].Brick_hit(*this) : 0;
				}
				else Check--;
				if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
					Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
					(Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Brick_hit(*this) : 0;
				}
				else Check--;
				if (Check == 2){
					if ((Brick_Y&&Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y - 1][Brick_X + 1].Health != 0 : true){
						if (random(2)){
							Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
							Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
						}
						else {
							Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
							Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
						}
						(Brick_Y&&Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y - 1][Brick_X + 1].Brick_hit(*this) : 0;
					}
				}
				else if (Check == 1){
					(Brick_Y&&Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y - 1][Brick_X + 1].Brick_hit(*this) : 0;
				}
			}
			else if (Ref_X1&&Ref_Y1){
				if ((Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Health != 0 : true){
					Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
					(Brick_X != BOARD_SIZE - 1) ? Board[Brick_Y][Brick_X + 1].Brick_hit(*this) : 0;
				}
				else Check--;
				if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
					(Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Brick_hit(*this) : 0;
				}
				else Check--;
				if (Check == 2){
					if ((Brick_X != BOARD_SIZE - 1 && Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X + 1].Health != 0 : true){
						if (random(2)){
							Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
							Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
						}
						else {
							Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
							Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
						}
						(Brick_X != BOARD_SIZE - 1 && Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X + 1].Brick_hit(*this) : 0;
					}
				}
				else if (Check == 1){
					(Brick_X != BOARD_SIZE - 1 && Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X + 1].Brick_hit(*this) : 0;
				}
			}
			else if (Ref_Y1&&Ref_X0){
				if ((Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Health != 0 : true){
					Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
					Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
					(Brick_Y != BOARD_SIZE - 1) ? Board[Brick_Y + 1][Brick_X].Brick_hit(*this) : 0;
				}
				else Check--;
				if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){
					Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
					Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
					(Brick_X) ? Board[Brick_Y][Brick_X - 1].Brick_hit(*this) : 0;
				}
				else Check--;
				if (Check == 2){
					if ((Brick_Y != BOARD_SIZE - 1 && Brick_X) ? Board[Brick_Y + 1][Brick_X - 1].Health != 0 : true){
						if (random(2)){
							Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_Y_temp;
							Lastball_V_num_temp = (40 - Lastball_V_num_temp) % 40;
						}
						else {
							Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball_X_temp;
							Lastball_V_num_temp = (60 - Lastball_V_num_temp) % 40;
						}
						(Brick_Y != BOARD_SIZE - 1 && Brick_X) ? Board[Brick_Y + 1][Brick_X - 1].Brick_hit(*this) : 0;
					}
				}
				else if (Check == 1){
					(Brick_Y != BOARD_SIZE - 1 && Brick_X) ? Board[Brick_Y + 1][Brick_X - 1].Brick_hit(*this) : 0;
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
void C_board::Play_ball(unsigned Ball_num){
	if (Ball_num >= Balls.Num()){ return; }
	C_ball& Ball = *(Balls.Read_editable(Ball_num));
	C_ball::C_ball_vector Lastball = Ball.Ball_rec.Read(Ball.Ball_rec.Num() - 1);//현재 공.(마지막공).
	if (Ball.Tick > 0){//정지 아님.
		if (Ball.Tick == Ball.Speed_frame){
			Ball.Tick = 1;
			//Play
			//"Reflector점검(및 반사)";
			if (Play_ball_ref(Ball)){ ; }//"결과가 참일시 끝".
			else {//"결과가 거짓일시 Brick점검";
				Play_ball_brick(Ball);
			}
		}
		else { Ball.Tick++; }
	}
}
void C_board::Print_hitting_zone(){
	int H[4] = {
		(BRICK_HEIGHT + FIELD_SIZE - 1) * BRICK_SIZE,
		(BRICK_HEIGHT + FIELD_SIZE) * BRICK_SIZE,
		(BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE) * BRICK_SIZE - 1,
		(BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + 1) * BRICK_SIZE - 1,
	};
	int Temp1, Temp2;
	Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
	for (Temp1 =H[0]; Temp1 <=H[3];Temp1++){
		Screen << Coord(Temp1 * 2, H[0]) << "━" << Coord(Temp1 * 2, H[3]) << "━";
		Screen << Coord(H[0]*2, Temp1) << "┃" << Coord(H[3]*2, Temp1) << "┃";
	}
	Screen << Coord(H[0] * 2, H[0]) << "┏" << Coord(H[0] * 2, H[3]) << "┗" << Coord(H[3] * 2, H[3]) << "┛" << Coord(H[3] * 2, H[0]) << "┓";
	for (Temp1 = H[1]; Temp1 <= H[2]; Temp1++){
		Screen << Coord(Temp1 * 2, H[1]) << "━" << Coord(Temp1 * 2, H[2]) << "━";
		Screen << Coord(H[1] * 2, Temp1) << "┃" << Coord(H[2] * 2, Temp1) << "┃";
	}
	Screen << Coord(H[1] * 2, H[1]) << "┏" << Coord(H[1] * 2, H[2]) << "┗" << Coord(H[2] * 2, H[2]) << "┛" << Coord(H[2] * 2, H[1]) << "┓";
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
void C_board::Move_reflector(Direction Input){
	switch (Input){
	case R:
		Reflector.X += INTERNAL_RESOLUTION;
		"공과 부딪히는 경우 검사 방법?";
		"Hitting zone 영역 검사 방법?";
		break;
	case D:
		Reflector.Y += INTERNAL_RESOLUTION;
		break;
	case L:
		Reflector.X -= INTERNAL_RESOLUTION;
		break;
	case U:
		Reflector.Y -= INTERNAL_RESOLUTION;
		break;
	case UL://시계반대방향 회전.
		if (Reflector.Position == U){ Reflector.Position = UL; }
		else if (Reflector.Position == UL){ Reflector.Position = L; }
		else if (Reflector.Position == L){ Reflector.Position = DL; }
		else if (Reflector.Position == DL){ Reflector.Position = D; }
		else if (Reflector.Position == D){ Reflector.Position = DR; }
		else if (Reflector.Position == DR){ Reflector.Position = R; }
		else if (Reflector.Position == R){ Reflector.Position = UR; }
		else if (Reflector.Position == UR){ Reflector.Position = U; }
		break;
	case UR://시계방향 회전.
		if (Reflector.Position == U){Reflector.Position = UR;}
		else if (Reflector.Position == UR){ Reflector.Position = R; }
		else if (Reflector.Position == R){ Reflector.Position = DR; }
		else if (Reflector.Position == DR){ Reflector.Position = D; }
		else if (Reflector.Position == D){ Reflector.Position = DL; }
		else if (Reflector.Position == DL){ Reflector.Position = L; }
		else if (Reflector.Position == L){ Reflector.Position = UL; }
		else if (Reflector.Position == UL){ Reflector.Position = U; }
		break;
	default:
		break;
	}
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







