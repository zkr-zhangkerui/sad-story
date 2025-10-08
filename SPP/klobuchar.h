/**
* @file
* @brief    This file gives the format of comments
*           klobucarģ�ͽ��е�������
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
* @brief       �������Ƿ�λ�ǣ����ڵ�����ӳټ���
* @param[in]   blh[3]   ��վ����blh����
*			   pos      ��վ����xyz����
*              result   �洢����λ�õĽṹ��
* @return      double �������Ƿ�λ��
* @note
*/
extern double pos_angle(double blh[3], Eigen::MatrixXd pos, SateResult result);

/**
* @brief       ���������ӳٸ������ף�
* @param[in]   pos_angle   ���Ƿ�λ�ǣ��ȣ�
*			   ion         �����˲���
*              high_angle  ���Ǹ߶Ƚǣ����ȣ�
*              UT          ����Ԫ��UTʱ���룩
*              blh[3]      ��վ����blh����
* @return      double ���ص�����ӳٸ������ף�
* @note
*/
extern double klobuchar(double pos_angle, Ion ion, double high_angle, double UT, double blh[3]);
#endif