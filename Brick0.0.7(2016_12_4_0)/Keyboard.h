#ifndef KEYBOARD_H
#define KEYBOARD_H

class C_keyboard{
protected:
	static bool Key_state[];

public:
	enum Key_code{
		LMOUSE = 0,
		RMOUSE,
		BACKSPACE,
		ENTER,
		SHIFT,
		LSHIFT,
		RSHIFT,
		CONTROL,
		LCONTROL,
		RCONTROL,
		SPACE,
		LEFT,
		UP,
		RIGHT,
		DOWN,
		NUM0,
		NUM1,
		NUM2,
		NUM3,
		NUM4,
		NUM5,
		NUM6,
		NUM7,
		NUM8,
		NUM9,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		NUMPAD0,
		NUMPAD2,
		NUMPAD3,
		NUMPAD4,
		NUMPAD5,
		NUMPAD6,
		NUMPAD7,
		NUMPAD8,
		NUMPAD9,
	};

	static void Check_all();
	static void Check(Key_code Input);
	static bool Check_press(Key_code Input);
	static bool Check_out(Key_code Input);
};

#endif