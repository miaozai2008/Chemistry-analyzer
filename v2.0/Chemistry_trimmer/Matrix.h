#pragma once
#include "rat.h"
#include <QString>
#include <QVector>
#include <QList>

class Matrix
{
private:
	QList<QVector<rat>>mat;

public:
	Matrix(const QList<QVector<rat>>& matrix)
	{
		if (matrix.isEmpty())
			throw QStringLiteral("矩阵不能为空");
		for (int i = 0; i < matrix.size(); i++)		
			if (matrix.at(i).size() != matrix.first().size())			
				throw QStringLiteral("矩阵行列不统一");
		mat = matrix;
	}

	Matrix() {  }
	~Matrix() {  }
	inline void operator =(const Matrix& x) { mat = x.mat; }
	inline bool operator !=(const Matrix& x)const { return x.mat != mat; }
	inline bool operator ==(const Matrix& x)const { return x.mat == mat; }
	inline const QList<QVector<rat>>& solution()const { return mat; }
	inline int sizeH()const { return mat.size(); }//行数
	inline int sizeV()const { return mat.isEmpty() ? 0 : mat.constFirst().size(); }//列数
	inline bool isEmpty()const { return mat.isEmpty(); }
	inline const rat& at(int i, int j)const { return mat.at(i).at(j); }//只读
	inline const QVector<rat>& at(int i)const { return mat.at(i); }//只读 行
	inline rat& value(int i, int j) { return mat[i][j]; }//可修改
	inline const rat& firstH(int i)const { return mat.at(i).constFirst(); }
	inline const rat& lastH(int i)const { return mat.at(i).constLast(); }
	inline const rat& firstV(int i)const { return mat.constFirst().at(i); }
	inline const rat& lastV(int i)const { return mat.constLast().at(i); }
	inline const QVector<rat>& first()const { return mat.constFirst(); }
	inline const QVector<rat>& last()const { return mat.constLast(); }

	
	inline bool appendH(const QVector<rat>& vec)
	{
		if (vec.isEmpty() || !mat.isEmpty() && vec.size() != sizeV())
			return false;
		mat.append(vec);
		return true;
	}

	inline bool appendV(const QVector<rat>& vec)
	{
		if (vec.isEmpty() || !mat.isEmpty() && vec.size() != sizeH())
			return false;
		for (int i = 0; i < sizeH(); i++)
			mat[i].append(vec.at(i));
		return true;
	}

	inline void axis()//行列交换
	{
		if (mat.isEmpty())return;
		QList<QVector<rat>>r;
		for (int i = 0; i < sizeV(); i++)
		{
			QVector<rat>v;
			for (int j = 0; j < sizeH(); j++)
				v.append(mat.at(j).at(i));
			r.append(v);
		}
		mat.swap(r);
	}

	inline void operator *=(const rat& r)
	{
		for (int i = 0; i < sizeH(); i++)
			for (int j = 0; j < sizeV(); j++)
				if (mat.at(i).at(j) != 0)
					mat[i][j] *= r;
	}

	inline friend QDebug operator<<(QDebug de, const Matrix& x)
	{
		de << "Matrix:";
		for (int i = 0; i < x.sizeH(); i++)
		{
			de << "\n";
			for (int j = 0; j < x.sizeV(); j++)
				de << x.mat.at(i).at(j);
		}
		return de;
	}

	inline void simplify()
	{
		if (mat.isEmpty())return;
		//化为上三角阵
		for (int i = 0; i < sizeV(); i++)//i 列
		{
			for (int j = i + 1; j < sizeH(); j++)//j 行
			{
				if (mat.at(j).at(i) == 0)continue;
				mat.swap(j, i);
				if (mat.at(j).at(i) == 0)continue;
				rat a = mat.at(j).at(i) / mat.at(i).at(i);
				for (int k = i; k < mat.first().size(); k++)//倍增后减过去
					if (mat.at(i).at(k) != 0)
						mat[j][k] -= mat.at(i).at(k) * a;
			}
		}
	}

	inline bool solve()
	{
		if (mat.isEmpty())return false;
		QVector<int>c;
		//化为简化行阶梯阵，移除空行
		for (int i = sizeH() - 1; i > -1; i--)
		{
			int j = i;
			for (; j < sizeV(); j++)
				if (mat.at(i).at(j) != 0)break;
			if (j >= sizeV())
			{
				mat.removeAt(i);
				continue;
			}
			for (int k = i - 1; k > -1; k--)
			{
				if (mat.at(k).at(j) == 0)continue;
				rat r = mat.at(k).at(j) / mat.at(i).at(j);
				for (int t = k; t < sizeV(); t++)
					if (mat.at(i).at(t) != 0)
						mat[k][t] -= mat.at(i).at(t) * r;
			}
			c.append(j);//获得主元列
			if (mat.at(i).at(j) == 1)continue;
			for (int k = sizeV() - 1; k > j - 1; k--)
				if (mat.at(i).at(k) != 0)
					mat[i][k] /= mat.at(i).at(j);
		}
		//判断无解情况
		if (sizeH() + 1 == sizeV())
			for (int i = 0; i < sizeH(); i++)
				if (mat.at(i).count(0) == sizeV() && mat.at(i).last() != 0)
					return false;
		//生成-F矩阵 拼接I矩阵
		int size = sizeV();
		for (int i = size - 1; i > -1; i--)
			if (c.contains(i))
				for (int j = 0; j < sizeH(); j++)
					mat[j].remove(i);
		*this *= -1;
		for (int i = 0; i < sizeV(); i++)
		{
			QVector<rat> v;
			for (int j = 0; j < sizeV(); j++)
				if (j == i)v.append(1);
				else v.append(0);
			mat.append(v);
		}
		//重新排序
		if (sizeV() == 1)return true;
		qSort(c);
		for (int i = 0; i < size; i++)
			if (!c.contains(i))c.append(i);
		QList<QVector<rat>>l;
		for (int i = 0; i < size; i++)
			l.append(mat.at(c.at(i)));
		mat.swap(l);
		return true;
	}
};