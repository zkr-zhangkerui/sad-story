#include"GNSSCmnFun.h"
#include "..\BASEDLL\BaseSDC.h"
#include<cmath>

bool XYZ2BLH(Eigen::MatrixXd XYZ, double* BLH)
{
    double p = std::sqrt(XYZ(0, 0) * XYZ(0, 0) + XYZ(1, 0) * XYZ(1, 0));

    // 处理极点情况
    if (p < 1e-12)
    {
        if (std::abs(XYZ(2, 0)) < 1e-12)
        {
            return false; // 坐标原点无法转换
        }
        BLH[0] = (XYZ(2, 0) > 0) ? PI / 2 : -PI / 2;
        BLH[1] = 0.0;
        BLH[2] = XYZ(2, 0) - ((XYZ(2, 0) > 0) ? WGS84_B : -WGS84_B);
        return true;
    }

    BLH[1] = std::atan2(XYZ(1, 0), XYZ(0, 0));

    // 初始化迭代
    double N = WGS84_A;
    double current_H = sqrt(p * p + XYZ(2, 0) * XYZ(2, 0)) - sqrt(WGS84_A * WGS84_B);
    double current_B = std::atan(XYZ(2, 0) * (N + current_H) / (p * (N * (1 - WGS84_E_SQ)) + current_H));
    const int max_iter = 100;

    for (int i = 0; i < max_iter; ++i) {
        double sin_B = std::sin(current_B);
        N = WGS84_A / std::sqrt(1 - WGS84_E_SQ * sin_B * sin_B);
        double cos_B = std::cos(current_B);

        if (cos_B < 1e-12)
        {
            return false; // 避免除以零
        }

        BLH[2] = p / cos_B - N;
        double denominator = p * (N * (1 - WGS84_E_SQ) + BLH[2]);

        if (std::abs(denominator) < 1e-12)
        {
            return false; // 分母过小
        }
        double next_B = std::atan(XYZ(2, 0) * (N + BLH[2]) / denominator);

        if (std::abs(next_B - current_B) < 1e-12)
        {
            BLH[0] = next_B;
            BLH[2] = p / cos_B - N;
            return true; // 收敛成功
        }
        current_B = next_B;
    }
    return false; // 超过最大迭代次数
}


bool BLH2XYZ(const double BLH[3], double XYZ[3])
{
    if (BLH[0] < -PI/2 || BLH[0] > PI/2) {
        return false; // 纬度范围无效
    }

    double sin_B = std::sin(BLH[0]);
    double cos_B = std::cos(BLH[0]);
    double sin_L = std::sin(BLH[1]);
    double cos_L = std::cos(BLH[1]);

    double N = WGS84_A / std::sqrt(1 - WGS84_E_SQ * sin_B * sin_B);
    XYZ[0] = (N + BLH[2]) * cos_B * cos_L;
    XYZ[1] = (N + BLH[2]) * cos_B * sin_L;
    XYZ[2] = ((1 - WGS84_E_SQ) * N + BLH[2]) * sin_B;

    return true;
}

Eigen::MatrixXd XYZ2ENU(Eigen::MatrixXd XYZ, double* enu,double origin[3],double origin_blh[3])
{
    // 计算目标点相对于参考点的位移
    double dx = XYZ(0,0) - origin[0];
    double dy = XYZ(1,0) - origin[1];
    double dz = XYZ(2,0) - origin[2];

    // 计算旋转矩阵元素
    double sin_lat = sin(origin_blh[0]);
    double cos_lat = cos(origin_blh[0]);
    double sin_lon = sin(origin_blh[1]);
    double cos_lon = cos(origin_blh[1]);

    // 计算ENU坐标
    enu[0] = -sin_lon * dx + cos_lon * dy;
    enu[1] = -sin_lat * cos_lon * dx - sin_lat * sin_lon * dy + cos_lat * dz;
    enu[2] = cos_lat * cos_lon * dx + cos_lat * sin_lon * dy + sin_lat * dz;

    //计算转换矩阵
    Eigen::MatrixXd R; R.resize(3, 3);
    R(0, 0) = -sin_lon; R(0, 1) = cos_lon; R(0, 2) = 0.0;
    R(1, 0) = -sin_lat * cos_lon;R(1, 1) = -sin_lat * sin_lon;R(1, 2) = cos_lat;
    R(2, 0) = cos_lat * cos_lon;R(2, 1) = cos_lat * sin_lon;R(2, 2) = sin_lat;

    return R;
}