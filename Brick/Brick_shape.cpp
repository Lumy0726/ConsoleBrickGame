#include "Brick_shape.h"
#include "Coordinate_outputstream.h"
#include "My_debugstop.h"

C_brick_shape::C_brick_shape(unsigned _X_size, unsigned _Y_size) {
	unsigned Loop1, Loop2;
	X_size = _X_size;
	Y_size = _Y_size;
	Shape = new bool*[Y_size];
	for (Loop1 = 0; Loop1 < Y_size; Loop1++) {
		Shape[Loop1] = new bool[X_size];
	}
	for (Loop1 = 0; Loop1 < Y_size; Loop1++) {
		for (Loop2 = 0; Loop2 < X_size; Loop2++) {
			Shape[Loop1][Loop2] = false;
		}
	}
}
C_brick_shape::C_brick_shape(const C_brick_shape& Input) {
	unsigned Loop1, Loop2;
	X_size = Input.X_size;
	Y_size = Input.Y_size;
	Shape = new bool*[Y_size];
	for (Loop1 = 0; Loop1 < Y_size; Loop1++) {
		Shape[Loop1] = new bool[X_size];
	}
	for (Loop1 = 0; Loop1 < Y_size; Loop1++) {
		for (Loop2 = 0; Loop2 < X_size; Loop2++) {
			Shape[Loop1][Loop2] = Input.Shape[Loop1][Loop2];
		}
	}
}
C_brick_shape& C_brick_shape::operator=(const C_brick_shape& Input) {
	unsigned Loop1, Loop2;
	if (this != &Input) {
		if (X_size != Input.X_size || Y_size != Input.Y_size) {//Size다름, 재할당.
			for (Loop1 = 0; Loop1 < Y_size; Loop1++) {
				delete[] Shape[Loop1];
			}
			delete[] Shape;
			X_size = Input.X_size; Y_size = Input.Y_size;
			Shape = new bool*[Y_size];
			for (Loop1 = 0; Loop1 < Y_size; Loop1++) {
				Shape[Loop1] = new bool[X_size];
			}
		}
		for (Loop1 = 0; Loop1 < Y_size; Loop1++) {
			for (Loop2 = 0; Loop2 < X_size; Loop2++) {
				Shape[Loop1][Loop2] = Input.Shape[Loop1][Loop2];
			}
		}
	}
	return *this;
}
C_brick_shape::~C_brick_shape() {
	unsigned Loop1;
	for (Loop1 = 0; Loop1 < Y_size; Loop1++) {
		delete[] Shape[Loop1];
	}
	delete[] Shape;
}
bool& C_brick_shape::Access(unsigned X, unsigned Y) {
	if (X < X_size &&Y < Y_size) {
		return Shape[Y][X];
	}
	else {//일기 오류.
		ERROR_DEBUGSTOP(true);
		//throw "Not vaild access";
		return Shape[0][0];
	}
}
bool C_brick_shape::Read(unsigned X, unsigned Y)const {
	if (X < X_size &&Y < Y_size) {
		return Shape[Y][X];
	}
	else {//일기 오류.
		ERROR_DEBUGSTOP(true);
		//throw "Not vaild access";
		return false;
	}
}
unsigned C_brick_shape::Get_X_size()const {
	return X_size;
}
unsigned C_brick_shape::Get_Y_size()const {
	return Y_size;
}
C_brick_shape& C_brick_shape::Rotate(unsigned Num) {
	C_brick_shape Temp(Y_size, X_size);
	unsigned Loop1, Loop2;
	for (Loop1 = 0; Loop1 < Y_size; Loop1++) {
		for (Loop2 = 0; Loop2 < X_size; Loop2++) {
			Temp.Access(Y_size - 1 - Loop1, Loop2) = Read(Loop2, Loop1);
		}
	}
	return *this = Temp;
}

C_brick_shape_list::C_brick_shape_list() :Brick_shape_num(18){
	Shape = new C_brick_shape*[Brick_shape_num];

	//2x2정사각형.
	Shape[0] = new C_brick_shape(2,2);
	Shape[0]->Access(0, 0) = true;
	Shape[0]->Access(0, 1) = true;
	Shape[0]->Access(1, 0) = true;
	Shape[0]->Access(1, 1) = true;

	//2x1.
	Shape[1] = new C_brick_shape(2, 1);
	Shape[1]->Access(0, 0) = true;
	Shape[1]->Access(1, 0) = true;

	//OO
	//OX
	Shape[2] = new C_brick_shape(2, 2);
	Shape[2]->Access(0, 0) = true;
	Shape[2]->Access(1, 0) = true;
	Shape[2]->Access(0, 1) = true;

	//3x1.
	Shape[3] = new C_brick_shape(3, 1);
	Shape[3]->Access(0, 0) = true;
	Shape[3]->Access(1, 0) = true;
	Shape[3]->Access(2, 0) = true;

	//OOO
	//OXX
	Shape[4] = new C_brick_shape(3, 2);
	Shape[4]->Access(0, 0) = true;
	Shape[4]->Access(1, 0) = true;
	Shape[4]->Access(2, 0) = true;
	Shape[4]->Access(0, 1) = true;

	//OOO
	//XXO
	Shape[5] = new C_brick_shape(3, 2);
	Shape[5]->Access(0, 0) = true;
	Shape[5]->Access(1, 0) = true;
	Shape[5]->Access(2, 0) = true;
	Shape[5]->Access(2, 1) = true;

	//OOO
	//OXO
	Shape[6] = new C_brick_shape(3, 2);
	Shape[6]->Access(0, 0) = true;
	Shape[6]->Access(1, 0) = true;
	Shape[6]->Access(2, 0) = true;
	Shape[6]->Access(0, 1) = true;
	Shape[6]->Access(2, 1) = true;

	//OOO
	//XOX
	Shape[7] = new C_brick_shape(3, 2);
	Shape[7]->Access(0, 0) = true;
	Shape[7]->Access(1, 0) = true;
	Shape[7]->Access(2, 0) = true;
	Shape[7]->Access(1, 1) = true;

	//OOX
	//XOO
	Shape[8] = new C_brick_shape(3, 2);
	Shape[8]->Access(0, 0) = true;
	Shape[8]->Access(1, 0) = true;
	Shape[8]->Access(1, 1) = true;
	Shape[8]->Access(2, 1) = true;

	//XOO
	//OOX
	Shape[9] = new C_brick_shape(3, 2);
	Shape[9]->Access(1, 0) = true;
	Shape[9]->Access(2, 0) = true;
	Shape[9]->Access(0, 1) = true;
	Shape[9]->Access(1, 1) = true;

	//OOO
	//OOX
	Shape[10] = new C_brick_shape(3, 2);
	Shape[10]->Access(0, 0) = true;
	Shape[10]->Access(1, 0) = true;
	Shape[10]->Access(2, 0) = true;
	Shape[10]->Access(0, 1) = true;
	Shape[10]->Access(1, 1) = true;

	//OOO
	//XOO
	Shape[11] = new C_brick_shape(3, 2);
	Shape[11]->Access(0, 0) = true;
	Shape[11]->Access(1, 0) = true;
	Shape[11]->Access(2, 0) = true;
	Shape[11]->Access(1, 1) = true;
	Shape[11]->Access(2, 1) = true;

	//OOO
	//XOX
	//XOX
	Shape[12] = new C_brick_shape(3, 3);
	Shape[12]->Access(0, 0) = true;
	Shape[12]->Access(1, 0) = true;
	Shape[12]->Access(2, 0) = true;
	Shape[12]->Access(1, 1) = true;
	Shape[12]->Access(1, 2) = true;

	//OOO
	//OXX
	//OXX
	Shape[13] = new C_brick_shape(3, 3);
	Shape[13]->Access(0, 0) = true;
	Shape[13]->Access(1, 0) = true;
	Shape[13]->Access(2, 0) = true;
	Shape[13]->Access(0, 1) = true;
	Shape[13]->Access(0, 2) = true;

	//OOX
	//XOX
	//XOO
	Shape[14] = new C_brick_shape(3, 3);
	Shape[14]->Access(0, 0) = true;
	Shape[14]->Access(1, 0) = true;
	Shape[14]->Access(1, 1) = true;
	Shape[14]->Access(1, 2) = true;
	Shape[14]->Access(2, 2) = true;

	//XOO
	//XOX
	//OOX
	Shape[15] = new C_brick_shape(3, 3);
	Shape[15]->Access(2, 0) = true;
	Shape[15]->Access(1, 0) = true;
	Shape[15]->Access(1, 1) = true;
	Shape[15]->Access(1, 2) = true;
	Shape[15]->Access(0, 2) = true;

	//OOO
	//OXO
	//OXO
	Shape[16] = new C_brick_shape(3,3);
	Shape[16]->Access(0, 0) = true;
	Shape[16]->Access(1, 0) = true;
	Shape[16]->Access(2, 0) = true;
	Shape[16]->Access(2, 1) = true;
	Shape[16]->Access(2, 2) = true;
	Shape[16]->Access(1, 2) = false;
	Shape[16]->Access(0, 2) = true;
	Shape[16]->Access(0, 1) = true;

	//XOX
	//OOO
	//XOX
	Shape[17] = new C_brick_shape(3, 3);
	Shape[17]->Access(1, 0) = true;
	Shape[17]->Access(0, 1) = true;
	Shape[17]->Access(1, 1) = true;
	Shape[17]->Access(2, 1) = true;
	Shape[17]->Access(1, 2) = true;
}
C_brick_shape_list::~C_brick_shape_list() {
	unsigned Loop1;
	for (Loop1 = 0; Loop1 < Brick_shape_num; Loop1++) {
		delete Shape[Loop1];
	}
	delete[] Shape;
}
C_brick_shape C_brick_shape_list::Get_shape()const {
	//0~2 = 2x2
	//3~11 = 2x3
	//12~17 = 3x3
	if (Probability(3, 5)) {
		return (*this)[random(3)];
	}
	if (Probability(1, 2)) {
		return (*this)[random(9) + 3];
	}
	else {
		return (*this)[random(6) + 12];
	}
}
C_brick_shape C_brick_shape_list::operator[](unsigned Input)const {
	if (Input < Brick_shape_num) {
		return *Shape[Input];
	}
	else {
		ERROR_DEBUGSTOP(true);
		//throw "Not valid access";
		return C_brick_shape();
	}
}

const C_brick_shape_list Brick_shape_list;

//verge
