#include "BigFloat.h"
#include <math.h>

int vectorAt(const std::vector<int>& v, const int i);
void BigFloat::deMantify()
{
	const int mantSize = this->mant.size();
	for (int i = 0; i < mantSize; i++)
	{
		int item = *(this->mant.begin());
		this->exp.push_back(item);
		this->mant.erase(this->mant.begin());
	}
}
void BigFloat::mantify(const int nShifts)
{
	int expSize = this->exp.size();
	if (nShifts > expSize) { throw std::exception("Cannot shift by more than the number of digits in the exponent"); }
	for (int i = 1; i <= nShifts; i++)
	{
		int item = *(this->exp.end() - 1);
		this->mant.insert(this->mant.begin(), item);
		this->exp.pop_back();
	}
}

void BigFloat::normalize()
{
	int iExp = 0;
	int iMant = this->mant.size() - 1;

	while (iExp < this->exp.size() - 1 && this->exp[iExp] <= 0)
		iExp++;
	while (iMant > 0 && this->mant[iMant] <= 0)
		iMant--;
	for (int i = 0; i < iExp; i++)
	{
		this->exp.erase(exp.begin());
	}
	if (iMant != this->mant.size() - 1)
		this->mant.resize(iMant + 1);
}

BigFloat BigFloat::absSum(const BigFloat& f2) const
{
	BigFloat SUM(*this);
	const int maxExpSize = std::max(this->exp.size(), f2.exp.size()) + 1;
	SUM.exp.resize(maxExpSize);

	int carry = 0;
	for (int i = 0; i < maxExpSize; i++)
	{
		int currSum = vectorAt(this->exp, this->exp.size() - 1 - i) + vectorAt(f2.exp, f2.exp.size() - 1 - i);
		SUM.exp[maxExpSize - 1 - i] = (carry + (currSum % BASE));
		carry = currSum / BASE;
	}

	int maxMantSize = std::max(this->mant.size(), f2.mant.size());
	SUM.mant.resize(maxMantSize);
	carry = 0;
	for (int i = 0; i < maxMantSize; i++)
	{
		int currSum = vectorAt(this->mant, maxMantSize - 1 - i) + vectorAt(f2.mant, maxMantSize - 1 - i);
		SUM.mant[maxMantSize - 1 - i] = (carry + (currSum % BASE));
		carry = currSum / BASE;
	}
	BigFloat carryBigFloat(std::to_string(carry), BASE);
	SUM.normalize();
	if (carry > 0)
		SUM = SUM.absSum(carryBigFloat);

	return SUM;
}

BigFloat BigFloat::absSub(const BigFloat& f2) const
{
	int largerMantSize = std::max(this->mant.size(), f2.mant.size());
	int largerExpSize = std::max(this->exp.size(), f2.exp.size());
	BigFloat diff(largerExpSize, largerMantSize, BASE);
	int carried = 0;
	for (int i = 0; i < diff.exp.size(); i++)
	{
		int currDiff = vectorAt(this->exp, this->exp.size() - 1 - i) - vectorAt(f2.exp, f2.exp.size() - 1 - i);

		currDiff -= carried / BASE;
		if (currDiff < 0)
		{
			carried = BASE;
			currDiff += BASE;
		}
		else
			carried = 0;
		(diff.exp)[diff.exp.size() - 1 - i] = currDiff;
	}

	carried = 0;
	for (int i = 0; i < largerMantSize; i++)
	{
		int currDiff = vectorAt(this->mant, largerMantSize - 1 - i) - vectorAt(f2.mant, largerMantSize - 1 - i);
		currDiff -= carried / BASE;
		if (currDiff < 0)
		{
			carried = BASE;
			currDiff += BASE;
		}
		else if (i != largerMantSize - 1)
			carried = 0;
		diff.mant[largerMantSize - 1 - i] = currDiff;
		if (i == largerMantSize - 1)
		{
			int currDiff = vectorAt(diff.exp, diff.exp.size() - 1) - carried / BASE;
			if (currDiff < 0)
			{
				carried = BASE;
				currDiff += BASE;
			}
			else
				carried = 0;
			(diff.exp)[diff.exp.size() - 1] = currDiff;
		}
	}

	diff.normalize();
	return std::move(diff);
}

BigFloat BigFloat::absMult(const BigFloat& f2) const
{
	BigFloat f1(*this);
	int mSize = f1.mant.size() + f2.mant.size();
	f1.deMantify();
	BigFloat cpyF2 = f2;
	cpyF2.deMantify();
	BigFloat prod(BASE);
	const BigFloat ZERO("0", BASE);
	const BigFloat UNARY("1", BASE);
	BigFloat R(BASE), C(BASE);
	BigFloat limit = cpyF2;
	while (limit.AbsExpCmp(ZERO.exp) == 1)
	{
		R = f1;
		C = UNARY;
		while (limit.AbsExpCmp((C.absSum(C)).exp) != -1)
		{
			R = R.absSum(R);
			C = C.absSum(C);
		}
		R.normalize();
		prod = prod.absSum(R);
		limit = limit.absSub(C);
	}
	f1 = prod;
	f1.mantify(mSize);
	return f1;
}
BigFloat BigFloat::absDiv(const BigFloat& f2) const
{
	BigFloat quotient("0", BASE);
	BigFloat f1(*this); f1.deMantify();
	BigFloat cpyF2(f2); cpyF2.deMantify();

	BigFloat remainder = f1;
	BigFloat C("1", BASE);
	while (remainder.AbsExpCmp(cpyF2.exp) == 1)
	{
		BigFloat weighted = C.absMult(cpyF2);
		remainder = remainder.absSub(weighted);
		quotient = quotient.absSum(C);
		if ((weighted.absSum(weighted)).AbsExpCmp(remainder.exp) == 1)
			C = BigFloat("1", BASE);
		else
			C = C.absSum(C);
	}
	quotient.normalize();
	quotient.mantify(abs(int(this->mant.size() - f2.mant.size())));
	return std::move(quotient);
}

BigFloat::BigFloat(int _BASE)
	:sign(0), exp(0), mant(0), BASE(_BASE)
{
}
BigFloat::BigFloat(const int expSize, const int mantSize, int _BASE)
	:sign(0), exp(0), mant(0), BASE(_BASE)
{
	this->exp.resize(expSize);
	this->mant.resize(mantSize);
}
BigFloat::BigFloat(const std::string& str, int _BASE)
	:BigFloat(_BASE)
{
	int i = 0;
	if (str[0] == '-')
	{
		this->sign = 1; i++;
	}
	const int strSize = str.size();
	while (str[i] != '.' && i < strSize)
	{
		if (str[i] >= 'A')
			exp.push_back(str[i] - 55);
		else
			exp.push_back(str[i] - '0');
		i++;
	}
	i++;
	while (i < strSize)
	{
		if (str[i] >= 'A')
			mant.push_back(str[i] - 55);
		else
			mant.push_back(str[i] - '0');
		i++;
	}
}

BigFloat::BigFloat(const BigFloat& src)
	:sign(src.sign), exp(src.exp), mant(src.mant), BASE(src.BASE)
{
}

BigFloat::BigFloat(BigFloat&& src) noexcept
	:sign(src.sign), exp(std::move(src.exp)), mant(std::move(src.mant)), BASE(src.BASE)
{
}

BigFloat::~BigFloat()
{
}

BigFloat BigFloat::operator+(const BigFloat& f2) const
{
	BigFloat SUM(BASE);
	if (this->sign == f2.sign)
	{
		SUM = this->absSum(f2);
		SUM.sign = this->sign;
	}
	else
	{
		if (this->AbsNumCmp(f2) == 1)
		{
			SUM = this->absSub(f2);
			SUM.sign = this->sign;
		}
		else
		{
			SUM = f2.absSub(*this);
			SUM.sign = f2.sign;
		}
	}
	return SUM;
}

void BigFloat::operator = (const BigFloat& f2)
{
	this->sign = f2.sign;
	this->exp = f2.exp;
	this->mant = f2.mant;
	this->BASE = f2.BASE;
}

BigFloat BigFloat::operator-(const BigFloat& f2) const
{
	return *this + (-f2);
}
BigFloat BigFloat::operator-() const
{
	BigFloat negThis = *this;
	negThis.sign = !negThis.sign;
	return negThis;
}

BigFloat BigFloat::operator*(const BigFloat& f2) const
{
	BigFloat f1(*this);
	int mSize = f1.mant.size() + f2.mant.size();
	f1.deMantify();
	BigFloat cpyF2 = f2;
	cpyF2.deMantify();
	BigFloat prod(BASE);
	const BigFloat ZERO("0", BASE);
	const BigFloat UNARY("1", BASE);
	BigFloat R(BASE), C(BASE);
	BigFloat limit = cpyF2;
	while (limit.AbsExpCmp(ZERO.exp) == 1)
	{
		R = f1;
		C = UNARY;
		while (limit.AbsExpCmp((C.absSum(C)).exp) != -1)
		{
			R = R.absSum(R);
			C = C.absSum(C);
		}
		R.normalize();
		prod = prod.absSum(R);
		limit = limit.absSub(C);
	}
	f1 = prod;
	f1.mantify(mSize);
	if (this->sign != f2.sign)
	{
		f1.sign = 1;
	}
	return f1;
}

BigFloat BigFloat::operator/(const BigFloat& f2) const
{
	BigFloat quotient("0", BASE);
	BigFloat f1(*this); f1.deMantify();
	BigFloat cpyF2(f2); cpyF2.deMantify();

	BigFloat remainder = f1;
	BigFloat C("1", BASE);
	while (remainder.AbsExpCmp(cpyF2.exp) == 1)
	{
		BigFloat weighted = C.absMult(cpyF2);
		remainder = remainder.absSub(weighted);
		quotient = quotient.absSum(C);
		if ((weighted.absSum(weighted)).AbsExpCmp(remainder.exp) == 1)
			C = BigFloat("1", BASE);
		else
			C = C.absSum(C);
	}
	quotient.normalize();
	quotient.mantify(abs(int(this->mant.size() - f2.mant.size())));
	if (this->sign != f2.sign)
	{
		quotient.sign = 1;
	}
	return std::move(quotient);
}

std::ostream& operator<<(std::ostream& os, const BigFloat& f)
{
	os << (f.sign == 1 ? '-' : char(0));
	for (int i : f.exp)
	{
		if (i >= 10)
		{
			os << char(i + 55);
		}
		else
		{
			os << i;
		}
	}
	os << '.';
	if (f.mant.size() == 0)
		os << 0;
	else
		for (int i : f.mant)
			os << i;
	return os;
}
bool BigFloat::operator > (const BigFloat& f2) const
{
	if (this->sign != f2.sign)
		return this->sign < f2.sign;
	for (int i = 0; i < this->exp.size(); i++)
	{
		if (this->exp[i] > f2.exp[i])
			return true;
	}
	for (int i = 0; i < this->mant.size(); i++)
	{
		if (this->mant[i] > f2.mant[i])
			return true;
	}
	return false;
}
bool BigFloat::operator < (const BigFloat& f2) const
{
	return !(*this > f2) && (*this != f2);
}
bool BigFloat::operator != (const BigFloat& f2) const
{
	return !(*this == f2);
}
int BigFloat::AbsExpCmp(const std::vector<int>& v) const
{
	if (this->exp.size() > v.size())
		return 1;
	else if (this->exp.size() < v.size())
		return -1;
	else
	{
		for (int i = 0; i < this->exp.size(); i++)
		{
			if (this->exp[i] > v[i])
				return 1;
			else if (this->exp[i] < v[i])
				return -1;
		}
		return 0;
	}
}
int BigFloat::AbsMantCmp(const std::vector<int>& v) const
{

	for (int i = 0; i < this->mant.size(); i++)
	{
		if (this->mant[i] > v[i])
			return 1;
		else if (this->mant[i] < v[i])
			return -1;
	}
	return 0;
}
int BigFloat::AbsNumCmp(const BigFloat& f2) const
{
	int expCmp = this->AbsExpCmp(f2.exp);
	if (expCmp != 0)
		return expCmp;
	return this->AbsMantCmp(f2.mant);
}

bool BigFloat::operator == (const BigFloat& f2) const
{
	return this->sign == f2.sign && this->exp == f2.exp && this->mant == f2.mant;
}

int vectorAt(const std::vector<int>& v, const int i)
{
	if (i >= v.size() || i < 0)
		return 0;
	return v[i];
}
