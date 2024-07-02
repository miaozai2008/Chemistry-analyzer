#pragma once
#include "matrix.h"
#include "substance.h"
#include <map>
#include <list>
#include <regex>

using namespace std;

class equation
{
private:
	list<pair<substance, bool>>substances;//物质列表
	double t = -1;//开尔文温度
	ratmatrix mat;//储存系数结果矩阵 外层列 元素|电荷 内层行 未知数|系数
	wstring condition;//反应条件

public:
	equation(const wstring& str, const wstring& cond) :condition(L" --" + cond + L"-- ")
	{
		//按连接符号分割
		size_t p = str.find(L"--");
		if (p == wstring::npos || p != str.rfind(L"--"))
			throw wstring(L"连接符号错误");
		wstring left = move(str.substr(0, p)), right = move(str.substr(p + 2));
		if (left.empty() || right.empty())throw wstring(L"化学式不能为空");
		//分为左右，正则进一步分割
		static const wregex split(L"\\+(?!\\>)");
		wsregex_token_iterator end;
		unsigned pos = 0;
		try
		{
			for (wsregex_token_iterator it(left.cbegin(), left.cend(), split, -1); it != end; it++)
			{	
				pos++;
				substances.push_back(pair<substance, bool>(move(substance(it->first, it->second)), false));
			}
			for (wsregex_token_iterator it(right.cbegin(), right.cend(), split, -1); it != end; it++)
			{
				pos++;
				substances.push_back(pair<substance, bool>(move(substance(it->first, it->second)), true));
			}
		}
		catch (const wchar_t* pt)
		{
			throw L"物质" + to_wstring(pos) + L':' + wstring(pt);
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
					x = int(_Gcd(mat[j][i].up, x));
					y = int(_Lcm(mat[j][i].down, y));
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

	const wstring print()const//输出配平结果
	{
		auto join = [&](list<wstring>& l, const wchar_t* w)
			{
				if (l.size() == 1)return move(l.front());
				int i = 0;
				wstring result;
				for (wstring& str : l)
				{
					result.append(move(str));
					result.append(w);
					i++;
					if (i == l.size() - 1)break;
				}
				result.append(move(l.back()));
				return result;
			};
		list<wstring> list_;
		for (int i = 0; i < mat.sizev(); i++)//遍历列
		{
			list<wstring> left, right;
			int j = -1;
			for (const pair<substance, bool>& p : substances)
			{
				j++;
				if (mat[j][i] == 0)continue;
				wstring text = (abs(mat[j][i]) != 1 ? L"<span style=\"background-color: #3c9eef;\">"
					+ (abs(mat[j][i])).to_wstring() + L"</span>" : L"") + p.first.html;
				if (mat[j][i] > 0 != p.second)left.push_back(move(text));
				else right.push_back(move(text));
			}
			list_.push_back(move(join(left, L" + ") + condition + join(right, L" + ")));
		}
		list<wstring> empty;
		int i = -1;
		for (const pair<substance, bool>& p : substances)
		{
			i++;
			if (mat.count(i, 0) == mat.sizev())	empty.push_back(p.first.html);
		}
		if (!empty.empty())list_.push_back(move(L"可能未参与物质:" + join(empty, L" ")));
		return join(list_, L"<br>");
	}

	~equation() { }
};