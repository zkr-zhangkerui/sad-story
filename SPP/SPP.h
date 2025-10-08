/**
* @file
* @brief    This file gives the format of comments
*           SPP定位解算相关结构体和函数
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/05/21
* @version  1.0.20250521
* @par      History:
*           2025/05/21,kerui zhang
*/

#ifndef SPP_H //防止定义重复包含
#define SPP_H //如果没有定义上一行就执行这一行

#include "GNSSCmnFun.h"
#include "SatePos.h"


#define f1_G (1575.42)//MHz
#define f2_G (1227.6)//MHz
#define B1_I (1561.098)//MHz
#define B3_I (1268.520)//MHz

const double k_1_3 = pow(B1_I / B3_I, 2);

/**
* @brief 单点定位结构体
*
* 最小二乘解算所需矩阵
* 精度评定，单点测速相关矩阵
* 定位结果在xyz，enu，blh下的坐标
*/
struct Pos
{
	int true_num;//当前可用观测值个数
	Eigen::MatrixXd P_IF;//伪距无电离层组合
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

	//定位精度评定
	Eigen::MatrixXd V;
	Eigen::MatrixXd std_after;
	Eigen::MatrixXd D_after;

    double recpos_blh[3];
	double recpos_enu[3];
	double origin[3];
	double origin_blh[3];
	double std;
	
	//单点测速
    Eigen::MatrixXd H_V;
	Eigen::MatrixXd H_V_change;
	Eigen::MatrixXd P_V;
	Eigen::MatrixXd Z_V;
	Eigen::MatrixXd Q_V;
	Eigen::MatrixXd Q_V_Inv;
	Eigen::MatrixXd vel;
	Eigen::MatrixXd D_obs;//多普勒观测值
	Eigen::MatrixXd V_vel;
	double std_after_vel;
	Eigen::MatrixXd D_after_vel;

	int row;
	int col;
	int sys=2;//标识是单系统还是双系统，GPS：0，北斗：1，双：2
};


extern Pos pos;

/**
* @brief       spp主函数
* @param[in]   out_obs    Out_obs*    观测数据结构体
*              out_nav_g  Out_nav_g*  GPS星历
*              out_nav_b  Out_nav_b*  北斗星历
*              pos        Pos*        计算所用矩阵和计算结果
*              out_obs0   Out_obs*    上一历元观测数据 一致性检验时使用
*              epochCount int&        历元数
* @return      int  定位解算是否成功
* @note
*/
extern int pvt_spp(Out_obs* out_obs, Out_nav* out_nav_g, Out_nav* out_nav_b, FILE* sate_pos_file,Pos* pos, Out_obs* out_obs0, int& epochCount);

/**
* @brief       计算卫星高度角
* @param[in]   SatPos     SateResult*     卫星位置等数据
*              RCVPos     Eigen::MatrixXd 接收机近似位置
* @return      double  卫星高度角（弧度）
* @note
*/
extern double SatElev(SateResult* SatPos, Eigen::MatrixXd RCVPos);


//extern double Norm(int dim, double** M);
//extern double VectDot(int dim, double** M1, double** M2);
//extern double** alloc_2d_double(int rows, int cols);
//extern void free_2d_double(double** ptr);

/**
* @brief       初始化结构体
* @param[in]   pos        Pos*        计算所用矩阵和计算结果
*              n          int         行数
*              m          int         列数
* @return      无
* @note
*/
extern void allocate_pos_memory(Pos* pos, int n, int m);
//extern void free_pos_memory(Pos* pos);
//extern void realloc_pos_intermediate(Pos* pos, Row_Col* rc);
//extern void init_matrix(double** mat, int rows, int cols, double value

/**
* @brief       位置计算 去除全零行和列，调整行列数便于运算
* @param[in]   pos        Pos*        计算所用矩阵和计算结果
*              threshold  double      极小值
*              dim        int         H矩阵列数
* @return      无
* @note
*/
extern void remove_zero_rows_and_cols(Pos* pos, double threshold,int dim);

/**
* @brief       去除全零行
* @param[in]   matrix     Eigen::MatrixXd&        需要调整的矩阵
*              threshold  double      极小值
* @return      Eigen::MatrixXd  调整后的矩阵
* @note
*/
extern Eigen::MatrixXd removeZeroRows(Eigen::MatrixXd& matrix, double threshold);

/**
* @brief       去除全零列
* @param[in]   matrix     Eigen::MatrixXd&        需要调整的矩阵
*              threshold  double      极小值
* @return      Eigen::MatrixXd  调整后的矩阵
* @note
*/
extern Eigen::MatrixXd removeZeroCols(Eigen::MatrixXd& matrix, double threshold);

/**
* @brief       安全求逆
* @param[in]   Q     Eigen::MatrixXd&        需要求逆的矩阵
*              lambda  double      一个极小的数，防止矩阵奇异
* @return      Eigen::MatrixXd  求逆后的矩阵
* @note
*/
extern Eigen::MatrixXd safeInverse(const Eigen::MatrixXd& Q, double lambda);

/**
* @brief       速度计算 去除全零行和列，调整行列数便于运算
* @param[in]   pos        Pos*        计算所用矩阵和计算结果
*              threshold  double      极小值
* @return      Eigen::MatrixXd  调整后的矩阵
* @note
*/
extern void remove_zero_rows_and_cols_V(Pos* pos, double threshold);

/**
* @brief       保存该历元的观测数据
* @param[in]   out_obs0   Out_obs*    上一历元观测数据 一致性检验时使用
*              out_obs   Out_obs*     该历元观测数据 一致性检验时使用
* @return      无
* @note
*/
extern void copy_obs(Out_obs* out_obs0, Out_obs* out_obs);

/**
* @brief       一致性检验
* @param[in]   out_obs0   Out_obs*    上一历元观测数据
*              out_obs   Out_obs*     该历元观测数据
*              epochCount  int        历元数，跳过第一个历元
*              prn         int        进行检验的卫星号
*              sys         int        进行检验的卫星系统
*              i           int        该历元中第i个观测数据
* @return      是否通过一致性检验
* @note
*/
extern bool consistency_test(int epochCount, int prn, int sys, Out_obs* out_obs, Out_obs* out_obs0, int i);

#endif