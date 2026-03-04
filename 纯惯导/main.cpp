/**
* @file
* @brief    主函数
* @details  设置文件路径，调用惯导相关函数
* @author   Kerui Zhang. Email: krzhang@whu.edu.cn
* @date     2025/12/03
* @version  1.1.20251203
* @par      无
*/

#include "ins.h"

int main() {
	//标定
	//main_calibration();
	//粗对准
	//main_align();
	//纯惯导算法
	std::string filename = "E:\\新建文件夹\\大三上\\惯导\\惯导机械编排\\group5.ASC";
	std::string reffile = "E:\\新建文件夹\\大三上\\惯导\\惯导机械编排\\IEproject5.ref";
	//std::string filename = "E:\\新建文件夹\\大三上\\惯导\\纯惯导程序调试示例数据\\01示例数据\\IMU.bin";
	//std::string reffile = "E:\\新建文件夹\\大三上\\惯导\\纯惯导程序调试示例数据\\01示例数据\\PureINS.bin";
	main_ins(filename, reffile);

	return 0;
}