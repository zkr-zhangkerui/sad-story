#ifndef READ_CONVERT_H
#define READ_CONVERT_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include "matrix.h"

#define ACC_SCALE (1.5258789063e-6)     /*比力因子*/
#define GYO_SCALE (1.0850694444e-7)     /*角速度因子*/
#define HZ        (100)                 /*采样率*/
#define LAT       (30.5282362393)       /*纬度  23.1373950000度*/
#define H         (36.402880867)        /*椭球高  米*/
//#define LAT       (30.531651244)        /*纬度 度*/
//#define H         (28.2134)             /*椭球高 米*/
#define G         (9.7936174)           /*重力加速度*/
#define W_e       (7.292115*pow(10,-5)) /*地球自转角速度 弧度/秒*/
#define PI        (3.1415926539)        /*圆周率*/
#define alpha     (360)                 /*标定时转动的角度 度*/

/*增量格式 m/s和rad*/
struct IMUData {
	double time;
	double acc_z;
	double acc_y;
	double acc_x;
	double gyo_z;
	double gyo_y;
	double gyo_x;
};

extern bool parseIMUData(const std::string& line, IMUData& data);
extern std::vector<IMUData> readIMUFile(const std::string& filename);
extern void cal_mean(std::vector<IMUData> imudata, double* mean,double begin,double end);
extern my_eigen::Matrix cal_acc(my_eigen::Matrix measure);
extern double cal_sum(std::vector<IMUData> imudata,int begin, int end, int dim);
extern std::vector<IMUData> correct(my_eigen::Matrix result, std::vector<IMUData>imudata, std::vector<IMUData>imudata_w, int which);
extern my_eigen::Matrix cal_w(std::vector<IMUData> imuData_w, my_eigen::Matrix result_w, std::string* filename_w,double begin_end[6][2]);
extern int main_calibration();

#endif // !READ_CONVERT_H
