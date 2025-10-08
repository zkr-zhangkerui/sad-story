/**
* @file
* @brief    This file gives the format of comments
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/04/22
* @version  1.0.20250422
* @par      History:
*           2025/04/22,kerui zhang
*/
#ifndef GNSS_CMN_FUNC_H
#define GNSS_CMN_FUNC_H

//#include "..\BASEDLL\eigen_safe.h"
#include <Eigen/Dense>
/**
* @brief       XYZ坐标转化到BLH坐标
* @param[in]   XYZ[3]   XYZ坐标
* @return      bool 返回是否转换成功
* @note
*/
bool XYZ2BLH(Eigen::MatrixXd XYZ, double *BLH);

/**
* @brief       BLH坐标转化到XYZ坐标
* @param[in]   BLH[3]   BLH坐标
* @return      bool 返回是否转换成功
* @note
*/
bool BLH2XYZ(const double BLH[3], double XYZ[3]);

/**
* @brief       XYZ坐标转化到ENU坐标
* @param[in]   XYZ		  Eigen::MatrixXd      XYZ坐标
*			   enu        double*              转换矩阵
*              origin     double[3]            基站XYZ坐标
*              origin_blh double               基站blh坐标
* @return      Eigen::MatrixXd 返回转换结果
* @note
*/
Eigen::MatrixXd XYZ2ENU(Eigen::MatrixXd XYZ, double* enu, double origin[3], double origin_blh[3]);
#endif