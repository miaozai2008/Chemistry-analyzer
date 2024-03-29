#pragma once
#include "Matrix.h"
#include "Substance.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QVector>

class Equation
{
private:
	QList<Substance>substances;//物质列表
	double t = -1;//开尔文温度
	Matrix mat;//储存系数结果矩阵 外层列 元素|电荷 内层行 未知数|系数
	QString condition;//反应条件

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
		}
		catch (QString fatal) { throw QStringLiteral("物质") + QString::number(pos) + ":" + fatal; }
		condition = QStringLiteral(" —") + (t == -1 ? "" : QString::number(t) + "K") + QStringLiteral("→ ");
	}

	void balance(bool int_ = true)
	{
		//统计元素及其个数
		QMap<short, rat> elemsmap;//统计每种元素
		for (const Substance& s : substances)
			for (const short& key : s.elements().keys())
				elemsmap[key] += s.amount() * s.elements().value(key);
		//判断有无待定系数
		bool has = false;
		for (const Substance& s : substances)
		{
			if (s.amount() != 0)
			{
				has = true;
				break;
			}
		}
		//向矩阵中插入元素
		for (const short& key : elemsmap.keys())
		{
			QVector<rat> nums;
			for (const Substance& m : substances)
				if (m.amount() == 0)
					nums.append(m.elements().value(key));
			if (has)nums.append(elemsmap.value(key));
			mat.appendH(nums);
		}
		//求解赋值
		mat.simplify();
		if (!mat.solve())throw QStringLiteral("矩阵无解");
		mat *= -1;//临时补救
		if (!int_)return;
		for (int i = 0; i < mat.sizeV(); i++)
		{
			QVector<rat*>vec;
			for (int j = 0; j < mat.sizeH(); j++)
				vec.append(&mat.value(j, i));
			integer(vec);
		}
	}

	void operator=(const Equation& x)
	{
		substances = x.substances;
		t = x.t;
		mat = x.mat;
	}

	const QString print()const//输出配平结果
	{
		QStringList list;
		for (int i = 0; i < mat.sizeV(); i++)//遍历列
		{
			QStringList left, right;
			int j = 0;
			for (const Substance& sub : substances)
			{
				rat num = sub.amount() == 0 ? mat.at(j, i) : mat.lastV(i);
				if (num != 0)
				{
					if (num > 0)left << sub.print(num);
					else right << sub.print(-num);
				}
				if (sub.amount() == 0)j++;
			}
			list << (left.join(" + ") + condition + right.join(" + "));
		}
		QStringList empty;
		int j = 0;
		for (const Substance& sub : substances)
		{
			if ((sub.amount() == 0 ? mat.at(j) : mat.last()).count(0) == mat.sizeV())empty << sub.print();
			if (sub.amount() == 0)j++;
		}
		if (!empty.isEmpty())list << (QStringLiteral("可能未参与反应物质:") + empty.join(" "));
		return list.join("<br>");
	}

	~Equation() { }
};