#include "read_convert.h"

// 解析单行数据
bool parseIMUData(const std::string& line, IMUData& data) {
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

        // 提取最后6个数据字段
        size_t startIdx = tokens.size() - 6;
        data.acc_z = std::stoi(tokens[startIdx])*ACC_SCALE*HZ;
        data.acc_y = -std::stoi(tokens[startIdx + 1]) * ACC_SCALE * HZ;
        data.acc_x = std::stoi(tokens[startIdx + 2]) * ACC_SCALE * HZ;
        data.gyo_z = std::stoi(tokens[startIdx + 3]) * GYO_SCALE;
        data.gyo_y = -std::stoi(tokens[startIdx + 4]) * GYO_SCALE;
        data.gyo_x = std::stoi(tokens[startIdx + 5]) * GYO_SCALE;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "解析错误: " << e.what() << std::endl;
        return false;
    }
}

// 读取文件并解析数据
std::vector<IMUData> readIMUFile(const std::string& filename) {
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
        if (parseIMUData(line, data)) {
            imuDataList.push_back(data);
        }
        else {
            std::cerr << "第 " << lineCount << " 行解析失败: " << line << std::endl;
        }
    }

    file.close();
    return imuDataList;
}

//求比力平均值
void cal_mean(std::vector<IMUData> imudata,double*mean,double begin,double end) {
    double sum[6] = { 0.0, 0.0,0.0,0.0,0.0,0.0 }; int num[6] = { 0,0,0,0,0,0 };
    for (int i =begin ; i < end; i++) {
        if (abs(imudata[i].acc_x) > 1e-12) { 
            sum[0] = sum[0] + imudata[i].acc_x; 
            num[0]++;
        }
        if (abs(imudata[i].acc_y) > 1e-12) {
            sum[1] = sum[1] + imudata[i].acc_y;
            num[1]++;
        }
        if (abs(imudata[i].acc_z) > 1e-12) {
            sum[2] = sum[2] + imudata[i].acc_z;
            num[2]++;
        }
        if (abs(imudata[i].gyo_x) > 1e-12) {
            sum[3] = sum[3] + imudata[i].gyo_x;
            num[3]++;
        }
        if (abs(imudata[i].gyo_y) > 1e-12) {
            sum[4] = sum[4] + imudata[i].gyo_y;
            num[4]++;
        }
        if (abs(imudata[i].gyo_z) > 1e-12) {
            sum[5] = sum[5] + imudata[i].gyo_z;
            num[5]++;
        }
    }
    for (int i = 0; i < 6; i++) {
        mean[i] = sum[i] / num[i];
    }
}

//计算加速度计零偏，比例因子和交轴耦合
my_eigen::Matrix cal_acc(my_eigen::Matrix measure){
    //标准值
    my_eigen::Matrix measure_acc = measure.block(0, 0, 3, 6);
    my_eigen::Matrix standard = my_eigen::Matrix::zeros(4, 6);
    for (int j = 0; j < 6; j++) {
        standard(3, j) = 1;
    }
    standard(0, 0) = G; standard(0, 1) = -G;
    standard(1, 2) = G; standard(1, 3) = -G;
    standard(2, 4) = G; standard(2, 5) = -G;

    my_eigen::Matrix result= my_eigen::Matrix::zeros(3, 4);
    result = measure_acc * (standard.transpose()) * ((standard * (standard.transpose())).inverse());

    return result;
}

//求角速度增量
double cal_sum(std::vector<IMUData> imudata,int begin,int end,int dim) {
    double sum = 0.0;
    for (int i = begin; i < end+1; i++) {
        if (dim == 0) {
            if (abs(imudata[i].gyo_x) > 1e-12)sum = sum + imudata[i].gyo_x;
        }
        if (dim == 1) {
            if (abs(imudata[i].gyo_y) > 1e-12)sum = sum + imudata[i].gyo_y;
        }
        if (dim == 2) {
            if (abs(imudata[i].gyo_z) > 1e-12)sum = sum + imudata[i].gyo_z;
        }
    }
    return sum;
}

//误差补偿
std::vector<IMUData> correct(my_eigen::Matrix result, std::vector<IMUData>imudata, std::vector<IMUData>imudata_w, int which) {
    std::vector<IMUData>imudata_correct;
    my_eigen::Matrix result0 = result.block(0, 0, 3, 3);
    my_eigen::Matrix diff = my_eigen::Matrix::zeros(3, 1);
    my_eigen::Matrix imu = my_eigen::Matrix::zeros(3, 1);
   
    if (which == 0) {//加速度计
        imudata_correct.resize(imudata.size());
        for (int i = 0; i < imudata.size(); i++) {
            imudata_correct[i].time = imudata[i].time;
            diff(0, 0) = imudata[i].acc_x - result(0, 3);
            diff(1, 0) = imudata[i].acc_y - result(1, 3);
            diff(2, 0) = imudata[i].acc_z - result(2, 3);

            imu = (result0.inverse()) * diff;

            imudata_correct[i].acc_x = imu(0, 0);
            imudata_correct[i].acc_y = imu(1, 0);
            imudata_correct[i].acc_z = imu(2, 0);
        }
    }
    if (which == 1) {//陀螺 得到的数据是角速度
        imudata_correct.resize(imudata_w.size());
        for (int i = 0; i < imudata_w.size(); i++) {
            imudata_correct[i].time = imudata_w[i].time;
            diff(0, 0) = imudata_w[i].gyo_x * HZ - result(0, 3);
            diff(1, 0) = imudata_w[i].gyo_y * HZ - result(1, 3);
            diff(2, 0) = imudata_w[i].gyo_z * HZ - result(2, 3);
            
            imu = (result0.inverse()) * diff;
           
            imudata_correct[i].gyo_x = imu(0, 0);
            imudata_correct[i].gyo_y = imu(1, 0);
            imudata_correct[i].gyo_z = imu(2, 0);
        }
    }
    return imudata_correct;
}

/*计算陀螺交轴耦合*/
my_eigen::Matrix cal_w(std::vector<IMUData> imuData_w, my_eigen::Matrix result_w,std::string*filename_w,double begin_end[6][2]) {
    my_eigen::Matrix gyo_test = my_eigen::Matrix::zeros(3, 6);
    my_eigen::Matrix gyo_true = my_eigen::Matrix::zeros(3, 6);
    my_eigen::Matrix gyo = my_eigen::Matrix::zeros(3, 2);
    gyo_true(0, 0) = 10 * PI / 180; gyo_true(0, 1) = -10 * PI / 180;
    gyo_true(1, 2) = 10 * PI / 180; gyo_true(1, 3) = -10 * PI / 180;
    gyo_true(2, 4) = 10 * PI / 180; gyo_true(2, 5) = -10 * PI / 180;
    for (int i = 0; i < 6; i++) {
        imuData_w = readIMUFile(filename_w[i]);
        double mean_w[6] = { 0.0,0.0,0.0,0.0,0.0,0.0 };
        std::cout << "成功读取 " << imuData_w.size() << " 条数据" << std::endl;

        for (int j = 0; j < imuData_w.size(); j++) {
            imuData_w[j].gyo_x = imuData_w[j].gyo_x*HZ;
            imuData_w[j].gyo_y = imuData_w[j].gyo_y * HZ;
            imuData_w[j].gyo_z = imuData_w[j].gyo_z * HZ;
        }
        cal_mean(imuData_w, mean_w,begin_end[i][0],begin_end[i][1]);
        for (int j = 0; j < 3; j++)gyo_test(j, i) = mean_w[j + 3]-result_w(j,3)-gyo_true(j,i)* (result_w(j, j));
    }
    
    my_eigen::Matrix result = gyo_test * (gyo_true.transpose()) * ((gyo_true * gyo_true.transpose()).inverse());

    return result;
}