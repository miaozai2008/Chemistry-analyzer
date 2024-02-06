#pragma once
#include "Substance.h"
#include <QVector>

class Equation
{
private:
	QList<Substance>substances;//物质列表
	double t = -1;//开尔文温度
	double dh;//焓变
	double ds;//熵变
	double dg;//吉布斯自由能

public:
	Equation(const QString& str)
	{
		//处理反应条件
		QRegularExpression re("[—\\-](-?\\d+.?\\d*[KC])?[—\\-]");
		QRegularExpressionMatch match = re.match(str);
		if (!match.hasMatch()) throw QStringLiteral("连接符号错误");
		if (!match.capturedRef(1).isEmpty())
		{
			match = QRegularExpression("(-?)(\\d+.?\\d*)(K|C)").match(match.capturedRef(1));
			t = match.capturedRef(2).toDouble();
			if (!match.capturedRef(1).isEmpty())t *= -1;
			if (match.capturedRef(3) == "C")t += 273.15;
			if (t <= 0)throw QStringLiteral("开尔文温度不能小于0");
		}
		//分割物质，调节系数
		QStringList list = str.split(re);
		if (list.length() != 2)throw QStringLiteral("连接符号错误");
		unsigned pos = 0;
		re.setPattern("\\+(?!\\>)");
		try
		{
			for (const QStringRef& m : list[0].splitRef(re))
			{
				pos++;
				substances.append(Substance(m, false));
			}
			for (const QStringRef& m : list[1].splitRef(re))
			{
				pos++;
				substances.append(Substance(m, true));
			}
			bool set = false;
			for (const Substance& m : substances)
			{
				if (m.num() != 0)
				{
					set = true;
					break;
				}
			}
			if (!set)substances.first().setnum(1);
		}
		catch (QString fatal) { throw "物质" + QString::number(pos) + ":" + fatal; }		
	}

	void balance()
	{
		QList<QVector<rat>> matrix;//外层列 元素|电荷 内层行 未知数|系数
		//统计元素及其个数
		QMap<QString, rat> elemsmap;//统计每种元素
		for (const Substance& s : substances)
			for (const QString& key : s.content().keys())
				elemsmap[key] += s.num() * s.content().value(key);
		//向矩阵中插入元素
		for (const QString& key : elemsmap.keys())
		{
			QVector<rat> nums;
			for (const Substance& m : substances)
				if (m.num() == 0)
					nums.append(m.content().value(key));
			nums.append(elemsmap.value(key));
			matrix.append(nums);
		}
		Matrix m(matrix);
		m.simplify();
		m.slove();
		int j = 0;
		for (int i = 0; i < substances.size(); i++)
		{
			if (substances.at(i).num() == 0)
			{
				substances[i].nums = m.solution().at(j);
				j++;
			}
			else substances[i].nums = m.solution().last();
			substances[i].apply();
		}
		for (int i = 0; i < m.solution().first().size(); i++)
		{
			QVector<rat*>vec;
			for (Substance& m : substances)
				vec.append(&m.nums[i]);
			integer(vec);
		}
	}

	void operator=(const Equation& x)
	{
		substances = x.substances;
		t = x.t;
		dg = x.dg;
		dh = x.dh;
		ds = x.ds;
	}

	const QString print()const//输出配平结果
	{
		QStringList list;
		for (int i = 0; i < substances.first().nums.size(); i++)
		{
			QString left;
			QString right;
			for (const Substance& m : substances)
			{
				if (m.nums.at(i) == 0)continue;
				if (m.nums.at(i) > 0 != m.hasReverse())left += " + " + m.print(i);
				else right += " + " + m.print(i);
			}
			left.remove(0, 3);
			right.remove(0, 3);
			list << (left + " ==" + (t == -1 ? "" : QString::number(t) + "K") + "== " + right);
		}
		QString empty;
		for (const Substance& m : substances)
		{
			if (m.nums.count(0) == m.nums.size())
				empty += m.print() + " ";
		}
		if (!empty.isEmpty())list << "可能未参与反应物质:" + empty;
		return list.join("<br/>");
	}

	~Equation() { }

	void getData1()
	{

	}

	const QString printData1()
	{

	}
};