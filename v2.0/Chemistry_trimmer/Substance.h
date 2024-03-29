#pragma once
#include "rat.h"
#include <QMap>
#include <QString>

class Substance
{
private:
	rat n = 0; //指定的个数
	QMap<short, int> elems; //元素(short)及个数(int)列表
	int cnum = 0; //电荷数
	QString html;//对外显示化学式
	QString formula;//物质化学式
	bool r;//是否反转

	int dispose(const QStringRef& content, QMap<short,int>& map_)const
	{
		QMap<short, int>map;
		for (int i = 0; i < content.size(); i++)
		{
			if (content.at(i) == '(' || content.at(i) == '[')
				i += dispose(content.mid(i + 1), map);
			else if (content.at(i) == ')' || content.at(i) == ']')
			{
				int j;
				for (j = i + 1; j < content.size(); j++)
					if (!content.at(j).isDigit())break;
				int amplifier = j == i + 1 ? 1 : content.mid(i + 1, j - 1 - i).toInt();
				for (const short& key : map.keys())
					map_[key] += map.value(key) * amplifier;
				return j;
			}
			else
			{
				if (!content.at(i).isUpper())throw QStringLiteral("元素必须以大写开头");
				short key = content.at(i).QChar::toLatin1() - 'A';
				if (i + 1 < content.size() && content.at(i + 1).isLower())
					key += (content.at(i + 1).QChar::toLatin1() - 'a' + 1) * 26;
				int j = i + 1 + (key >= 26);
				int k = j;
				for (; k < content.size(); k++)if (!content.at(k).isDigit())break;
				map[key] += k == j ? 1 : content.mid(j, k - j).toInt();
				i = k - 1;
			}
		}
		map_ = map;
		return 0;
	}

public:
	QVector<rat>nums;//个数

	Substance(const QStringRef& strref, bool reverse = false) :r(reverse)
	{
		//检查括号是否匹配 数字超过8位
		int x = 0, y = 0, cnt = 0;
		bool f = true;
		for (const QChar& c : strref)
		{
			if (c.isDigit())
			{
				cnt++;
				if (cnt > 8)throw QStringLiteral("超过8位数字");
				if (f && c.digitValue() == 0)throw QStringLiteral("系数不能为0");
				f = false;
			}
			else
			{
				f = true;
				cnt = 0;
				if (c == '[')x++;
				else if (c == ']')x--;
				else if (c == '(')y++;
				else if (c == ')')y--;
				if (x < 0 || y < 0)throw QStringLiteral("括号不匹配");
				if (y != 0 && (c == '[' || c == ']'))throw QStringLiteral("括号嵌套错误");
			}
		}
		if (x != 0 || y != 0)throw QStringLiteral("括号不匹配");
		//分离系数
		int i = 0;
		for (; i < strref.size() - 1; i++)if (!strref.at(i).isDigit())break;
		if (i != 0)
		{
			if (strref.at(i) == '/')
			{
				int j = i + 1;
				for (; j < strref.size(); j++)if (!strref.at(j).isDigit())break;
				n = rat(true, strref.left(i).toInt(), strref.mid(i + 1, j - i - 1).toInt());
				i = j;
			}
			else n = strref.left(i).toInt();
		}
		//分离电荷数
		int j = strref.indexOf('<'), k = strref.indexOf('>');
		if (j != k)
		{
			if (!(j < k - 1) || strref.at(k - 1) != '-' && strref.at(k - 1) != '+')throw QStringLiteral("电荷数格式错误");
			if (k - j == 2)cnum = 1;
			else cnum = strref.mid(j + 1, k - j - 2).toInt();
			if (strref.at(k - 1) == '-')cnum *= -1;
		}
		QStringRef ref = strref.mid(i, j == -1 ? -1 : j - i);
		if (ref.isEmpty())throw QStringLiteral("格式错误");
		formula = ref.toString();
		//分离出元素和对应个数
		if (ref != "e")
		{
			//分割元素
			if (ref.indexOf(QStringLiteral("·")) != -1)
			{
				if (ref.front() != '[' || ref.back() != ']')
					throw QStringLiteral("首尾应有[]");
				for (const QStringRef& str : ref.mid(1, ref.size() - 1).split(QStringLiteral("·")))
				{
					int i = 0;
					for (; i < str.size() - 1; i++)	if (!str.at(i).isDigit())break;
					int amplifier = i == 0 ? 1 : str.left(i).toInt();
					QMap<short, int>map;
					dispose(str.mid(i), map);
					for (const short& key : map.keys())
						elems[key] += map.value(key) * amplifier;
				}
			}
			else { dispose(ref, elems); }
		}
		else if (cnum == 0)throw QStringLiteral("无效的e");
		if (cnum != 0)elems.insert(-1, cnum);
		//将表达式html化
		html = formula;
		for (int i = 1; i < html.size(); i++)
		{
			if (html.at(i).isDigit() && html.at(i - 1) != '[' && html.at(i - 1) != QStringLiteral("·"))
			{
				html.insert(i, "<sub>");
				i += 5;
				for (; i < html.size(); i++)if (!html.at(i).isDigit())break;
				html.insert(i, "</sub>");
				i += 6;
			}
		}
		if (cnum != 0)
		{
			if (absv(cnum) != 1)
				html += "<sup>" + QString::number(absv(cnum)) + "<b>" + (cnum < 0 ? "-" : "+") + "</b></sup>";
			else
				html += "<sup><b>" + QString(cnum < 0 ? "-" : "+") + "</b></sup>";
		}
	}

	inline void operator=(const Substance& x)
	{
		n = x.n;
		cnum = x.cnum;
		elems = x.elems;
		html = x.html;
		formula = x.formula;
		nums = x.nums;
	}

	inline const QString print(const rat& r = 1)const//输出html化学式
	{
		if (r == 0)return "";
		if ((n == 0 ? r : r * n) == 1)return html;
		if ((n == 0 ? r : r * n) == -1)return "<span style=\"background-color: #3c9eef;\">-</span>" + html;
		return "<span style=\"background-color: #3c9eef;\">" + QString(n == 0 ? r : r * n) + "</span>" + html;
	}

	inline const rat& amount()const { return n; }
	inline bool hasReverse()const { return r; }
	~Substance() { }
	const QMap<short, int>& elements()const { return elems; }
};
