#ifndef INTERGRITY_H
#define INTERGRITY_H

#include<iostream>
#include<vector>
#include "read_obs.h"

//存储每个卫星，每个频率的有效观测值个数
struct sat_fre_obsnum
{
	std::string frequence;
	int num = 0;//系统总历元数
	double sys_valid_num=0.0;//系统有效历元数
	std::vector<double> fre_valid_num;//每个频率有效历元数和数据完整率
	double sys_intergrity = 0.0;//系统数据完整率
};

extern void int_count(EpochData epoch, sat_fre_obsnum* GPS, sat_fre_obsnum* BDS, sat_fre_obsnum* GLO);
extern void int_cal(sat_fre_obsnum* GPS);

#endif // !INTERGRITY_H

