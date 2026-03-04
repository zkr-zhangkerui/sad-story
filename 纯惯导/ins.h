/**
* @file
* @brief    INS纯惯导解算
* @details  
* @author   Kerui Zhang. Email: krzhang@whu.edu.cn
* @date     2025/12/03
* @version  1.1.20251203
* @par      无
* @par      History:
*           2025/12/03,Kerui Zhang, 初步完成参考结果文件读取函数、位置速度和姿态更新函数、初始化函数，定义结构体和宏\n
*           2025/12/04,Kerui Zhang, 增加经纬度到ENU转换函数、求误差函数、基站经纬高、INSResult结构体中增加enu下位置\n
*           2025/12/08,Kerui Zhang, 增加示例数据的读取函数\n
*           2025/12/09,Kerui Zhang, 增加求反对称矩阵和更新地理参数的函数
*/

#ifndef INS_H
#define INS_H

#include"attitude.h"
#include"align.h"

#define   a     (6378137.0)              /*长半轴 GRS80=WGS84 P27*/
#define   e     (0.081819191042816)      /*偏心率 WGS84 0.081819190842621 CRS80 0.081819191042816*/
#define   b     (6356752.3142)           /*短半轴 WGS84*/
#define   GM    (3.986004418*pow(10,14)) /*地心引力常数 WGS84*/
#define   we    (7.292115*pow(10,-5))    /*地球自转角速率 GRS80=WGS84 rad/s*/
#define   g1    (9.7803267715)           /*正常重力计算模型第一个参数 指导书中公式*/
#define   g2    (0.0052790414)           /*第二个参数*/
#define   g3    (0.0000232718)           /*第三个参数*/
#define   lat0  (30.5282362393)          /*基站纬度 23.1373950000 30.5282362393*/
#define   lon0  (114.3569980452)         /*基站经度 113.3713650000 114.3569980452*/
#define   hei   (36.402880867)           /*基站大地高*/
#define   dX    (0.18)                   /*杆臂改正E方向*/
#define   dY    (-0.12)                  /*杆臂改正N方向*/
#define   dZ    (0.9)                    /*杆臂改正U方向*/
#define   t0    (98129)                  /*小推车数据开始计算的时间*/
#define   e2    (0.082094437949696)      /*第二偏心率 WGS84*/
#define   f     (1/298.257223563)        /*扁率 WGS84*/
#define   gama  (9.7803253359)           /*赤道正常重力 WGS84*/
#define   gamb  (9.8321849378)           /*极点正常重力 WGS84*/

/**
* @brief 计算结果结构体
*
* pos：纬度（rad）、经度（rad）和大地高 pos_enu:ENU下位置\n 
* vel：NED下速度（前右下）\n
* Attitude 姿态结构体
*/
struct INSResult {
	double GPS_sec;//GPS周内秒
	int GPS_week;//GPS周
	my_eigen::Matrix pos;//纬度、经度和大地高 弧度
	my_eigen::Matrix pos_enu;//东北天
	my_eigen::Matrix vel;//NED下速度
	Attitude attitude;
};

/**
* @brief 地理参数结构体
*
* Rm：子午圈半径
* Rn：卯酉圈半径
* wn：地球自转角速度
* wen：地球自转速率
* gn：正常重力向量
*/
struct Earth {
	double Rm;//子午圈半径
	double Rn;//卯酉圈半径
	my_eigen::Matrix wn;//地球自转角速度分量
	my_eigen::Matrix wen;//地球自转速率
	my_eigen::Matrix gn;//正常重力向量
};

/**
	* @brief       读取小车数据参考结果
	* @param[in]   filename   std::string&            文件名
	* @return      data       std::vector<INSResult>  结果结构体
	* @note        姿态角顺序：翻滚、俯仰、航向 zyx\n
	*/
extern std::vector<INSResult> readINSFile(const std::string& filename);

/**
	* @brief       惯导解算主函数
	* @param[in]   filename   std::string&            观测数据文件名
	* @param[in]   reffile    std::string&            参考数据文件名
	* @return      int 返回是否解算成功,0表示解算完成，-1表示解算失败
	* @note        
	*/
extern int main_ins(std::string filename, std::string reffile);

/**
	* @brief       解算结果结构体初始化
	* @param[out]  result   INSResult*  解算结果结构体
	* @return      无
	* @note        为结构体中的Eigen矩阵分配内存赋初值
	*/
extern void init_INSResult(INSResult* result);

/**
	* @brief       速度更新
	* @param[in]   pos1     my_eigen::Matrix  k-1历元位置矩阵
	* @param[in]   vel1     my_eigen::Matrix  k-1历元速度矩阵
	* @param[in]   pos2     my_eigen::Matrix  k-2历元位置矩阵
	* @param[in]   vel2     my_eigen::Matrix  k-2历元速度矩阵
	* @param[in]   data     IMUData           该历元IMU观测数据
	* @param[in]   data0    IMUData           上一历元IMU观测数据
	* @param[in]   C0       my_eigen::Matrix  上一历元方向余弦矩阵
	* @param[in]   is_first int             0：第一个历元 1：后续历元
	* @param[in]   dt       double            时间间隔
	* @return      my_eigen::Matrix 返回更新后的速度矩阵
	* @note        
	*/
extern my_eigen::Matrix UpdateVel(my_eigen::Matrix pos1, my_eigen::Matrix vel1, my_eigen::Matrix pos2, my_eigen::Matrix vel2, IMUData data, IMUData data0, my_eigen::Matrix C0, int is_first,double dt,FILE* fp);

/**
	* @brief       位置更新
	* @param[in]   pos0   my_eigen::Matrix  上一历元的位置矩阵
	* @param[in]   vel0   my_eigen::Matrix  上一历元的速度矩阵
	* @param[in]   vel    my_eigen::Matrix  刚刚更新得到的该历元速度矩阵
	* @param[in]   dt     double            时间间隔
	* @return      my_eigen::Matrix 返回更新后的位置矩阵
	* @note        
	*/
extern my_eigen::Matrix UpdatePos(my_eigen::Matrix pos0, my_eigen::Matrix vel0, my_eigen::Matrix vel, double dt,FILE* fp);

/**
	* @brief       姿态更新
	* @param[in]   pos0       my_eigen::Matrix  上一历元的位置矩阵
	* @param[in]   vel0		  my_eigen::Matrix  上一历元的速度矩阵
	* @param[in]   pos        my_eigen::Matrix  该历元的位置矩阵
	* @param[in]   vel        my_eigen::Matrix  该历元的速度矩阵
	* @param[in]   data       IMUData           该历元IMU观测数据
	* @param[in]   data0      IMUData           上一历元IMU观测数据
	* @param[in]   attitude0  Attitude          上一历元姿态结构体
	* @param[in]   dt         double            时间间隔
	* @return      Attitude  返回更新后的姿态结构体
	* @note        
	*/
extern Attitude UpdateAttitude(my_eigen::Matrix pos0, my_eigen::Matrix vel0, my_eigen::Matrix vel, my_eigen::Matrix pos, IMUData data, IMUData data0, Attitude attitude0, double dt,FILE* fp);

/**
	* @brief       在首历元初始化结果结构体并赋初值
	* @param[in]   result_ref   INSResult*  参考结果结构体
	* @param[in]   angle[3]     double      粗对准得到的欧拉角
	* @param[out]  result       INSResult*  解算结果结构体
	* @return      无
	* @note        目前使用参考结果的位置欧拉角，粗对准的姿态角不对\n
	*              速度初值为0
	*/
extern void InitInsResult(INSResult* result, INSResult* result_ref, double angle[3]);

/**
	* @brief       简单方法经纬度转到ENU
	* @param[in]   llh   my_eigen::Matrix    经纬度位置
	* @return      my_eigen::Matrix 返回得到的ENU下的位置
	* @note       
	*/
extern my_eigen::Matrix llhToenu(my_eigen::Matrix llh);

/**
	* @brief       计算结果于真值之差
	* @param[in]   ref      INSResult*  参考结果结构体
	* @param[in]   result   INSResult*  计算结果结构体
	* @return      INSResult 返回位置（米）、速度和欧拉角（rad）与真值之差
	* @note        考虑杆臂改正（改的不知道对不对）
	*/
extern INSResult CalDiff(INSResult* ref, INSResult* result);

/**
	* @brief       读取算例的IMU观测数据
	* @param[in]   filename   std::string&            文件名
	* @return      data       std::vector<IMUData>    观测数据结构体
	* @note        增量
	*/
extern std::vector<IMUData> ReadBin(std::string filename);

/**
	* @brief       读取算例的参考结果
	* @param[in]   filename   std::string&            文件名
	* @return      data       std::vector<INSRedult>  结果结构体
	* @note        
	*/
extern std::vector<INSResult> ReadBinRef(std::string filename);

/**
	* @brief       更新地理参数
	* @param[in]   lat        double            纬度 rad
	* @param[in]   h          double            大地高 m
	* @param[in]   ve         double            东方向速度
	* @param[in]   vn         double            北方向速度
	* @return      Earth      更新后的地理参数结构体
	* @note
	*/
extern Earth UpdataEarth(double lat, double h, double ve, double vn);
#endif // !INS_H
