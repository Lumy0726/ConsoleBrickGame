#include <windows.h>
#include "Keyboard.h"

static unsigned Key_info[] = {
	VK_LBUTTON,
	VK_RBUTTON,
	VK_BACK,
	VK_RETURN,
	VK_SHIFT,
	VK_LSHIFT,
	VK_RSHIFT,
	VK_CONTROL,
	VK_LCONTROL,
	VK_RCONTROL,
	VK_SPACE,
	VK_LEFT,
	VK_UP,
	VK_RIGHT,
	VK_DOWN,
	0x30,
	0x31,
	0x32,
	0x33,
	0x34,
	0x35,
	0x36,
	0x37,
	0x38,
	0x39,
	0x41,
	0x42,
	0x43,
	0x44,
	0x45,
	0x46,
	0x47,
	0x48,
	0x49,
	0x4a,
	0x4b,
	0x4c,
	0x4d,
	0x4e,
	0x4f,
	0x50,
	0x51,
	0x52,
	0x53,
	0x54,
	0x55,
	0x56,
	0x57,
	0x58,
	0x59,
	0x5a,
	VK_NUMPAD0,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
	VK_NUMPAD5,
	VK_NUMPAD6,
	VK_NUMPAD7,
	VK_NUMPAD8,
	VK_NUMPAD9,
};
bool C_keyboard::Key_state[sizeof(Key_info)/sizeof(int)];

//fuction define
void C_keyboard::Check_all(){
	int Loop1;
	for (Loop1 = 0; Loop1 < sizeof(Key_info) / sizeof(int); Loop1++){
		Key_state[Loop1] = (GetAsyncKeyState(Key_info[Loop1]) & 0x8000) ? true : false;
	}
}
void C_keyboard::Check(C_keyboard::Key_code Input){
	Key_state[Input] = (GetAsyncKeyState(Key_info[Input]) & 0x8000) ? true : false;
}
bool C_keyboard::Check_press(C_keyboard::Key_code Input){
	bool Temp = Key_state[Input];
	Check(Input);
	if (Temp == false && Key_state[Input]==true){
		return true;
	}
	else return false;
}
bool C_keyboard::Check_out(C_keyboard::Key_code Input){
	bool Temp = Key_state[Input];
	Check(Input);
	if (Temp == true && Key_state[Input] == false){
		return true;
	}
	else return false;
}
bool C_keyboard::Check_state(C_keyboard::Key_code Input){
	Check(Input);
	return Key_state[Input];
}
//verge

