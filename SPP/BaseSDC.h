/**
* @file
* @brief    This file gives the format of comments
*           定义不同时间系统的结构体
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/04/15
* @version  1.0.20250415
* @par      History:
*           2025/04/15,kerui zhang
*/

#ifndef BASE_SDC_H //防止定义重复包含
#define BASE_SDC_H //如果没有定义上一行就执行这一行

#include<math.h>
#include<string>

//define
#define PI                     (3.1415926535898)
#define IPS_EPSILON            (2.220446049250313e-16)//计算机的最小精度 两个值差的绝对值小于最小值就认为相等
#define D2R                    (0.0174532925199432957692369076849)//度转换到弧度
#define R2D                    (57.295779513082322864647721871734)//弧度转换到度
#define MIN(x,y)               ((x)<=(y)?(x):(y))//如果x<=y,返回x，否则返回y
#define MAX(x,y)               ((x)>=(y)?(x):(y))
#define SWAP(x,y)              (double z=x;x=y;y=z;)//交换x和y的值
#define FILE_ANTENNAINF        ("")//文件名 用/斜杠
#define ROUND(x)               (int)floor((x)+0.5)//四舍五入取整 floor向下取整
#define IPS_US                 unsigned short
#define WGS84_A                (6378137.0) // 长半轴
#define WGS84_B                (6356752.314245)  // 短半轴
#define WGS84_E_SQ             (6.69437999014e-3)  // 第一偏心率的平方


/**
* 枚举类型 cpp中用法 IPS_FILE_TYPE file_type=IPSDILE_TXT
* 等价于 int file_type
*/
enum IPS_FILE_TYPE {
	IPSFILE_NONE=0,              //默认
	IPSFILE_TXT,                 //文本文件 1           
	IPSFILE_BIN,                 //二进制文件 2
};

//const 电脑自动计算值 gs->globol static
static const double gs_min = D2R / 60.0;                       //一个单位角分对应的弧度
static const double gs_sec = D2R / 3500.0;                     //一个单位角秒对应的弧度
static const double gs_chisqr_0p999[100] = {}; 
const double SEC_PER_WEEK = 604800.0;
const int GPS_BDS_SECONDS_OFFSET = -14; // BDT = GPST - 14秒
const int GPS_BDS_WEEK_OFFSET = 1356;   // BDT起始时刻对应GPST的1356周
const double WGS84_F = 1.0 / 298.257223563;                    //地球扁率

//不同时间系统结构体

/**
* @brief 简化儒略历
* 
* 定义简化儒略历的整数部分和小数部分
* 并定义构造函数进行初始化为0
*/
struct MJD 
{
	unsigned long mjd;//简化儒略历的整数部分
	double fracofday;//一天中的小数部分

	MJD()
	{
		mjd = 0;
		fracofday = 0.0;
	}
};

/**
* @brief 年月日时分秒
*
* 定义公历的年月日和时分秒
* 定义构造函数进行初始化为0
* 定义含参数的构造函数进行赋值
*/
struct YMDHMS //年月日时分秒
{
	IPS_US year;
	IPS_US month;
	IPS_US day;
	IPS_US hour;
	IPS_US min;
	double sec;

	YMDHMS()
	{
		year = 0;
		month = 0;
		day = 0;
		hour = 0;
		min = 0;
		sec = 0.0;
	}

	YMDHMS(IPS_US year, IPS_US month, IPS_US day, IPS_US hour, IPS_US min, double sec)
	{
		this->year = year;
		this->month = month;
		this->day = day;
		this->hour = hour;
		this->min = min;
		this->sec = sec;
	}
};

/**
* @brief GPS/BDS时间
*
* 定义GPS系统和BDS系统的时间 周和周内秒
* 其中周内秒分整数部分和小数部分
* 定义构造函数进行初始化
* 定义含参数的构造函数进行赋值
* 定义操作符-进行时间相减
*/
struct GPSTIME //GPS时间
{
	int GPSweek;                                               //GPS 周
	int secsofweek;                                            //GPS 秒的整数部分
	double fracofsec;                                          //GPS 秒的小数部分 为了提高精度

	//构造函数 定义结构体时赋值
	GPSTIME()
	{
		GPSweek = -1;
		secsofweek = 0;
		fracofsec = 0;
	}

	//将外面给的时间赋值给结构体中元素
	GPSTIME(int week, double sec)
	{
		GPSweek = week;
		secsofweek = int(sec);
		fracofsec = sec - secsofweek;

		if (fracofsec < IPS_EPSILON) fracofsec = 0.0;
	}

	//GPS时相减 定义操作符 看作一个函数 可以定义加减乘和叹号
	double operator-(GPSTIME gt)
	{
		double lsec = (this->GPSweek - gt.GPSweek) * 604800 + secsofweek - gt.secsofweek;//this 指向当前结构体的指针
		double t = lsec + fracofsec - gt.fracofsec;
		return t;
		
	}
};


#endif