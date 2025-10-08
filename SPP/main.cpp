//入口函数
//查看指针变量 指针名字后加，加要看的数量

#include "..\SateNavDLL\GNSSDLL\ome_rinex.h"
#include "..\SateNavDLL\GNSSDLL\read_ip.h"

option opt = {
		1,//双频
		2,//双系统
		0,//改对流层
		1//不进行一致性检验
};


int main(int argc, char* argv[])
{
	const char* str = "dbwicbeh";
	char filename[] = "E:\\myproject\\SateNav\\NovatelOEM20211114-01.log";
	char filename1[] = "E:\\myproject\\SateNav\\数据\\NovatelOEM20211114-01.nav";

	READ_IP readIp = {
		"47.114.134.129",  // 自动填充NetIP数组
		7190,              // 赋值给NetPort
		"NovatelOEM20211114-01_ip.log" // 自动填充ObsDatFile
	};

	int isReadFile=0;//1：实时数据流 0：文件
	if (isReadFile == 0)return main_ome4(filename);
	else return  main_Ip(readIp,raw);

}