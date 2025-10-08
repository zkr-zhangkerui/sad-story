/**
* @file
* @brief    This file gives the format of comments
*           klobucar模型进行电离层改正
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/07/27
* @version  1.0.20250727
* @par      History:
*           2025/04/22,kerui zhang
*/

#ifndef KLOBUCHAR_H
#define KLOBUCHAR_H

#include "SatePos.h"

/**
* @brief       计算卫星方位角，用于电离层延迟计算
* @param[in]   blh[3]   测站近似blh坐标
*			   pos      测站近似xyz坐标
*              result   存储卫星位置的结构体
* @return      double 返回卫星方位角
* @note
*/
extern double pos_angle(double blh[3], Eigen::MatrixXd pos, SateResult result);

/**
* @brief       计算电离层延迟改正（米）
* @param[in]   pos_angle   卫星方位角（度）
*			   ion         电离层八参数
*              high_angle  卫星高度角（弧度）
*              UT          该历元的UT时（秒）
*              blh[3]      测站近似blh坐标
* @return      double 返回电离层延迟改正（米）
* @note
*/
extern double klobuchar(double pos_angle, Ion ion, double high_angle, double UT, double blh[3]);
#endif