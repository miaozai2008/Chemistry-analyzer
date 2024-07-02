#include "equation.h"
#include "substance.h"
#include <iostream>
using namespace std;

int main()
{
	wcout.imbue(locale("zh_CN", LC_CTYPE));//处理wcout的错误
	wcin.imbue(locale("zh_CN", LC_CTYPE));//处理wcin错误
	wstring formula, condition;
	while (true)
	{
		wcin >> formula;
		if (formula == L"[exit]")break;
		wcin >> condition;
		if (condition == L"<none>")condition.clear();
		try
		{
			equation eq(formula, condition);
			eq.balance();
			wcout << eq.print() << endl;
		}
		catch (const wstring& error)
		{
			wcout << L"[错误]" << error;
		}
	}
	return 0;
}