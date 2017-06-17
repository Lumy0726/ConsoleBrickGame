//Visual Studio Version
#include <iostream>
#include <time.h>
#include <Windows.h>
using namespace std;
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") 
#include "Coordinate_Outputstream.h"

//mecro
#define LENGTH 80
#define HEIGHT 25

//function define
//C_time
C_time::C_time() :Start_time_sec((int)time(NULL)) {
	timeBeginPeriod(1);
	Start_time = timeGetTime();
}
C_time::~C_time(){
	timeEndPeriod(1);
}
int C_time::Get_time() const {
	return (int)timeGetTime() - Start_time;
}
int C_time::Get_time_sec() const {
	return (int)time(NULL) - Start_time_sec;
}
void C_time::Reset_time(){
	Start_time = timeGetTime();
	Start_time_sec = (int)time(NULL);
}
void C_time::Sleep_sec(int n){
	int Time = (int)time(NULL);
	if (n > 0){
		while ((int)time(NULL) - Time < n);
	}
}

//C_screen
C_screen::C_screen() : Length(LENGTH), Height(HEIGHT){//생성자 - size 디폴트.
	int Temp1, Temp2;
	Screen = new char*[Height];
	Screen_buf = new char*[Height];
	Input_rec = new Coord[Length*Height];
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		Screen[Temp1] = new char[Length + 1];
		Screen_buf[Temp1] = new char[Length];
	}
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen[Temp1][Temp2] = ' ';
			Screen_buf[Temp1][Temp2] = ' ';
		}
		Screen[Temp1][Temp2] = '\0';
	}
	Rec_num = X = Y = 0;
}
C_screen::C_screen(unsigned L, unsigned H) : Length(L), Height(H){//생성자 - size설정.
	int Temp1, Temp2;
	Screen = new char*[Height];
	Screen_buf = new char*[Height];
	Input_rec = new Coord[Length*Height];
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		Screen[Temp1] = new char[Length + 1];
		Screen_buf[Temp1] = new char[Length];
	}
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen[Temp1][Temp2] = ' ';
			Screen_buf[Temp1][Temp2] = ' ';
		}
		Screen[Temp1][Temp2] = '\0';
	}
	/*sprintf_s(Buf, "mode con cols=%d lines=%d", Length, Height + 1);
	system(Buf);*/
	Rec_num = X = Y = 0;
}
C_screen::~C_screen(){//파괴자.
	int Temp1;
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		delete[] Screen[Temp1];
		delete[] Screen_buf[Temp1];
	}
	delete[] Screen;
	delete[] Screen_buf;
	delete[] Input_rec;
}
C_screen::C_screen(const C_screen& Input) : Length(Input.Length), Height(Input.Height){//복사생성자.
	int Temp1, Temp2;
	Screen = new char*[Height];
	Screen_buf = new char*[Height];
	Input_rec = new Coord[Length*Height];
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		Screen[Temp1] = new char[Length + 1];
		Screen_buf[Temp1] = new char[Length];
	}
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen[Temp1][Temp2] = Input.Screen[Temp1][Temp2];
			Screen_buf[Temp1][Temp2] = Input.Screen_buf[Temp1][Temp2];
		}
		Screen[Temp1][Temp2] = '\0';
	}
	for (Temp1 = 0; Temp1 < Length*Height; Temp1++){
		Input_rec[Temp1]=Input.Input_rec[Temp1];
	}
	X = Input.X; Y = Input.Y; Rec_num = Input.Rec_num;
}
C_screen& C_screen::operator=(const C_screen& Input){//대입연산자 - 동일size에만 작동.
	int Temp1, Temp2;
	if (Length == Input.Length && Height == Input.Height){
		for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
			for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
				Screen[Temp1][Temp2] = Input.Screen[Temp1][Temp2];
				Screen_buf[Temp1][Temp2] = Input.Screen_buf[Temp1][Temp2];
			}
		}
		for (Temp1 = 0; Temp1 < Length*Height; Temp1++){
			Input_rec[Temp1] = Input.Input_rec[Temp1];
		}
		X = Input.X; Y = Input.Y; Rec_num = Input.Rec_num;
	}
	return *this;
}
C_screen& C_screen::operator<<(const Coord& Input){ //좌표 대입 연산자.
	X = (Input.X < Length) ? Input.X : Length - 1;
	Y = (Input.Y < Height) ? Input.Y : Height - 1;
	return *this;
}
C_screen& C_screen::operator<<(char Input){//문자 출력대기 연산자.
	if (Rec_num < Length*Height){
		Input_rec[Rec_num++] = Coord(X,Y);
	}
	Screen[Y][X++] = Input;
	if (X == Length){
		X = 0; Y++;
		if (Y == Height){ Y = X = 0; }
	}
	return *this;
}
C_screen& C_screen::operator<<(int Input){//정수 출력대기 연산자.
	char Buf[20];
	sprintf_s(Buf, "%d", Input);
	return (*this << Buf);
}
C_screen& C_screen::operator<<(const char* Input){//문자열 출력대기 연산자.
	while (1){
		if (*Input){
			(*this) << (*Input++);
			//Screen[Y][X++] = *Input++;
		}
		else {
			break;
		}
		if (X == Length){
			X = 0; Y++;
			if (Y == Height){ Y = X = 0; break; }
		}
	}
	return *this;
}
unsigned C_screen::GetX() const{
	return X;
}
unsigned C_screen::GetY() const{
	return Y;
}
unsigned C_screen::GetL() const{
	return Length;
}
unsigned C_screen::GetH() const{
	return Height;
}
void C_screen::Set_consol(){
	int Temp1, Temp2;
	char Buf[30];
	sprintf_s(Buf, "mode con cols=%d lines=%d", Length, Height + 1);
	system(Buf);
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){//콘솔 크기 변경시, 실제 화면 지워짐.
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen_buf[Temp1][Temp2] = ' ';
		}
	}
	Rec_num = Length*Height;//버퍼가 리셋되었기 때문에, 화면 변경 기록이 무의미함.
}
void  C_screen::Show(){//화면 출력.
	int Temp1, Temp2;
	//화면 출력 저장.
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen_buf[Temp1][Temp2] = Screen[Temp1][Temp2];
		}
	}
	gotoxy(0, 0);
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		/*for (Temp2=0; Temp2<(int)Length; Temp2++){
			cout << Screen[Temp1][Temp2];
			}
			cout<<endl;*/
		cout << Screen[Temp1];
	}
	Rec_num = 0;
}
void C_screen::Show_opt(){//화면 최적화 출력.
	int Temp1, Temp2;
	if (Rec_num < Length*Height){//화면 변경 기록 사용 경우.
		for (Temp1 = 0; Temp1 < Rec_num; Temp1++){
			if (Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] != Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X]){//다를때.
				gotoxy(Input_rec[Temp1].X, Input_rec[Temp1].Y);
				cout << Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//출력.
				Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] = Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//저장.
			}
		}
	}
	else {
		for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
			for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
				if (Screen_buf[Temp1][Temp2] != Screen[Temp1][Temp2]){//다를때.
					gotoxy(Temp2, Temp1);
					cout << Screen[Temp1][Temp2];//출력.
					Screen_buf[Temp1][Temp2] = Screen[Temp1][Temp2];//저장.
				}
			}
		}
	}
	Rec_num = 0;
}
void C_screen::Show_opt_kr(){//화면 최적화 출력(2바이트 문자용)(1바이트 1바이트 혼용시 오류 주의)(2바이트 문자의 x좌표는 반드시 짝수 이어야함).
	int Temp1, Temp2;
	if (Rec_num < Length*Height){//화면 변경 기록 사용 경우.
		for (Temp1 = 0; Temp1 < Rec_num; Temp1++){
			if (Input_rec[Temp1].X % 2 == 0 && (int)Temp1!=Rec_num-1){
				if (Input_rec[Temp1].X + 1 == Input_rec[Temp1 + 1].X){
					//2바이트 문자 가능성 있는 경우.
					if ((Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] != Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X]) ||
						(Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1] != Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1])){//다를때.
						gotoxy(Input_rec[Temp1].X, Input_rec[Temp1].Y);
						cout << Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X] << Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1];//출력.
						Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] = Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//저장.
						Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1] = Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1];//저장.
					}
					Temp1++;
					continue;
				}
			}
			//2바이트 문자 가능성 없는 경우.
			if (Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] != Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X]){//다를때.
				gotoxy(Input_rec[Temp1].X, Input_rec[Temp1].Y);
				cout << Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//출력.
				Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] = Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//저장.
			}
		}
	}
	else {
		for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
			for (Temp2 = 0; Temp2 < (int)Length; Temp2 += 2){
				if ((Screen_buf[Temp1][Temp2] != Screen[Temp1][Temp2]) ||
					(Screen_buf[Temp1][Temp2 + 1] != Screen[Temp1][Temp2 + 1])){//다를때.
					gotoxy(Temp2, Temp1);
					cout << Screen[Temp1][Temp2] << Screen[Temp1][Temp2 + 1];//출력.
					Screen_buf[Temp1][Temp2] = Screen[Temp1][Temp2];//저장.
					Screen_buf[Temp1][Temp2 + 1] = Screen[Temp1][Temp2 + 1];//저장.
				}
			}
		}
	}
	Rec_num = 0;
}
C_screen& C_screen::Clrscr(){//화면 비우기(출력 안함).
	int Temp1, Temp2;
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen[Temp1][Temp2] = ' ';
		}
	}
	Rec_num = Length*Height;//화면 전체를 변경하였기 때문에 변경 기록이 무의미함.
	return *this;
}

//C_screen_color
C_screen_color::C_screen_color() :C_screen(), Color(0xf0){
	int Temp1, Temp2;
	Screen_col = new char*[Height];
	Screen_col_buf = new char*[Height];
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		Screen_col[Temp1] = new char[Length];
		Screen_col_buf[Temp1] = new char[Length];
	}
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen_col[Temp1][Temp2] = 0xf0;
			Screen_col_buf[Temp1][Temp2] = 0xf0;
		}
	}
}
C_screen_color::C_screen_color(unsigned L, unsigned H, T_color Letter, T_color Back) :C_screen(L, H), Color(Back * 16 + Letter){
	int Temp1, Temp2;
	Screen_col = new char*[Height];
	Screen_col_buf = new char*[Height];
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		Screen_col[Temp1] = new char[Length];
		Screen_col_buf[Temp1] = new char[Length];
	}
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen_col[Temp1][Temp2] = Color;
			Screen_col_buf[Temp1][Temp2] = Color;
		}
	}
}
C_screen_color::~C_screen_color(){
	int Temp1;
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		delete[] Screen_col[Temp1];
		delete[] Screen_col_buf[Temp1];
	}
	delete[] Screen_col;
	delete[] Screen_col_buf;
}
C_screen_color::C_screen_color(const C_screen& Input) :C_screen(Input), Color(0xf0){
	int Temp1, Temp2;
	Screen_col = new char*[Height];
	Screen_col_buf = new char*[Height];
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		Screen_col[Temp1] = new char[Length];
		Screen_col_buf[Temp1] = new char[Length];
	}
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen_col[Temp1][Temp2] = 0xf0;
			Screen_col_buf[Temp1][Temp2] = 0xf0;
		}
	}
}
C_screen_color::C_screen_color(const C_screen_color&Input): C_screen(Input){
	int Temp1, Temp2;
	Screen_col = new char*[Height];
	Screen_col_buf = new char*[Height];
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		Screen_col[Temp1] = new char[Length];
		Screen_col_buf[Temp1] = new char[Length];
	}
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen_col[Temp1][Temp2] = Input.Screen_col[Temp1][Temp2];
			Screen_col_buf[Temp1][Temp2] = Input.Screen_col_buf[Temp1][Temp2];
		}
	}
	Color = Input.Color;
}

C_screen_color& C_screen_color::operator=(const C_screen_color& Input){
	C_screen::operator=(Input);
	int Temp1, Temp2;
	if (Length == Input.Length && Height == Input.Height){
		for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
			for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
				Screen_col[Temp1][Temp2] = Input.Screen_col[Temp1][Temp2];
				Screen_col_buf[Temp1][Temp2] = Input.Screen_col_buf[Temp1][Temp2];
			}
		}
		Color = Input.Color;
	}
	return *this;
}
C_screen_color& C_screen_color::operator<<(const Coord& Input){ C_screen::operator<<(Input); return *this; }
C_screen_color& C_screen_color::operator<<(char Input){
	Screen_col[Y][X] = Color;
	C_screen::operator<<(Input);
	return *this;
}
C_screen_color& C_screen_color::operator<<(int Input){
	char Buf[20];
	sprintf_s(Buf, "%d", Input);
	return (*this << Buf);
}
C_screen_color& C_screen_color::operator<<(const char* Input){
	C_screen::operator<<(Input);
	/*while (1){
		if (*Input){
			Screen_col[Y][X] = Color;
			Screen[Y][X++] = *Input++;
		}
		else {
			break;
		}
		if (X == Length){
			X = 0; Y++;
			if (Y == Height){ Y = X = 0; break; }
		}
	}*/
	return *this;
}
void C_screen_color::Setcolor(T_color Letter, T_color Back){
	Color = Back * 16 + Letter;
}
void C_screen_color::Setcolor(char Input){
	Color = Input;
}
char C_screen_color::Get_color() const{
	return Color;
}
void C_screen_color::Set_consol(){
	C_screen::Set_consol();
	int Temp1, Temp2;
	char Buf[30];
	sprintf_s(Buf, "color %02x", Color&0xff);
	system(Buf);
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen_col_buf[Temp1][Temp2] = Color;
		}
	}
}
void C_screen_color::Show(){
	HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
	int Temp1, Temp2;
	//화면 출력 저장.
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen_buf[Temp1][Temp2] = Screen[Temp1][Temp2];
			Screen_col_buf[Temp1][Temp2] = Screen_col[Temp1][Temp2];
		}
	}
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			gotoxy(Temp2, Temp1);//출력.
			SetConsoleTextAttribute(H, Screen_col[Temp1][Temp2] & 0xff);
			cout << Screen[Temp1][Temp2];
		}
	}
	Rec_num = 0;
}
void C_screen_color::Show_opt(){
	int Temp1, Temp2;
	HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
	if (Rec_num < Length*Height){//화면 변경 기록 사용 경우.
		for (Temp1 = 0; Temp1 < Rec_num; Temp1++){
			if ((Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] != Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X]) ||
				(Screen_col_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] != Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X])){//다를때.
				gotoxy(Input_rec[Temp1].X, Input_rec[Temp1].Y);
				SetConsoleTextAttribute(H, Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X] & 0xff);
				cout << Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//출력.
				Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] = Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//저장.
				Screen_col_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] = Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X];
			}
		}
	}
	else {
		for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
			for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
				if ((Screen_buf[Temp1][Temp2] != Screen[Temp1][Temp2]) ||
					(Screen_col_buf[Temp1][Temp2] != Screen_col[Temp1][Temp2])){//다를때.
					gotoxy(Temp2, Temp1);//출력.
					SetConsoleTextAttribute(H, Screen_col[Temp1][Temp2] & 0xff);
					cout << Screen[Temp1][Temp2];
					Screen_buf[Temp1][Temp2] = Screen[Temp1][Temp2];//저장.
					Screen_col_buf[Temp1][Temp2] = Screen_col[Temp1][Temp2];
				}
			}
		}
	}
	Rec_num = 0;
}
void C_screen_color::Show_opt_kr(){
	int Temp1, Temp2;
	HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
	if (Rec_num < Length*Height){//화면 변경 기록 사용 경우.
		for (Temp1 = 0; Temp1 < Rec_num; Temp1++){
			if (Input_rec[Temp1].X % 2 == 0 && (int)Temp1 != Rec_num - 1){
				if (Input_rec[Temp1].X + 1 == Input_rec[Temp1 + 1].X){
					//2바이트 문자 가능성 있는 경우.
					if ((Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] != Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X]) ||
						(Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1] != Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1]) ||
						(Screen_col_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] != Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X]) ||
						(Screen_col_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1] != Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1])){//다를때.
						gotoxy(Input_rec[Temp1].X, Input_rec[Temp1].Y);
						SetConsoleTextAttribute(H, Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X] & 0xff);
						cout << Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//출력.
						SetConsoleTextAttribute(H, Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1] & 0xff);
						cout << Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1];
						Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] = Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//저장.
						Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1] = Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1];//저장.
						Screen_col_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] = Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X];//저장.
						Screen_col_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1] = Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X + 1];//저장.
					}
					Temp1++;
					continue;
				}
			}
			//2바이트 문자 가능성 없는 경우.
			if ((Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] != Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X]) ||
				(Screen_col_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] != Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X])){//다를때.
				gotoxy(Input_rec[Temp1].X, Input_rec[Temp1].Y);
				SetConsoleTextAttribute(H, Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X] & 0xff);
				cout << Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//출력.
				Screen_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] = Screen[Input_rec[Temp1].Y][Input_rec[Temp1].X];//저장.
				Screen_col_buf[Input_rec[Temp1].Y][Input_rec[Temp1].X] = Screen_col[Input_rec[Temp1].Y][Input_rec[Temp1].X];
			}
		}
	}
	else {
		for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
			for (Temp2 = 0; Temp2 < (int)Length; Temp2 += 2){
				if ((Screen_buf[Temp1][Temp2] != Screen[Temp1][Temp2]) ||
					(Screen_buf[Temp1][Temp2 + 1] != Screen[Temp1][Temp2 + 1]) ||
					(Screen_col_buf[Temp1][Temp2] != Screen_col[Temp1][Temp2]) ||
					(Screen_col_buf[Temp1][Temp2 + 1] != Screen_col[Temp1][Temp2 + 1])){//다를때.
					gotoxy(Temp2, Temp1);
					SetConsoleTextAttribute(H, Screen_col[Temp1][Temp2] & 0xff);
					cout << Screen[Temp1][Temp2];//출력.
					SetConsoleTextAttribute(H, Screen_col[Temp1][Temp2 + 1] & 0xff);
					cout << Screen[Temp1][Temp2 + 1];
					Screen_buf[Temp1][Temp2] = Screen[Temp1][Temp2];//저장.
					Screen_buf[Temp1][Temp2 + 1] = Screen[Temp1][Temp2 + 1];//저장.
					Screen_col_buf[Temp1][Temp2] = Screen_col[Temp1][Temp2];//저장.
					Screen_col_buf[Temp1][Temp2 + 1] = Screen_col[Temp1][Temp2 + 1];//저장.
				}
			}
		}
	}
	Rec_num = 0;
}
C_screen& C_screen_color::Clrscr(){
	int Temp1, Temp2;
	for (Temp1 = 0; Temp1 < (int)Height; Temp1++){
		for (Temp2 = 0; Temp2 < (int)Length; Temp2++){
			Screen[Temp1][Temp2] = ' ';
			Screen_col[Temp1][Temp2] = Color;
		}
	}
	Rec_num = Length*Height;//화면 전체를 변경하였기 때문에 변경 기록이 무의미함.
	return *this;
}

int random(int N){
	static unsigned int A = (unsigned int)time(NULL);
	if (N>100000 || 1 > N) return -1;
	A = ((A << 3) - 1) % 100000000;
	return int((int)(A / 1000) % N);
}

void gotoxy(int x, int y)
{
	COORD Cur;
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

//verge











