#ifndef MP_H
#define MP_H

#include "read_obs.h"
#include<iostream>

#define f1_G (1.57542E+09)
#define f2_G (1.2276E+09)
#define f1_C (1.561098E+09)
#define f2_C (1.20714E+09)
#define f3_C (1.26852E+09)
#define f1_R (1.602E+09)
#define f2_R (1.246E+09)
#define c    (3E+8)

static const double G_f1 = (f1_G * f1_G + f2_G * f2_G) / (f1_G * f1_G - f2_G * f2_G);
static const double G_f2= (2* f2_G * f2_G) / (f1_G * f1_G - f2_G * f2_G);
static const double G_f3 = (2 * f1_G * f1_G) / (f1_G * f1_G - f2_G * f2_G);

static const double C_f1 = (f1_C * f1_C + f3_C * f3_C) / (f1_C * f1_C - f3_C * f3_C);
static const double C_f2 = (2 * f3_C * f3_C) / (f1_C * f1_C - f3_C * f3_C);
static const double C_f3 = (2 * f1_C * f1_C) / (f1_C * f1_C - f3_C * f3_C);

static const double R_f1 = (f1_R * f1_R + f2_R * f2_R) / (f1_R * f1_R - f2_R * f2_R);
static const double R_f2 = (2 * f2_R * f2_R) / (f1_R * f1_R - f2_R * f2_R);
static const double R_f3 = (2 * f1_R * f1_R) / (f1_R * f1_R - f2_R * f2_R);

struct epoch_MP//每颗卫星
{
	std::string satID;
	std::vector<double> MP1;//每个历元
	std::vector<double> MP2;
};


extern void cal_MP(EpochData epoch, FILE* obs_fp, FILE* MP_C, FILE* MP_R, std::vector<epoch_MP>*epoch_mp_g, std::vector<epoch_MP>*epoch_mp_c, std::vector<epoch_MP>*epoch_mp_r, int epochCount);
extern void cal_MP_ave(std::vector<epoch_MP> epoch_mp_g, std::vector<int> slip_epoch, FILE* MP);
extern double GLO_fre(double center_fre, int prn, double d_fre);
extern void diff_MP(std::vector<epoch_MP>* epoch_mp_g, int epochCount);
#endif // !MP_H

