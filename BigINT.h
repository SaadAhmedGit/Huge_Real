#pragma once
#include <iostream>
#include <fstream>
class BigINT
{
public:
	BigINT();
	BigINT(const int _size);
	~BigINT();
	BigINT(const BigINT& src);
	BigINT(BigINT&& src) noexcept;
	void load(std::ifstream& file);

	//Arithmatic operators
	BigINT operator + (const BigINT& num2) const;
	BigINT operator - (const BigINT& num2) const;
	BigINT operator - () const;	//Negate
	BigINT operator * (const BigINT& num2) const;
	BigINT operator / (const BigINT& num2) const;
	BigINT operator % (const BigINT& num2) const;

	//Copy/Move assignment operators
	BigINT& operator = (const BigINT& num2);
	BigINT& operator = (BigINT&& num2) noexcept;

	//Arithmatic assignment operators
	const BigINT& operator += (const BigINT& num2);
	const BigINT& operator -= (const BigINT& num2);
	const BigINT& operator *= (const BigINT& num2);
	const BigINT& operator /= (const BigINT& num2);

	//Relational operators
	bool operator < (const BigINT& num2) const;
	bool operator <= (const BigINT& num2) const;
	bool operator > (const BigINT& num2) const;
	bool operator >= (const BigINT& num2) const;
	bool operator == (const BigINT& num2) const;
	bool operator != (const BigINT& num2) const;

protected:
	int* arr;
	int size;
	char sign;


	//Helper functions
	static BigINT unary();
	void removePrecedingZeros();
	BigINT AbsSub(const BigINT& num2) const;
	BigINT AbsAdd(const BigINT& num2) const;
	int numCmp(const BigINT& num2) const;
	int AbsNumCmp(const BigINT& num2) const;
	const int operator [] (const int i) const;

	//Friend overloads
	friend std::ostream& operator << (std::ostream& OUT, const BigINT& num);
};
std::ostream& operator << (std::ostream& OUT, const BigINT& num);