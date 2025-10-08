#ifndef INTERGRITY_H
#define INTERGRITY_H

#include<iostream>
#include<vector>
#include "read_obs.h"

//�洢ÿ�����ǣ�ÿ��Ƶ�ʵ���Ч�۲�ֵ����
struct sat_fre_obsnum
{
	std::string frequence;
	int num = 0;//ϵͳ����Ԫ��
	double sys_valid_num=0.0;//ϵͳ��Ч��Ԫ��
	std::vector<double> fre_valid_num;//ÿ��Ƶ����Ч��Ԫ��������������
	double sys_intergrity = 0.0;//ϵͳ����������
};

extern void int_count(EpochData epoch, sat_fre_obsnum* GPS, sat_fre_obsnum* BDS, sat_fre_obsnum* GLO);
extern void int_cal(sat_fre_obsnum* GPS);

#endif // !INTERGRITY_H

