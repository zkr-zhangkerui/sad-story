/**
* @file
* @brief    姿态角基本运算
* @details  包括由方向余弦矩阵计算姿态角，由姿态四元数计算方向余弦矩阵、由等效旋转矢量计算姿态四元数
* @author   Kerui Zhang. Email: krzhang@whu.edu.cn
* @date     2025/12/03
* @version  1.1.20251203
* @par      无
* @par      History:
*           2025/12/03,Kerui Zhang, 初步完成四元数、方向余弦矩阵、欧拉角、等效旋转矢量的转换函数\n
*           2025/12/04,Kerui Zhang, 增加四元数乘法、由姿态角到四元数、由欧拉角到方向余弦矩阵\n
*           2025/12/05,Kerui Zhang, 增加四元数规范化和方向余弦矩阵正交化\n
*           2025/12/08,Kerui Zhang, 修改与欧拉角顺序有关的部分,增加方向余弦矩阵转四元数\n
*           2025/12/22,Kerui Zhang, 修改四元数乘法函数的错误、矩阵正交化函数的错误
*/

#include "attitude.h"

//由方向余弦矩阵求欧拉角
void CToAngle(Attitude* attitude) {
    attitude->angle[1] = atan(-attitude->C(2, 0) / sqrt(attitude->C(2, 1) * attitude->C(2, 1) + attitude->C(2, 2) * attitude->C(2, 2)));//俯仰
    if (abs(attitude->C(2, 0)) < 0.999) {
        attitude->angle[0] = atan2(attitude->C(2, 1), attitude->C(2, 2));//横滚
        attitude->angle[2] = atan2(attitude->C(1, 0), attitude->C(0, 0));//航向
        //航向角范围化归到0-360 小车数据的参考结果是0-360，示例数据的参考结果是-180~180
        if ((attitude->angle[2] < 0) && (attitude->angle[2] > -PI))attitude->angle[2] = attitude->angle[2] + 2*PI;
    }
    else {
        std::cout << "C(2,0)" << std::endl;
        std::exit(1);
    }
}

//由姿态四元数计算方向余弦矩阵
void QuatToC(Attitude* attitude) {
    //姿态四元数是四行一列的，元素从上到下为q0，q1，q2，q3
    double q0 = attitude->quat(0, 0);
    double q1 = attitude->quat(1, 0);
    double q2 = attitude->quat(2, 0);
    double q3 = attitude->quat(3, 0);

    attitude->C(0, 0) = q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3;
    attitude->C(0, 1) = 2 * (q1 * q2 - q0 * q3);
    attitude->C(0, 2) = 2 * (q1 * q3 + q0 * q2);
    attitude->C(1, 0) = 2 * (q1 * q2 + q0 * q3);
    attitude->C(1, 1) = q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3;
    attitude->C(1, 2) = 2 * (q2 * q3 - q0 * q1);
    attitude->C(2, 0) = 2 * (q1 * q3 - q0 * q2);
    attitude->C(2, 1) = 2 * (q2 * q3 + q0 * q1);
    attitude->C(2, 2) = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;
}

//由等效旋转矢量计算姿态四元数
void RotvceToQuat(Attitude* attitude) {
    double norm_rotvce = attitude->rotvec.norm();
    attitude->quat(0, 0) = cos(0.5 * norm_rotvce);
    attitude->quat(1, 0) = sin(0.5 * norm_rotvce) * attitude->rotvec(0, 0) / norm_rotvce;
    attitude->quat(2, 0) = sin(0.5 * norm_rotvce) * attitude->rotvec(1, 0) / norm_rotvce;
    attitude->quat(3, 0) = sin(0.5 * norm_rotvce) * attitude->rotvec(2, 0) / norm_rotvce;
}

//为Attitude结构体分配内存赋初值0
void InitAttitude(Attitude* attitude) {
    attitude->angle[0] = 0.0; attitude->angle[1] = 0.0; attitude->angle[2] = 0.0;
    attitude->C = my_eigen::Matrix::zeros(3, 3);
    attitude->quat = my_eigen::Matrix::zeros(4, 1);
    attitude->rotvec = my_eigen::Matrix::zeros(3, 1);
}

//四元数的乘法
my_eigen::Matrix QuatSquare(my_eigen::Matrix p, my_eigen::Matrix q){
    my_eigen::Matrix Mp = my_eigen::Matrix::zeros(4, 4);
    my_eigen::Matrix result = my_eigen::Matrix::zeros(4, 1);
    for (int i = 0; i < 4; i++) {
        Mp(i, i) = p(0,0);
    }
    Mp(0, 1) = -p(1, 0); Mp(0, 2) = -p(2, 0); Mp(0, 3) = -p(3, 0);
    Mp(1, 0) = p(1, 0); Mp(1, 2) = -p(3, 0); Mp(1, 3) = p(2, 0);
    Mp(2, 0) = p(2, 0); Mp(2, 1) = p(3, 0); Mp(2, 3) = -p(1, 0);
    Mp(3, 0) = p(3, 0); Mp(3, 1) = -p(2, 0); Mp(3, 2) = p(1, 0);
    result = Mp * q;
    return result;
}

//由姿态角得到四元数
void AngleToQuat(Attitude* attitude) {
    double angle3 = attitude->angle[0]/2;//翻滚
    double angle1 = attitude->angle[1]/2;//俯仰
    double angle2 = attitude->angle[2]/2;//航向
    attitude->quat(0, 0) = cos(angle3) * cos(angle1) * cos(angle2) + sin(angle3) * sin(angle1) * sin(angle2);
    attitude->quat(1, 0) = sin(angle3) * cos(angle1) * cos(angle2) - cos(angle3) * sin(angle1) * sin(angle2);
    attitude->quat(2, 0) = cos(angle3) * sin(angle1) * cos(angle2) + sin(angle3) * cos(angle1) * sin(angle2);
    attitude->quat(3, 0) = cos(angle3) * cos(angle1) * sin(angle2) - sin(angle3) * sin(angle1) * cos(angle2);
}

//由欧拉角得到方向余弦矩阵
void AngleToC(Attitude* attitude) {
    double angle3 = attitude->angle[0];//翻滚
    double angle2 = attitude->angle[1];//俯仰
    double angle1 = attitude->angle[2];//航向
    attitude->C(0, 0) = cos(angle2) * cos(angle1);
    attitude->C(0, 1) = -cos(angle3) * sin(angle1) + sin(angle3) * sin(angle2) * cos(angle1);
    attitude->C(0, 2) = sin(angle3) * sin(angle1) + cos(angle3) * sin(angle2) * cos(angle1);
    attitude->C(1, 0) = cos(angle2) * sin(angle1);
    attitude->C(1, 1) = cos(angle3) * cos(angle1) + sin(angle3) * sin(angle2) * sin(angle1);
    attitude->C(1, 2) = -sin(angle3) * cos(angle1) + cos(angle3) * sin(angle2) * sin(angle1);
    attitude->C(2, 0) = -sin(angle2);
    attitude->C(2, 1) = sin(angle3) * cos(angle2);
    attitude->C(2, 2) = cos(angle3) * cos(angle2);
}

//四元数规范化
void QuatNorm(Attitude* attitude) {
    double n = attitude->quat.norm();
    for (int i = 0; i < 4; i++) {
        attitude->quat(i, 0) = attitude->quat(i, 0) / n;
    }
}

//方向余弦矩阵正交化
void CNormalize(Attitude* attitude) {
    my_eigen::Matrix col1 = my_eigen::Matrix::zeros(3, 1);
    my_eigen::Matrix col2 = my_eigen::Matrix::zeros(3, 1);
    my_eigen::Matrix col3 = my_eigen::Matrix::zeros(3, 1);
    for (int i = 0; i < 3; i++) {
        col1(i, 0) = attitude->C(i, 0);
        col2(i, 0) = attitude->C(i, 1);
        col3(i, 0) = attitude->C(i, 2);
    }
    my_eigen::Matrix col1_normalize=col1.normalized();
    col2 = col2 - (col1_normalize.dot(col2)) * col1_normalize;
    my_eigen::Matrix col2_normalize = col2.normalized();
    col3 = col1_normalize.cross(col2_normalize);
    my_eigen::Matrix col3_normalize = col3.normalized();
    attitude->C.set_block(0, 0, col1_normalize);
    attitude->C.set_block(0, 1, col2_normalize);
    attitude->C.set_block(0, 2, col3_normalize);
}

//方向余弦矩阵转四元数
void CToQuat(Attitude* attitude) {
    double tr_C = 0.0;
    for (int i = 0; i < attitude->C.rows(); i++)tr_C = tr_C + attitude->C(i, i);
    double P1 = 1 + tr_C;
    double P2 = 1 + 2 * attitude->C(0, 0) - tr_C;
    double P3 = 1 + 2 * attitude->C(1, 1) - tr_C;
    double P4 = 1 + 2 * attitude->C(2, 2) - tr_C;

    if ((P1 >= P2) && (P1 >= P3) && (P1 >= P4)) {
        attitude->quat(0, 0) = 0.5 * sqrt(P1);
        attitude->quat(1, 0) = (attitude->C(2, 1) - attitude->C(1, 2)) / (4 * attitude->quat(0, 0));
        attitude->quat(2, 0) = (attitude->C(0, 2) - attitude->C(2, 0)) / (4 * attitude->quat(0, 0));
        attitude->quat(3, 0) = (attitude->C(1, 0) - attitude->C(0, 1)) / (4 * attitude->quat(0, 0));
    }
    if ((P2 >= P1) && (P2 >= P3) && (P2 >= P4)) {
        attitude->quat(1, 0) = 0.5 * sqrt(P2);
        attitude->quat(2, 0) = (attitude->C(1, 0) + attitude->C(0, 1)) / (4 * attitude->quat(1, 0));
        attitude->quat(3, 0) = (attitude->C(0, 2) + attitude->C(2, 0)) / (4 * attitude->quat(1, 0));
        attitude->quat(0, 0) = (attitude->C(2, 1) - attitude->C(1, 2)) / (4 * attitude->quat(1, 0));
    }
    if ((P3 >= P1) && (P3 >= P2) && (P3 >= P4)) {
        attitude->quat(2, 0) = 0.5 * sqrt(P3);
        attitude->quat(3, 0) = (attitude->C(2, 1) + attitude->C(1, 2)) / (4 * attitude->quat(2, 0));
        attitude->quat(0, 0) = (attitude->C(0, 2) - attitude->C(2, 0)) / (4 * attitude->quat(2, 0));
        attitude->quat(1, 0) = (attitude->C(1, 0) + attitude->C(0, 1)) / (4 * attitude->quat(2, 0));
    }
    if ((P4 >= P1) && (P4 >= P3) && (P4 >= P2)) {
        attitude->quat(3, 0) = 0.5 * sqrt(P4);
        attitude->quat(0, 0) = (attitude->C(1, 0) - attitude->C(0, 1)) / (4 * attitude->quat(3, 0));
        attitude->quat(1, 0) = (attitude->C(0, 2) + attitude->C(2, 0)) / (4 * attitude->quat(3, 0));
        attitude->quat(2, 0) = (attitude->C(2, 1) + attitude->C(1, 2)) / (4 * attitude->quat(3, 0));
    }
}

//求等效旋转矢量的反对称矩阵（使用其它向量）
my_eigen::Matrix reverse_M(my_eigen::Matrix rotvec) {
    my_eigen::Matrix reverse_rotvec_b = my_eigen::Matrix::zeros(3, 3);//等效旋转矢量的反对称矩阵
    reverse_rotvec_b(0, 1) = -rotvec(2, 0); reverse_rotvec_b(0, 2) = rotvec(1, 0);
    reverse_rotvec_b(1, 0) = rotvec(2, 0); reverse_rotvec_b(1, 2) = -rotvec(0, 0);
    reverse_rotvec_b(2, 0) = -rotvec(1, 0); reverse_rotvec_b(2, 1) = rotvec(0, 0);
    return reverse_rotvec_b;
}