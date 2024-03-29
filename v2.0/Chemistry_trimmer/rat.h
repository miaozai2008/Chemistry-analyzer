#pragma once
#include <QString>
#include <QDebug>
#include <QList>

const unsigned gcd(unsigned a, unsigned b) { return b == 0 ? a : gcd(b, a % b); }//最大公约数
inline const unsigned lcm(unsigned a, unsigned b) { return a / gcd(a, b) * b; }//最小公倍数
inline const int absv(const int& x) { return x < 0 ? -x : x; }//取绝对值

class rat
{
private:
	bool si; //true 正 false 负
	unsigned nu; //分子
	unsigned de; //分母

	inline void simplify()
	{
		if (de == 0)throw QStringLiteral("产生无效数字");
		if (nu == 0)
		{
			de = 1;
			si = true;
			return;
		}
		unsigned x = gcd(nu, de);
		nu /= x;
		de /= x;
	}

public:
	inline rat(const bool _sign, unsigned _nume, unsigned _deno) :si(_sign), nu(_nume), de(_deno) { this->simplify(); }
	inline rat(const int num = 0) :de(1), si(num >= 0), nu(absv(num)) {	}
	inline ~rat() { }
	inline unsigned nume()const { return nu; }
	inline unsigned deno()const { return de; }
	inline bool isNegative()const { return si; }

	inline void operator= (const rat& x) { si = x.si; de = x.de; nu = x.nu; }
	inline void operator=(const int& x) { de = 1; si = x >= 0; nu = absv(x); }
	inline const rat operator-()const { return rat(!si, nu, de); }
	inline const rat abs()const { return rat(true, nu, de); }
	inline operator double()const { return (si ? 1 : -1) * double(nu) / de; }
	explicit inline operator QString()const { return (si ? "" : "-") + QString::number(nu) + (de == 1 ? "" : "/" + QString::number(de)); }
	inline bool operator==(const rat& x)const { return si == x.si && de == x.de && nu == x.nu; }
	inline bool operator==(const int& x)const { return de == 1 && (si ? nu : -nu) == x; }
	inline bool operator!=(const rat& x)const { return si != x.si || de != x.de || nu != x.nu; }
	inline bool operator!=(const int& x)const { return de != 1 || (si ? nu : -nu) != x; }
	inline const rat operator*(const rat& x)const { return rat(si == x.si, x.nu * nu, x.de * de); }
	inline const rat operator*(const int& x)const { return rat(si == x > 0, nu * absv(x), de); }
	inline void operator*=(const rat& x) { *this = rat(si == x.si, x.nu * nu, x.de * de); }
	inline void operator*=(const int& x) { *this = rat(si == x > 0, nu * absv(x), de); }
	inline const rat operator/(const rat& x)const { return rat(si == x.si, x.de * nu, x.nu * de); }
	inline const rat operator/(const int& x)const { return rat(si == x > 0, nu, de * absv(x)); }
	inline void operator/=(const rat& x) { *this = rat(si == x.si, x.de * nu, x.nu * de); }
	inline void operator/=(const int& x) { *this = rat(si == x > 0, nu, de * absv(x)); }
	inline friend QDebug operator<<(QDebug debug, const rat& x) { debug << QString(x); return debug; }

	const rat operator+(const rat& x) const
	{
		unsigned _lcm = lcm(de, x.de);
		int a = int(nu) * (si ? 1 : -1) * (_lcm / de);
		int b = int(x.nu) * (x.si ? 1 : -1) * (_lcm / x.de);
		return rat(a + b > 0, (a + b > 0 ? 1 : -1) * (a + b), _lcm);
	}

	void operator+=(const rat& x)
	{
		unsigned _lcm = lcm(de, x.de);
		int a = int(nu) * (si ? 1 : -1) * (_lcm / de);
		int b = int(x.nu) * (x.si ? 1 : -1) * (_lcm / x.de);
		*this = rat(a + b > 0, (a + b > 0 ? 1 : -1) * (a + b), _lcm);
	}

	const rat operator-(const rat& x) const
	{
		unsigned _lcm = lcm(de, x.de);
		int a = int(nu) * (si ? 1 : -1) * (_lcm / de);
		int b = int(x.nu) * (x.si ? -1 : 1) * (_lcm / x.de);
		return rat(a + b > 0, (a + b > 0 ? 1 : -1) * (a + b), _lcm);
	}

	void operator-=(const rat& x)
	{
		unsigned _lcm = lcm(de, x.de);
		int a = int(nu) * (si ? 1 : -1) * (_lcm / de);
		int b = int(x.nu) * (x.si ? -1 : 1) * (_lcm / x.de);
		*this = rat(a + b > 0, (a + b > 0 ? 1 : -1) * (a + b), _lcm);
	}

	inline friend void integer(QVector<rat*>& vector)
	{
		unsigned x = 1;
		for (const rat* num : vector)
			x = lcm(x, num->de);
		for (rat* num : vector)
		{
			num->nu *= x / num->de;
			num->de = 1;
		}
		x = 1;
		for (const rat* num : vector)
			x = gcd(x, num->nu);
		for (rat* num : vector)
			num->nu = num->nu / x;
	}
};