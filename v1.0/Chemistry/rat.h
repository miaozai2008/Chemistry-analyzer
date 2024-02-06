#pragma once
#include <QString>
#include <QDebug>
#include <QList>
#include "NoMessyCode.h"

const unsigned gcd(unsigned a, unsigned b) { return b == 0 ? a : gcd(b, a % b); }//最大公约数
inline const unsigned lcm(unsigned a, unsigned b) { return a / gcd(a, b) * b; }//最小公倍数

class rat
{
private:
	bool sign; //true 正 false 负
	unsigned nume; //分子
	unsigned deno; //分母

	inline void simplify()
	{
		if (deno == 0)throw QString("产生无效数字");
		if (nume == 0)
		{
			deno = 1;
			sign = true;
			return;
		}
		unsigned x = gcd(nume, deno);
		nume /= x;
		deno /= x;
	}

	inline int ab(const int& x)const { return x < 0 ? x * -1 : x; }
	inline rat(const bool _sign, unsigned _nume, unsigned _deno) :sign(_sign), nume(_nume), deno(_deno) { this->simplify(); }

public:
	inline rat(const int num = 0) :deno(1), sign(num >= 0), nume(sign ? num : num * -1) {	}
	inline ~rat() { }

	inline explicit rat(const QString& str) :deno(1)
	{
		if (str.size() - (str.indexOf("-") ? 1 : 0) > 8)throw QString("数字超出范围");
		int num = str.toInt();
		sign = num >= 0;
		nume = sign ? num : num * -1;
	}

	inline explicit rat(const QStringRef& ref) :deno(1)
	{
		if (ref.size() - (ref.indexOf("-") ? 1 : 0) > 8)throw QString("数字超出范围");
		int num = ref.toInt();
		sign = num >= 0;
		nume = sign ? num : num * -1;
	}

	inline void operator= (const rat& x)
	{
		sign = x.sign;
		deno = x.deno;
		nume = x.nume;
	}

	inline const double toDouble()const { return (sign ? 1 : -1) * double(nume) / deno; }
	inline bool operator== (const rat& x) const { return sign == x.sign && deno == x.deno && nume == x.nume; }
	inline bool operator==(const int& x)const { return this->toDouble() == x; }
	inline bool operator!=(const rat& x) const { return sign != x.sign || deno != x.deno || nume != x.nume; }
	inline bool operator!=(const int& x)const { return this->toDouble() != x; }
	inline bool operator>(const rat& x) const { return this->toDouble() > x.toDouble(); }
	inline bool operator>=(const rat& x)const { return *this > x || *this == x; }
	inline bool operator<(const rat& x)const { return this->toDouble() < x.toDouble(); }
	inline bool operator<=(const rat& x)const { return *this < x || *this == x; }
	inline const rat operator*(const rat& x)const { return rat(sign == x.sign, x.nume * nume, x.deno * deno); }
	inline const rat operator*(const int& x)const { return rat(sign == x > 0, nume * ab(x), deno); }
	inline void operator*=(const rat& x) { *this = rat(sign == x.sign, x.nume * nume, x.deno * deno); }
	inline void operator*=(const int& x) { *this = rat(sign == x > 0, nume * ab(x), deno); }
	inline const rat operator/(const rat& x)const { return rat(sign == x.sign, x.deno * nume, x.nume * deno); }
	inline const rat operator/(const int& x)const { return rat(sign == x > 0, nume, deno * ab(x)); }
	inline void operator/=(const rat& x) { *this = rat(sign == x.sign, x.deno * nume, x.nume * deno); }
	inline void operator/=(const int& x) { *this = rat(sign == x > 0, nume, deno * ab(x)); }

	const rat operator+ (const rat& x) const
	{
		unsigned _lcm = lcm(deno, x.deno);
		int a = int(nume) * (sign ? 1 : -1) * (_lcm / deno);
		int b = int(x.nume) * (x.sign ? 1 : -1) * (_lcm / x.deno);
		return rat(a + b > 0, (a + b > 0 ? 1 : -1) * (a + b), _lcm);
	}

	void operator+= (const rat& x)
	{
		unsigned _lcm = lcm(deno, x.deno);
		int a = int(nume) * (sign ? 1 : -1) * (_lcm / deno);
		int b = int(x.nume) * (x.sign ? 1 : -1) * (_lcm / x.deno);
		*this = rat(a + b > 0, (a + b > 0 ? 1 : -1) * (a + b), _lcm);
	}

	const rat operator- (const rat& x) const
	{
		unsigned _lcm = lcm(deno, x.deno);
		int a = int(nume) * (sign ? 1 : -1) * (_lcm / deno);
		int b = int(x.nume) * (x.sign ? -1 : 1) * (_lcm / x.deno);
		return rat(a + b > 0, (a + b > 0 ? 1 : -1) * (a + b), _lcm);
	}

	void operator-= (const rat& x)
	{
		unsigned _lcm = lcm(deno, x.deno);
		int a = int(nume) * (sign ? 1 : -1) * (_lcm / deno);
		int b = int(x.nume) * (x.sign ? -1 : 1) * (_lcm / x.deno);
		*this = rat(a + b > 0, (a + b > 0 ? 1 : -1) * (a + b), _lcm);
	}

#ifdef _DEBUG
	inline friend QDebug operator<<(QDebug debug, const rat& x)
	{
		debug << x.toQString();
		return debug;
	}
#endif

	inline const QString toQString()const { return (sign ? "" : "-") + QString::number(nume) + (deno == 1 ? "" : "/" + QString::number(deno)); }
	inline const rat abvalue()const { return rat(true, this->nume, this->deno); }//绝对值

	friend void integer(QVector<rat*>& vector)
	{
		unsigned x = 1;
		for (const rat* num : vector)
			x = lcm(x, num->deno);
		for (rat* num : vector)
		{
			num->nume *= x / num->deno;
			num->deno = 1;
		}
		x = vector[0]->abvalue().toDouble();
		for (const rat* num : vector)
			x = gcd(x, num->nume);
		for (rat* num : vector)
			num->nume = num->nume / x;
	}
};