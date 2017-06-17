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

#define VECTOR_LENGTH 60//��� ���͵��� ���α���.

//type, class
class C_ball;
class C_board{
private:
	C_board(const C_board& Input);
	C_board& operator=(const C_board& Input);
public:
//protected:
	enum{
		BOARD_VERGE=12,//ȭ�� ���� ���� ��.
		OUTPUT_TICK = 30,//ȭ�� ��°���.(���� ms).
		BRICK_SIZE = 5,//"�ϳ��� ������ ��� �����ΰ�?".
		//5�� ��������� ������ ��(�������). �׷��� ������ �۰� �������� ����.
		//6�� ¦������ �������� �ְ�� ���ɼ��� ����.
		//7�� Ȧ���̸鼭 Ŀ�� �������� ������ ��������� ���� ��.
		INTERNAL_RESOLUTION = 100,//¦�� �̾����.
		//�ʼ�����. BRICK_SIZE*INTERNAL_RESOLUTON >= VECTOR_LENGTH.
		BALL_MAX_NUM = 10,//�� �ִ� ����.
		LEVELUP_SEC = 20,//������ �ֱ�.
		PLAYBRICK_SPEED_TICK_START=300,
		PLAYBRICK_SPEED_TICK_END=160,
	};
	const unsigned BASE_SIZE;//������� ����.(�� ������ ������ ������ ���ӿ���)(������ ����).
	const unsigned FIELD_SIZE;//HITTING_ZONE���κ��� �������� �⺻�Ÿ�.(������ ����).
	const unsigned BRICK_HEIGHT;
	const unsigned BOARD_SIZE;// FIELD + (HEIGHT+HITTING) * 2
	const unsigned BOARD_SIZE_IN;//BOARD_SIZE*BRICK_SIZE*INTERNAL_RESOLUTION. IN(TERANL BOARD)(for great quality).
	const C_screen_color::T_color BACK_COLOR;

	class C_brick{
	public:
		unsigned Health;
		enum Brick_state{
			NONE = 0,//NONE�� �����ϼ� ���� ��(���� ������ �� ����).
			NORMAL=1,//NORMAL�̰� Health==0�϶� �� ������ �ǹ�.
			BALLOUT,//������ ���� 3�� ����.
		} State;
		C_brick(){ Health = 1; State = NONE; }//NONE�� �ݻ缺�� ����� �ϱ� ������ ü���� 0�� �ƴ�.
		C_brick(int H){ if (H >= 0){ Health = H; State = NORMAL; } else { Health = 0; State = NONE; } }
		C_brick(unsigned H, Brick_state _State){ Health = H; State = _State; }
	};
	friend class C_board::C_brick;

	C_brick **Board;
	C_queue<C_ball> Balls;
	C_screen_color Screen;
	static const char Letter[][5];
	unsigned Playbrick_speed_tick;
	unsigned Playbrick_tick;
	unsigned Level;
	unsigned Score;
	enum Direction{
		R = 0, DR = 5, D = 10, DL = 15, L = 20, UL = 25, U = 30, UR = 35,
	};
	struct T_reflector {
		int X, Y;
		unsigned Position;
		unsigned Move_length;//Reflector�� ��ĭ �̵� ����(��������: INTERNAL_RESOLUTION).
		unsigned Length;//Reflector������ ����(��������: VECTOR_LENGTH).
	} Reflector;

	//function.
	void Game_init();
	void Print_ball(unsigned Ball_num, bool Print = true);
	void Print_Reflector(bool Print=true);
	void Print_brick(unsigned X, unsigned Y);
	void Print_hitting_zone();
	void Get_reflector_edge(int * X1, int * Y1, int * X2, int * Y2);//Reflector�� ������ ������ ����.
	int Dead_verge();
	bool Play_brick();//true�� ���ӿ���.
	bool Play_ball(unsigned Ball_num);//���� ������� �� ��� true��ȯ.
	bool Play_ball_ref(C_ball& Ball);
	void Play_ball_brick(C_ball&Ball);
	void Move_reflector(Direction Input, unsigned Speed=1);
	void Arrange_reflector();
	void Arrange_ball_in_brick(unsigned Ball_num);//���� ������ ���� ��츦 ����.
	void Arrange_ball_zoneout(unsigned Ball_num);//���� Hitting_zone���� ������ ���ϴ� ���� ������.
	void Brick_add(unsigned X,unsigned Y);//������ ���̵� �������� �� �Լ��ȿ� ����.
	void Brick_hit(unsigned X, unsigned Y, unsigned Speed);
	void Screen_recorvery();
	void Print_line(int X1, int Y1, int X2, int Y2, const char* Letter, int X_ref=-1, int Y_ref=-1, char Ref_color=0);
	static C_fraction Level_func(unsigned Input, C_fraction Start, C_fraction End, unsigned High_level, unsigned Speed);//�������� 1~High_level������ ���̵��� Start���� End + (Start-End)/SPEED���� �����Լ��� ��ȭ�ϸ�, �� ���ķδ� End�� �ٻ�������.
	static const char* Get_letter(char Input);
	void Draw_letter(unsigned X, unsigned Y, const char * Input, const char * Str, char color);//BOARD_VERGE������ǥ.(Screen��ü�� ��ǥ).

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

	static bool Box_in_line(int X1, int Y1, int X2, int Y2, int Box_X1, int Box_Y1, int Box_X2, int Box_Y2);//line�� ���� ���.
	static bool Line_in_line(int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4, C_fraction* T = NULL);//line�� ���� ���.�Ű����������� ������ ������ t���� T�� ����.
	static bool Dot_is_right_side(int X1, int Y1, int X2, int Y2, int X3, int Y3);//(X1,Y1)���� (X2,Y2)�� ���� ���͸� ���� ��, (X3,Y3)���� �� ������ �����ʿ� �ִ°�?(��輱�� ����).
};

class C_ball{
	friend class C_board;

protected:
	static const int Vector[40][2];//Vector[DIRECTION_NUM][2].
	enum{
		SHADOW_NUM=50,//�׸��ڼ�.
		DIRECTION_NUM = 40,//���� ���� ��.
	};
	class C_ball_vector{
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
	};

	C_queue<C_ball_vector> Ball_rec;
	unsigned Speed_tick;
	unsigned Speed_movenum;
	//�ӷ�=������*����=����/�ֱ�. Tick�� �ֱ� ȿ���̰�, movenum�� ���� ȿ��.
	//���� Speed_tick�� 1�� �����Ǿ� ����(���� �������).
	unsigned Tick;//ƽ�� 0�̸� ����, ƽ�� 1���� Speed_tick���� ��ȯ.
	enum T_basezone_state{
		BASEZONE_OUT, BASEZONE_IN, REF, START
	}Basezone_state;

	bool Speed_edit(int Offset, unsigned Set = 0);//���� �䱸�� ���� ���н� false��ȯ.
	bool Basezone_check(int H1, int H2);//���� �䱸�� ���� ���н� false��ȯ.


public:
	C_ball(int _X = 0, int _Y = 0, unsigned V_num = 0) :Ball_rec(SHADOW_NUM){
  		C_ball_vector Temp(_X, _Y, V_num);
		Ball_rec.In(Temp); Speed_tick = 1; Speed_movenum = 1; Tick = 1; Basezone_state = START; "";//�� �ӵ� ����.
	}
	C_ball(const C_ball& Input) :Ball_rec(Input.Ball_rec){
		Speed_tick = Input.Speed_tick; Speed_movenum = Input.Speed_movenum; Tick = Input.Tick; Basezone_state = Input.Basezone_state;
	}
	C_ball& operator=(const C_ball& Input){
		Ball_rec = Input.Ball_rec; Speed_tick = Input.Speed_tick; Speed_movenum = Input.Speed_movenum; Tick = Input.Tick; Basezone_state = Input.Basezone_state; return *this;
	}
};

//variable

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
	Screen(BOARD_SIZE*BRICK_SIZE * 2, BOARD_SIZE*BRICK_SIZE + BOARD_VERGE, C_screen_color::BLACK, BACK_COLOR){
	int Temp1, Temp2;
	//Board�Ҵ�.
	Board = new C_brick*[BOARD_SIZE];
	for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
		Board[Temp1] = new C_brick[BOARD_SIZE];
	}

	//���� ���� �� ���� �Ұ�.
	system("color f0");
	system("title 4Way Brick");
	cout << "4���� ��������" << endl;
	cout << "������: �̸���" << endl;
	cout << "if you can't read above letter, you need EUC-KR format. (Also required in game)" << endl << endl;

	cout << "�ݻ��� �����̱�: ����Ű.(���� ����Ʈ�� ���̴����� 3�� ����.)" << endl;
	cout << "�ݻ��� ������: W,D (���� ����Ʈ�� ���̴����� 2�� ����.)" << endl;
	cout << "���� ���� �߰��ϱ�: E" << endl;
	cout << "ù �� �߻�: �����̽�Ű�� ��� ������ ����." << endl << endl;

	cout << "���� ��." << endl;
	cout << "���� �ð����� ��濡�� ������ ���ɴϴ�. ���� �� ������ �߾��� ���� ������ ��� �Ǹ� ���ӿ����Դϴ�." << endl;
	cout << "���� ������ ������ �ӵ��� ������, �ݻ������� ���� �о �ӵ��� ������ų�� �ֽ��ϴ�." << endl;
	cout << "���� �ӵ��� ���� ���¿��� ������ ����, �������� ������ �����ϴ�." << endl;
	cout << "���� �ݻ��ǿ� ���� �ʰ� �߾��� ���� ������ ������ ����ϸ�, ���� �ӵ��� ������ ��������, �̹� �������� ��쿡�� ���� ������ϴ�." << endl;
	cout << "���࿡ ��� ���� �� �������, �װ͵� ���ӿ����Դϴ�." << endl;
	cout << "���� �߿��� ���� �����ϴ� ������ �ֽ��ϴ�. �� Ȱ���ϼ���." << endl << endl;

	cout << "���� �÷����߿� ����â�� ũ�⺯������ ���� ȭ������� �Ͼ�ٸ�, �����̽�Ű�� �������ּ���." << endl;
	cout << "�������� �÷��̸� ���ؼ�, ����â�� ��Ŭ���Ͽ� �Ӽ����� ��Ʈũ�⸦ �ſ��۰�(7 ����) �������ּ���." << endl;
	cout << "������ �Ϸ�Ǿ��ٸ�, �����̽�Ű�� �������ּ���.";
	C_keyboard::Check(C_keyboard::SPACE);
	while (C_keyboard::Check_out(C_keyboard::SPACE) == false){ ; }//�����̽��� ���� �ܼ�ũ�⺯��.
	Screen.Setcolor(C_screen_color::BLACK, C_screen_color::WHITE);//��� �������� ������ȭ�� �ߴ°� ����.
	Screen.Set_consol();
}
void C_board::Game_init(){
	int Temp1, Temp2, H1, H2, Vector_num;
	C_time Time;

	Level = 1; Score = 0;
	//Screen�ʱ�ȭ.
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	Screen.Clrscr();
	Screen.Setcolor(BACK_COLOR, C_screen_color::BLACK);
	Draw_letter(0, 0, "Score:", "��", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
	Draw_letter(0, 6, "Level:", "��", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
	for (Temp1 = 0; Temp1 < (2 * FIELD_SIZE + BASE_SIZE) * BRICK_SIZE + 2; Temp1++){
		Screen << Coord((BRICK_HEIGHT * BRICK_SIZE + Temp1 - 1) * 2, BOARD_VERGE - 1) << "  ";
	}
	//Board�ʱ�ȭ.
	for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
		for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){
			if ((BRICK_HEIGHT <= Temp1&&Temp1 < BOARD_SIZE - BRICK_HEIGHT) ^
				(BRICK_HEIGHT <= Temp2&&Temp2 < BOARD_SIZE - BRICK_HEIGHT)){
				Brick_add(Temp2, Temp1);
			}
			else if ((BRICK_HEIGHT <= Temp1&&Temp1 < BOARD_SIZE - BRICK_HEIGHT) &&
				(BRICK_HEIGHT <= Temp2&&Temp2 < BOARD_SIZE - BRICK_HEIGHT)){
				Board[Temp1][Temp2].Health = 0;
				Board[Temp1][Temp2].State = C_brick::NORMAL;
			}
		}
	}
	Board[BRICK_HEIGHT][BRICK_HEIGHT - 1].State = C_brick::BALLOUT;
	Board[BOARD_SIZE - BRICK_HEIGHT - 1][BOARD_SIZE - BRICK_HEIGHT].State = C_brick::BALLOUT;
	Playbrick_speed_tick = PLAYBRICK_SPEED_TICK_START;
	Playbrick_tick = 1;
	//�� �ʱ�ȭ.
	while (Balls.Num()){ Balls.Out(); }
	//Reflector �ʱ�ȭ.
	Reflector.X = Reflector.Y = FLOOR(BOARD_SIZE_IN / 2, INTERNAL_RESOLUTION) + INTERNAL_RESOLUTION / 2;
	Reflector.Move_length = 3;
	Reflector.Length = CEIL(BRICK_SIZE * INTERNAL_RESOLUTION * 4 / (unsigned)3, VECTOR_LENGTH) / VECTOR_LENGTH;
	Reflector.Position = C_ball::DIRECTION_NUM/4;
	//Screen ����
	for (Temp1 = 0; Temp1 < BOARD_SIZE; Temp1++){
		for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){
			Print_brick(Temp2, Temp1);
		}
	}
	H1 = BRICK_HEIGHT * BRICK_SIZE - 1;
	H2 = (BOARD_SIZE - BRICK_HEIGHT) * BRICK_SIZE;
	Screen.Setcolor(C_screen_color::RED, C_screen_color::BLACK);
	for (Temp1 = 0; Temp1 <= H1; Temp1++){
		Screen << Coord(H1 * 2, Temp1 + BOARD_VERGE) << "  " << Coord(H2 * 2, Temp1 + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H1 + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H2 + BOARD_VERGE) << "  ";
	}
	for (Temp1 = H2; Temp1 < BOARD_SIZE*BRICK_SIZE; Temp1++){
		Screen << Coord(H1 * 2, Temp1 + BOARD_VERGE) << "  " << Coord(H2 * 2, Temp1 + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H1 + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H2 + BOARD_VERGE) << "  ";
	}
	Screen.Show_opt_kr();

	//�� �߻�.
	C_keyboard::Check(C_keyboard::SPACE);
	while (C_keyboard::Check_press(C_keyboard::SPACE) == false){ ; }//�����̽��� ������ �� ���� ����.
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
		if (C_keyboard::Check_state(C_keyboard::SPACE) == false){ break; }//�����̽��� ���� �߻�.
		Vector_num = (Vector_num + 1) % C_ball::DIRECTION_NUM;
		Print_line(
			BOARD_SIZE_IN / 2,
			BOARD_SIZE_IN / 2,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][0] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			(int)BOARD_SIZE_IN / 2 + (C_ball::Vector[Vector_num][1] * 3 * BRICK_SIZE * INTERNAL_RESOLUTION) / VECTOR_LENGTH,
			"��"
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
	C_time Stime;
	int Lag_check;

	char Buf[50];
	while (1){
		//������ ����. �� ���.
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
		Stime.Reset_time();
		while (Check){//�� ������.
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
			Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
			for (Temp1 = 0; Temp1 < Screen.GetL(); Temp1 += 2){
				if (Temp1 < (Balls.Read(0).Speed_tick) * 2){
					Screen << Coord(Temp1, 1) << "��";
				}
				else {
					Screen << Coord(Temp1, 1) << "  ";
				}
			}
			for (Temp1 = 0; Temp1 < Screen.GetL(); Temp1 += 2){
				if (Temp1 < (Balls.Read(0).Speed_movenum) * 2){
					Screen << Coord(Temp1, 0) << "��";
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
						Screen << Coord(Temp1, 4) << "��";
					}
					else {
						Screen << Coord(Temp1, 4) << "  ";
					}
				}
				for (Temp1 = 0; Temp1 < Screen.GetL(); Temp1 += 2){
					if (Temp1 < (Balls.Read(1).Speed_movenum) * 2){
						Screen << Coord(Temp1, 3) << "��";
					}
					else {
						Screen << Coord(Temp1, 3) << "  ";
					}
				}
			}
#endif

			//������ �÷���. ���� �� ������ Reflector����
			Level = CEIL(Stime.Get_time_sec(), LEVELUP_SEC) / LEVELUP_SEC;
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
				if (C_keyboard::Check_state(C_keyboard::LSHIFT)){
					Move_reflector(UL);//�ð�ݴ���� ȸ��.
				}
				Move_reflector(UL);//�ð�ݴ���� ȸ��.
			}
			if (C_keyboard::Check_state(C_keyboard::D)){
				if (C_keyboard::Check_state(C_keyboard::LSHIFT)){
					Move_reflector(UR);//�ð���� ȸ��.
				}
				Move_reflector(UR);//�ð���� ȸ��.
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
				Playbrick_speed_tick = int(Level_func(Level, PLAYBRICK_SPEED_TICK_START, PLAYBRICK_SPEED_TICK_END, 600/LEVELUP_SEC, 100).Get_value());
				if (Play_brick()){ Check = false; }//���ӿ���.
			}
			else { Playbrick_tick++; }
			//Ball Play.
			for (Temp1 = 0; Temp1 < Balls.Num(); Temp1++){
				if (Play_ball(Temp1)){
					//�� ���� �ʿ�.
					if (Temp1 == 0){//ù��°���ΰ��.
						Balls.Out();//ù��°�� ����.
					}
					else {
						*(Balls.Read_editable(Temp1)) = Balls.Read(0);//���� ù��°������ �����.
						Balls.Out();//ù��°�� ����.
						Temp1--;
					}
				}
			}
			if (Balls.Num() == 0){
				Check = false;//���ӿ���.
			}
			//ȭ�� ���.
			Print_hitting_zone();
			Print_Reflector();
			for (Temp1 = 0; Temp1 < Balls.Num(); Temp1++){
				Print_ball(Temp1);
			}
			sprintf_s(Buf, "%09d", Score);
			Draw_letter(48, 0, Buf, "��", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
			sprintf_s(Buf, "%09d", Level);
			Draw_letter(48, 6, Buf, "��", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
			Screen.Show_opt_kr();

			Lag_check = (int)OUTPUT_TICK - Time.Get_time();//�ܿ� �ð�.
			if (Lag_check<0){//�ð� ���� �߻�.
				Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
				for (Temp1 = 0; Temp1 < BRICK_HEIGHT*BRICK_SIZE*INTERNAL_RESOLUTION*2; Temp1 += 2){
					if ((int)Temp1 * 3 < -1*Lag_check){//�׸� ĭ��*6�� ���� �ð�(ms)
						Screen << Coord(Temp1, Screen.GetH()) << "��";
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
						Screen << Coord(Temp1, Screen.GetH()) << "��";
					}
					else {
						Screen << Coord(Temp1, Screen.GetH()) << "  ";
					}
				}
			}

			while (Time.Get_time() < OUTPUT_TICK){ ; }//�ӵ���������.
		}
		//���� ����.
		//��õ� �� ���� ����.
		Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
		for (Temp1 = 12; Temp1 < 27; Temp1++){
			Screen << Coord(0, Temp1);
			for (Temp2 = 0; Temp2 < Screen.GetL(); Temp2++){
				Screen << ' ';
			}
		}
		Draw_letter(ROUND(Screen.GetL() / 2, 2) - 48, 12, "game over!!", "��", (16 * BACK_COLOR + C_screen_color::RED) & 0xff);
		Draw_letter(ROUND(Screen.GetL() / 2, 2) - 60, 17, "Retry:ENTER KEY", "��", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
		Draw_letter(ROUND(Screen.GetL() / 2, 2) - 36, 22, "END:E KEY", "��", (16 * BACK_COLOR + C_screen_color::BLACK) & 0xff);
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
		if (Check==false){ break; }//����.
		//���� �ƴϸ� ��õ�.
	}
}

void C_board::Print_ball(unsigned Ball_num, bool Print){
	int Temp1;
	int X_ref, Y_ref;
	if (Ball_num >= Balls.Num()){ return; }
	char *Str;
	if (Print){ Str = "��"; }
	else { Str = "  "; }
	C_queue<C_ball::C_ball_vector> Ball_rec = Balls.Read(Ball_num).Ball_rec;

	for (Temp1 = 0; Temp1 < Ball_rec.Num() - 1; Temp1++){//�� �׸���.(���ݻ�).
		Screen.Setcolor(C_screen_color::D_GREY, BACK_COLOR);
		if (Temp1 == Ball_rec.Num() - 2){
			Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
		}
		if (Ball_rec.Read(Temp1 + 1).State == C_ball::C_ball_vector::NONE){//���ݻ� ���.
			Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1 + 1).X, Ball_rec.Read(Temp1 + 1).Y, Str);
		}
	}
	for (Temp1 = 0; Temp1 < Ball_rec.Num() - 1; Temp1++){//�� �׸���.(�ݻ�.)
		Screen.Setcolor(C_screen_color::D_GREY, BACK_COLOR);
		if (Temp1 == Ball_rec.Num() - 2){
			Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR); 
		}
		if (Ball_rec.Read(Temp1 + 1).State == C_ball::C_ball_vector::REF_REF){//Reflector�ݻ�.
			Print_line(Ball_rec.Read(Temp1).X, Ball_rec.Read(Temp1).Y, Ball_rec.Read(Temp1 + 1).X, Ball_rec.Read(Temp1 + 1).Y, Str);
			Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
			Screen << Coord(
				2 * ((Ball_rec.Read(Temp1).Vector_X() > 0) ? CEIL(Ball_rec.Read(Temp1 + 1).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION - 1 : FLOOR(Ball_rec.Read(Temp1 + 1).X, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION),
				((Ball_rec.Read(Temp1).Vector_Y() > 0) ? CEIL(Ball_rec.Read(Temp1 + 1).Y, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION - 1 : FLOOR(Ball_rec.Read(Temp1 + 1).Y, INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION) + BOARD_VERGE
				) << Str;
		}
		else if (Ball_rec.Read(Temp1 + 1).State == C_ball::C_ball_vector::BRICK_REF){//���� �ݻ�.
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
	//���� ��.
	Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
	Screen << Coord(
		((Ball_rec.Read(Temp1).Vector_X() >= 0) ? FLOOR(Ball_rec.Read(Temp1).X, INTERNAL_RESOLUTION) : CEIL(Ball_rec.Read(Temp1).X, INTERNAL_RESOLUTION) - INTERNAL_RESOLUTION) * 2 / INTERNAL_RESOLUTION,
		(((Ball_rec.Read(Temp1).Vector_Y() >= 0) ? FLOOR(Ball_rec.Read(Temp1).Y, INTERNAL_RESOLUTION) : CEIL(Ball_rec.Read(Temp1).Y, INTERNAL_RESOLUTION) - INTERNAL_RESOLUTION) / INTERNAL_RESOLUTION) + BOARD_VERGE
		) << Str;
}
void C_board::Print_Reflector(bool Print){
	int X1, Y1, X2, Y2;
	char *Str;
	if (Print){ Str = "��"; }
	else { Str = "  "; }
	Get_reflector_edge(&X1, &Y1, &X2, &Y2);
	Screen.Setcolor(C_screen_color::BLACK, BACK_COLOR);
	Print_line(X1, Y1, X2, Y2, Str);
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

	"";//�����Ӽ��߰�.
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
			for (Temp1 = X1; Temp1 <= X2; Temp1++){
				for (Temp2 = Y1; Temp2 <= Y2; Temp2++){
					Screen << Coord(Temp1 * 2, Temp2 + BOARD_VERGE) << "  ";
				}
			}
		}
		else if (Board[Y][X].Health > 0){
			//���� �׵θ�.
			Screen << Coord(X1 * 2, Y1 + BOARD_VERGE) << "��";
			Screen << Coord(X1 * 2, Y2 + BOARD_VERGE) << "��";
			Screen << Coord(X2 * 2, Y1 + BOARD_VERGE) << "��";
			Screen << Coord(X2 * 2, Y2 + BOARD_VERGE) << "��";
			for (Temp1 = 0; Temp1 < BRICK_SIZE - 2; Temp1++){
				Screen << Coord(X1 * 2, Y1 + Temp1 + 1 + BOARD_VERGE) << "��";
				Screen << Coord(X2 * 2, Y1 + Temp1 + 1 + BOARD_VERGE) << "��";
				Screen << Coord((X1 + Temp1 + 1) * 2, Y1 + BOARD_VERGE) << "��";
				Screen << Coord((X1 + Temp1 + 1) * 2, Y2 + BOARD_VERGE) << "��";
			}
			//���� ����.
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
	if (Verge != Dead_verge()){//Verge�޶���. ����� �ʿ�.
		H[0] = (BRICK_HEIGHT + FIELD_SIZE - Verge + 1) * BRICK_SIZE;
		H[1] = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge - 1) * BRICK_SIZE - 1;
		for (Temp1 = H[0]; Temp1 <= H[1]; Temp1++){
			Screen << Coord(Temp1 * 2, H[0] + BOARD_VERGE) << "  " << Coord(Temp1 * 2, H[1] + BOARD_VERGE) << "  ";
			Screen << Coord(H[0] * 2, Temp1 + BOARD_VERGE) << "  " << Coord(H[1] * 2, Temp1 + BOARD_VERGE) << "  ";
		}
	}
	//���.
	Verge = Dead_verge();
	H[0] = (BRICK_HEIGHT + FIELD_SIZE - Verge + 1) * BRICK_SIZE;
	H[1] = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge - 1) * BRICK_SIZE - 1;
	H[2] = (BRICK_HEIGHT + FIELD_SIZE) * BRICK_SIZE;
	H[3] = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE) * BRICK_SIZE - 1;
	for (Temp1 = H[0]; Temp1 <= H[1]; Temp1++){
		Screen << Coord(Temp1 * 2, H[0] + BOARD_VERGE) << "��" << Coord(Temp1 * 2, H[1] + BOARD_VERGE) << "��";
		Screen << Coord(H[0] * 2, Temp1 + BOARD_VERGE) << "��" << Coord(H[1] * 2, Temp1 + BOARD_VERGE) << "��";
	}
	Screen << Coord(H[0] * 2, H[0] + BOARD_VERGE) << "��" << Coord(H[0] * 2, H[1] + BOARD_VERGE) << "��" << Coord(H[1] * 2, H[1] + BOARD_VERGE) << "��" << Coord(H[1] * 2, H[0] + BOARD_VERGE) << "��";
	Screen.Setcolor(C_screen_color::RED, BACK_COLOR);
	for (Temp1 = H[2]; Temp1 <= H[3]; Temp1++){
		Screen << Coord(Temp1 * 2, H[2] + BOARD_VERGE) << "��" << Coord(Temp1 * 2, H[3] + BOARD_VERGE) << "��";
		Screen << Coord(H[2] * 2, Temp1 + BOARD_VERGE) << "��" << Coord(H[3] * 2, Temp1 + BOARD_VERGE) << "��";
	}
	Screen << Coord(H[2] * 2, H[2] + BOARD_VERGE) << "��" << Coord(H[2] * 2, H[3] + BOARD_VERGE) << "��" << Coord(H[3] * 2, H[3] + BOARD_VERGE) << "��" << Coord(H[3] * 2, H[2] + BOARD_VERGE) << "��";
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
	"";
	switch (Way){
	case 0://��
		for (Temp1 = 0; Temp1 < BASE_SIZE + 2 * FIELD_SIZE; Temp1++){//�� ��.
			X = BRICK_HEIGHT + Temp1;
			for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){//�� ���� ó��.
				if (Board[Temp2][X].Health == 0){//���� ���� ����.-->�и��� ����.
					for (Temp3 = Temp2; Temp3 > 0; Temp3--){
						Board[Temp3][X] = Board[Temp3 - 1][X];
						Print_brick(X, Temp3);
					}
					Brick_add(X, 0);
					Print_brick(X, 0);
					//���� ���ԵǸ� �б�.
					for (Temp3 = 0; Temp3 < Balls.Num(); Temp3++){//�ϳ��� ��.
						for (Temp4 = 0; Temp4 < Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//���� �׸����� �ϳ�.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = X*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->X &&
								Ball->X <= X_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in <= Ball->Y &&
								Ball->Y <= Y_in + BRICK_SIZE*INTERNAL_RESOLUTION
								){//���� �׸����� �ϳ��� ���� ���� �� �¿쿡 �����ϴ� ���.
								Arrange_ball_in_brick(Temp3);
							}
						}
					}
					break;
				}
			}
		}
		break;
	case 1://������
		for (Temp1 = 0; Temp1 < BASE_SIZE + 2 * FIELD_SIZE; Temp1++){//�� ��.
			Y = BRICK_HEIGHT + Temp1;
			for (Temp2 = BOARD_SIZE - 1; Temp2 >= 0; Temp2--){//�� ���� ó��.
				if (Board[Y][Temp2].Health == 0){//���� ���� ����.-->�и��� ����.
					for (Temp3 = Temp2; Temp3 < BOARD_SIZE - 1; Temp3++){
						Board[Y][Temp3] = Board[Y][Temp3 + 1];
						Print_brick(Temp3, Y);
					}
					Brick_add(BOARD_SIZE - 1, Y);
					Print_brick(BOARD_SIZE - 1, Y);
					//���� ���ԵǸ� �б�.
					for (Temp3 = 0; Temp3 < Balls.Num(); Temp3++){//�ϳ��� ��.
						for (Temp4 = 0; Temp4 < Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//���� �׸����� �ϳ�.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Y*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in <= Ball->X &&
								Ball->X <= X_in + BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->Y &&
								Ball->Y <= Y_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION
								){//���� �׸����� �ϳ��� ���� ���� �� �¿쿡 �����ϴ� ���.
								Arrange_ball_in_brick(Temp3);
							}
						}
					}
					break;
				}
			}
		}
		break;
	case 2://�Ʒ�
		for (Temp1 = 0; Temp1 < BASE_SIZE + 2 * FIELD_SIZE; Temp1++){//�� ��.
			X = BRICK_HEIGHT + Temp1;
			for (Temp2 = BOARD_SIZE - 1; Temp2 >= 0; Temp2--){//�� ���� ó��.
				if (Board[Temp2][X].Health == 0){//���� ���� ����.-->�и��� ����.
					for (Temp3 = Temp2; Temp3 < BOARD_SIZE - 1; Temp3++){
						Board[Temp3][X] = Board[Temp3 + 1][X];
						Print_brick(X, Temp3);
					}
					Brick_add(X, BOARD_SIZE - 1);
					Print_brick(X, BOARD_SIZE - 1);
					//���� ���ԵǸ� �б�.
					for (Temp3 = 0; Temp3 < Balls.Num(); Temp3++){//�ϳ��� ��.
						for (Temp4 = 0; Temp4 < Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//���� �׸����� �ϳ�.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = X*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->X &&
								Ball->X <= X_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in <= Ball->Y &&
								Ball->Y <= Y_in + BRICK_SIZE*INTERNAL_RESOLUTION
								){//���� �׸����� �ϳ��� ���� ���� �� �¿쿡 �����ϴ� ���.
								Arrange_ball_in_brick(Temp3);
							}
						}
					}
					break;
				}
			}
		}
		break;
	case 3://����
		for (Temp1 = 0; Temp1 < BASE_SIZE + 2 * FIELD_SIZE; Temp1++){//�� ��.
			Y = BRICK_HEIGHT + Temp1;
			for (Temp2 = 0; Temp2 < BOARD_SIZE; Temp2++){//�� ���� ó��.
				if (Board[Y][Temp2].Health == 0){//���� ���� ����.-->�и��� ����.
					for (Temp3 = Temp2; Temp3 > 0; Temp3--){
						Board[Y][Temp3] = Board[Y][Temp3 - 1];
						Print_brick(Temp3, Y);
					}
					Brick_add(0, Y);
					Print_brick(0, Y);
					//���� ���ԵǸ� �б�.
					for (Temp3 = 0; Temp3 < Balls.Num(); Temp3++){//�ϳ��� ��.
						for (Temp4 = 0; Temp4 < Balls.Read(Temp3).Ball_rec.Num(); Temp4++){//���� �׸����� �ϳ�.
							Ball = Balls.Read_editable(Temp3)->Ball_rec.Read_editable(Temp4);
							X_in = Temp2*BRICK_SIZE*INTERNAL_RESOLUTION;
							Y_in = Y*BRICK_SIZE*INTERNAL_RESOLUTION;
							if (
								X_in <= Ball->X &&
								Ball->X <= X_in + BRICK_SIZE*INTERNAL_RESOLUTION &&
								Y_in - BRICK_SIZE*INTERNAL_RESOLUTION <= Ball->Y &&
								Ball->Y <= Y_in + 2 * BRICK_SIZE*INTERNAL_RESOLUTION
								){//���� �׸����� �ϳ��� ���� ���� �� �¿쿡 �����ϴ� ���.
								Arrange_ball_in_brick(Temp3);
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
	if (Dead_verge() == 0){
		return true;
	}
	return false;//���ӿ��� �ƴ�.
}
bool C_board::Play_ball(unsigned Ball_num){
	if (Ball_num >= Balls.Num()){ return false; }
	C_ball& Ball = *(Balls.Read_editable(Ball_num));
	unsigned Temp1;
	if (Ball.Tick > 0){//���� �ƴ�.
		if (Ball.Tick == Ball.Speed_tick){
			Ball.Tick = 1;
			//Play
			//"Reflector����(�� �ݻ�)";
			for (Temp1 = 0; Temp1 < Ball.Speed_movenum; Temp1++){
				if (Play_ball_ref(Ball)){ Ball.Basezone_state = C_ball::REF; }//����� ���Ͻ� '�� ���� �ݻ��' ���� ����.
				else {//"����� �����Ͻ� Brick����";
					Play_ball_brick(Ball);
				}
				if (Ball.Basezone_check((BRICK_HEIGHT + FIELD_SIZE)*BRICK_SIZE*INTERNAL_RESOLUTION, (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE)*BRICK_SIZE*INTERNAL_RESOLUTION)
					== false){//���� ���ӺҰ��� ������ߵ�.
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
	�Ű����� ���� (a+(c-a)t,b+(d-b)t)�� �� (X1, Y1)(X2, Y2)�� �̷���� ������ t����, [(a-X1)(Y1-Y2)+(b-Y1)(X2-X1)] / [(a-c)(Y1-Y2)+(b-d)(X2-X1)]
	*/
	C_ball::C_ball_vector Lastball = Ball.Ball_rec.Read(Ball.Ball_rec.Num() - 1);//���� ��.(��������).
	int Ball_X, Ball_Y, Ref_X1, Ref_Y1, Ref_X2, Ref_Y2;
	C_fraction Ref_t = 2;
	bool Check;
	int Ball_is_right;

	Get_reflector_edge(&Ref_X1, &Ref_Y1, &Ref_X2, &Ref_Y2);
	Ball_X = Lastball.X + Lastball.Vector_X(); Ball_Y = Lastball.Y + Lastball.Vector_Y();

	Check = Line_in_line(Lastball.X, Lastball.Y, Ball_X, Ball_Y, Ref_X1, Ref_Y1, Ref_X2, Ref_Y2, &Ref_t);
	if (Check){//�ݻ� �Ͼ.
		if (Dot_is_right_side(Ref_X2, Ref_Y2, Ref_X1, Ref_Y1, Lastball.X, Lastball.Y)){ Ball_is_right = 1; }//�ݻ�Ǵ� ���� �����������ΰ�?.
		else { Ball_is_right = -1; }
		if (Ball.Ball_rec.Num() == Ball.SHADOW_NUM){ Ball.Ball_rec.Out(); }//Ball_rec���� Ȯ��.
		Ball.Ball_rec.In(
			C_ball::C_ball_vector(
			int(Lastball.X + Lastball.Vector_X()*(Ref_t.Get_value()) + 3 * Ball_is_right * C_ball::Vector[Reflector.Position][0] / (double)VECTOR_LENGTH),
			int(Lastball.Y + Lastball.Vector_Y()*(Ref_t.Get_value()) + 3 * Ball_is_right * C_ball::Vector[Reflector.Position][1] / (double)VECTOR_LENGTH),
			(2 * Reflector.Position + C_ball::DIRECTION_NUM * 3 / 2 - Lastball.Vector_num) % C_ball::DIRECTION_NUM)
			);
		Ball.Ball_rec.Read_editable(Ball.Ball_rec.Num() - 1)->State = C_ball::C_ball_vector::REF_REF;//�ݻ�Ǿ��ٴ� ǥ��.
		return true;
	}
	else return false;//�ݻ簡 �Ͼ�� ����.
}
void C_board::Play_ball_brick(C_ball& Ball){
	C_ball::C_ball_vector Lastball = Ball.Ball_rec.Read(Ball.Ball_rec.Num() - 1);//���� ��.(��������).
	unsigned Brick_X, Brick_Y;
	int Lastball_X_temp = Lastball.X, Lastball_Y_temp = Lastball.Y, Lastball_V_num_temp = Lastball.Vector_num;
	bool Ref_X0, Ref_X1, Ref_Y0, Ref_Y1;
	C_fraction Ref_tX0, Ref_tX1, Ref_tY0, Ref_tY1, Temp;
	unsigned Check = 0;
	bool Is_ref = false;
	//���� ���� ���� Brick�� ��ǥ�� ����.
	if ((Lastball.X % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0) && (Lastball.Vector_X() < 0)){ Brick_X = Lastball.X / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1; }//���� ���� ��輱���� ���� ��ǥ ������ ���� ���.
	else { Brick_X = FLOOR(Lastball.X, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION); }
	if ((Lastball.Y % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0) && (Lastball.Vector_Y() < 0)){ Brick_Y = Lastball.Y / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1; }//���� ���� ��輱���� ���� ��ǥ ������ ���� ���.
	else { Brick_Y = FLOOR(Lastball.Y, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION); }

	if (Lastball.Vector_X() == 0 || Lastball.Vector_Y() == 0){//���� ���� ������.
		if (Lastball.Vector_X() == 0){//Y�� ������.
			(Lastball.Vector_Y() > 0) ?
				Brick_Y = FLOOR(Lastball.Y + Lastball.Vector_Y(), BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION) :
				Brick_Y = CEIL(Lastball.Y + Lastball.Vector_Y(), BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1;
			if (Lastball.X % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0){//Y�� ��輱.
				if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){//�ݻ��.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y :
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X - 1, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
				else if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//�ݻ��.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y :
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
			else {//Y�� ���輱.
				if ((0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//�ݻ��.
					(Lastball.Vector_Y() > 0) ?
						Lastball_Y_temp = 2 * Brick_Y*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y :
						Lastball_Y_temp = 2 * (Brick_Y + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.Y;
					Lastball_V_num_temp = (C_ball::DIRECTION_NUM - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_Y && Brick_Y < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
		}
		else {//X�� ������.
			(Lastball.Vector_X() > 0) ?
				Brick_X = FLOOR(Lastball.X + Lastball.Vector_X(), BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION) :
				Brick_X = CEIL(Lastball.X + Lastball.Vector_X(), BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION) - 1;
			if (Lastball.Y % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0){//X�� ��輱.
				if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y - 1][Brick_X].Health != 0 : true){//�ݻ��.
					(Lastball.Vector_X() > 0) ?
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X :
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X;
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y - 1, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
				else if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//�ݻ��.
					(Lastball.Vector_X() > 0) ?
						Lastball_X_temp = 2 * Brick_X*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X :
						Lastball_X_temp = 2 * (Brick_X + 1)*BRICK_SIZE*INTERNAL_RESOLUTION - Lastball.X;
					Lastball_V_num_temp = (20 - Lastball_V_num_temp) % C_ball::DIRECTION_NUM;
					(0 <= Brick_X && Brick_X < BOARD_SIZE) ? Brick_hit(Brick_X, Brick_Y, Ball.Speed_movenum / Ball.Speed_tick) : 0;
					Is_ref = true;
				}
			}
			else {//Y�� ���輱.
				if ((0 <= Brick_X && Brick_X < BOARD_SIZE) ? Board[Brick_Y][Brick_X].Health != 0 : true){//�ݻ��.
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
	else {//�� �밢��.
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

		if (Check == 1){//���� �ݻ簡�ɼ�.
			//1���ݻ�˻�.
			if (Ref_X0) {

				if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){//�ݻ��� ���.
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
			//2���ݻ�˻�.
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
			if (Check == 2){//�ݻ�� ���.
				if (Temp == Ref_tX0){
					if ((Brick_X) ? Board[Brick_Y][Brick_X - 1].Health != 0 : true){//�ݻ��� ���.
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
		else if (Check == 2){//�𼭸��� ���� �ݻ�.
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
				if (Check == 2){//�� �������� �ݻ�ȵ� ���.(�밢�� ���� �˻�).
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
				else if (Check == 1){//�� ���� 1������ �ݻ�� ���.
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

	//�� �ݻ� ��� ����.
	Lastball_X_temp = Lastball_X_temp + C_ball::Vector[Lastball_V_num_temp][0];
	Lastball_Y_temp = Lastball_Y_temp + C_ball::Vector[Lastball_V_num_temp][1];
	if (Ball.Ball_rec.Num() == Ball.SHADOW_NUM){ Ball.Ball_rec.Out(); }//Ball_rec���� Ȯ��.
	Ball.Ball_rec.In(C_ball::C_ball_vector(Lastball_X_temp, Lastball_Y_temp, Lastball_V_num_temp));
	if (Is_ref){
		Ball.Ball_rec.Read_editable(Ball.Ball_rec.Num() - 1)->State = C_ball::C_ball_vector::BRICK_REF;
	}
}
void C_board::Move_reflector(Direction Input, unsigned Speed){
	int X1, Y1, X2, Y2;
	int Ref_X1, Ref_Y1, Ref_X2, Ref_Y2;
	C_ball::C_ball_vector* Lastball;
	bool Temp, Ball_speedup;
	int Loop1;
	
	Get_reflector_edge(&Ref_X1, &Ref_Y1, &Ref_X2, &Ref_Y2);//Reflector�̵� ���� ��ġ ����.
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
	case UL://�ð�ݴ���� ȸ��.(��ī��Ʈ��ǥ�� �ð�ݴ����).
		Reflector.Position = (C_ball::DIRECTION_NUM + Reflector.Position - 1) % C_ball::DIRECTION_NUM;
		break;
	case UR://�ð���� ȸ��.(��ī��Ʈ��ǥ�� �ð����).
		Reflector.Position = (Reflector.Position + 1) % C_ball::DIRECTION_NUM;
		break;
	default:
		break;
	}
	//Hitting Zone�ٱ��� ��� �������� ���̵�.
	Arrange_reflector();

	//Reflector�ȿ� ���� ���� ��츦 ����.
	Get_reflector_edge(&X1, &Y1, &X2, &Y2);
	if ((X1 == Ref_X1) && (Y1 == Ref_Y1) && (X2 == Ref_X2) && (Y2 == Ref_Y2)){//Reflector������ ����.
		return;
	}
	for (Loop1 = 0; Loop1 < Balls.Num(); Loop1++){
		Ball_speedup = false;
		Lastball = Balls.Read_editable(Loop1)->Ball_rec.Read_editable((Balls.Read(Loop1).Ball_rec.Num() - 1));//�� ��(Loop1��)�� ���� ��ġ(������ ��).
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
				;){//���� Reflector�� �����ӿ� ���� ���.
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
		case UL://�ð�ݴ���� ȸ��.(��ī��Ʈ��ǥ�� �ð�ݴ����).
		case UR://�ð���� ȸ��.(��ī��Ʈ��ǥ�� �ð����).
			for (;
				(Dot_is_right_side(Reflector.X, Reflector.Y, X1, Y1, Lastball->X, Lastball->Y) == Dot_is_right_side(X1, Y1, Ref_X1, Ref_Y1, Lastball->X, Lastball->Y)) &&
				(Dot_is_right_side(X1, Y1, Ref_X1, Ref_Y1, Lastball->X, Lastball->Y) == Dot_is_right_side(Ref_X1, Ref_Y1, Reflector.X, Reflector.Y, Lastball->X, Lastball->Y))
				;){//���� Reflector�� �����ӿ� ���� ���.(1�������� Position���⿡�� �ð���⿡ ����(��ī��Ʈ))
				Ball_speedup = true;
				switch (Input){
				case UL://�ð�ݴ���� ȸ��.(��ī��Ʈ��ǥ�� �ð�ݴ����).
					Lastball->X += C_ball::Vector[Reflector.Position][0];
					Lastball->Y += C_ball::Vector[Reflector.Position][1];
					break;
				case UR://�ð���� ȸ��.(��ī��Ʈ��ǥ�� �ð����).
					Lastball->X -= C_ball::Vector[Reflector.Position][0];
					Lastball->Y -= C_ball::Vector[Reflector.Position][1];
					break;
				}
			}
			for (;
				(Dot_is_right_side(Reflector.X, Reflector.Y, X2, Y2, Lastball->X, Lastball->Y) == Dot_is_right_side(X2, Y2, Ref_X2, Ref_Y2, Lastball->X, Lastball->Y)) &&
				(Dot_is_right_side(X2, Y2, Ref_X2, Ref_Y2, Lastball->X, Lastball->Y) == Dot_is_right_side(Ref_X2, Ref_Y2, Reflector.X, Reflector.Y, Lastball->X, Lastball->Y))
				;){//���� Reflector�� �����ӿ� ���� ���.
				Ball_speedup = true;
				switch (Input){
				case UL://�ð�ݴ���� ȸ��.(��ī��Ʈ��ǥ�� �ð�ݴ����).
					Lastball->X -= C_ball::Vector[Reflector.Position][0];
					Lastball->Y -= C_ball::Vector[Reflector.Position][1];
					break;
				case UR://�ð���� ȸ��.(��ī��Ʈ��ǥ�� �ð����).
					Lastball->X += C_ball::Vector[Reflector.Position][0];
					Lastball->Y += C_ball::Vector[Reflector.Position][1];
					break;
				}
			}
		}
		if (Ball_speedup){//���� �������� �ִ� ���.
			Balls.Read_editable(Loop1)->Speed_edit(1);//�ӵ� ����.
			Balls.Read_editable(Loop1)->Basezone_state = C_ball::REF;
		}
	}
}
void C_board::Arrange_reflector(){
	int X1, Y1, X2, Y2, X_small, X_large, Y_small, Y_large;
	int Verge = Dead_verge() - 1;//Reflector�� �������� ��ĭ ������ �־�� ��.�׷��� ���� ��쿡�� �ݻ� �˰����򿡼� ���� ������ ����.
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
void C_board::Arrange_ball_in_brick(unsigned Ball_num){//���� ������ ���� ��츦 ����.
	if (Ball_num >= Balls.Num()){ return; }
	C_ball& Ball = *Balls.Read_editable(Ball_num);
	C_ball::C_ball_vector * Lastball;
	int Brick_X, Brick_Y;
	int Brick_X_find[2], Brick_Y_find[2];
	int Dis, Dis_X, Dis_Y;
	int Loop1, Loop2;

	while (Ball.Ball_rec.Num() != 1){//�� �׸��� ����.
		Ball.Ball_rec.Out();
	}
	Lastball = Ball.Ball_rec.Read_editable(0);//������ ��.

	if (Lastball->X % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0){ Lastball->X++; }//���� ��輱�� �ִ°�츦����(�̼��ϰ� ���� �޶�������..ū ���� ����).
	if (Lastball->X > BOARD_SIZE_IN){ Lastball->X -= 2; }
	if (Lastball->Y % (BRICK_SIZE*INTERNAL_RESOLUTION) == 0){ Lastball->Y++; }
	if (Lastball->Y > BOARD_SIZE_IN){ Lastball->Y -= 2; }

	Brick_X = FLOOR(Lastball->X, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION);
	Brick_Y = FLOOR(Lastball->Y, BRICK_SIZE*INTERNAL_RESOLUTION) / (BRICK_SIZE*INTERNAL_RESOLUTION);
	if (Board[Brick_Y][Brick_X].Health > 0){//���� ���� �ӿ� ����.
		//�ֺ� ���� Ž��.
		for (Dis = 1; true; Dis++){//Dis==�߾� �������κ����� �Ÿ�.
			for (Dis_X = 0; Dis_X <= Dis; Dis_X++){
				Dis_Y = Dis - Dis_X;
				Brick_X_find[0] = Brick_X + Dis_X;
				Brick_X_find[1] = Brick_X - Dis_X;
				Brick_Y_find[0] = Brick_Y + Dis_Y;
				Brick_Y_find[1] = Brick_Y - Dis_Y;
				for (Loop1 = 0; Loop1 < 2; Loop1++){
					for (Loop2 = 0; Loop2 < 2; Loop2++){
						if (0 <= Brick_X_find[Loop1] && Brick_X_find[Loop1] < BOARD_SIZE && 0 <= Brick_Y_find[Loop2] && Brick_Y_find[Loop2] < BOARD_SIZE){
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
void C_board::Arrange_ball_zoneout(unsigned Ball_num){//���� Hitting_zone���� ������ ���ϴ� ���� ������.
	if (Ball_num >= Balls.Num()){ return; }
	C_ball::C_ball_vector& Lastball = *(Balls.Read_editable(Ball_num)->Ball_rec.Read_editable(Balls.Read(Ball_num).Ball_rec.Num() - 1));
	int Verge = Dead_verge() - 1;//Reflector�� ��輱 ����.
	int H1 = (BRICK_HEIGHT + FIELD_SIZE - Verge) * BRICK_SIZE*INTERNAL_RESOLUTION;
	int H2 = (BOARD_SIZE - BRICK_HEIGHT - FIELD_SIZE + Verge) * BRICK_SIZE*INTERNAL_RESOLUTION - 1;
	bool Check = false;
	if (Lastball.State == C_ball::C_ball_vector::NONE){//���� �ٸ� �ݻ� ������ ���� ������ ������. ���� �̻��ϰ� Ʋ���� ��¿����� ���°��� ����.
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
	"";//�����Ӽ��߰�.
	C_fraction Temp;
	unsigned Loop1;
	static const unsigned High_level = 600 / LEVELUP_SEC;//10�� ���� ����.
	static const unsigned Diff_speed = 100;//���̵� ��� �ӵ�.

	static const C_fraction H_probability[4][2] = {
		{ C_fraction(9, 10), C_fraction(1, 20) },//Health==1�� ���� Ȯ��. ����Ȯ���� ������Ȯ��(���ټ�).
		{ C_fraction(1, 3), C_fraction(1, 3) },//Health==1�� �ƴҶ� Health==2�� ���� Ȯ��. ����Ȯ���� ������Ȯ��(���ټ�).
		{ C_fraction(1, 3), C_fraction(1, 3) },//Health==2�� �ƴҶ� Health==3�� ���� Ȯ��.
		{ C_fraction(1, 3), C_fraction(1, 2) },//Health==3�� �ƴҶ� Health==4�� ���� Ȯ��.
		//�� �� Health==5.
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

	//�Ӽ�.
	Board[Y][X].State = C_brick::NORMAL;
	if (random(Balls.Num() * 20) == 0){
		Board[Y][X].State = C_brick::BALLOUT;
	}
}
void C_board::Brick_hit(unsigned X, unsigned Y, unsigned Speed){
	int Temp1;
	static const unsigned int Damageup_speed = (2 * C_board::BRICK_SIZE * C_board::INTERNAL_RESOLUTION / 5) / VECTOR_LENGTH;//���� �ִ� �ӵ��� 5���.
	"";//�����Ӽ��߰�.
	if (Board[Y][X].State == C_brick::NONE){
		return;
	}
	if (Board[Y][X].Health > 0){
		if (Board[Y][X].State == C_brick::NORMAL){
			for (Temp1 = 0; Temp1 < CEIL(Speed, Damageup_speed) / Damageup_speed; Temp1++){
				if (Board[Y][X].Health > 0){
					Board[Y][X].Health--;
				}
			}
			Print_brick(X, Y);
			Score += 10 * Speed;
		}
		if (Board[Y][X].State == C_brick::BALLOUT){
			for (Temp1 = 0; Temp1 < CEIL(Speed, Damageup_speed) / Damageup_speed; Temp1++){
				if (Board[Y][X].Health > 0){
					Board[Y][X].Health--;
				}
			}
			Print_brick(X, Y);
			Score += 10 * Speed;
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
				Screen << Coord(Screen_X, Screen_Y + BOARD_VERGE) << Str;
			}

			Temp2 += INTERNAL_RESOLUTION;
		} while (Temp2 < CEIL(HIGH(Y1, Y2), INTERNAL_RESOLUTION));
		Temp1 += INTERNAL_RESOLUTION;
	} while (Temp1 < CEIL(HIGH(X1, X2), INTERNAL_RESOLUTION));
}
C_fraction C_board::Level_func(unsigned Input, C_fraction Start, C_fraction End, unsigned High_level, unsigned Speed){
	//�������� 1~High_level������ ���̵��� Start���� End + (Start-End)/Speed���� �����Լ��� ��ȭ�ϸ�, �� ���ķδ� End�� �ٻ�������.
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

bool C_board::Box_in_line(int X1, int Y1, int X2, int Y2, int Box_X1, int Box_Y1, int Box_X2, int Box_Y2){
	//����--- (Y1 - Y2)x + (X2 - X1)y + (Y2 - Y1)X1 + (X1 - X2)Y1 = 0;
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
		if ((X1 - X3)*(Y3 - Y4) + (Y1 - Y3)*(X4 - X3)){//����. T�������� ����.
			T1 = -1; T2 = -1;
		}
		else {//������ ��ġ��.(4���� 3���� �ణ Ʋ� ó�����θ����� ������ ���������� ������ ���� �� ���� ���� T���� ����.)
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
void C_board::Draw_letter(unsigned X, unsigned Y, const char * Input, const char * Str, char color){//BOARD_VERGE������ǥ.(Screen��ü�� ��ǥ).
	const char * Letter;
	const char L[3] = { Str[0], Str[1], '\0' };
	unsigned Loop1, Loop2, Loop3;
	Loop1 = 0;
	Screen.Setcolor(color);
	for (; *Input != '\0'; Input++){//�� ����.
		Letter = Get_letter(*Input);
		for (Loop2 = 0; Loop2 < 5; Loop2++){//�� ��.
			Screen << Coord(X + Loop1, Y + Loop2);
			for (Loop3 = 0; Loop3 < 3; Loop3++){//�� ����.
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
		if (Speed_movenum == 1 && Set == 1 && Offset <= 0){//�ӵ��� ������ �䱸�Ͽ����� �̹� ������ ���.
			Check = false;
		}
		Speed_movenum = unsigned(
			((int)Set + Offset) ?
			(((int)Set + Offset <= Max_movenum) ? (int)Set + Offset : Max_movenum)
			: 1
			);
	}
	else {
		if (Speed_movenum == 1 && Offset < 0){//�ӵ��� ���߶�� �䱸�Ͽ����� �̹� ������ ���.
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
		){//Basezone�ȿ� �ִ� ���.
		if (Basezone_state == BASEZONE_OUT){//basezone���� ���°��.
			Basezone_state = BASEZONE_IN;
		}
		if (Basezone_state == START){//���ۺ��� basezone�̸� �ݻ�Ǿ��ٰ� ħ.
			Basezone_state = REF;
		}
	}
	else {//Basezone�ۿ� �ִ� ���.
		if (Basezone_state == BASEZONE_IN){//basezone������ ���� ���(�ݻ��ǿ� ���� �ʰ�).
			if (Speed_edit(0, 1) == false){
				return false;
			}
		}
		Basezone_state = BASEZONE_OUT;
	}
	return true;
}

//const array variable
const int C_ball::Vector[40][2] = {//��ǥ ��� Ư¡ ������, y��ǥ�� �Ʒ����� ����̹Ƿ�, ���� ���濡�� �ð� �������� ���.
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
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },//����.
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
�� �� �� �� �� �� �� �� �� �� ��
�� �� �� �� �� �� �� �� �� �� ��
�� �� �� �� ��
�� �� �� �� ��
�� �� �� �� �� �� �� ��
�� �� �� �� �� �� �� ��
�� �� �� �� �� �� �� ��
�� �� �� �� �� �� �� �� �� �� �� ��
��ܡ����������ۡ�䢷����墸����
*/






