#ifndef ALIGN_H
#define ALIGN_H

#include "read_convert.h"

#define HZ_align  (100)                 /*采样率 示例数据是200 小车数据是100*/

/**
	* @brief       计算一段时间IMU测量数据的平均值
	* @param[in]   imudata    std::vector<IMUData>     IMU观测数据结构体
	* @param[out]  mean       double*                  平均值结果
	* @return      无
	* @note        
	*/
extern void cal_mean_align(std::vector<IMUData> imudata, double* mean);

/**
	* @brief       计算姿态角
	* @param[in]   w_f[6]     double                   IMU观测数据平均值
	* @param[out]  angle      double*                  姿态角结果
	* @return      无
	* @note
	*/
extern void cal_angle(double w_f[6], double* angle);
extern void correct_angle(double* angle, double* angle_standard);
extern int main_align();

/**
	* @brief       解析每一行IMU观测数据
	* @param[in]   line       const std::string&       当前需要解析的行
	* @param[out]  data       IMUData&                 IMU观测数据结构体
	* @return      是否解析成功
	* @note
	*/
extern bool parseIMUData_align(const std::string& line, IMUData& data);

/**
	* @brief       解析IMU观测数据
	* @param[in]   filename   const std::string&       文件名
	* @return      IMU观测数据结构体数组
	* @note
	*/
extern std::vector<IMUData> readIMUFile_align(const std::string& filename);
extern int static_time(std::vector<IMUData> imudata, int size);

#endif // !ALIGN_H

