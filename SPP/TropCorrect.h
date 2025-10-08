/**
* @file
* @brief    This file gives the format of comments
*           对流层延迟改正
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/05/24
* @version  1.0.20250524
* @par      History:
*           2025/05/24,kerui zhang
*/


#ifndef TROP_CORRECT_H //防止定义重复包含
#define TROP_CORRECT_H //如果没有定义上一行就执行这一行

#include"../BASEDLL/BaseSDC.h"
#include<iostream>
#include<fstream>
#include<vector>
#include<iomanip>
#include<sstream>
#include<string>
#include<cmath>
using namespace std;

#define TD0  (288.16)//标准温度（开尔文）
#define PR0  (1013.25)//标准压力（mbar）
#define HR0  (0.5)//标准湿度
#define hw   (11000)

/**
* @brief 对流层延迟改正所需参数
*
*/
struct Trop
{
	double h;//测站高程
	double HR;//相对湿度（%）
	double PR;//压力（mbar）
	double RI;//降雨量（0.1mm）
	double TD;//温度（摄氏度）
};

/**
*@brief       hopefiele模型解算对流层延迟改正
*@param[in]   trop      Trop*       对流层延迟改正所需参数
*             E         double      卫星高度角
*@return      int 返回是否解码成功
*@note
*/
extern double hopfield(Trop*trop, double E);


/**
*@brief       saastaminen模型解算对流层延迟改正
*@param[in]   trop      Trop*       对流层延迟改正所需参数
*             E         double      卫星高度角
*@return      int 返回是否解码成功
*@note
*/
extern double Saastaminen(Trop*trop, double E);

extern Trop trop;
//extern Trop_S trop_s;

#endif