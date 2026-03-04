#include "align.h"
#include "read_convert.h"

// 解析单行数据
bool parseIMUData_align(const std::string& line, IMUData& data) {
    // 查找*的位置
    size_t markPos = line.find('*');
    if (markPos == std::string::npos) {
        return false; // 没有找到*，格式错误
    }

    // 截取*之前的部分
    std::string dataPart = line.substr(0, markPos);

    // 用逗号分割字符串
    std::vector<std::string> tokens;
    std::stringstream ss(dataPart);
    std::string token;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    // 检查是否有足够的数据字段
    if (tokens.size() < 9) { // 至少需要9个字段（包括开头的%RAWIMUSA）
        return false;
    }

    try {
        // 提取时间戳（第2个字段）
        data.time = std::stod(tokens[2]);
        if (data.time < 98129)return false;

        // 提取最后6个数据字段 以及轴向变换
        size_t startIdx = tokens.size() - 6;
        data.acc_z = -std::stod(tokens[startIdx]) * ACC_SCALE;
        data.acc_y = std::stod(tokens[startIdx + 2]) * ACC_SCALE;
        data.acc_x = -std::stod(tokens[startIdx + 1]) * ACC_SCALE;
        data.gyo_z = -std::stod(tokens[startIdx + 3]) * GYO_SCALE;
        data.gyo_y = std::stod(tokens[startIdx + 5]) * GYO_SCALE;
        data.gyo_x = -std::stod(tokens[startIdx + 4]) * GYO_SCALE;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "解析错误: " << e.what() << std::endl;
        return false;
    }
}

// 读取文件并解析数据
std::vector<IMUData> readIMUFile_align(const std::string& filename) {
    std::vector<IMUData> imuDataList;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return imuDataList;
    }

    std::string line;
    int lineCount = 0;

    while (std::getline(file, line)) {
        lineCount++;

        // 跳过空行和非RAWIMUSA行
        if (line.empty() || line.find("%RAWIMUSA") == std::string::npos) {
            continue;
        }

        IMUData data;
        if (parseIMUData_align(line, data)) {
            imuDataList.push_back(data);
        }
        else {
            std::cerr << "第 " << lineCount << " 行解析失败: " << line << std::endl;
        }
    }

    file.close();
    return imuDataList;
}

//求比力和加速度平均值
void cal_mean_align(std::vector<IMUData> imudata, double* mean) {
    double sum[6] = { 0.0, 0.0,0.0,0.0,0.0,0.0 }; int num[6] = { 0,0,0,0,0,0 };//前3个是比力
    for (int i = 0; i < imudata.size(); i++) {
        //if (abs(imudata[i].acc_x) > 1e-12) {
        sum[0] = sum[0] + HZ_align*imudata[i].acc_x;num[0]++;
        sum[1] = sum[1] + HZ_align * imudata[i].acc_y;num[1]++;
        sum[2] = sum[2] + HZ_align * imudata[i].acc_z;num[2]++;
        sum[3] = sum[3] + HZ_align * imudata[i].gyo_x;num[3]++;
        sum[4] = sum[4] + HZ_align * imudata[i].gyo_y;num[4]++;
        sum[5] = sum[5] + HZ_align * imudata[i].gyo_z;num[5]++;
    }
    for (int i = 0; i < 6; i++) {
        mean[i] = sum[i] / num[i];
    }
}

//计算姿态角
void cal_angle(double w_f[6],double*angle) {
    my_eigen::Matrix g_n = my_eigen::Matrix::zeros(3, 1);
    my_eigen::Matrix g_e = my_eigen::Matrix::zeros(3, 1);
    my_eigen::Matrix w_n = my_eigen::Matrix::zeros(3, 1);
    my_eigen::Matrix w_e = my_eigen::Matrix::zeros(3, 1);
    g_n(2, 0) = G;
    w_n(0, 0) = W_e * cos(LAT * PI / 180);
    w_n(2, 0) = -W_e * sin(LAT * PI / 180);
    for (int i = 0; i < 3; i++) {
        g_e(i, 0) = -w_f[i];
        w_e(i, 0) = w_f[i + 3];
    }
    my_eigen::Matrix vg_normalize = g_n.normalized();
    my_eigen::Matrix vw = g_n.cross(w_n);
    my_eigen::Matrix vw_normalize = vw.normalized();
    my_eigen::Matrix vgw = vw.cross(g_n);
    my_eigen::Matrix vgw_normalize = vgw.normalized();

    my_eigen::Matrix wg_normalize = g_e.normalized();
    my_eigen::Matrix ww = g_e.cross(w_e);
    my_eigen::Matrix ww_normalize = ww.normalized();
    my_eigen::Matrix wgw = ww.cross(g_e);
    my_eigen::Matrix wgw_normalize = wgw.normalized();

    my_eigen::Matrix w= my_eigen::Matrix::zeros(3, 3);
    my_eigen::Matrix v = my_eigen::Matrix::zeros(3, 3);
    v.set_block(0, 0, vg_normalize);
    v.set_block(0, 1, vw_normalize);
    v.set_block(0, 2, vgw_normalize);
    w.set_block(0, 0, wg_normalize.transpose());
    w.set_block(1, 0, ww_normalize.transpose());
    w.set_block(2, 0, wgw_normalize.transpose());
    //姿态矩阵
    my_eigen::Matrix C_b_n = v * w;
    //欧拉角
    angle[2] = atan2(C_b_n(1, 0), C_b_n(0, 0));//航向
    angle[1] = atan(-C_b_n(2, 0) / sqrt(C_b_n(2, 1) * C_b_n(2, 1) + C_b_n(2, 2) * C_b_n(2, 2)));//俯仰
    angle[0] = atan2(C_b_n(2, 1), C_b_n(2, 2));//翻滚
}

//把atan2的结果规划到合理范围，避免发生跳跃
void correct_angle(double* angle,double*angle_standard) {
    for (int i = 0; i < 3; i++) {
        if ((angle[i] - angle_standard[i]) >  PI) {
            angle[i] = angle[i] - 2 * PI;
        }
        else if ((angle[i] - angle_standard[i]) < - PI) {
            angle[i] = angle[i] + 2 * PI;
        }
    }
}

//选择初始静止的时间段
int static_time(std::vector<IMUData> imudata,int size) {
    for (int i = 0; i < size; i++) {
        if (sqrt(imudata[i].acc_x * imudata[i].acc_x + imudata[i].acc_y * imudata[i].acc_y + imudata[i].acc_z * imudata[i].acc_z) < (9.9 / HZ_align)) {
            continue;
        }
        else {
            if (i > 300 * HZ_align)return i;
            else return 0;
        }
    }
    return 0;
}