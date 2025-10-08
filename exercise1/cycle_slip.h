#ifndef CYCLE_SLIP_H
#define CYCLE_SLIP_H

#include "read_obs.h"
#include<../eigen-3.3.9/Eigen/Eigen>
#include<vector>

struct L_MW
{
	double L_MW_ti[3] = { 0.0,0.0,0.0 };
	double delat_L_MW_ti[3] = { 0.0,0.0,0.0 };
	double ave_L_MW_ti[3] = { 0.0,0.0,0.0 };
	int sys_slip=0;//ͳ��ÿ��ϵͳ���������Ĵ���
	std::vector<int> sate_slip;//ͳ��ÿ�����Ƿ��������Ĵ��� ���Ǻž�������
	std::vector<int>sate_num;//ͳ��ÿ�����ǳ��ֵĴ���
};

struct GF
{
	double L_GF_ti[3]= { 0.0,0.0,0.0 };
	Eigen::VectorXd P_GF;
	Eigen::VectorXd Q_GF;
	int sys_slip=0;//ͳ��ÿ��ϵͳ���������Ĵ���
	std::vector<int> sate_slip;//ͳ��ÿ�����Ƿ��������Ĵ��� ���Ǻž�������
	std::vector<int>sate_num;//ͳ��ÿ�����ǳ��ֵĴ���
};

extern bool test_MW(EpochData epoch, L_MW* l_mw_G, L_MW* l_mw_C, L_MW* l_mw_R, int epochCount, FILE* slip_MW);
extern bool test_GF(EpochData epoch, GF* GF_G, GF* GF_C, GF* GF_R, int epochCount);
extern void cal_sate_slip(GF GF_G, FILE* file);
extern Eigen::VectorXd cal_Q_GF(int N, Eigen::VectorXd P_GF);
extern bool sys_test_MW(EpochData epoch, L_MW* l_mw_G, int epochCount,char sys, double f1, double f2, FILE* slip_MW);
extern bool sys_test_GF(EpochData epoch, GF* GF_G, int epochCount, char sys, double f1, double f2);
#endif