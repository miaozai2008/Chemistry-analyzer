#pragma once
#include "matrix.h"
#include "substance.h"
#include <map>
#include <list>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class equation
{
private:
	list<pair<substance, bool>>substances;//物质列表
	double t = -1;//开尔文温度
	ratmatrix mat;//储存系数结果矩阵 外层列 元素|电荷 内层行 未知数|系数
	QString condition;//反应条件

public:
	equation(const QString& str, const QString& cond) :condition(" --" + cond + "-- ")
	{
		//分割物质，调节系数
		QStringList list = str.split("--");
		if (list.length() != 2)throw wstring(L"连接符号错误");
		int pos = 0;
		QRegularExpression re("\\+(?!\\>)");
		try
		{
			for (const QString& m : list[0].split(re))
			{
				pos++;
				substances.push_back(pair<substance, bool>(substance(m.toStdWString()), false));
			}
			for (const QString& m : list[1].split(re))
			{
				pos++;
				substances.push_back(pair<substance, bool>(substance(m.toStdWString()), true));
			}
		}
		catch (const wchar_t* ptr)
		{
			throw L"物质" + to_wstring(pos) + L":" + wstring(ptr);
		}
	}

	void balance(bool int_ = true)//是否整数化
	{
		//统计元素及其个数
		map<short, rat>elems;//统计每种元素
		for (const pair<substance, bool>& pair_ : substances)
			for (const pair<const short, int>& p : pair_.first.elements)
				elems[p.first] += pair_.first.count * p.second * (pair_.second ? -1 : 1);
		//判断有无待定系数
		bool has = false;
		for (const pair<substance, bool>& p : substances)
		{
			if (p.first.count != 0)
			{
				has = true;
				break;
			}
		}
		//向矩阵中插入元素
		for (const pair<const short, rat>& pair_ : elems)
		{
			vector<rat> nums;
			for (const pair<substance, bool>& p : substances)
				if (p.first.count == 0)
				{
					const auto it = p.first.elements.find(pair_.first);
					if (it != p.first.elements.cend())
						nums.push_back(it->second * (p.second ? -1 : 1));
					else nums.push_back(0);
				}
			if (has)nums.push_back(pair_.second);
			mat.append(move(nums));
		}
		//求解赋值
		if (mat.sizev() == 1)throw wstring(L"所有系数已被决定");
		mat.simplify();
		if (!mat.solve())throw wstring(L"矩阵无解");
		//把待定系数项放回去
		if (has)
		{
			ratmatrix mat_;
			int i = 0;
			for (const pair<substance, bool>& p : substances)
			{
				if (p.first.count == 0)
				{
					mat_.append(mat[i]);
					i++;
					continue;
				}
				vector<rat>vec = mat.back();
				for (rat& r : vec)r *= p.first.count;
				mat_.append(move(vec));
			}
			swap(mat, mat_);
		}
		//整数化
		if (int_)
			for (int i = 0; i < mat.sizev(); i++)
			{
				int x = 0, y = 1;//分子，分母
				for (int j = 0; j < mat.sizeh(); j++)
				{
					x = _Gcd(mat[j][i].up, x);
					y = _Lcm(mat[j][i].down, y);
				}
				rat r(x, y);
				for (int j = 0; j < mat.sizeh(); j++)
					mat.in(j, i) /= r;
			}
	}
	
	void operator=(const equation& x)
	{
		substances = x.substances;
		t = x.t;
		mat = x.mat;
		condition = x.condition;
	}

	const QString print()const//输出配平结果
	{
		QStringList list_;
		for (int i = 0; i < mat.sizev(); i++)//遍历列
		{
			QStringList left, right;
			int j = -1;
			for (const pair<substance, bool>& p : substances)
			{
				j++;
				if (mat[j][i] == 0)continue;
				QString text = (mat[j][i] != 1 ? "<span style=\"background-color: #3c9eef;\">"
					+ QString(abs(mat[j][i])) + "</span>" : "") + QString::fromStdWString(p.first.html);
				if (mat[j][i] > 0 != p.second)left << text;
				else right << text;
			}
			list_ << left.join(" + ") + condition + right.join(" + ");
		}
		QStringList empty;
		int i = -1;
		for (const pair<substance, bool>& p : substances)
		{
			i++;
			if (mat.count(i, 0) == mat.sizev())	empty << QString::fromStdWString(p.first.html);
		}
		if (!empty.isEmpty())list_ << QStringLiteral("可能未参与反应物质:") + empty.join(" ");
		return list_.join("<br>");
	}

	~equation() { }
};