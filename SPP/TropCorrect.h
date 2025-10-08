/**
* @file
* @brief    This file gives the format of comments
*           �������ӳٸ���
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/05/24
* @version  1.0.20250524
* @par      History:
*           2025/05/24,kerui zhang
*/


#ifndef TROP_CORRECT_H //��ֹ�����ظ�����
#define TROP_CORRECT_H //���û�ж�����һ�о�ִ����һ��

#include"../BASEDLL/BaseSDC.h"
#include<iostream>
#include<fstream>
#include<vector>
#include<iomanip>
#include<sstream>
#include<string>
#include<cmath>
using namespace std;

#define TD0  (288.16)//��׼�¶ȣ������ģ�
#define PR0  (1013.25)//��׼ѹ����mbar��
#define HR0  (0.5)//��׼ʪ��
#define hw   (11000)

/**
* @brief �������ӳٸ����������
*
*/
struct Trop
{
	double h;//��վ�߳�
	double HR;//���ʪ�ȣ�%��
	double PR;//ѹ����mbar��
	double RI;//��������0.1mm��
	double TD;//�¶ȣ����϶ȣ�
};

/**
*@brief       hopefieleģ�ͽ���������ӳٸ���
*@param[in]   trop      Trop*       �������ӳٸ����������
*             E         double      ���Ǹ߶Ƚ�
*@return      int �����Ƿ����ɹ�
*@note
*/
extern double hopfield(Trop*trop, double E);


/**
*@brief       saastaminenģ�ͽ���������ӳٸ���
*@param[in]   trop      Trop*       �������ӳٸ����������
*             E         double      ���Ǹ߶Ƚ�
*@return      int �����Ƿ����ɹ�
*@note
*/
extern double Saastaminen(Trop*trop, double E);

extern Trop trop;
//extern Trop_S trop_s;

#endif