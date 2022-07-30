#include "BigINT.h"


BigINT::BigINT()
	:arr(nullptr), size(0), sign('+')
{
}
BigINT::BigINT(const int _size)
	:arr(new int[_size] {}), size(_size), sign('+')
{
}
BigINT::~BigINT()
{
	delete[] this->arr;
	arr = nullptr;
}
BigINT::BigINT(const BigINT& src)
	:size(src.size), sign(src.sign)
{
	arr = new int[this->size]{};
	for (int i = 0; i < this->size; i++)
	{
		arr[i] = src[i];
	}
}
BigINT::BigINT(BigINT&& src) noexcept
	:arr(src.arr), size(src.size), sign(src.sign)
{
	src.arr = nullptr;
}
void BigINT::load(std::ifstream& file)
{
	if (this->arr != nullptr)
		delete[] this->arr;
	std::string numStr;
	char ch = 0;
	file.get(ch);
	if (ch == '-' || ch == '+')
		this->sign = ch;
	else
		numStr += ch;
	while (ch != '\n' && !file.eof())
	{
		file.get(ch);
		if (ch != '\n' && !file.eof())
			numStr += ch;
	}
	this->size = numStr.length();
	this->arr = new int[size] {};
	for (int i = 0; i < size; i++)
	{
		(*this).arr[size - 1 - i] = int(numStr[i] - '0');
	}
}


BigINT BigINT::operator+(const BigINT& num2) const
{
	if (this->sign == num2.sign)
	{
		BigINT Sum = this->AbsAdd(num2);
		Sum.sign = this->sign;
		return std::move(Sum);
	}
	else
	{
		const int absCMP = this->AbsNumCmp(num2);
		const BigINT& larger = (absCMP == 1 ? *this : num2);
		const BigINT& smaller = (absCMP == 1 ? num2 : *this);
		BigINT Sum = larger.AbsSub(smaller);
		Sum.sign = larger.sign;
		return std::move(Sum);
	}
}
BigINT BigINT::operator-(const BigINT& num2) const
{
	return *this + (-num2);
}
BigINT BigINT::operator-() const
{
	BigINT Sub = *this;
	Sub.sign = (Sub.sign == '+' ? '-' : '+');
	return std::move(Sub);
}

BigINT BigINT::operator*(const BigINT& num2) const
{
	BigINT prod(this->size + num2.size);
	BigINT R, C;
	BigINT ZERO(1);
	BigINT limit = num2;
	while (limit.AbsNumCmp(ZERO) == 1)
	{
		R = *this;
		C = unary();
		while (limit.AbsNumCmp(C.AbsAdd(C)) == 1)
		{
			R = R.AbsAdd(R);
			C = C.AbsAdd(C);
		}
		R.removePrecedingZeros();
		prod = prod.AbsAdd(R);
		limit = limit.AbsSub(C);
	}
	prod.removePrecedingZeros();
	if (this->sign != num2.sign)
		prod.sign = '-';
	return std::move(prod);
}
BigINT BigINT::operator/(const BigINT& num2) const
{
	BigINT quotient(1);
	BigINT remainder = *this;
	BigINT C = unary();
	while (remainder.AbsNumCmp(num2) == 1)
	{
		BigINT weighted = C * num2;
		remainder = remainder.AbsSub(weighted);
		quotient = quotient.AbsAdd(C);
		if ((weighted.AbsAdd(weighted)).AbsNumCmp(remainder) == 1)
			C = unary();
		else
			C = C.AbsAdd(C);
	}
	quotient.removePrecedingZeros();
	if (this->sign != num2.sign)
		quotient.sign = '-';
	return std::move(quotient);
}
BigINT BigINT::operator%(const BigINT& num2) const
{
	BigINT remainder = *this - (*this / num2) * num2;
	remainder.removePrecedingZeros();
	return std::move(remainder);
}


//Copy/Move assignment operators
BigINT& BigINT::operator=(const BigINT& src)
{
	if (this->arr != nullptr)
		delete[] this->arr;
	this->size = src.size;
	this->sign = src.sign;
	this->arr = new int[this->size]{};
	for (int i = 0; i < this->size; i++)
	{
		this->arr[i] = src[i];
	}
	return *this;
}
BigINT& BigINT::operator=(BigINT&& num2) noexcept
{
	if (this->arr != nullptr)
		delete[] arr;
	this->arr = num2.arr;
	this->size = num2.size;
	this->sign = num2.sign;

	num2.arr = nullptr;

	return *this;
}

//Arithmatic assignment operators
const BigINT& BigINT::operator += (const BigINT& num2)
{
	*this = *this + num2;
	return *this;
}
const BigINT& BigINT::operator -= (const BigINT& num2)
{
	*this = *this - num2;
	return *this;
}
const BigINT& BigINT::operator*=(const BigINT& num2)
{
	*this = *this * num2;
	return *this;
}
const BigINT& BigINT::operator/=(const BigINT& num2)
{
	*this = *this / num2;
	return *this;
}


//Relational operators
bool BigINT::operator<(const BigINT& num2) const
{
	return this->numCmp(num2) == -1;
}
bool BigINT::operator<=(const BigINT& num2) const
{
	return this->numCmp(num2) <= 0;
}
bool BigINT::operator>(const BigINT& num2) const
{
	return this->numCmp(num2) == 1;
}
bool BigINT::operator>=(const BigINT& num2) const
{
	return this->numCmp(num2) >= 0;
}
bool BigINT::operator==(const BigINT& num2) const
{
	return this->numCmp(num2) == 0;
}
bool BigINT::operator!=(const BigINT& num2) const
{
	return this->numCmp(num2) != 0;
}




//Private
void BigINT::removePrecedingZeros()
{
	for (int i = this->size - 1; i > 0; i--)
	{
		if ((*this)[i] == 0)
			size--;
		else
			break;
	}
	//Shrink the array
	int* newArr = new int[size] {};
	for (int i = 0; i < size; i++)
	{
		newArr[i] = (*this)[i];
	}
	delete[] this->arr;
	this->arr = newArr;
}
int BigINT::numCmp(const BigINT& num2) const
{
	if (this->sign == '-' && num2.sign == '+')
		return -1;
	else if (this->sign == '+' && num2.sign == '-')
		return 1;
	else if (this->sign == '-' && num2.sign == '-')
		return num2.AbsNumCmp(*this);
	return this->AbsNumCmp(num2);

}
BigINT BigINT::unary()
{
	BigINT unary(1);
	unary.arr[0] = 1;
	return std::move(unary);
}
BigINT BigINT::AbsSub(const BigINT& num2) const
{
	BigINT diff(std::max(this->size, num2.size));
	int carried = 0;
	for (int i = 0; i < diff.size; i++)
	{
		int currDiff = (*this)[i] - num2[i];

		currDiff -= carried / 10;
		if (currDiff < 0)
		{
			carried = 10;
			currDiff += 10;
		}
		else
			carried = 0;
		diff.arr[i] = currDiff;
	}
	diff.removePrecedingZeros();
	return std::move(diff);
}
BigINT BigINT::AbsAdd(const BigINT& num2) const
{
	const int maxSize = std::max(this->size, num2.size) + 1;
	BigINT sumINT(maxSize);
	int carry = 0;
	for (int i = 0; i < maxSize; i++)
	{
		int currSum = (*this)[i] + num2[i];
		sumINT.arr[i] = carry + (currSum % 10);
		carry = currSum / 10;
	}
	sumINT.removePrecedingZeros();
	return std::move(sumINT);
}

int BigINT::AbsNumCmp(const BigINT& num2) const
{
	if (this->size > num2.size)
		return 1;
	else if (this->size < num2.size)
		return -1;
	else
	{
		for (int i = this->size - 1; i >= 0; i--)
		{
			if ((*this)[i] > num2[i])
				return 1;
			else if ((*this)[i] < num2[i])
				return -1;
		}
	}
	return 0;
}
const int BigINT::operator[](const int i) const
{
	if (i < 0 || i > size - 1)
		return 0;
	return this->arr[i];
}
std::ostream& operator<<(std::ostream& OUT, const BigINT& num)
{
	OUT << char(num.sign == '-' ? '-' : '\0');
	const int size = num.size;
	for (int i = 0; i < size; i++)
	{
		OUT << num[size - 1 - i];
	}
	return OUT;
}