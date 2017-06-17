//Visual Studio Version
#ifndef COORDINATE_HED //헤더 중복금지.
#define COORDINATE_HED
//mecro

//type, class
class C_time{
protected:
	int Start_time;
	int Start_time_sec;
public:
	C_time();
	virtual ~C_time();
	virtual int Get_time() const;
	virtual int Get_time_sec() const;
	virtual void Reset_time();
	static void Sleep_sec(int n);
};

struct Coord{
	unsigned X, Y;
	Coord(){ X = 0; Y = 0; }
	Coord(unsigned x, unsigned y){ X = x; Y = y; }
};

class C_screen{
protected:
	char **Screen;
	char **Screen_buf;
	Coord* Input_rec;//화면 변경 기록(출력 최적화 위함).
	unsigned Rec_num;
	unsigned X, Y;
	const unsigned Length;
	const unsigned Height;
public:
	C_screen();
	C_screen(unsigned L, unsigned H);
	virtual ~C_screen();
	C_screen(const C_screen& Input);
	virtual C_screen& operator=(const C_screen& Input);
	virtual C_screen& operator<<(const Coord& Input);
	virtual C_screen& operator<<(char Input);
	virtual C_screen& operator<<(int Input);
	virtual C_screen& operator<<(const char* Input);
	unsigned GetX() const;
	unsigned GetY() const;
	unsigned GetL() const;
	unsigned GetH() const;
	virtual void Set_consol();
	virtual void Show();
	virtual void Show_opt();
	virtual void Show_opt_kr();
	virtual void Show_opt_kr(unsigned Time);
	virtual C_screen& Clrscr();
};

class C_screen_color : public C_screen{
protected:
	char **Screen_col;
	char **Screen_col_buf;
	char Color;

public:
	typedef enum{
		BLACK,
		D_BLUE,
		D_GREEN,
		D_SKYBLUE,
		D_RED,
		D_VIOLET,
		D_YELLOW,
		GREY,
		D_GREY,
		BLUE,
		GREEN,
		SKYBLUE,
		RED,
		VIOLET,
		YELLOW,
		WHITE,
	} T_color;

	C_screen_color();
	C_screen_color(unsigned L, unsigned H, T_color Letter = BLACK, T_color Back = WHITE);
	virtual ~C_screen_color();
	C_screen_color(const C_screen& Input);
	C_screen_color(const C_screen_color&Input);

	virtual C_screen_color& operator=(const C_screen_color& Input);
	virtual C_screen_color& operator<<(const Coord& Input);
	virtual C_screen_color& operator<<(char Input);
	virtual C_screen_color& operator<<(int Input);
	virtual C_screen_color& operator<<(const char* Input);
	virtual void Set_consol();
	virtual void Setcolor(T_color Letter, T_color Back);
	virtual void Setcolor(char Input);
	virtual void Reverse_color();
	virtual char Get_color() const;
	virtual void Show();
	virtual void Show_opt();
	virtual void Show_opt_kr();
	virtual void Show_opt_kr(unsigned Time);
	virtual C_screen& Clrscr();
};

//function
int random(unsigned N);
bool Probability(unsigned N, unsigned D);
template <typename T>
T HIGH(T a, T b){ return a > b ? a : b; }
template <typename T>
T LOW(T a, T b){ return a < b ? a : b; }
unsigned Mymod(int a, unsigned b);
int FLOOR(int a, unsigned b);//내림.
int CEIL(int a, unsigned b);//올림.
int ROUND(int a, unsigned b);//반올림.
void gotoxy(int x, int y);
#endif //COORDINATE_HED
//verge







