/**
* @file
* @brief    This file gives the format of comments
*           解码相关结构体和函数
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/05/08
* @version  1.0.20250508
* @par      History:
*           2025/05/08,kerui zhang
*/

#ifndef OME_RINEX_H //防止定义重复包含
#define OME_RINEX_H //如果没有定义上一行就执行这一行

#include "..\BASEDLL\BaseSDC.h"
#include <ctime>
#include <iostream>
#include <cstdint>
#include <vcruntime_string.h>
#include <cstdio>

#define MAXRAWLEN   (16384)               /* max length of receiver raw message*/
#define MINNAVLEN   (196)                 /*广播星历的最短长度*/
#define OEM4SYNC1   (0xAA)                /* 同步字节1*/
#define OEM4SYNC2   (0x44)                /* 同步字节2*/
#define OEM4SYNC3   (0x12)                /* 同步字节3*/
#define OEM4HEADLEN (28)                  /*消息头的长度*/
#define POLYCRC32   0xEDB88320u           /* CRC32 polynomial */
#define RANGEID     (43)                  /*观测数据*/
#define GPSEPHID    (7)					  /*GPS星历*/
#define BDSEPHID    (1696)                /*北斗星历*/
#define ION         (8)                   /*klobuchar模型参数*/
#define GPS_SYS     (0)                   /*GPS系统*/
#define GPS_L1      (0)                   /*GPS系统 L1C/A*/
#define GPS_L2      (9)                   /*GPS系统 L2P*/
#define BDS_SYS     (4)                   /*北斗系统*/
#define BDS3_D1     (2)                   /*北斗3 ID1*/
#define BDS1_D1     (0)                   /*北斗1 ID1*/
#define BDS1_D2     (4)                   /*北斗1 ID2*/
#define BDS3_D2     (6)                   /*北斗3 ID2*/
#define MAXSATEGPS  (32)                  /*GPS系统的最大卫星数量*/
#define MAXSATEBDS  (63)                  /*北斗系统的最大卫星数量*/
#define SINGLAGPS   (2)                   /*GPS系统的最大频率数量*/
#define SINGLABDS   (2)                   /*北斗系统的最大频率数量*/
#define LENOFOBS    (44)                  /*一个观测值数据的长度*/
#define MAXOBS      (40960)               /*一个数据包最大的观测值数量*/

typedef unsigned char      uint8_t;
const int MAXSATE = MAXSATEBDS + MAXSATEGPS ;
const int MAXSINGLA = SINGLABDS + SINGLAGPS;
const char obs_types[2][8][32] = { {"C1C","L1C","D1C","S1C", "C2W","L2W", "D2W", "S2W"},
	{"C2I", "L2I", "D2I", "S2I", "C6I", "L6I", "D6I", "S6I"} };

/**
*@brief 临时存储解码数据
*
*临时存储观测信息
*/
struct Range
{
	int num;//观测次数
	int prn_num=0;//观测的卫星数
	int sys[MAXSATE];//每个卫星的系统
	int singla[2][MAXSATE];//每个卫星的信号类型
	int prn[MAXSATE];
	int glofreq;//频率吧？
	//double psr[MAXSATE][MAXSINGLA];//伪距观测值 m
	//float d_psr;//伪距观测值标准偏差 m
	//double adr[MAXSATE][MAXSINGLA];//载波相位观测值 周
	//float d_adr;//载波相位观测值标准偏差 周
	//float dopp[MAXSATE][MAXSINGLA];//瞬时多普勒 hz
	//float c_no[MAXSATE][MAXSINGLA];//载噪比 目前不用
	//double obs_t[MAXOBS][MAXSINGLA * 4];//存储所有观测值 顺序：载波相位 伪距 多普勒 载噪比 GPS：L1C/A L2P 北斗：D1/D3
	float locktime;//连续跟踪秒数（无周跳） 秒
	int ch_tr_states;//跟踪状态 是哪个系统哪个频率
	int sate_system;//卫星系统
	int signal_type;//信号类型
	GPSTIME t_obs;//数据观测的GPS时间
	YMDHMS ymdhms_obs;//第一个和最后一个数据观测的年月日时分秒时间
};

/**
*@brief 存储一颗卫星的广播星历数据
*
*北斗和GPS卫星使用一个结构体
*/
struct Eph
{
	int sys, singla;
	int prn;
	double tow;//卫星信号的发送时间 GPS周内秒
	double health;//卫星健康状态
	double IODE1, IODE2;//标识星历数据的版本,两个一致则认为星历数据有效
	int week;//星历参考时刻的周数
	double toe;//星历参考时刻 秒
	double A;//长半轴长 米
	double d_N;//平均角速度改正 弧度/s
	double M0;//参考时刻平近点角 弧度
	double e;//偏心率
	double omage;//近地点角距 弧度
	//摄动改正项 弧度
	double cuc, cus, crc, crs, cic, cis;
	double I0;//参考时刻轨道倾角 弧度
	double d_I;//轨道倾角变化率 弧度/s
	double omage0;//升交点赤经 弧度
	double d_omage0;//升交点赤经变化率 弧度/s
	double IODC;//钟的数据龄期
	double toc;//钟的参考时刻吧？ 秒
	double tgd[2];//时延 TGD 秒 GPS有一个，BDS有两个
	double f0;//钟差
	double f1;//钟偏
	double f2;//钟漂
	double AS;//是否有AS政策 0否 1是
	double N;//不知道是啥
	double URA;//用户测距精度 m平方
	double sqrt_A;//长半轴长 根号米
	//北斗特有
	double AODC;//钟的数据龄期
	double AODE;//星历的数据龄期
	
};


/**
*@brief 存储一个历元的观测数据信息
*
*/
struct Out_obs
{
	double obs_t[MAXOBS][MAXSINGLA * 4];//存储所有观测值 顺序：载波相位 伪距 多普勒 载噪比 GPS：L1C/A L2P 北斗：D1/D3
	YMDHMS ymdhms_obs;//观测时间
	GPSTIME t_obs;
	int prn_num;//观测的卫星数
	int prn[MAXOBS];
	int sys[MAXOBS];
	int num;//观测值数量
	int singla[MAXOBS];//卫星信号类型
	
};

/**
*@brief 存储一组最新的星历 用于输出和SPP解算
*
*/
struct Out_nav
{
	int is_change;//标记这个卫星的星历是否更新
	int prn[MAXOBS];
	double toc[MAXOBS];
	double toe[MAXOBS];
	int week[MAXOBS];
	double health[MAXOBS];
	int num ;//计数 第几个星历
	double TGD[2];
	Eph out_eph[MAXSATEBDS+MAXSATEGPS];//保存最新的一组星历
	Eph cal_eph[MAXSATEBDS + MAXSATEGPS];//用于迭代计算的星历
};

/**
*@brief 存储原始二进制数据和部分该数据包的信息
*
*/
struct Raw
{
	int len;//数据的总长度
	uint8_t buff[MAXOBS];//缓冲区
	//uint8_t* buff;//缓冲区
	//int buff_len;//缓冲区长度
	int msg_len;//消息体的长度
	int msgID;//消息的类型
	Range range;//观测值结构体
	Eph eph;//广播星历结构体
};

/*观测信号类型*/
struct SYS_OBS_TYPES
{
	char sys_types;//卫星系统
	int num;//该系统的卫星数量
	char* sys_obs;//观测值类型
};

/*相位校正*/
struct Phase_shift
{
	char sys_types;//卫星系统
	char* sys_obs_phase;//相位观测值类型
	double correct;//改正 周
	int num;//涉及的卫星数
	char* sate_list;//卫星列表
};

/*输出头文件结构体*/
struct Header
{
	double version = 3.04;//rinex版本
	char sate_sys;//卫星系统
	char PGM[100];//Name of program creating current file
	char RUN_BY[100];//Name of agency creating current file 
	char date[20];//Date and time of file creation UTC
	char comment[100];
	char MARKER_NAME;//Name of antenna marker
	char MARKER_NUMBER;//Number of antenna marker
	char MARKER_TYPE;//Type of the marker
	char OBSERVER;//Name of observer / agency 
	char* REC_num;//Receiver number
	char* REC_type;//Receiver type
	char* REC_ver; //Receiver version
	char ANT_type;//Antenna number and type 
	double pos_xyz[3];//接收机坐标 m
	double pos_enu[3];//天线参考中心坐标 m
	SYS_OBS_TYPES SysObsTypes;
	double DBHZ;//载噪比
	double interval;//采样间隔
	YMDHMS begin_time;//第一个观测值的记录时间
	YMDHMS over_time;//最后一个观测值的记录时间
	Phase_shift phase_shift;//相位校正

};


/**
*@brief kolubuchatr模型进行电离层延迟改正的参数
*
*/
struct Ion
{
	double a1;
	double a2;
	double a3;
	double a4;
	double b1;
	double b2;
	double b3;
	double b4;
	double deltat_ls;//当前闰秒数
};


/**
*@brief 不同解算策略的设置
*
*/
struct option
{
	int fre;//0：单频 1：双频
	int sys;//0：单GPS 1：单北斗 2：双系统
	int is_ZTD;//0：进行对流层改正 1：不进行对流层改正
	int is_consistant;//0：进行一致性检验 1：不进行一致性检验
};

//声明
extern Range range;
extern Raw raw;
extern Header header;
extern Out_obs out_obs,out_obs0;
extern Out_nav out_nav_g, out_nav_b;
extern Ion ion;
extern option opt;

/**
*@brief       同步、数据头解码函数
*@param[in]   data          char      每次从文件中读取的一个字符
*             raw           Raw*      一个数据包的数据
*             sate_pos_file FILE*     输出文件指针
*@return      int 返回是否解码成功
*@note
*/
extern int header_oem4(char data, Raw* raw,FILE*sate_pos_file);

/**
* @brief      观测值数据解码函数
* @param[in]  raw      Raw*     原始观测数据
*             out_obs  Out_obs* 该历元观测数据
* @return     int 返回解码是否成功
*/
extern int decode_range(Raw* raw, Out_obs* out_obs);

/**
* @brief      GPS星历数据解码函数
* @param[in]  raw      Raw*     原始观测数据
*             out_nav  Out_nav* 最新一组卫星星历数据
* @return     int 返回解码是否成功
*/
extern int decode_gpseph(Raw* raw,Out_nav*out_nav);

/**
* @brief      北斗星历数据解码函数
* @param[in]  raw      Raw*     原始观测数据
*             out_nav  Out_nav* 最新一组卫星星历数据
* @return     int 返回解码是否成功
*/
extern int decode_bdseph(Raw* raw, Out_nav* out_nav);

/**
* @brief      星历数据输出函数
* @param[in]  raw      Raw*     原始观测数据
*             nav      FILE*    指向输出文件的指针
*             header   Header*  指向输出文件头结构体的指针
*             out_nav  Out_nav* 最新一组卫星星历数据
* @return     int 返回输出是否成功
*/
extern int outnav(Raw* raw, FILE* nav, Header* header, Out_nav* out_nav);

/**
* @brief      观测数据输出函数
* @param[in]  out_obs  Out_obs* 该历元观测数据
*             obs      FILE*    指向输出文件的指针
* @return     int 返回输出是否成功
*/
extern int outobs(Out_obs*out_obs, FILE* obs);

/**
* @brief      星历数据头文件输出函数
* @param[in]  raw      Raw*     原始观测数据
*             nav      FILE*    指向输出文件的指针
*             header   Header*  指向输出文件头结构体的指针
* @return     int 返回输出是否成功
*/
extern int outnavh(Raw* raw, FILE* nav, Header* header);

/**
* @brief      观测数据头文件输出函数
* @param[in]  raw      Raw*     原始观测数据
*             obs      FILE*    指向输出文件的指针
*             header   Header*  指向输出文件头结构体的指针
* @return     int 返回输出是否成功
*/
extern int outobsh(Raw* raw, FILE* obs, Header* header);

/**
* @brief      获取卫星观测信号
* @param[in]  raw      Raw*     原始观测数据
*             i        int      该历元的第i个观测数据
* @return     int 返回输出是否成功
*/
extern int get_singla(Raw* raw,int i);

/**
* @brief      获取卫星系统
* @param[in]  raw      Raw*     原始观测数据
*             i        int      该历元的第i个观测数据
* @return     int 返回输出是否成功
*/
extern int get_sys(Raw* raw, int i);

/**
* @brief      文件数据处理入口
* @param[in]  fileame    char*    文件名
* @return     int 
*/
extern int main_ome4(char* filename);

/**
* @brief      解码电离层八参数
* @param[in]  raw      Raw*     原始观测数据
*             ion      Ion*     电离层八参数结构体
* @return     int
*/
extern int decode_ion(Raw* raw, Ion* ion);

/**
* @brief      CRC校验
* @param[in]  buff    uint8_t*    数据缓冲区
*             len     int         缓冲区长度
* @return     uint32_t   返回校验是否成功
*/
extern uint32_t rtk_crc32(const uint8_t* buff, int len);

/**
* @brief      导航电文格式转换
* @param[in]  value    double    需要转换的值
*             fp       FILE*     指向输出文件的指针
*             n        int       科学计数法的数
*/
extern void outnavf_n(FILE* fp, double value, int n);

//extern void free_raw(Raw* raw);
//extern int init_raw(Raw* raw);
//extern int init_out_obs(Out_obs* out_obs);
//extern int init_out_nav(Out_nav* out_nav);
//extern int free_out_nav(Out_nav* out_nav);
//extern int free_out_obs(Out_obs* out_obs);
static uint16_t U2(uint8_t* p) { uint16_t u; memcpy(&u, p, 2); return u; }
static uint32_t U4(uint8_t* p) { uint32_t u; memcpy(&u, p, 4); return u; }
static int32_t  I4(uint8_t* p) { int32_t  i; memcpy(&i, p, 4); return i; }
static float    R4(uint8_t* p) { float    r; memcpy(&r, p, 4); return r; }
static double   R8(uint8_t* p) { double   r; memcpy(&r, p, 8); return r; }

/**
* @brief      查找一个元素 goal 在当前数组中是否存在,若存在返回其索引值
* @param[in]  goal      T      查找目标
*             array[]   T      当前数组
*             size      int    数组大小
* @return     int   返回该元素在该数组中的索引值
*/
template<class T>//固定写法 dst是double还是int还是别的都可以直接用
extern int find(T goal, T array[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (array[i] == goal)
		{
			return i;
		}
	}
	return -1;
}

#endif