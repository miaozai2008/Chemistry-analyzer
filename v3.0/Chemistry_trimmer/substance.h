#pragma once
#include "rat.h"
#include <map>
#include <string>
#include <regex>
#include <stack>

class substance
{
private:
	wstring formula;//物质化学式

	using const_wit = wstring::const_iterator;
	
public:
	__readonly wstring html;//对外显示化学式
	__readonly rat count = 0;//用户定义系数
	__readonly map<short, int> elements; //元素及个数列表

	substance(const wstring& str)
	{	
		const_wit it_begin = str.cbegin(), it_end = str.cend();
		//检查括号是否匹配 数字超过8位
		auto check = [&](const wstring& str)
			{
				if (str.empty())return false;
				int x = 0, y = 0, cnt = 0;
				for (const wchar_t& c : str)
				{
					if (iswdigit(c))
					{
						cnt++;
						if (cnt > 8 || cnt == 1 && c == L'0')return false;
					}
					else
					{
						cnt = 0;
						if (c == L'[')x++;
						else if (c == L']')x--;
						else if (c == L'(')y++;
						else if (c == L')')y--;
						if (x < 0 || y < 0)return false;
						if (y != 0 && (c == L'[' || c == L']'))return false;
					}
				}
				if (x != 0 || y != 0)return false;
				return true;
			};
		if (!check(str))throw L"括号不匹配或无效数字";
		static wsmatch match;
		//分离系数
		static const wregex reg1(L"^(\\d+)/?(\\d+)?");
		if (regex_search(it_begin,it_end, match, reg1))
		{
			count = move(match.size() == 3 ? stoi(match[1]) : rat(stoi(match[1]), stoi(match[2])));
			it_begin += match.length();
		}
		//分离状态
		wstring state;
		static const wregex reg2(L"\\(([a-z]+?)\\)$");
		if (regex_search(it_begin, it_end, match, reg2))
		{
			state = match[1];
			it_end -= match.length();
		}
		//填写formula
		formula = move(wstring(it_begin, it_end) + (state.empty() ? L"" : L'_' + state));
		//分离电荷数
		int cnum = 0;
		static const wregex reg3(L"<(\\d*)(\\+|\\-)>$");
		if (regex_search(it_begin, it_end, match, reg3))
		{
			cnum = (match.length(1) == 0 ? 1 : stoi(match[1])) * (match[2] == L'-' ? -1 : 1);
			it_end -= match.length();
		}
		//分离出元素和对应个数 不支持·
		if (it_begin == it_end)throw L"物质不能为空";
		if (wstring_view(it_begin, it_end) != L"e")
		{
			stack<int>amps;
			int amp_ = 1;
			wstring num;
			for (const_wit it = it_end-1;;it--)
			{
				if (iswdigit(*it))num.append(&*it);
				else if (*it == L'(' || *it == L'[')
				{
					amp_ /= amps.top();
					amps.pop();
				}
				else
				{
					int cnt = num.empty() ? 1 : stoi(num);
					num.clear();
					amps.push(cnt);
					amp_ *= cnt;
					if (*it == L')' || *it == L']')continue;
					if (iswupper(*it))
					{
						elements[*it - L'A'] += amp_;
						amp_ /= amps.top();
						amps.pop();
					}
					else if (iswlower(*it))
					{
						short key = (*it - L'a' + 1) * 26;
						if (it == it_begin)throw L"不合法的元素";
						it--;
						if (!iswupper(*it))throw L"不合法的元素";
						elements[*it - L'A' + key] += amp_;
						amp_ /= amps.top();
						amps.pop();
					}
					else throw L"未知字符";
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