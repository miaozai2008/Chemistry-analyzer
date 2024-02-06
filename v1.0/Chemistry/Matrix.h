#pragma once
#include "NoMessyCode.h"
#include "rat.h"
#include <QString>
#include <QVector>
#include <QList>

class Matrix
{
private:
	QList<QVector<rat>>mat;
	QVector<int>c;

public:
	Matrix(const QList<QVector<rat>>& matrix)
	{
		if (matrix.isEmpty())
			throw QStringLiteral("矩阵不能为空");
		for (const QVector<rat>& vector : matrix)
			if (vector.size() != matrix[0].size())
				throw QStringLiteral("矩阵行列不统一");
		mat = matrix;
	}

	~Matrix() { mat.~QList(); }
	inline void operator =(const Matrix& x) { mat = x.mat; }
	inline bool operator !=(const Matrix& x)const { return x.mat != mat; }
	inline bool operator ==(const Matrix& x)const { return x.mat == mat; }
	inline const QList<QVector<rat>>& solution()const { return mat; }

	inline void operator *=(const rat& r)
	{
		for (int i = 0; i < mat.size(); i++)
			for (int j = 0; j < mat.at(i).size(); j++)
				if (mat.at(i).at(j) != 0)
					mat[i][j] *= r;
	}

#ifdef _DEBUG
	inline friend QDebug operator<<(QDebug de, const Matrix& x)
	{
		de << "Matrix:";
		for (int i = 0; i < x.mat.size(); i++)
		{
			de << "\n";
			for (int j = 0; j < x.mat.first().size(); j++)
				de << x.mat[i][j];
		}
		return de;
	}
#endif

	inline void simplify()
	{
		//化为上三角阵
		for (int i = 0; i < mat.first().size(); i++)//i 列
		{
			for (int j = i + 1; j < mat.size(); j++)//j 行
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
		//化为简化行阶梯阵，移除空行
		for (int i = mat.size() - 1; i > -1; i--)
		{
			int j = i;
			for (; j < mat.first().size(); j++)
				if (mat.at(i).at(j) != 0)break;
			if (j >= mat.first().size())
			{
				mat.removeAt(i);
				continue;
			}
			for (int k = i - 1; k > -1; k--)
			{
				if (mat.at(k).at(j) == 0)continue;
				rat r = mat.at(k).at(j) / mat.at(i).at(j);
				for (int t = k; t < mat.first().size(); t++)
					if (mat.at(i).at(t) != 0)
						mat[k][t] -= mat.at(i).at(t) * r;
			}
			c.append(j);//获得主元列
			if (mat.at(i).at(j) == 1)continue;
			for (int k = mat.first().size() - 1; k > j - 1; k--)
				if (mat.at(i).at(k) != 0)
					mat[i][k] /= mat.at(i).at(j);
		}
	}

	inline void slove()
	{
		//判断无解情况
		if (mat.size() + 1 == mat.first().size())
			for (const QVector<rat>& vec : mat)
				if (vec.count(0) == vec.size() - 1 && vec.last() != 0)
					throw QStringLiteral("矩阵无解");
		//生成-F矩阵
		int size = mat.first().size();
		for (int i = size - 1; i > -1; i--)
			if (c.contains(i))
				for (int j = 0; j < mat.size(); j++)
					mat[j].remove(i);
		*this *= -1;
		//拼接I矩阵
		for (int i = 0; i < mat.first().size(); i++)
		{
			QVector<rat>vec;
			for (int j = 0; j < mat.first().size(); j++)
				if (j == i)vec.append(1);
				else vec.append(0);
			mat.append(vec);
		}
		//重新排序
		qSort(c);
		for (int i = 0; i < size; i++)
			if (!c.contains(i))c.append(i);
		QList<QVector<rat>>l;
		for (int i = 0; i < size; i++)
			l.append(mat.at(c.at(i)));
		mat.swap(l);
	}
};