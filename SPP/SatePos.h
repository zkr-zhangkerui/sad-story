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


#ifndef SATE_POS_H //防止定义重复包含
#define SATE_POS_H //如果没有定义上一行就执行这一行

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

//使用的常量
#define GM_GPS (3.986005 * pow(10, 14))
#define omage_e_GPS (7.2921151467 * pow(10, -5))
#define F_GPS (-4.442807633)*pow(10,-10)
#define GM_BDS (3.986004418*pow(10,14))
#define omage_e_BDS (7.2921150*pow(10,-5))
#define c (2.99792458 * pow(10, 8))

/**
* @brief    卫星位置，速度，钟差，钟速
* 相对论改正
*/
struct SateResult
{
	//信号发射时刻
	double X = 0;//地固系下x
	double Y = 0;//地固系下y
	double Z = 0;//地固系下z

	//地球自转改正后
	double X1 = 0;//地固系下x
	double Y1 = 0;//地固系下y
	double Z1 = 0;//地固系下z

	double d_X = 0;//地固系下x方向速度
	double d_Y = 0;//地固系下y方向速度
	double d_Z = 0;//地固系下z方向速度

	double clkbias = 0;//钟偏
	double clk = 0;//钟差

	//相对论改正卫星钟差
	double tr;
	double E;//偏近点角

	//信号发射时刻
	double f0 = 0.0;
	double f1 = 0.0;
	double f2 = 0.0;
	int week = 0;
	double toc = 0.0;
	double P = 0.0;
	int sys;
};

/**
* @brief       迭代计算偏近点角
* @param[in]   e     double     偏心率
*			   m     double     平近点角
* @return      返回偏近点角（弧度）
* @note
*/
extern double m_to_E(double m, double e);

/**
* @brief       将读取的字符串解析成数字
* @param[in]   line    string    从文件中读取的一行数据
* @return      可变数组 以double形式存储这行数据
* @note
*/
extern vector<double> string_double(string line);

/**
* @brief       把角度限制在一定范围内
* @param[in]   I    double     需要归化的角度
*              pai  double     需要限制的范围
* @return      double   归化后的角度
* @note
*/
extern double rad_convert(double I, double pai);

/**
* @brief       计算卫星位置
* @param[in]   eph   Eph*       该卫星的广播星历
*              tk    GPSTIME    目标时刻
*              result SateResult*  计算结果
*              sate_pos    FILE*   输出文件指针
*/
extern int satePos(Eph* eph, GPSTIME tk, SateResult* result, FILE* sate_pos);

#endif