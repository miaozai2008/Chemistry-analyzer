#pragma once
#include "rat.h"
#include <map>
#include <string>
#include <regex>
#include <stack>

using namespace std;

class substance
{
private:
	wstring formula;//物质化学式
	using const_wit = wstring::const_iterator;
	
public:
	__readonly wstring html;//对外显示化学式
	__readonly rat count = 0;//用户定义系数
	__readonly map<short, int> elements; //元素及个数列表

	substance(const_wit it_begin, const_wit it_end)
	{
		wsmatch match;
		//分离状态
		wstring state;
		static const wregex reg1(L"\\(([a-z]+)\\)$");
		if (regex_search(it_begin, it_end, match, reg1))
		{
			state = match[1];
			it_end -= match.length();
		}
		//检查括号是否匹配 数字超过8位 小写前是否有大写
		auto check = [&](const const_wit& begin, const const_wit& end)
			{
				int x = 0, y = 0, cnt = 0;
				for (const_wit it = begin; it < end; it++)
					if (iswdigit(*it))
						if (*it == L'0' && cnt == 0)return false;
						else cnt++;
					else
					{
						if (*it == L'(')x++;
						else if (*it == L')')x--;
						else if (*it == L'[')
						{
							if (x > 0)return false;
							y++;
						}
						else if (*it == L']')
						{
							if (x > 0)return false;
							y--;
						}
						if (cnt > 8 || x < 0 || y < 0)return false;
						cnt = 0;
					}
				return x == 0 && y == 0 && cnt <= 8;
			};
		if (!check(it_begin, it_end))throw L"格式错误或过长数字";
		//分离系数
		static const wregex reg2(L"^(\\d+)/?(\\d+)?");
		if (regex_search(it_begin, it_end, match, reg2))
		{
			count = move(rat(stoi(match[1]), (match.length(2) ? stoi(match[2]) : 1)));
			it_begin += match.length();
		}
		//填写formula
		formula = move(wstring(it_begin, it_end) + (state.empty() ? L"" : L'_' + state));
		//分离电荷数
		int cnum = 0;
		static const wregex reg3(L"<(\\d*)(\\+|\\-)>$");
		if (regex_search(it_begin, it_end, match, reg3))
		{
			cnum = (match.length(1) ? stoi(match[1]) : 1) * (match[2] == L'-' ? -1 : 1);
			it_end -= match.length();
		}
		//分离出元素和对应个数 不支持·
		if (it_begin == it_end)throw L"物质不能为空";
		if (wstring_view(it_begin, it_end) != L"e")
		{
			stack<int>amps;
			int amp_ = 1;
			wstring num;
			for (const_wit it = it_end - 1;; it--)
			{
				if (iswdigit(*it))num.append(&*it);
				else if (*it == L'(' || *it == L'[')
				{
					amp_ /= amps.top();
					amps.pop();
				}
				else
				{
					const int cnt = num.empty() ? 1 : stoi(num);
					num.clear();
					amps.push(cnt);
					amp_ *= cnt;
					if (*it == L')' || *it == L']')continue;
					if (iswupper(*it))elements[*it - L'A'] += amp_;
					else if (iswlower(*it))
					{
						if (it == it_begin)throw L"无效元素";
						short key = (*it - L'a' + 1) * 26;
						it--;
						if (!iswupper(*it))throw L"无效元素";
						elements[*it - L'A' + key] += amp_;
					}
					else throw L"未知符号";
					amp_ /= cnt;
					amps.pop();
				}
				if (it == it_begin)break;
			}
		}
		else if (cnum == 0)throw L"无效的e";
		if (cnum != 0)elements.insert(pair<short, int>(-1, cnum));
		//将表达式html化
		html = move(wstring(it_begin, it_end));
		for (int i = 1; i < html.size(); i++)
		{
			if (iswdigit(html.at(i)))
			{
				html.insert(i, L"<sub>");
				i += 5;
				for (; i < html.size(); i++)if (!iswdigit(html.at(i)))break;
				html.insert(i, L"</sub>");
				i += 6;
			}
		}
		if (cnum != 0)
		{
			if (abs(cnum) == 1)html += move(L"<sup>" + wstring(cnum < 0 ? L"-" : L"+") + L"</sup>");
			else html += move(L"<sup>" + to_wstring(abs(cnum)) + (cnum < 0 ? L"-" : L"+") + L"</sup>");
		}
		if (!state.empty())html += move(L'(' + move(state) + L')');
	}
	
	inline bool has_state()const { return formula.find(L'_') != wstring::npos; }

	inline void operator=(const substance& x)
	{
		count = x.count;
		elements = x.elements;
		html = x.html;
		formula = x.formula;
	}

	~substance() { }
};