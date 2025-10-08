/**
* @file
* @brief    This file gives the format of comments
*           ���岻ͬʱ��ϵͳ�Ľṹ��
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/04/15
* @version  1.0.20250415
* @par      History:
*           2025/04/15,kerui zhang
*/

#ifndef BASE_SDC_H //��ֹ�����ظ�����
#define BASE_SDC_H //���û�ж�����һ�о�ִ����һ��

#include<math.h>
#include<string>

//define
#define PI                     (3.1415926535898)
#define IPS_EPSILON            (2.220446049250313e-16)//���������С���� ����ֵ��ľ���ֵС����Сֵ����Ϊ���
#define D2R                    (0.0174532925199432957692369076849)//��ת��������
#define R2D                    (57.295779513082322864647721871734)//����ת������
#define MIN(x,y)               ((x)<=(y)?(x):(y))//���x<=y,����x�����򷵻�y
#define MAX(x,y)               ((x)>=(y)?(x):(y))
#define SWAP(x,y)              (double z=x;x=y;y=z;)//����x��y��ֵ
#define FILE_ANTENNAINF        ("")//�ļ��� ��/б��
#define ROUND(x)               (int)floor((x)+0.5)//��������ȡ�� floor����ȡ��
#define IPS_US                 unsigned short
#define WGS84_A                (6378137.0) // ������
#define WGS84_B                (6356752.314245)  // �̰���
#define WGS84_E_SQ             (6.69437999014e-3)  // ��һƫ���ʵ�ƽ��


/**
* ö������ cpp���÷� IPS_FILE_TYPE file_type=IPSDILE_TXT
* �ȼ��� int file_type
*/
enum IPS_FILE_TYPE {
	IPSFILE_NONE=0,              //Ĭ��
	IPSFILE_TXT,                 //�ı��ļ� 1           
	IPSFILE_BIN,                 //�������ļ� 2
};

//const �����Զ�����ֵ gs->globol static
static const double gs_min = D2R / 60.0;                       //һ����λ�Ƿֶ�Ӧ�Ļ���
static const double gs_sec = D2R / 3500.0;                     //һ����λ�����Ӧ�Ļ���
static const double gs_chisqr_0p999[100] = {}; 
const double SEC_PER_WEEK = 604800.0;
const int GPS_BDS_SECONDS_OFFSET = -14; // BDT = GPST - 14��
const int GPS_BDS_WEEK_OFFSET = 1356;   // BDT��ʼʱ�̶�ӦGPST��1356��
const double WGS84_F = 1.0 / 298.257223563;                    //�������

//��ͬʱ��ϵͳ�ṹ��

/**
* @brief ��������
* 
* ��������������������ֺ�С������
* �����幹�캯�����г�ʼ��Ϊ0
*/
struct MJD 
{
	unsigned long mjd;//������������������
	double fracofday;//һ���е�С������

	MJD()
	{
		mjd = 0;
		fracofday = 0.0;
	}
};

/**
* @brief ������ʱ����
*
* ���幫���������պ�ʱ����
* ���幹�캯�����г�ʼ��Ϊ0
* ���庬�����Ĺ��캯�����и�ֵ
*/
struct YMDHMS //������ʱ����
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
* @brief GPS/BDSʱ��
*
* ����GPSϵͳ��BDSϵͳ��ʱ�� �ܺ�������
* ������������������ֺ�С������
* ���幹�캯�����г�ʼ��
* ���庬�����Ĺ��캯�����и�ֵ
* ���������-����ʱ�����
*/
struct GPSTIME //GPSʱ��
{
	int GPSweek;                                               //GPS ��
	int secsofweek;                                            //GPS �����������
	double fracofsec;                                          //GPS ���С������ Ϊ����߾���

	//���캯�� ����ṹ��ʱ��ֵ
	GPSTIME()
	{
		GPSweek = -1;
		secsofweek = 0;
		fracofsec = 0;
	}

	//���������ʱ�丳ֵ���ṹ����Ԫ��
	GPSTIME(int week, double sec)
	{
		GPSweek = week;
		secsofweek = int(sec);
		fracofsec = sec - secsofweek;

		if (fracofsec < IPS_EPSILON) fracofsec = 0.0;
	}

	//GPSʱ��� ��������� ����һ������ ���Զ���Ӽ��˺�̾��
	double operator-(GPSTIME gt)
	{
		double lsec = (this->GPSweek - gt.GPSweek) * 604800 + secsofweek - gt.secsofweek;//this ָ��ǰ�ṹ���ָ��
		double t = lsec + fracofsec - gt.fracofsec;
		return t;
		
	}
};


#endif