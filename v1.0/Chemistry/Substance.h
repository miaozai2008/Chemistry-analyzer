#pragma once
#include "rat.h"
#include "NoMessyCode.h"
#include "Matrix.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QMap>
#include <QString>

class Substance
{
private:
	rat n = 0; //指定的个数
	QMap<QString, int> elements; //元素及个数列表
	int cnum = 0; //电荷数 charge number
	QString html;//对外显示化学式
	QString formula;//物质化学式
	double dh;//焓
	double ds;//熵
	bool r;//是否反转

	const QMap<QString, int> dispose(const QStringRef& content, int& length)const
	{
		QMap<QString, int>map;
		QRegularExpression re("^([A-Z][a-z]?)(\\d*)");
		QRegularExpressionMatch mat;
		for (int i = 0; i < content.size(); i++)
		{
			if (content[i] == '(' || content[i] == '[')
			{
				int len;
				QMap<QString, int>map_ = dispose(content.mid(i + 1), len);
				for (const QString& key : map_.keys())
					map[key] += map_[key];
				i += len;
			}
			else if (content[i] == ')' || content[i] == ']')
			{
				int j;
				for (j = i + 1; j < content.size(); j++)
					if (!content[j].isDigit())break;
				length = j;
				int amplifier = j == i + 1 ? 1 : content.mid(i + 1, j - 1 - i).toInt();
				if (amplifier == 0)throw QStringLiteral("系数不能为0");
				for (int& value : map)
					value *= amplifier;
				return map;
			}
			else
			{
				mat = re.match(content.mid(i));
				if (!mat.hasMatch())throw QStringLiteral("元素匹配错误");
				int count = mat.captured(2).isEmpty() ? 1 : mat.captured(2).toInt();
				if (count == 0)throw QStringLiteral("系数不能为0");
				map[mat.captured(1)] += count;
				i += mat.capturedLength(0) - 1;
			}
		}
		return map;
	}

public:
	QVector<rat>nums;//多解时个数

	Substance(const QStringRef& strref, bool reverse = false) :r(reverse)
	{
		//匹配格式
		QRegularExpression re("^(?<num>\\d*)?(?<content>[A-Za-z0-9·\\[\\]\\(\\)]+)+(?<cnum>\\<\\d*[+-]\\>)?$");
		QRegularExpressionMatch match = re.match(strref);
		if (!match.hasMatch()) throw QStringLiteral("物质格式错误");
		//填入个数，电荷数和进一步处理内容
		QStringRef ref = match.capturedRef("content");
		formula = match.captured("content");
		if (!match.capturedRef("num").isEmpty())
		{
			n = rat(match.capturedRef("num"));
			if (n == 0) throw QStringLiteral("物质系数不能为0");
		}
		if (!match.capturedRef("cnum").isEmpty())
		{
			if (match.capturedLength("cnum") == 3)cnum = 1;
			else cnum = match.capturedRef("cnum").mid(1, match.capturedLength("cnum") - 3).toInt();
			if (cnum == 0)throw QStringLiteral("电荷数格式错误");
			if (match.capturedRef("cnum")[match.capturedLength("cnum") - 2] == '-') { cnum *= -1; }
		}
		//分离出元素和对应个数
		if (ref != "e")
		{
			//检查括号是否匹配
			int x = 0, y = 0;
			for (const QChar& c : ref)
			{
				if (c == '[')x++;
				else if (c == ']')x--;
				else if (c == '(')y++;
				else if (c == ')')y--;
				if (x < 0 || y < 0)throw QStringLiteral("括号不匹配");
				if (y != 0 && (c == '[' || c == ']'))throw QStringLiteral("括号嵌套错误");
			}
			if (x != 0 || y != 0)throw QStringLiteral("括号不匹配");
			//分割元素
			QRegularExpressionMatch mat;
			QRegularExpression re("^(\\d*)");
			for (const QStringRef& str : (ref.indexOf("·") == -1 ? ref : ref.mid(1, ref.length() - 2)).split("·"))
			{
				mat = re.match(str);
				int len;
				QMap map = dispose(str.mid(mat.capturedLength(1)), len);
				int amplifier = mat.capturedRef(1).isEmpty() ? 1 : mat.capturedRef(1).toInt();
				if (amplifier == 0)throw QStringLiteral("系数不能为0");
				for (const QString& key : map.keys())
					elements[key] += map[key] * amplifier;
			}
		}
		//把电荷当成元素输入 反转
		if (cnum != 0)elements.insert("e", cnum);
		if (r)for (int& value : elements)value *= -1;
		//将表达式html化
		re.setPattern("[A-Z\\)\\]a-z](\\d+)");
		html = match.captured("content");
		while (true)
		{
			match = re.match(html);
			if (!match.hasMatch())break;
			html.replace(match.capturedStart(1), match.capturedLength(1), "<sub>" + match.capturedRef(1) + "</sub>");
		}
		if (cnum != 0)
		{
			if (cnum != -1 && cnum != 1)
				html += "<sup>" + QString::number(cnum < 0 ? cnum * -1 : cnum) + "<b>" + (cnum < 0 ? "-" : "+") + "</b></sup>";
			else
				html += "<sup><b>" + QString(cnum < 0 ? "-" : "+") + QStringLiteral("</b></sup>");
		}
	}

	//将系数应用到多解上
	inline void apply() { if (n != 0)for (rat& r : nums)r *= n; }
	inline const rat& num()const { return n; }
	inline void setnum(const rat& r) { n = r; }
	inline bool hasReverse()const { return r; }

	inline void operator=(const Substance& x)
	{
		n = x.n;
		cnum = x.cnum;
		elements = x.elements;
		html = x.html;
		formula = x.formula;
		dh = x.dh;
		ds = x.ds;
		nums = x.nums;
	}

	inline const QString print(const int& i = -1)const//输出html化学式
	{
		if (i == -1 || nums.at(i).abvalue() == 1 || nums.at(i) == 0)return html;
		return "<span style=\"background-color: #3c9eef;\">" + nums.at(i).abvalue().toQString() + "</span>" + html;
	}

	~Substance() { }
	const QMap<QString, int>& content()const { return elements; }
	const double dH()const { return dh; }
	const double dS()const { return ds; }

	void searchSH()
	{

	}
};