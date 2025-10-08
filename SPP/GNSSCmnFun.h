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
* @brief       XYZ����ת����BLH����
* @param[in]   XYZ[3]   XYZ����
* @return      bool �����Ƿ�ת���ɹ�
* @note
*/
bool XYZ2BLH(Eigen::MatrixXd XYZ, double *BLH);

/**
* @brief       BLH����ת����XYZ����
* @param[in]   BLH[3]   BLH����
* @return      bool �����Ƿ�ת���ɹ�
* @note
*/
bool BLH2XYZ(const double BLH[3], double XYZ[3]);

/**
* @brief       XYZ����ת����ENU����
* @param[in]   XYZ		  Eigen::MatrixXd      XYZ����
*			   enu        double*              ת������
*              origin     double[3]            ��վXYZ����
*              origin_blh double               ��վblh����
* @return      Eigen::MatrixXd ����ת�����
* @note
*/
Eigen::MatrixXd XYZ2ENU(Eigen::MatrixXd XYZ, double* enu, double origin[3], double origin_blh[3]);
#endif