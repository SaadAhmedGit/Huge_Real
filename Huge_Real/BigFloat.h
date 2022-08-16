#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdio>

class BigFloat
{
private:
	bool sign;
	int BASE;
	std::vector<int> exp;
	std::vector<int> mant;
	friend int main();	//TODO: Remove this lmao

	//Utility functions
	void normalize();
	void deMantify();
	void mantify(const int nShifts);
	BigFloat absSum(const BigFloat& f2) const;
	BigFloat absSub(const BigFloat& f2) const;
	BigFloat absMult(const BigFloat& f2) const;
	BigFloat absDiv(const BigFloat& f2) const;
public:

	BigFloat(int _BASE = 10);
	BigFloat(const int expSize, const int mantSize, int _BASE = 10);
	BigFloat(const std::string& str, int _BASE = 10);
	BigFloat(const BigFloat& src);
	BigFloat(BigFloat&& src) noexcept;
	//BigFloat(const int src);
	//BigFloat(const double src);
	~BigFloat();

	void operator = (const BigFloat& f2);

	BigFloat operator + (const BigFloat& f2) const;
	BigFloat operator - (const BigFloat& f2) const;
	BigFloat operator - () const;
	BigFloat operator * (const BigFloat& f2) const;
	BigFloat operator / (const BigFloat& f2) const;

	//Comparisons
	bool operator == (const BigFloat& f2) const;
	bool operator != (const BigFloat& f2) const;
	bool operator > (const BigFloat& f2) const;
	bool operator < (const BigFloat& f2) const;

	//Helper comaprisons
	int AbsExpCmp(const std::vector<int>& v) const;
	int AbsMantCmp(const std::vector<int>& v) const;
	int AbsNumCmp(const BigFloat& f2) const;

	//Friend overloads
	friend std::ostream& operator << (std::ostream& os, const BigFloat& f);
};

