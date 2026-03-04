/**
* @file
* @brief    INS主函数
* @details  包括读取IMU观测数据和参考数据，轴向转换（右前上到前右下），设置初始位置、速度和姿态，惯导解算，误差计算等
* @author   Kerui Zhang. Email: krzhang@whu.edu.cn
* @date     2025/12/03
* @version  1.1.20251203
* @par      无
* @par      History:
*           2025/12/03,Kerui Zhang, 初步完成文件读取\n
*           2025/12/04,Kerui Zhang, 初步完成逐历元IMU解算，增加零速修正\n 
*           2025/12/05,Kerui Zhang, 修改了一些错误，增加四元数规范化和方向余弦矩阵正交化\n 
*           2025/12/08,Kerui Zhang, 增加读取示例数据 \n 
*           2025/12/09,Kerui Zhang, 修改了粗对准部分，使用前5分钟的静止数据进行粗对准；把求反对称矩阵和更新地理参数写成函数 更进一步的改进：隔一段时间加入rtk位置/补偿IMU零偏
*/

#include "ins.h"

int main_ins(std::string filename,std::string reffile) {
	//测试attitude函数和矩阵功能都没有问题

	//读取IMU观测数据文件 得到加速度和角速度(rad/s) 已经经过轴向调整
	std::vector<IMUData> imudata;
	imudata = readIMUFile_align(filename);
	//imudata = ReadBin(filename);
	std::cout<<"观测数据读取完成"<<std::endl;

	//读取参考结果 纬经高（弧度），enu速度和横滚,俯仰,航向，（弧度）
	std::vector<INSResult> ref_ins;
	ref_ins = readINSFile(reffile);
	//ref_ins = ReadBinRef(reffile);
	std::cout << "参考结果读取完成" << std::endl;

	//输出结果
	FILE* fp_ins_result; std::string file = "INS_result.txt";
	fopen_s(&fp_ins_result, file.c_str(), "w");
	if (fp_ins_result == nullptr){
		printf("Fail to open file: %s to write.\n", file.c_str());
		return -1;
	}
	//中间结果文件
	FILE* fp_temp; std::string file_temp = "INS_temp.txt";
	fopen_s(&fp_temp, file_temp.c_str(), "w");
	if (fp_temp == nullptr) {
		printf("Fail to open file: %s to write.\n", file_temp.c_str());
		return -1;
	}

	//进行零速修正的时间点
	//double stop[4] = { 98840.00,99200.00,99540.00,99880.00 };

	//粗对准以及赋初值
	double mean_f_w[6]; double angle[3]; double angle0[3];//整段数据的平均结果
	INSResult result0; INSResult result1;//分别是前2，1两个历元的结果
	INSResult result;//除了第一个历元，当前历元的解算结果
	std::vector<IMUData> begin_data(imudata.begin(), imudata.begin() + 300*HZ_align);//使用起始5~10分钟的静止数据进行粗对准 
	if (begin_data.size() != 0) {
		cal_mean_align(begin_data, mean_f_w);
		cal_angle(mean_f_w, angle0);
		InitInsResult(&result0, &ref_ins[0], angle0);
	}
	else {
		for(int i=0;i<3;i++)angle0[i] = ref_ins[0].attitude.angle[i];
		InitInsResult(&result0, &ref_ins[0], angle0);
	}
	
	//计算每个历元的结果
	std::cout << "开始解算" << std::endl;
	int last_index = 0;//上一次参考数据对应的索引
	for (int epoch = 1; epoch < imudata.size(); epoch++) {
		double dt = imudata[epoch].time - imudata[epoch - 1].time;
		if (abs(dt - (1.0 / HZ_align)) > 1e-6) {
			std::cout << "数据有跳跃" << dt << std::endl; std::exit(1);
		}
		if (epoch == 1) {//第一个历元
			my_eigen::Matrix temp1 = my_eigen::Matrix::zeros(3, 1);//仅占位
			my_eigen::Matrix temp2 = my_eigen::Matrix::zeros(3, 1);
			init_INSResult(&result1);
			result1.GPS_sec = imudata[epoch].time;
			result1.vel = UpdateVel(result0.pos, result0.vel, temp1, temp2, imudata[1], imudata[0], result0.attitude.C, 0, dt,fp_temp);
			result1.pos = UpdatePos(result0.pos, result0.vel, result1.vel, dt, fp_temp);
			result1.attitude = UpdateAttitude(result0.pos, result0.vel, result1.vel, result1.pos, imudata[1], imudata[0], result0.attitude, dt, fp_temp);
			result1.pos_enu = llhToenu(result1.pos);
			//精度评定 找参考数据相同时间数据
			INSResult result_diff;init_INSResult(&result_diff);
			for (int j = last_index; j < ref_ins.size(); j++) {
				if (abs(ref_ins[j].GPS_sec-imudata[epoch].time)<1e-6) {
					result_diff = CalDiff(&ref_ins[j], &result1); last_index = j; break;
				}
			}
			fprintf(fp_ins_result, "%20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf\n", 
				result1.GPS_sec, result1.pos(0, 0)*180/PI, result1.pos(1, 0)*180/PI, result1.pos(2, 0),result1.pos_enu(0, 0), result1.pos_enu(1, 0), result1.pos_enu(2, 0),
				result1.vel(0, 0), result1.vel(1, 0), result1.vel(2, 0), result1.attitude.angle[0]*180/PI, result1.attitude.angle[1]*180/PI, result1.attitude.angle[2]*180/PI,
				result_diff.pos(0, 0), result_diff.pos(1, 0), result_diff.pos(2, 0), result_diff.pos_enu(0, 0), result_diff.pos_enu(1, 0), result_diff.pos_enu(2, 0),
				result_diff.vel(0, 0), result_diff.vel(1, 0), result_diff.vel(2, 0), result_diff.attitude.angle[0]*180/PI, result_diff.attitude.angle[1]*180/PI, result_diff.attitude.angle[2]*180/PI);
		}
		else {
			init_INSResult(&result);
			result.GPS_sec = imudata[epoch].time;
			bool is_find = false;
			for (int j = last_index; j < ref_ins.size(); j++) {
				if (abs(ref_ins[j].GPS_sec - imudata[epoch].time) < 1e-6) {
					last_index = j; is_find = true; break;
				}
			}
			//零速修正
			if (false) {
			//if(sqrt(pow(ref_ins[last_index].vel(0,0),2)+ pow(ref_ins[last_index].vel(1, 0), 2)+ pow(ref_ins[last_index].vel(2, 0), 2))<0.01) {
				result.vel = my_eigen::Matrix::zeros(3, 1);
			}
			else result.vel = UpdateVel(result1.pos, result1.vel, result0.pos, result0.vel, imudata[epoch], imudata[epoch - 1], result1.attitude.C, 1, dt,fp_temp);
			result.pos = UpdatePos(result1.pos, result1.vel, result.vel, dt, fp_temp);
			result.attitude = UpdateAttitude(result1.pos, result1.vel, result.vel, result.pos, imudata[epoch], imudata[epoch - 1], result1.attitude, dt, fp_temp);
			result.pos_enu = llhToenu(result.pos);
			//精度评定 找参考数据相同时间数据
			INSResult result_diff; init_INSResult(&result_diff);
			if (is_find) {
				result_diff = CalDiff(&ref_ins[last_index], &result);
			}
			fprintf(fp_ins_result, "%20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf\n", 
				result.GPS_sec, result.pos(0, 0)*180/PI, result.pos(1, 0)*180/PI, result.pos(2, 0),result.pos_enu(0, 0), result.pos_enu(1, 0), result.pos_enu(2, 0),
				result.vel(0, 0), result.vel(1, 0), result.vel(2, 0), result.attitude.angle[0]*180/PI, result.attitude.angle[1]*180/PI, result.attitude.angle[2]*180/PI,
				result_diff.pos(0, 0), result_diff.pos(1, 0), result_diff.pos(2, 0), result_diff.pos_enu(0, 0), result_diff.pos_enu(1, 0), result_diff.pos_enu(2, 0),
				result_diff.vel(0, 0), result_diff.vel(1, 0), result_diff.vel(2, 0), result_diff.attitude.angle[0]*180/PI, result_diff.attitude.angle[1]*180/PI, result_diff.attitude.angle[2]*180/PI);
			result0 = result1;
			result1 = result;
		}
		std::cout << epoch << std::endl;
	}

	fclose(fp_ins_result);
	fclose(fp_temp);
	return 0;
}