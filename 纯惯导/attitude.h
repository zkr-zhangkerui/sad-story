/**
* @file
* @brief    姿态角基本运算
* @details  包括由方向余弦矩阵计算姿态角，由姿态四元数计算方向余弦矩阵、由等效旋转矢量计算姿态四元数
* @author   Kerui Zhang. Email: krzhang@whu.edu.cn
* @date     2025/12/03
* @version  1.1.20251203
* @par      无
* @par      History:
*           2025/12/03,Kerui Zhang, 初步完成四元数、方向余弦矩阵、欧拉角、等效旋转矢量的转换函数，定义结构体和宏\n
*           2025/12/04,Kerui Zhang, 增加四元数乘法、由姿态角到四元数、由欧拉角到方向余弦矩阵\n
*           2025/12/05,Kerui Zhang, 增加四元数规范化和方向余弦矩阵正交化，修改欧拉角顺序\n
*           2025/12/08,Kerui Zhang, 增加方向余弦矩阵转四元数
*/

#ifndef ATTITUDE_H
#define ATTITUDE_H

#include "read_convert.h"

/**
* @brief 姿态结构体
*
* 欧拉角顺序“翻滚、俯仰、航向 度
*/
struct Attitude {
	my_eigen::Matrix C;//方向余弦矩阵
	my_eigen::Matrix rotvec;//等效旋转矢量 rad
	my_eigen::Matrix quat;//四元数的矩阵形式
	double angle[3];//姿态角 翻滚、俯仰、航向、 弧度
};

/**
	* @brief       方向余弦矩阵转欧拉角
	* @param[out]  attitude    Attitude*     姿态结构体
	* @return      无
	* @note        得到欧拉角的单位：rad
	*/
extern void CToAngle(Attitude* attitude);

/**
	* @brief       四元数转方向余弦矩阵
	* @param[out]  attitude    Attitude*     姿态结构体
	* @return      无
	* @note        
	*/
extern void QuatToC(Attitude* attitude);

/**
	* @brief       等效旋转矩阵转四元数
	* @param[out]  attitude    Attitude*     姿态结构体
	* @return      无
	* @note        
	*/
extern void RotvceToQuat(Attitude* attitude);

/**
	* @brief       为姿态结构体分配内存赋初值0
	* @param[out]  attitude    Attitude*     姿态结构体
	* @return      无
	* @note        
	*/
extern void InitAttitude(Attitude* attitude);

/**
	* @brief       四元数乘法
	* @param[in]   p   my_eigen::Matrix  相乘的四元数1
	* @param[in]   q   my_eigen::Matrix  相乘的四元数2
	* @return      my_eigen::Matrix 返回计算结果
	* @note       
	*/
extern my_eigen::Matrix QuatSquare(my_eigen::Matrix p, my_eigen::Matrix q);

/**
	* @brief       欧拉角转四元数
	* @param[out]  attitude    Attitude*     姿态结构体
	* @return      无
	* @note        
	*/
extern void AngleToQuat(Attitude* attitude);

/**
	* @brief       欧拉角转方向余弦矩阵
	* @param[out]  attitude    Attitude*     姿态结构体
	* @return      无
	* @note        
	*/
extern void AngleToC(Attitude* attitude);

/**
	* @brief       四元数归一化
	* @param[out]  attitude    Attitude*     姿态结构体
	* @return      无
	* @note        
	*/
extern void QuatNorm(Attitude* attitude);

/**
	* @brief       方向余弦矩阵正交化
	* @param[out]  attitude    Attitude*     姿态结构体
	* @return      无
	* @note        
	*/
extern void CNormalize(Attitude* attitude);

/**
	* @brief       方向余弦矩阵转四元数
	* @param[out]  attitude    Attitude*     姿态结构体
	* @return      无
	* @note        
	*/
extern void CToQuat(Attitude* attitude);

/**
	* @brief       求向量的反对称矩阵
	* @param[in]   rotvec    my_eigen::Matrix     等效旋转矢量/向量
	* @return      my_eigen::Matrix    反对称矩阵
	* @note
	*/
extern my_eigen::Matrix reverse_M(my_eigen::Matrix rotvec);

#endif // !ATTITUDE_H
