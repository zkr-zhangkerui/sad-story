/**
* @file
* @brief    This file gives the format of comments
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/04/15
* @version  2.3.20250415
* @par      Copyright(c) 2012-2021 School of Geodesy and Geomatics, University of Wuhan. All Rights Reserved.
*           POSMind Software Kernel, named IPS (Inertial Plus Sensing), this file is a part of IPS.
* @par      History:
*           2025/04/15,kerui zhang, new
*/

#ifndef BASE_MATRIX_H
#define BASE_MATRIX_H

#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<cmath>
#include"BaseSDC.h"
#include<exception>
#include <stdexcept>

template<class T> //固定写法 dst是double还是int还是别的都可以直接用
//inline 解决频繁调用的函数大量消耗内存的问题，代码比较简单的
/**
* @brief       设置矩阵的值
* @param[in]   dst   T*    矩阵的名字
* @param[in]   val   T     值
* @param[in]   n     int   矩阵中元素的个数
* @note        
*/
inline void setVals(T* dst, T val, int n)
{
	for (int i = 0; i < n; i++)
	{
		dst[i] = val;
	}
}

/**
* @brief       将三行一列的矩阵的值赋值给另一个矩阵
* @param[in]   M     const double    输入矩阵
* @param[out]  M1    double          输出矩阵
* @note
*/
inline void M31EQU(const double M[3],double M1[3])
{
	for (int i = 0; i < 3; i++)M1[i] = M[i];
}

/**
* @brief       将三行一列的矩阵值的相反数赋值给另一个矩阵
* @param[in]   M     const double    输入矩阵
* @param[out]  M1    double          输出矩阵
* @note
*/
inline void M31EQU_1(const double M[3],double M1[3])
{
	for (int i = 0; i < 3; i++)M1[i] = -M[i];
}

/**
* @brief       将三行三列的矩阵的值赋值给另一个矩阵
* @param[in]   M     const double    输入矩阵
* @param[out]  M1    double          输出矩阵
* @note
*/

inline void M33EQU(const double M[9],double M1[9])
{
	for (int i = 0; i < 9; i++)M1[i] = M[i];
}

/**
* @brief       将三行三列的矩阵值的相反数赋值给另一个矩阵
* @param[in]   M     const double    输入矩阵
* @param[out]  M1    double          输出矩阵
* @note
*/
inline void M33_EQU(const double M[9],double M1[9])
{
	for (int i = 0; i < 9; i++)M1[i] = -M[i];
}

/**
* @brief       将三行一列的两个矩阵相加
* @param[in]   M2    const double    输入矩阵1
* @param[in]   M1    const double    输入矩阵2
* @param[out]  M     double          计算结果
* @note
*/
inline void M31M31(const double M1[3], const double M2[3], double M3[3])
{
	for (int i = 0; i < 3; i++)M3[i] = M1[i] + M2[i];
}

/**
* @brief       将三行一列的两个矩阵相减
* @param[in]   M2    const double    输入矩阵1
* @param[in]   M1    const double    输入矩阵2
* @param[out]  M     double          计算结果
* @note
*/
inline void M31_M31(const double M1[3], const double M2[3], double M3[3])
{
	for (int i = 0; i < 3; i++)M3[i] = M1[i] - M2[i];
}

/**
* @brief       将三行三列的两个矩阵相加
* @param[in]   M2    const double    输入矩阵1
* @param[in]   M1    const double    输入矩阵2
* @param[out]  M     double          计算结果
* @note
*/
inline void M33M33(const double M1[9], const double M2[9], double M3[9])
{
	for (int i = 0; i < 9; i++)M3[i] = M1[i] + M2[i];
}

/**
* @brief       将三行三列的两个矩阵相减
* @param[in]   M2    const double    输入矩阵1
* @param[in]   M1    const double    输入矩阵2
* @param[out]  M     double          计算结果
* @note
*/
inline void M33_M33(const double M1[9], const double M2[9], double M3[9])
{
	for (int i = 0; i < 9; i++)M3[i] = M1[i] - M2[i];
}

/**
* @brief       将三行三列的两个矩阵相乘
* @param[in]   M2    const double    输入矩阵1
* @param[in]   M1    const double    输入矩阵2
* @param[out]  M     double          计算结果
* @note
*/
inline void M33cM33(const double M1[9],const double M2[9],double M3[9])
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			M3[i] = M3[i] + M1[int(j+ floor(i / 3)*3)] * M2[int(i%3 + j*3)];
		}
	}
}

/**
* @brief       将三行一列和三行三列的两个矩阵相乘
* @param[in]   M2    const double    输入矩阵1
* @param[in]   M1    const double    输入矩阵2
* @param[out]  M     double          计算结果
* @note
*/
inline void M33cM31(const double M1[9], const double M2[3], double M3[3])
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			M3[i] = M3[i] + M2[j] * M1[i * 3 + j];
		}
	}
}

//对称正定求逆 matlab syms
/**
* @brief       三行三列的对称正定矩阵求逆
* @param[in]   M1    const double    输入矩阵
* @param[out]  M     double          计算结果
* @note
*/
inline void M33InvSP(const double M1[9],double M[9])
{
	for (int i = 0; i < 9; i++)
	{
		M[i] = M1[i];
	}
	int j = 0, k = 0;

	double a = 0;

	for (k = 0; k < 3; k++)//第k行
	{
		for (int i = 0; i < 3; i++)
		{
			if (i != k)   M[i * 3 + k] = -M[i * 3 + k] / M[k * 3 + k];
		}
		M[k * 3 + k] = 1.0 / M[k * 3 + k];

		for (int i = 0; i < 3; i++)
		{
			if (i != k)
			{
				for (j = 0; j < 3; j++)
				{
					if (j != k)  M[i * 3 + j] += M[k * 3 + j] * M[i * 3 + k];
				}
			}
		}
		for (j = 0; j < 3; j++)
		{
			if (j != k)  M[k * 3 + j] *= M[k * 3 + k];
		}
	}
}

/*普通矩阵相乘*/
extern void Mult(double** mat1, double**mat2, double **mat,int N,int M,int P);
/*矩阵和数相乘*/
extern void Mult1(double mat1, double** mat2, double** mat, int M, int P);
/*矩阵转置*/
extern void Change(double** mat1, double** mat,int M,int N);
/*矩阵求逆*/
extern bool Inv(double **mat,double **inv,int N);
/*矩阵相加*/
extern void Add(double** mat1, double** mat2, double** mat,int N,int M);
/*矩阵相减*/
extern void Minus(double** mat1, double** mat2, double** mat, int N, int M);
/*一个矩阵的值赋值为另一个矩阵*/
extern void Copy(double** mat1, double** mat, int N, int M);

//#define IPS_Windows; windows系统
//#define IPS_NO_Windows; //嵌入式系统
/**
* @brief 通用矩阵的结构体
*
* 定义指向矩阵的指针，矩阵的行和列
* 构造函数对矩阵进行初始化
* 指定矩阵的行数和列数，并分配内存
* 释放矩阵的内存
* 普通矩阵的加减乘求逆函数
* 查找矩阵指定位置的值/赋值
*/
struct Mat
{
	/*
	void Inv()
	{
#ifdef IPS_Windows
		//在这个函数里面调用Eigen库 Eigen在嵌入式里面不能用
		Eigen::Inv();
#else
		Xigen::Inv();//其它嵌入式可以用的矩阵运算的库
#endif

	}*/

	double* x;
	unsigned int row;//无符号
	unsigned int col;

	//函数可以在.h文件中定义，也可以在cpp文件中定义
	//构造函数
	Mat();
	
	Mat(unsigned int row, unsigned int col);

	void newMat(int nRow, int nCol, const double* M=NULL);

	void freeMat();

	void add(Mat m1, Mat m2);

	void Mul(Mat m1, Mat m2);

	void Minus(Mat m1, Mat m2);

	void InvSP(Mat M);

	double at(int i, int j);//输入第几行第几列，返回对应的值

	void at(int i, int j, double val);//函数重载，给第1行第j列赋值

	double cal_th(Mat m1, Mat m2);/*计算两个3行1列矩阵相减结果的模长*/

	void read(FILE* fp);//文件读取 const：不能更改x，row，col的值
	void write(FILE* fp);//文件写入
};
#endif