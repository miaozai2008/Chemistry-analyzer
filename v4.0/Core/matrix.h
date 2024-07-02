#pragma once
#include "rat.h"
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

template<typename _Elem>
class basic_matrix
{
private:
	vector<vector<_Elem>>mat;

public:
	//未说明默认行操作

	basic_matrix() {  }
	~basic_matrix() {  }
	constexpr size_t sizeh()const{ return mat.size(); }//行数
	constexpr size_t sizev()const{ return mat.empty() ? 0 : mat.front().size(); }//列数
	constexpr bool empty()const{ return mat.empty(); }
	constexpr const vector<_Elem>& operator[](const size_t& x)const { return mat[x]; }
	constexpr rat& in(const size_t& x, const size_t& y) { return mat[x][y]; }
	constexpr const vector<_Elem>& front()const { return mat.front(); }
	constexpr const vector<_Elem>& back()const { return mat.back(); }
	constexpr void operator =(const basic_matrix<_Elem>& x) { mat = x.mat; }
	constexpr bool operator !=(const basic_matrix<_Elem>& x)const{ return x.mat != mat; }
	constexpr bool operator ==(const basic_matrix<_Elem>& x)const{ return x.mat == mat; }

	inline int count(const int& l,const _Elem&t_)const
	{
		int cnt = 0;
		for (const _Elem& t : mat[l])
			if (t == t_)cnt++;
		return cnt;
	}

	constexpr basic_matrix<_Elem>(const vector<vector<_Elem>>& matrix)
	{
		assert(!matrix.empty());
		for (const vector<_Elem>& l : matrix)
			assert(l.size() == matrix.front().size());
		mat = matrix;
	}

	constexpr void append(const vector<_Elem>& v)
	{
		assert(!(v.empty() || !mat.empty() && v.size() != sizev()));
		mat.push_back(v);
	}

	constexpr void axis()noexcept//行列交换
	{
		if (mat.empty())return;
		vector<vector<_Elem>>_mat(sizev(), vector<_Elem>(sizeh()));
		for (int i = 0; i < sizeh(); i++)
			for (int j = 0; j < sizev(); j++)
				_mat[j][i] = move(mat[i][j]);
		mat.swap(_mat);
	}

	constexpr void operator *=(const _Elem& t)noexcept
	{
		for (vector<_Elem>& v : mat)
			for (_Elem& _t : v)
				_t *= t;
	}

#ifdef QDEBUG_H
	inline friend QDebug operator<<(QDebug de, const basic_matrix<_Elem>& x)noexcept
	{
		de << "matrix:";
		for (const vector<_Elem>& v : x.mat)
		{
			de << "\n";
			for (const _Elem& t : v)
				de << t;
		}
		return de;
	}
#endif

	inline void simplify()//化为上三角阵
	{
		if (mat.empty())return;
		for (int i = 0; i < sizev(); i++)//i 列
		{
			for (int j = i + 1; j < sizeh(); j++)//j 行
			{
				if (mat.at(j).at(i) == 0)continue;
				swap(mat[i], mat[j]);
				if (mat.at(j).at(i) == 0)continue;
				_Elem a = mat.at(j).at(i) / mat.at(i).at(i);
				for (int k = i; k < sizev(); k++)//倍增后减过去
					if (mat.at(i).at(k) != 0)
						mat[j][k] -= mat.at(i).at(k) * a;
			}
		}
		while (sizeh() > sizev())mat.pop_back();//删除空白末尾
	}

	bool solve()
	{
		if (mat.empty())return false;
		set<int>s;
		//化为简化行阶梯阵，移除空行
		for (int i = int(sizeh()) - 1; i > -1; i--)
		{
			int j = i;
			for (; j < sizev(); j++)
				if (mat.at(i).at(j) != 0)break;
			if (j >= sizev())
			{
				mat.erase(mat.begin() + i);
				continue;
			}
			for (int k = i - 1; k > -1; k--)
			{
				if (mat.at(k).at(j) == 0)continue;
				_Elem r = mat.at(k).at(j) / mat.at(i).at(j);
				for (int t = k; t < sizev(); t++)
					if (mat.at(i).at(t) != 0)
						mat[k][t] -= mat.at(i).at(t) * r;
			}
			s.insert(j);//获得主元列
			if (mat.at(i).at(j) == 1)continue;
			for (int k = (int)sizev() - 1; k > j - 1; k--)
				mat[i][k] /= mat.at(i).at(j);
		}
		//判断无解情况
		if (sizeh() >= sizev() - 1)			
			for (int i = 0; i < sizeh(); i++)
				if (mat.at(i).back() != 0)
				{
					bool t = false;
					for (int j = i; j < sizev() - 1; j++)
						if (mat.at(i).at(j) != 0)
						{
							t = true;
							break;
						}
					if (!t)return false;
				}
		//生成-F矩阵 拼接I矩阵
		int _size = (int)sizev();
		for (int i = int(sizev()) - 1; i > -1; i--)
			if (s.contains(i))
				for (int j = 0; j < sizeh(); j++)
					mat[j].erase(mat[j].begin() + i);
		*this *= -1;
		for (int i = 0; i < sizev(); i++)
		{
			vector<_Elem>vec(sizev(), 0);
			vec[i] = 1;
			mat.push_back(vec);
		}
		//重新排序
		if (sizev() == 1)return true;
		vector<int>v;
		for (const int& x : s)
			v.push_back(x);
		sort(v.begin(), v.end());
		for (int i = 0; i < _size; i++)
			if (!s.contains(i))v.push_back(i);
		vector<vector<_Elem>>_mat;
		for (int i = 0; i < _size; i++)
			_mat.push_back(mat.at(v.at(i)));
		mat.swap(_mat);
		return true;
	}
};

#if __has_include("rat.h")
using ratmatrix = basic_matrix<rat>;
#endif