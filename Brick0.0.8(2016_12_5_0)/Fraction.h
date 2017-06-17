#ifndef FRACTION_HEAD
#define FRACTION_HEAD

class C_fraction{
protected:
	int Numerator;
	unsigned Denominator;

	void Reduce();

public:
	C_fraction(int Input = 0);
	C_fraction(int Numer, unsigned Denom);

	const C_fraction operator+(const C_fraction& Input) const;
	const C_fraction operator-(const C_fraction& Input) const;
	const C_fraction operator*(const C_fraction& Input) const;
	const C_fraction operator/(const C_fraction& Input) const;

	bool operator>(const C_fraction& Input) const;
	bool operator<(const C_fraction& Input) const;
	bool operator>=(const C_fraction& Input) const;
	bool operator<=(const C_fraction& Input) const;
	bool operator==(const C_fraction& Input) const;
	bool operator!=(const C_fraction& Input) const;

	int Get_numer() const;
	unsigned Get_denom() const;
	double Get_value() const;
	static unsigned Get_divisor(unsigned a, unsigned b);
};
#endif