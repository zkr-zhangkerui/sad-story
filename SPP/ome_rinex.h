/**
* @file
* @brief    This file gives the format of comments
*           ������ؽṹ��ͺ���
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/05/08
* @version  1.0.20250508
* @par      History:
*           2025/05/08,kerui zhang
*/

#ifndef OME_RINEX_H //��ֹ�����ظ�����
#define OME_RINEX_H //���û�ж�����һ�о�ִ����һ��

#include "..\BASEDLL\BaseSDC.h"
#include <ctime>
#include <iostream>
#include <cstdint>
#include <vcruntime_string.h>
#include <cstdio>

#define MAXRAWLEN   (16384)               /* max length of receiver raw message*/
#define MINNAVLEN   (196)                 /*�㲥��������̳���*/
#define OEM4SYNC1   (0xAA)                /* ͬ���ֽ�1*/
#define OEM4SYNC2   (0x44)                /* ͬ���ֽ�2*/
#define OEM4SYNC3   (0x12)                /* ͬ���ֽ�3*/
#define OEM4HEADLEN (28)                  /*��Ϣͷ�ĳ���*/
#define POLYCRC32   0xEDB88320u           /* CRC32 polynomial */
#define RANGEID     (43)                  /*�۲�����*/
#define GPSEPHID    (7)					  /*GPS����*/
#define BDSEPHID    (1696)                /*��������*/
#define ION         (8)                   /*klobucharģ�Ͳ���*/
#define GPS_SYS     (0)                   /*GPSϵͳ*/
#define GPS_L1      (0)                   /*GPSϵͳ L1C/A*/
#define GPS_L2      (9)                   /*GPSϵͳ L2P*/
#define BDS_SYS     (4)                   /*����ϵͳ*/
#define BDS3_D1     (2)                   /*����3 ID1*/
#define BDS1_D1     (0)                   /*����1 ID1*/
#define BDS1_D2     (4)                   /*����1 ID2*/
#define BDS3_D2     (6)                   /*����3 ID2*/
#define MAXSATEGPS  (32)                  /*GPSϵͳ�������������*/
#define MAXSATEBDS  (63)                  /*����ϵͳ�������������*/
#define SINGLAGPS   (2)                   /*GPSϵͳ�����Ƶ������*/
#define SINGLABDS   (2)                   /*����ϵͳ�����Ƶ������*/
#define LENOFOBS    (44)                  /*һ���۲�ֵ���ݵĳ���*/
#define MAXOBS      (40960)               /*һ�����ݰ����Ĺ۲�ֵ����*/

typedef unsigned char      uint8_t;
const int MAXSATE = MAXSATEBDS + MAXSATEGPS ;
const int MAXSINGLA = SINGLABDS + SINGLAGPS;
const char obs_types[2][8][32] = { {"C1C","L1C","D1C","S1C", "C2W","L2W", "D2W", "S2W"},
	{"C2I", "L2I", "D2I", "S2I", "C6I", "L6I", "D6I", "S6I"} };

/**
*@brief ��ʱ�洢��������
*
*��ʱ�洢�۲���Ϣ
*/
struct Range
{
	int num;//�۲����
	int prn_num=0;//�۲��������
	int sys[MAXSATE];//ÿ�����ǵ�ϵͳ
	int singla[2][MAXSATE];//ÿ�����ǵ��ź�����
	int prn[MAXSATE];
	int glofreq;//Ƶ�ʰɣ�
	//double psr[MAXSATE][MAXSINGLA];//α��۲�ֵ m
	//float d_psr;//α��۲�ֵ��׼ƫ�� m
	//double adr[MAXSATE][MAXSINGLA];//�ز���λ�۲�ֵ ��
	//float d_adr;//�ز���λ�۲�ֵ��׼ƫ�� ��
	//float dopp[MAXSATE][MAXSINGLA];//˲ʱ������ hz
	//float c_no[MAXSATE][MAXSINGLA];//����� Ŀǰ����
	//double obs_t[MAXOBS][MAXSINGLA * 4];//�洢���й۲�ֵ ˳���ز���λ α�� ������ ����� GPS��L1C/A L2P ������D1/D3
	float locktime;//���������������������� ��
	int ch_tr_states;//����״̬ ���ĸ�ϵͳ�ĸ�Ƶ��
	int sate_system;//����ϵͳ
	int signal_type;//�ź�����
	GPSTIME t_obs;//���ݹ۲��GPSʱ��
	YMDHMS ymdhms_obs;//��һ�������һ�����ݹ۲��������ʱ����ʱ��
};

/**
*@brief �洢һ�����ǵĹ㲥��������
*
*������GPS����ʹ��һ���ṹ��
*/
struct Eph
{
	int sys, singla;
	int prn;
	double tow;//�����źŵķ���ʱ�� GPS������
	double health;//���ǽ���״̬
	double IODE1, IODE2;//��ʶ�������ݵİ汾,����һ������Ϊ����������Ч
	int week;//�����ο�ʱ�̵�����
	double toe;//�����ο�ʱ�� ��
	double A;//�����᳤ ��
	double d_N;//ƽ�����ٶȸ��� ����/s
	double M0;//�ο�ʱ��ƽ����� ����
	double e;//ƫ����
	double omage;//���ص�Ǿ� ����
	//�㶯������ ����
	double cuc, cus, crc, crs, cic, cis;
	double I0;//�ο�ʱ�̹����� ����
	double d_I;//�����Ǳ仯�� ����/s
	double omage0;//������ྭ ����
	double d_omage0;//������ྭ�仯�� ����/s
	double IODC;//�ӵ���������
	double toc;//�ӵĲο�ʱ�̰ɣ� ��
	double tgd[2];//ʱ�� TGD �� GPS��һ����BDS������
	double f0;//�Ӳ�
	double f1;//��ƫ
	double f2;//��Ư
	double AS;//�Ƿ���AS���� 0�� 1��
	double N;//��֪����ɶ
	double URA;//�û���ྫ�� mƽ��
	double sqrt_A;//�����᳤ ������
	//��������
	double AODC;//�ӵ���������
	double AODE;//��������������
	
};


/**
*@brief �洢һ����Ԫ�Ĺ۲�������Ϣ
*
*/
struct Out_obs
{
	double obs_t[MAXOBS][MAXSINGLA * 4];//�洢���й۲�ֵ ˳���ز���λ α�� ������ ����� GPS��L1C/A L2P ������D1/D3
	YMDHMS ymdhms_obs;//�۲�ʱ��
	GPSTIME t_obs;
	int prn_num;//�۲��������
	int prn[MAXOBS];
	int sys[MAXOBS];
	int num;//�۲�ֵ����
	int singla[MAXOBS];//�����ź�����
	
};

/**
*@brief �洢һ�����µ����� ���������SPP����
*
*/
struct Out_nav
{
	int is_change;//���������ǵ������Ƿ����
	int prn[MAXOBS];
	double toc[MAXOBS];
	double toe[MAXOBS];
	int week[MAXOBS];
	double health[MAXOBS];
	int num ;//���� �ڼ�������
	double TGD[2];
	Eph out_eph[MAXSATEBDS+MAXSATEGPS];//�������µ�һ������
	Eph cal_eph[MAXSATEBDS + MAXSATEGPS];//���ڵ������������
};

/**
*@brief �洢ԭʼ���������ݺͲ��ָ����ݰ�����Ϣ
*
*/
struct Raw
{
	int len;//���ݵ��ܳ���
	uint8_t buff[MAXOBS];//������
	//uint8_t* buff;//������
	//int buff_len;//����������
	int msg_len;//��Ϣ��ĳ���
	int msgID;//��Ϣ������
	Range range;//�۲�ֵ�ṹ��
	Eph eph;//�㲥�����ṹ��
};

/*�۲��ź�����*/
struct SYS_OBS_TYPES
{
	char sys_types;//����ϵͳ
	int num;//��ϵͳ����������
	char* sys_obs;//�۲�ֵ����
};

/*��λУ��*/
struct Phase_shift
{
	char sys_types;//����ϵͳ
	char* sys_obs_phase;//��λ�۲�ֵ����
	double correct;//���� ��
	int num;//�漰��������
	char* sate_list;//�����б�
};

/*���ͷ�ļ��ṹ��*/
struct Header
{
	double version = 3.04;//rinex�汾
	char sate_sys;//����ϵͳ
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
	double pos_xyz[3];//���ջ����� m
	double pos_enu[3];//���߲ο��������� m
	SYS_OBS_TYPES SysObsTypes;
	double DBHZ;//�����
	double interval;//�������
	YMDHMS begin_time;//��һ���۲�ֵ�ļ�¼ʱ��
	YMDHMS over_time;//���һ���۲�ֵ�ļ�¼ʱ��
	Phase_shift phase_shift;//��λУ��

};


/**
*@brief kolubuchatrģ�ͽ��е�����ӳٸ����Ĳ���
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
	double deltat_ls;//��ǰ������
};


/**
*@brief ��ͬ������Ե�����
*
*/
struct option
{
	int fre;//0����Ƶ 1��˫Ƶ
	int sys;//0����GPS 1�������� 2��˫ϵͳ
	int is_ZTD;//0�����ж�������� 1�������ж��������
	int is_consistant;//0������һ���Լ��� 1��������һ���Լ���
};

//����
extern Range range;
extern Raw raw;
extern Header header;
extern Out_obs out_obs,out_obs0;
extern Out_nav out_nav_g, out_nav_b;
extern Ion ion;
extern option opt;

/**
*@brief       ͬ��������ͷ���뺯��
*@param[in]   data          char      ÿ�δ��ļ��ж�ȡ��һ���ַ�
*             raw           Raw*      һ�����ݰ�������
*             sate_pos_file FILE*     ����ļ�ָ��
*@return      int �����Ƿ����ɹ�
*@note
*/
extern int header_oem4(char data, Raw* raw,FILE*sate_pos_file);

/**
* @brief      �۲�ֵ���ݽ��뺯��
* @param[in]  raw      Raw*     ԭʼ�۲�����
*             out_obs  Out_obs* ����Ԫ�۲�����
* @return     int ���ؽ����Ƿ�ɹ�
*/
extern int decode_range(Raw* raw, Out_obs* out_obs);

/**
* @brief      GPS�������ݽ��뺯��
* @param[in]  raw      Raw*     ԭʼ�۲�����
*             out_nav  Out_nav* ����һ��������������
* @return     int ���ؽ����Ƿ�ɹ�
*/
extern int decode_gpseph(Raw* raw,Out_nav*out_nav);

/**
* @brief      �����������ݽ��뺯��
* @param[in]  raw      Raw*     ԭʼ�۲�����
*             out_nav  Out_nav* ����һ��������������
* @return     int ���ؽ����Ƿ�ɹ�
*/
extern int decode_bdseph(Raw* raw, Out_nav* out_nav);

/**
* @brief      ���������������
* @param[in]  raw      Raw*     ԭʼ�۲�����
*             nav      FILE*    ָ������ļ���ָ��
*             header   Header*  ָ������ļ�ͷ�ṹ���ָ��
*             out_nav  Out_nav* ����һ��������������
* @return     int ��������Ƿ�ɹ�
*/
extern int outnav(Raw* raw, FILE* nav, Header* header, Out_nav* out_nav);

/**
* @brief      �۲������������
* @param[in]  out_obs  Out_obs* ����Ԫ�۲�����
*             obs      FILE*    ָ������ļ���ָ��
* @return     int ��������Ƿ�ɹ�
*/
extern int outobs(Out_obs*out_obs, FILE* obs);

/**
* @brief      ��������ͷ�ļ��������
* @param[in]  raw      Raw*     ԭʼ�۲�����
*             nav      FILE*    ָ������ļ���ָ��
*             header   Header*  ָ������ļ�ͷ�ṹ���ָ��
* @return     int ��������Ƿ�ɹ�
*/
extern int outnavh(Raw* raw, FILE* nav, Header* header);

/**
* @brief      �۲�����ͷ�ļ��������
* @param[in]  raw      Raw*     ԭʼ�۲�����
*             obs      FILE*    ָ������ļ���ָ��
*             header   Header*  ָ������ļ�ͷ�ṹ���ָ��
* @return     int ��������Ƿ�ɹ�
*/
extern int outobsh(Raw* raw, FILE* obs, Header* header);

/**
* @brief      ��ȡ���ǹ۲��ź�
* @param[in]  raw      Raw*     ԭʼ�۲�����
*             i        int      ����Ԫ�ĵ�i���۲�����
* @return     int ��������Ƿ�ɹ�
*/
extern int get_singla(Raw* raw,int i);

/**
* @brief      ��ȡ����ϵͳ
* @param[in]  raw      Raw*     ԭʼ�۲�����
*             i        int      ����Ԫ�ĵ�i���۲�����
* @return     int ��������Ƿ�ɹ�
*/
extern int get_sys(Raw* raw, int i);

/**
* @brief      �ļ����ݴ������
* @param[in]  fileame    char*    �ļ���
* @return     int 
*/
extern int main_ome4(char* filename);

/**
* @brief      ��������˲���
* @param[in]  raw      Raw*     ԭʼ�۲�����
*             ion      Ion*     �����˲����ṹ��
* @return     int
*/
extern int decode_ion(Raw* raw, Ion* ion);

/**
* @brief      CRCУ��
* @param[in]  buff    uint8_t*    ���ݻ�����
*             len     int         ����������
* @return     uint32_t   ����У���Ƿ�ɹ�
*/
extern uint32_t rtk_crc32(const uint8_t* buff, int len);

/**
* @brief      �������ĸ�ʽת��
* @param[in]  value    double    ��Ҫת����ֵ
*             fp       FILE*     ָ������ļ���ָ��
*             n        int       ��ѧ����������
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
* @brief      ����һ��Ԫ�� goal �ڵ�ǰ�������Ƿ����,�����ڷ���������ֵ
* @param[in]  goal      T      ����Ŀ��
*             array[]   T      ��ǰ����
*             size      int    �����С
* @return     int   ���ظ�Ԫ���ڸ������е�����ֵ
*/
template<class T>//�̶�д�� dst��double����int���Ǳ�Ķ�����ֱ����
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