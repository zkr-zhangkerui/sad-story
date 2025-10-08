/**
* @file
* @brief    This file gives the format of comments
*           SPP��λ������ؽṹ��ͺ���
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/05/21
* @version  1.0.20250521
* @par      History:
*           2025/05/21,kerui zhang
*/

#ifndef SPP_H //��ֹ�����ظ�����
#define SPP_H //���û�ж�����һ�о�ִ����һ��

#include "GNSSCmnFun.h"
#include "SatePos.h"


#define f1_G (1575.42)//MHz
#define f2_G (1227.6)//MHz
#define B1_I (1561.098)//MHz
#define B3_I (1268.520)//MHz

const double k_1_3 = pow(B1_I / B3_I, 2);

/**
* @brief ���㶨λ�ṹ��
*
* ��С���˽����������
* �������������������ؾ���
* ��λ�����xyz��enu��blh�µ�����
*/
struct Pos
{
	int true_num;//��ǰ���ù۲�ֵ����
	Eigen::MatrixXd P_IF;//α���޵�������
    Eigen::MatrixXd Z;
    Eigen::MatrixXd H;
    Eigen::MatrixXd H_change;
    Eigen::MatrixXd D;
    Eigen::MatrixXd Inv_D;
    Eigen::MatrixXd Q;
    Eigen::MatrixXd Inv_Q;
    Eigen::MatrixXd recpos;
    Eigen::MatrixXd recpos1;
    Eigen::MatrixXd recpos2;
    Eigen::MatrixXd recpos3;
    Eigen::MatrixXd recpos_0;
    Eigen::MatrixXd d_recpos;
	Eigen::MatrixXd Q1;

	//��λ��������
	Eigen::MatrixXd V;
	Eigen::MatrixXd std_after;
	Eigen::MatrixXd D_after;

    double recpos_blh[3];
	double recpos_enu[3];
	double origin[3];
	double origin_blh[3];
	double std;
	
	//�������
    Eigen::MatrixXd H_V;
	Eigen::MatrixXd H_V_change;
	Eigen::MatrixXd P_V;
	Eigen::MatrixXd Z_V;
	Eigen::MatrixXd Q_V;
	Eigen::MatrixXd Q_V_Inv;
	Eigen::MatrixXd vel;
	Eigen::MatrixXd D_obs;//�����չ۲�ֵ
	Eigen::MatrixXd V_vel;
	double std_after_vel;
	Eigen::MatrixXd D_after_vel;

	int row;
	int col;
	int sys=2;//��ʶ�ǵ�ϵͳ����˫ϵͳ��GPS��0��������1��˫��2
};


extern Pos pos;

/**
* @brief       spp������
* @param[in]   out_obs    Out_obs*    �۲����ݽṹ��
*              out_nav_g  Out_nav_g*  GPS����
*              out_nav_b  Out_nav_b*  ��������
*              pos        Pos*        �������þ���ͼ�����
*              out_obs0   Out_obs*    ��һ��Ԫ�۲����� һ���Լ���ʱʹ��
*              epochCount int&        ��Ԫ��
* @return      int  ��λ�����Ƿ�ɹ�
* @note
*/
extern int pvt_spp(Out_obs* out_obs, Out_nav* out_nav_g, Out_nav* out_nav_b, FILE* sate_pos_file,Pos* pos, Out_obs* out_obs0, int& epochCount);

/**
* @brief       �������Ǹ߶Ƚ�
* @param[in]   SatPos     SateResult*     ����λ�õ�����
*              RCVPos     Eigen::MatrixXd ���ջ�����λ��
* @return      double  ���Ǹ߶Ƚǣ����ȣ�
* @note
*/
extern double SatElev(SateResult* SatPos, Eigen::MatrixXd RCVPos);


//extern double Norm(int dim, double** M);
//extern double VectDot(int dim, double** M1, double** M2);
//extern double** alloc_2d_double(int rows, int cols);
//extern void free_2d_double(double** ptr);

/**
* @brief       ��ʼ���ṹ��
* @param[in]   pos        Pos*        �������þ���ͼ�����
*              n          int         ����
*              m          int         ����
* @return      ��
* @note
*/
extern void allocate_pos_memory(Pos* pos, int n, int m);
//extern void free_pos_memory(Pos* pos);
//extern void realloc_pos_intermediate(Pos* pos, Row_Col* rc);
//extern void init_matrix(double** mat, int rows, int cols, double value

/**
* @brief       λ�ü��� ȥ��ȫ���к��У�������������������
* @param[in]   pos        Pos*        �������þ���ͼ�����
*              threshold  double      ��Сֵ
*              dim        int         H��������
* @return      ��
* @note
*/
extern void remove_zero_rows_and_cols(Pos* pos, double threshold,int dim);

/**
* @brief       ȥ��ȫ����
* @param[in]   matrix     Eigen::MatrixXd&        ��Ҫ�����ľ���
*              threshold  double      ��Сֵ
* @return      Eigen::MatrixXd  ������ľ���
* @note
*/
extern Eigen::MatrixXd removeZeroRows(Eigen::MatrixXd& matrix, double threshold);

/**
* @brief       ȥ��ȫ����
* @param[in]   matrix     Eigen::MatrixXd&        ��Ҫ�����ľ���
*              threshold  double      ��Сֵ
* @return      Eigen::MatrixXd  ������ľ���
* @note
*/
extern Eigen::MatrixXd removeZeroCols(Eigen::MatrixXd& matrix, double threshold);

/**
* @brief       ��ȫ����
* @param[in]   Q     Eigen::MatrixXd&        ��Ҫ����ľ���
*              lambda  double      һ����С��������ֹ��������
* @return      Eigen::MatrixXd  �����ľ���
* @note
*/
extern Eigen::MatrixXd safeInverse(const Eigen::MatrixXd& Q, double lambda);

/**
* @brief       �ٶȼ��� ȥ��ȫ���к��У�������������������
* @param[in]   pos        Pos*        �������þ���ͼ�����
*              threshold  double      ��Сֵ
* @return      Eigen::MatrixXd  ������ľ���
* @note
*/
extern void remove_zero_rows_and_cols_V(Pos* pos, double threshold);

/**
* @brief       �������Ԫ�Ĺ۲�����
* @param[in]   out_obs0   Out_obs*    ��һ��Ԫ�۲����� һ���Լ���ʱʹ��
*              out_obs   Out_obs*     ����Ԫ�۲����� һ���Լ���ʱʹ��
* @return      ��
* @note
*/
extern void copy_obs(Out_obs* out_obs0, Out_obs* out_obs);

/**
* @brief       һ���Լ���
* @param[in]   out_obs0   Out_obs*    ��һ��Ԫ�۲�����
*              out_obs   Out_obs*     ����Ԫ�۲�����
*              epochCount  int        ��Ԫ����������һ����Ԫ
*              prn         int        ���м�������Ǻ�
*              sys         int        ���м��������ϵͳ
*              i           int        ����Ԫ�е�i���۲�����
* @return      �Ƿ�ͨ��һ���Լ���
* @note
*/
extern bool consistency_test(int epochCount, int prn, int sys, Out_obs* out_obs, Out_obs* out_obs0, int i);

#endif