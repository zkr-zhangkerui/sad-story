/**
* @file
* @brief    This file gives the format of comments
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/05/12
* @version  1.0.20250512
* @par      History:
*           2025/05/12,kerui zhang
*/


#ifndef SATE_POS_H //��ֹ�����ظ�����
#define SATE_POS_H //���û�ж�����һ�о�ִ����һ��

//#include "../BASEDLL/eigen_safe.h"
#include <Eigen/Dense>
#include "ome_rinex.h"
#include"../BASEDLL/BaseCmnFunc.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cmath>
#include<sstream>
#include<iomanip>
using namespace std;

//ʹ�õĳ���
#define GM_GPS (3.986005 * pow(10, 14))
#define omage_e_GPS (7.2921151467 * pow(10, -5))
#define F_GPS (-4.442807633)*pow(10,-10)
#define GM_BDS (3.986004418*pow(10,14))
#define omage_e_BDS (7.2921150*pow(10,-5))
#define c (2.99792458 * pow(10, 8))

/**
* @brief    ����λ�ã��ٶȣ��Ӳ����
* ����۸���
*/
struct SateResult
{
	//�źŷ���ʱ��
	double X = 0;//�ع�ϵ��x
	double Y = 0;//�ع�ϵ��y
	double Z = 0;//�ع�ϵ��z

	//������ת������
	double X1 = 0;//�ع�ϵ��x
	double Y1 = 0;//�ع�ϵ��y
	double Z1 = 0;//�ع�ϵ��z

	double d_X = 0;//�ع�ϵ��x�����ٶ�
	double d_Y = 0;//�ع�ϵ��y�����ٶ�
	double d_Z = 0;//�ع�ϵ��z�����ٶ�

	double clkbias = 0;//��ƫ
	double clk = 0;//�Ӳ�

	//����۸��������Ӳ�
	double tr;
	double E;//ƫ�����

	//�źŷ���ʱ��
	double f0 = 0.0;
	double f1 = 0.0;
	double f2 = 0.0;
	int week = 0;
	double toc = 0.0;
	double P = 0.0;
	int sys;
};

/**
* @brief       ��������ƫ�����
* @param[in]   e     double     ƫ����
*			   m     double     ƽ�����
* @return      ����ƫ����ǣ����ȣ�
* @note
*/
extern double m_to_E(double m, double e);

/**
* @brief       ����ȡ���ַ�������������
* @param[in]   line    string    ���ļ��ж�ȡ��һ������
* @return      �ɱ����� ��double��ʽ�洢��������
* @note
*/
extern vector<double> string_double(string line);

/**
* @brief       �ѽǶ�������һ����Χ��
* @param[in]   I    double     ��Ҫ�黯�ĽǶ�
*              pai  double     ��Ҫ���Ƶķ�Χ
* @return      double   �黯��ĽǶ�
* @note
*/
extern double rad_convert(double I, double pai);

/**
* @brief       ��������λ��
* @param[in]   eph   Eph*       �����ǵĹ㲥����
*              tk    GPSTIME    Ŀ��ʱ��
*              result SateResult*  ������
*              sate_pos    FILE*   ����ļ�ָ��
*/
extern int satePos(Eph* eph, GPSTIME tk, SateResult* result, FILE* sate_pos);

#endif