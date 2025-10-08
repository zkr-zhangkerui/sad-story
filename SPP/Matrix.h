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

template<class T> //�̶�д�� dst��double����int���Ǳ�Ķ�����ֱ����
//inline ���Ƶ�����õĺ������������ڴ�����⣬����Ƚϼ򵥵�
/**
* @brief       ���þ����ֵ
* @param[in]   dst   T*    ���������
* @param[in]   val   T     ֵ
* @param[in]   n     int   ������Ԫ�صĸ���
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
* @brief       ������һ�еľ����ֵ��ֵ����һ������
* @param[in]   M     const double    �������
* @param[out]  M1    double          �������
* @note
*/
inline void M31EQU(const double M[3],double M1[3])
{
	for (int i = 0; i < 3; i++)M1[i] = M[i];
}

/**
* @brief       ������һ�еľ���ֵ���෴����ֵ����һ������
* @param[in]   M     const double    �������
* @param[out]  M1    double          �������
* @note
*/
inline void M31EQU_1(const double M[3],double M1[3])
{
	for (int i = 0; i < 3; i++)M1[i] = -M[i];
}

/**
* @brief       ���������еľ����ֵ��ֵ����һ������
* @param[in]   M     const double    �������
* @param[out]  M1    double          �������
* @note
*/

inline void M33EQU(const double M[9],double M1[9])
{
	for (int i = 0; i < 9; i++)M1[i] = M[i];
}

/**
* @brief       ���������еľ���ֵ���෴����ֵ����һ������
* @param[in]   M     const double    �������
* @param[out]  M1    double          �������
* @note
*/
inline void M33_EQU(const double M[9],double M1[9])
{
	for (int i = 0; i < 9; i++)M1[i] = -M[i];
}

/**
* @brief       ������һ�е������������
* @param[in]   M2    const double    �������1
* @param[in]   M1    const double    �������2
* @param[out]  M     double          ������
* @note
*/
inline void M31M31(const double M1[3], const double M2[3], double M3[3])
{
	for (int i = 0; i < 3; i++)M3[i] = M1[i] + M2[i];
}

/**
* @brief       ������һ�е������������
* @param[in]   M2    const double    �������1
* @param[in]   M1    const double    �������2
* @param[out]  M     double          ������
* @note
*/
inline void M31_M31(const double M1[3], const double M2[3], double M3[3])
{
	for (int i = 0; i < 3; i++)M3[i] = M1[i] - M2[i];
}

/**
* @brief       ���������е������������
* @param[in]   M2    const double    �������1
* @param[in]   M1    const double    �������2
* @param[out]  M     double          ������
* @note
*/
inline void M33M33(const double M1[9], const double M2[9], double M3[9])
{
	for (int i = 0; i < 9; i++)M3[i] = M1[i] + M2[i];
}

/**
* @brief       ���������е������������
* @param[in]   M2    const double    �������1
* @param[in]   M1    const double    �������2
* @param[out]  M     double          ������
* @note
*/
inline void M33_M33(const double M1[9], const double M2[9], double M3[9])
{
	for (int i = 0; i < 9; i++)M3[i] = M1[i] - M2[i];
}

/**
* @brief       ���������е������������
* @param[in]   M2    const double    �������1
* @param[in]   M1    const double    �������2
* @param[out]  M     double          ������
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
* @brief       ������һ�к��������е������������
* @param[in]   M2    const double    �������1
* @param[in]   M1    const double    �������2
* @param[out]  M     double          ������
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

//�Գ��������� matlab syms
/**
* @brief       �������еĶԳ�������������
* @param[in]   M1    const double    �������
* @param[out]  M     double          ������
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

	for (k = 0; k < 3; k++)//��k��
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

/*��ͨ�������*/
extern void Mult(double** mat1, double**mat2, double **mat,int N,int M,int P);
/*����������*/
extern void Mult1(double mat1, double** mat2, double** mat, int M, int P);
/*����ת��*/
extern void Change(double** mat1, double** mat,int M,int N);
/*��������*/
extern bool Inv(double **mat,double **inv,int N);
/*�������*/
extern void Add(double** mat1, double** mat2, double** mat,int N,int M);
/*�������*/
extern void Minus(double** mat1, double** mat2, double** mat, int N, int M);
/*һ�������ֵ��ֵΪ��һ������*/
extern void Copy(double** mat1, double** mat, int N, int M);

//#define IPS_Windows; windowsϵͳ
//#define IPS_NO_Windows; //Ƕ��ʽϵͳ
/**
* @brief ͨ�þ���Ľṹ��
*
* ����ָ������ָ�룬������к���
* ���캯���Ծ�����г�ʼ��
* ָ��������������������������ڴ�
* �ͷž�����ڴ�
* ��ͨ����ļӼ������溯��
* ���Ҿ���ָ��λ�õ�ֵ/��ֵ
*/
struct Mat
{
	/*
	void Inv()
	{
#ifdef IPS_Windows
		//����������������Eigen�� Eigen��Ƕ��ʽ���治����
		Eigen::Inv();
#else
		Xigen::Inv();//����Ƕ��ʽ�����õľ�������Ŀ�
#endif

	}*/

	double* x;
	unsigned int row;//�޷���
	unsigned int col;

	//����������.h�ļ��ж��壬Ҳ������cpp�ļ��ж���
	//���캯��
	Mat();
	
	Mat(unsigned int row, unsigned int col);

	void newMat(int nRow, int nCol, const double* M=NULL);

	void freeMat();

	void add(Mat m1, Mat m2);

	void Mul(Mat m1, Mat m2);

	void Minus(Mat m1, Mat m2);

	void InvSP(Mat M);

	double at(int i, int j);//����ڼ��еڼ��У����ض�Ӧ��ֵ

	void at(int i, int j, double val);//�������أ�����1�е�j�и�ֵ

	double cal_th(Mat m1, Mat m2);/*��������3��1�о�����������ģ��*/

	void read(FILE* fp);//�ļ���ȡ const�����ܸ���x��row��col��ֵ
	void write(FILE* fp);//�ļ�д��
};
#endif