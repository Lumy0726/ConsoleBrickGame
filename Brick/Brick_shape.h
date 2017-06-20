#pragma once

class C_brick_shape {
protected:
	unsigned X_size, Y_size;
	bool **Shape;
public:
	C_brick_shape(unsigned _X_size = 1, unsigned _Y_size = 1);
	C_brick_shape(const C_brick_shape& Input);
	C_brick_shape& operator=(const C_brick_shape& Input);
	~C_brick_shape();
	bool& Access(unsigned X, unsigned Y);
	bool Read(unsigned X, unsigned Y)const;
	unsigned Get_X_size()const;
	unsigned Get_Y_size()const;
	C_brick_shape& Rotate(unsigned Num);
};

class C_brick_shape_list {
private:
	C_brick_shape_list(const C_brick_shape_list& Input);
	C_brick_shape_list& operator=(const C_brick_shape_list& Input);
protected:
	C_brick_shape** Shape;
public:
	const unsigned Brick_shape_num;
	C_brick_shape_list();
	~C_brick_shape_list();
	C_brick_shape Get_shape()const;
	C_brick_shape operator[](unsigned Input)const;
};

extern const C_brick_shape_list Brick_shape_list;

//verge
