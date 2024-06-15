#ifndef RAT
#define RAT
#include <string>
#include <QDebug>
#include <QString>

using namespace std;

class rat
{
private:
	constexpr void simplify()
	{
		assert(down);
		if (up == 0)
		{
			down = 1;
			return;
		}
		const int x = _Gcd(up, down);
		up /= x;
		down /= x;
		if (down < 0)
		{
			up = -up;
			down = -down;
		}
	}

public:
	__readonly int up;//分子
	__readonly int down;//分母

	constexpr rat(const int x, const int y) :up(x), down(y) { this->simplify(); }
	constexpr rat(const rat& x) : up(x.up), down(x.down) {  }
	constexpr rat(const int x = 0) : up(x), down(1) {  }
	constexpr ~rat() { }

	constexpr friend rat abs(const rat& r) { return r.up < 0 ? -r : r; }
	constexpr void operator=(const rat& x) { down = x.down; up = x.up; }
	constexpr rat operator-()const { return rat(-up, down); }
	constexpr double to_double()const { return double(up) / down; }
	constexpr friend string to_string(const rat& x) { return to_string(x.up) + (x.down == 1 ? "" : "/" + to_string(x.down)); }
	constexpr friend wstring to_wstring(const rat& x) { return to_wstring(x.up) + (x.down == 1 ? L"" : L"/" + to_wstring(x.down)); }

#ifdef QSTRING_H
	explicit inline operator QString()const { return QString::number(up) + (down == 1 ? "" : "/" + QString::number(down)); }
#ifdef QDEBUG_H
	inline friend QDebug operator<<(QDebug debug, const rat& x) { debug << QString(x); return debug; }
#endif
#endif

	constexpr bool operator==(const rat& x)const { return down == x.down && up == x.up; }
	constexpr bool operator!=(const rat& x)const { return down != x.down || up != x.up; }
	constexpr bool operator>(const rat& x)const { return up * x.down > x.up * down; }
	constexpr bool operator<(const rat& x)const { return up * x.down < x.up * down; }
	constexpr bool operator>=(const rat& x)const { return *this == x || *this > x; }
	constexpr bool operator<=(const rat& x)const { return *this == x || *this < x; }

	constexpr rat operator*(const rat& x)const { return rat(x.up * up, x.down * down); }
	constexpr void operator*=(const rat& x) { *this = move(rat(x.up * up, x.down * down)); }
	constexpr rat operator/(const rat& x)const { return rat(x.down * up, x.up * down); }
	constexpr void operator/=(const rat& x) { *this = move(rat(x.down * up, x.up * down)); }
	constexpr rat operator+(const rat& x)const { return rat(up * x.down + x.up * down, down * x.down); }
	constexpr void operator+=(const rat& x) { *this = move(rat(up * x.down + x.up * down, down * x.down)); }
	constexpr rat operator-(const rat& x)const { return rat(up * x.down - x.up * down, down * x.down); }
	constexpr void operator-=(const rat& x) { *this = move(rat(up * x.down - x.up * down, down * x.down)); }
};
#endif