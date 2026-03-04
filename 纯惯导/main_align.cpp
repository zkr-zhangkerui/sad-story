#include "align.h"
#include "read_convert.h"

int main_align() {
	std::vector<IMUData> imudata;
	imudata = readIMUFile_align("E:/新建文件夹/大三上/惯导/粗对准/group6/group6.ASC");

	//求观测时段内比力和角速度平均值
	double mean_f_w[6]; double angle[3]; double angle0[3];//整段数据的平均结果
	cal_mean_align(imudata, mean_f_w);
	cal_angle(mean_f_w, angle0);
	std::cout << angle0[0] << " " << angle0[1] << " " << angle0[2] << std::endl;

	//每秒比力和角速度平均值
	std::vector<std::vector<double>> mean_sec; 
	FILE* fp_sec; std::string file_sec = "angle_sec.txt";
	fopen_s(&fp_sec, file_sec.c_str(), "w");
	if (fp_sec == nullptr)
	{
		printf("Fail to open file: %s to write.\n", file_sec.c_str());
		return 0;
	}
	for (int i = 0; i < imudata.size() - HZ_align; i=i+HZ_align) {
		std::vector<IMUData> imudata_sec;
		double mean_f_w_sec[6];
		imudata_sec.assign(imudata.begin() + i, imudata.begin() + i+ HZ_align);
		cal_mean_align(imudata_sec, mean_f_w_sec);
		cal_angle(mean_f_w_sec, angle);
		fprintf(fp_sec, "%20.10lf,%20.10lf,%20.10lf,%20.10lf\n", imudata[i].time, angle[0], angle[1], angle[2]);
	}

	//每历元计算姿态角
	FILE* fp; std::string file = "angle_epoch.txt";
	fopen_s(&fp, file.c_str(), "w");
	if (fp == nullptr)
	{
		printf("Fail to open file: %s to write.\n", file.c_str());
		return 0;
	}
	for (int i = 0; i < imudata.size(); i++) {
		double f_w[6];
		f_w[0] = imudata[i].acc_x; f_w[1] = imudata[i].acc_y; f_w[2] = imudata[i].acc_z;
		f_w[3] = imudata[i].gyo_x; f_w[4] = imudata[i].gyo_y; f_w[5] = imudata[i].gyo_y;
		cal_angle(f_w, angle);
		//把atan2的结果规划到合理范围，避免发生跳跃
		correct_angle(angle, angle0);
		fprintf(fp, "%20.10lf,%20.10lf,%20.10lf,%20.10lf\n", imudata[i].time, angle[0], angle[1],angle[2]);
	}
	
}