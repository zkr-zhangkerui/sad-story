/**
* @file
* @brief    INS纯惯导解算辅助函数
* @details
* @author   Kerui Zhang. Email: krzhang@whu.edu.cn
* @date     2025/12/03
* @version  1.1.20251203
* @par      无
* @par      History:
*           2025/12/03,Kerui Zhang, 初步完成参考结果文件读取函数、位置速度和姿态更新函数、初始化函数\n
*           2025/12/04,Kerui Zhang, 增加经纬度到ENU转换函数、求误差函数\n 
*           2025/12/05,Kerui Zhang, 修改了一些错误\n 
*           2025/12/08,Kerui Zhang, 增加示例数据读取函数，在姿态更新部分增加用方向余弦矩阵计算的部分\n 
*           2025/12/09,Kerui Zhang, 修改速度顺序为NED，增加求反对称矩阵和更新地理参数的函数\n 
*           2025/12/22,Kerui Zhang, 修改位置更新函数的错误，公式针对的单位是rad\n 
*           2025/12/23,Kerui Zhang, 修改速度更新函数的错误，对速度修正时是上一历元的速度，不是外推得到的中间时刻
*/

#include "ins.h"

// 读取文件并提取指定列的函数
std::vector<INSResult> readINSFile(const std::string& filename) {
    std::vector<INSResult> data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return data;
    }

    std::string line;
    bool dataSection = false;      // 标记是否进入数据部分
    bool headerLinesSkipped = 0;  // 记录跳过的表头行数

    while (std::getline(file, line)) {
        // 跳过空行
        if (line.empty() || line[0] == '\r') continue;

        // 跳过文件头（直到找到数据开始的标记）
        if (!dataSection) {
            // 检查是否到达数据部分（以 "Week" 开头的列名行）
            if (line.find("Week") == 0) {
                dataSection = true;
                // 跳过第一行列名行
                continue;
            }
            continue;  // 跳过其他文件头
        }

        // 在数据部分，需要跳过第二行列名行（以 "week" 开头的单位行）
        if (line.find("week") == 0) {
            headerLinesSkipped++;
            continue;  // 跳过第二行列名行
        }

        // 使用 stringstream 分割，但处理多个空格
        std::vector<std::string> tokens;
        std::istringstream iss(line);
        std::string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        try {
            INSResult ins;
            init_INSResult(&ins);

            ins.GPS_week = std::stod(tokens[0]);      // Week
            ins.GPS_sec = std::stod(tokens[1]);       // GPSTime

            // 位置（纬经高程）
            ins.pos(0, 0) = std::stod(tokens[2])*PI/180;     // Latitude (deg)
            ins.pos(1, 0) = std::stod(tokens[3])*PI/180;     // Longitude (deg)
            ins.pos(2, 0) = std::stod(tokens[4]);     // Height (m)

            //ENU位置
            ins.pos_enu(0, 0) = std::stod(tokens[12]);     
            ins.pos_enu(1, 0) = std::stod(tokens[13]);   
            ins.pos_enu(2, 0) = std::stod(tokens[14]); 

            //ENU速度->NED
            ins.vel(1, 0) = std::stod(tokens[18]);
            ins.vel(0, 0) = std::stod(tokens[19]);
            ins.vel(2, 0) = -std::stod(tokens[20]);

            // 姿态
            size_t heading_index = 21;  // Heading (deg)
            size_t pitch_index = 22;    // Pitch (deg)
            size_t roll_index = 23;     // Roll (deg)

            if (tokens.size() > roll_index) {
                ins.attitude.angle[0] = std::stod(tokens[roll_index])*PI/180;
                ins.attitude.angle[1] = std::stod(tokens[pitch_index])*PI/180;
                ins.attitude.angle[2] = std::stod(tokens[heading_index])*PI/180;
            }

            data.push_back(ins);

        }
        catch (const std::exception& exc) {
            std::cerr << "警告: 解析行失败: " << exc.what() << std::endl;
            std::cerr << "行内容: " << line.substr(0, 120) << "..." << std::endl;
            std::cerr << "Tokens count: " << tokens.size() << std::endl;
            continue;
        }
    }

    return data;
}

//初始化INS结果结构体
void init_INSResult(INSResult* result) {
    InitAttitude(&result->attitude);
    result->pos = my_eigen::Matrix::zeros(3, 1);
    result->vel = my_eigen::Matrix::zeros(3, 1);
    result->pos_enu=my_eigen::Matrix::zeros(3, 1);
    result->GPS_sec = 0.0;
}

//每个历元速度更新 k-1时刻的结果：pos1 vel1  k-2时刻的结果：pos2 vel2 该历元(data)和上一历元(data0)的陀螺角度增量和加速度计增量 C0:上一历元方向余弦矩阵 返回更新的速度
//is_first:0:是第一个历元 只有pos1和vel1 is_first:后续历元
my_eigen::Matrix UpdateVel(my_eigen::Matrix pos1, my_eigen::Matrix vel1, my_eigen::Matrix pos2, my_eigen::Matrix vel2, IMUData data,IMUData data0,my_eigen::Matrix C0,int is_first,double dt,FILE* fp) {
    //计算中间时刻位置和速度
    my_eigen::Matrix pos0 = my_eigen::Matrix::zeros(3, 1);
    my_eigen::Matrix vel0 = my_eigen::Matrix::zeros(3, 1);
    if (is_first == 0) {
        pos0 = pos1; vel0 = vel1;
    }
    else {
        pos0 = 1.5 * pos1 - 0.5 * pos2;
        vel0 = 1.5 * vel1 - 0.5 * vel2;
    }
    
    //更新地理参数
    Earth earth0 = UpdataEarth(pos0(0, 0), pos0(2, 0), vel0(1, 0), vel0(0, 0));

    //计算b系比力积分项
    my_eigen::Matrix acc = my_eigen::Matrix::zeros(3, 1);//当前历元加速度增量
    my_eigen::Matrix gyo = my_eigen::Matrix::zeros(3, 1);//当前历元角速度增量
    my_eigen::Matrix acc0 = my_eigen::Matrix::zeros(3, 1);//上一历元加速度增量
    my_eigen::Matrix gyo0 = my_eigen::Matrix::zeros(3, 1);//上一历元角速度增量
    my_eigen::Matrix vb = my_eigen::Matrix::zeros(3, 1);//b系比力积分项
    acc(0, 0) = data.acc_x; acc(1, 0) = data.acc_y; acc(2, 0) = data.acc_z;
    acc0(0, 0) = data0.acc_x; acc0(1, 0) = data0.acc_y; acc0(2, 0) = data0.acc_z;
    gyo(0, 0) = data.gyo_x; gyo(1, 0) = data.gyo_y; gyo(2, 0) = data.gyo_z;
    gyo0(0, 0) = data0.gyo_x; gyo0(1, 0) = data0.gyo_y; gyo0(2, 0) = data0.gyo_z;
    vb = acc + (gyo.cross(acc))*0.5 + ((gyo0.cross(acc)) + (acc0.cross(gyo)))*(1.0/12);

    //计算比力积分项
    my_eigen::Matrix I3 = my_eigen::Matrix::identity(3);//单位阵
    my_eigen::Matrix vn = my_eigen::Matrix::zeros(3, 1);//比力积分项
    my_eigen::Matrix rotvec = (earth0.wn + earth0.wen) * dt;
    my_eigen::Matrix reverse_rotvec = reverse_M(rotvec);//等效旋转矢量的反对称矩阵
    vn = (I3 - 0.5 * reverse_rotvec) * C0 * vb;

    //计算重力哥式积分项
    my_eigen::Matrix agc = my_eigen::Matrix::zeros(3, 1);//重力哥式积分项的被积函数
    my_eigen::Matrix gcor = my_eigen::Matrix::zeros(3, 1);//重力哥式积分项
    agc = earth0.gn - (2 * earth0.wn + earth0.wen).cross(vel0);
    gcor = agc * dt;

    //更新速度
    my_eigen::Matrix vel = my_eigen::Matrix::zeros(3, 1);
    vel = vel1 + vn + gcor;

    //输出中间结果
    fprintf(fp, "%20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf ",
        data.time,earth0.Rm, earth0.Rn, earth0.gn(0, 0), earth0.gn(1, 0), earth0.gn(2, 0), earth0.wn(0, 0), earth0.wn(1, 0), earth0.wn(2, 0), earth0.wen(0, 0), earth0.wen(1, 0), earth0.wen(2, 0),
        vb(0, 0), vb(1, 0), vb(2, 0), vn(0, 0), vn(1, 0), vn(2, 0), agc(0, 0), agc(1, 0), agc(2, 0));

    return vel;
}

//每个历元位置更新  pos0和vel0是上一历元初始位置和速度 vel:当前历元更新后的速度 公式是中经纬度是rad，但是我存储的单位是°
my_eigen::Matrix UpdatePos(my_eigen::Matrix pos0, my_eigen::Matrix vel0,my_eigen::Matrix vel,double dt,FILE* fp) {
    my_eigen::Matrix vel_half = (vel0 + vel) * 0.5;
    my_eigen::Matrix pos = my_eigen::Matrix::zeros(3, 1);
    pos(2, 0) = pos0(2, 0) - vel_half(2, 0) * dt;//大地高更新
    double h = (pos(2, 0) + pos0(2, 0)) / 2;//中间时刻大地高

    double Rm = 0.0;//使用上一历元位置计算
    double Rn = 0.0;//使用上一历元和当前历元的中间时刻计算
    Rm = a * (1 - e * e) / pow((1 - e * e * sin(pos0(0, 0)) * sin(pos0(0, 0))), 1.5);
    pos(0,0) = pos0(0, 0) + dt * vel_half(0, 0) / (Rm + h);//纬度更新

    double lat = (pos0(0, 0) + pos(0, 0)) / 2;
    Rn = a / sqrt(1 - e * e * sin(lat) * sin(lat));
    pos(1,0) = pos0(1, 0) + dt * vel_half(1, 0) / ((Rn + h) * cos(lat));//经度更新

    fprintf(fp, "%20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf ", Rm, Rn, h, lat, vel_half(0, 0), vel_half(1, 0), vel_half(2, 0));

    return pos;
}

//每个历元姿态更新 pos0和vel0是上一历元初始位置和速度 vel和pos的当前历元更新后的速度和位置 attitude0:上一历元姿态结构体
Attitude UpdateAttitude(my_eigen::Matrix pos0, my_eigen::Matrix vel0, my_eigen::Matrix vel,my_eigen::Matrix pos, IMUData data, IMUData data0,Attitude attitude0,double dt,FILE* fp) {
    Attitude att; InitAttitude(&att);

    //重新更新地理参数
    my_eigen::Matrix pos_half = (pos0 + pos) * 0.5;
    my_eigen::Matrix vel_half = (vel0 + vel) * 0.5;
    Earth earth_half = UpdataEarth(pos_half(0, 0), pos_half(2, 0), vel_half(1, 0), vel_half(0, 0));
    my_eigen::Matrix wn = earth_half.wn;
    my_eigen::Matrix wen = earth_half.wen;

    //计算n系相对i系姿态四元数
    Attitude att_n; Attitude att_b;
    InitAttitude(&att_n); InitAttitude(&att_b);
    att_n.rotvec = (-1) * (wn + wen) * dt;//n系相对i系的等效旋转矢量
    //RotvceToQuat(&att_n);
    //QuatNorm(&att_n);
    //按方向余弦矩阵计算
    my_eigen::Matrix reverse_rotvec = reverse_M(att_n.rotvec);//等效旋转矢量的反对称矩阵
    double norm_n = att_n.rotvec.norm();
    att_n.C = my_eigen::Matrix::identity(3) + (sin(norm_n) / norm_n) * reverse_rotvec + ((1 - cos(norm_n)) / (norm_n * norm_n)) * reverse_rotvec * reverse_rotvec;
    CNormalize(&att_n);//方向余弦矩阵正交化

    //b系相对i系姿态四元数
    my_eigen::Matrix gyo = my_eigen::Matrix::zeros(3, 1);//当前历元角速度增量
    my_eigen::Matrix gyo0 = my_eigen::Matrix::zeros(3, 1);//上一历元角速度增量
    my_eigen::Matrix vb = my_eigen::Matrix::zeros(3, 1);//b系比力积分项
    gyo(0, 0) = data.gyo_x; gyo(1, 0) = data.gyo_y; gyo(2, 0) = data.gyo_z;
    gyo0(0, 0) = data0.gyo_x; gyo0(1, 0) = data0.gyo_y; gyo0(2, 0) = data0.gyo_z;
    att_b.rotvec = gyo + (gyo0.cross(gyo)) * (1.0 / 12);
    //RotvceToQuat(&att_b);
    //QuatNorm(&att_b);
    //按方向余弦矩阵计算
    my_eigen::Matrix reverse_rotvec_b = reverse_M(att_b.rotvec);//等效旋转矢量的反对称矩阵
    double norm_b = att_b.rotvec.norm();
    if (abs(norm_b) > 1e-12) {
        att_b.C = my_eigen::Matrix::identity(3) + (sin(norm_b) / norm_b) * reverse_rotvec_b + ((1 - cos(norm_b)) / (norm_b * norm_b)) * reverse_rotvec_b * reverse_rotvec_b;
    }
    else {//若某一历元陀螺增量为0->norm_b==0
        att_b.C = my_eigen::Matrix::identity(3);
    }
    CNormalize(&att_b);//方向余弦矩阵正交化

    //计算当前历元姿态四元数并转化到旋转矩阵和姿态角
    my_eigen::Matrix quat1 = my_eigen::Matrix::zeros(4, 1);//中间结果
    //quat1 = QuatSquare(att_n.quat, attitude0.quat);
    //att.quat = QuatSquare(quat1, att_b.quat);
    att.C = att_n.C * attitude0.C * att_b.C;
    //QuatNorm(&att);//四元数规范化
    //QuatToC(&att);//旋转矩阵
    CNormalize(&att);//方向余弦矩阵正交化
    CToAngle(&att);//姿态角
    CToQuat(&att);//四元数
    QuatNorm(&att);//四元数规范化

    fprintf(fp, "%20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf\n", 
            att_n.rotvec(0, 0), att_n.rotvec(1, 0), att_n.rotvec(2, 0), norm_n, att_b.rotvec(0, 0), att_b.rotvec(1, 0), att_b.rotvec(2, 0), norm_b);
    
    return att;
}

/*赋初值*/
void InitInsResult(INSResult* result, INSResult* result_ref,double angle[3]) {
    result->GPS_sec = result_ref->GPS_sec;
    result->pos = my_eigen::Matrix::zeros(3, 1);
    result->pos_enu = my_eigen::Matrix::zeros(3, 1);
    result->pos = result_ref->pos;
    result->vel = my_eigen::Matrix::zeros(3, 1);
    //result->vel = result_ref->vel;
    InitAttitude(&result->attitude);
    for (int i = 0; i < 3; i++) {
        result->attitude.angle[i] = angle[i];
    }
    AngleToC(&result->attitude);
    AngleToQuat(&result->attitude);
}

//经纬度转换为站心坐标 
my_eigen::Matrix llhToenu(my_eigen::Matrix llh) {
    my_eigen::Matrix enu = my_eigen::Matrix::zeros(3, 1);
    double Rm = a * (1 - e * e) / pow((1 - e * e * sin(llh(0, 0)) * sin(llh(0, 0))), 1.5);
    double Rn = a / sqrt(1 - e * e * sin(llh(0, 0)) * sin(llh(0, 0)));
    enu(1, 0) = (llh(0, 0) - lat0*PI/180) * (Rm + llh(2, 0));
    enu(0, 0) = (llh(1, 0) - lon0*PI/180) * (Rn + llh(2, 0)) * cos(llh(0, 0));
    return enu;
}

//计算与真值之差
INSResult CalDiff(INSResult*ref,INSResult*result) {
    INSResult diff_ins; //与真值的误差
    init_INSResult(&diff_ins);
    //注意杆臂改正
    diff_ins.pos = ref->pos - result->pos;
    diff_ins.pos_enu = my_eigen::Matrix::zeros(3, 1);
    diff_ins.pos_enu(0, 0) = ref->pos_enu(0, 0) - result->pos_enu(0, 0)-dX;
    diff_ins.pos_enu(1, 0) = ref->pos_enu(1, 0) - result->pos_enu(1, 0)-dY;
    diff_ins.pos_enu(2, 0) = ref->pos_enu(2, 0) - result->pos_enu(2, 0)-dZ;
    diff_ins.vel = ref->vel - result->vel;
    AngleToC(&ref->attitude);
    diff_ins.attitude.C = (ref->attitude.C.transpose()) * result->attitude.C;
    CToAngle(&(diff_ins.attitude));
    return diff_ins;
}

//读取算例数据
std::vector<IMUData> ReadBin(std::string filename) {
    std::vector<IMUData> imudata;
    FILE* fp; std::string file = filename;
    fopen_s(&fp, file.c_str(), "rb");
    if (fp == nullptr) {
        printf("Fail to open file: %s to read.\n", file.c_str());
    }
    FILE* fp_out; std::string file_out = "imudata.txt";
    fopen_s(&fp_out, file_out.c_str(), "w");
    if (fp_out == nullptr) {
        printf("Fail to open file: %s to read.\n", file_out.c_str());
    }
    double dataEpoch[7];
    while (fread(dataEpoch, sizeof(double), 7, fp)==7) {
        IMUData data;
        data.time = dataEpoch[0];
        data.gyo_x = dataEpoch[1]; data.gyo_y = dataEpoch[2]; data.gyo_z = dataEpoch[3];
        data.acc_x = dataEpoch[4]; data.acc_y = dataEpoch[5]; data.acc_z = dataEpoch[6];
        if (data.time > 91620) {
            imudata.push_back(data);
            fprintf(fp_out, "%20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf\n", data.time, data.acc_x, data.acc_y, data.acc_z, data.gyo_x, data.gyo_y, data.gyo_z);
        }
    }
    fclose(fp); fclose(fp_out);
    return imudata;
}

//读取算例参考数据
std::vector<INSResult> ReadBinRef(std::string filename) {
    std::vector<INSResult> resultdata;
    FILE* fp; std::string file = filename;
    fopen_s(&fp, file.c_str(), "rb");
    if (fp == nullptr) {
        printf("Fail to open file: %s to read.\n", file.c_str());
    }
    FILE* fp_out; std::string file_out = "resultdata.txt";
    fopen_s(&fp_out, file_out.c_str(), "w");
    if (fp_out == nullptr) {
        printf("Fail to open file: %s to read.\n", file_out.c_str());
    }
    while (!feof(fp)) {
        double dataEpoch[10];
        fread(dataEpoch, sizeof(double), 10, fp);
        INSResult data;
        data.pos = my_eigen::Matrix::zeros(3, 1);
        data.pos_enu = my_eigen::Matrix::zeros(3, 1);
        data.vel = my_eigen::Matrix::zeros(3, 1);
        InitAttitude(&data.attitude);
        data.GPS_sec = dataEpoch[0];
        data.pos(0,0) = dataEpoch[1]*PI/180; data.pos(1,0) = dataEpoch[2] * PI / 180; data.pos(2,0) = dataEpoch[3];
        data.pos_enu = llhToenu(data.pos);
        data.vel(0,0) = dataEpoch[4]; data.vel(1,0) = dataEpoch[5]; data.vel(2,0) = dataEpoch[6];
        data.attitude.angle[0] = dataEpoch[7] * PI / 180; data.attitude.angle[1] = dataEpoch[8] * PI / 180; data.attitude.angle[2] = dataEpoch[9] * PI / 180;
        resultdata.push_back(data);
        fprintf(fp_out, "%20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf %20.10lf\n",
            data.GPS_sec, data.pos(0, 0)*180/PI, data.pos(1, 0)*180/PI, data.pos(2, 0), data.pos_enu(0, 0), data.pos_enu(1, 0), data.pos_enu(2, 0),
            data.vel(0, 0),data.vel(1, 0), data.vel(2, 0), data.attitude.angle[0]*180/PI, data.attitude.angle[1]*180/PI, data.attitude.angle[2]*180/PI);
    }
    fclose(fp); fclose(fp_out);
    return resultdata;
}

//更新地理参数
Earth UpdataEarth(double lat,double h,double ve,double vn) {
    Earth earth;
    earth.gn = my_eigen::Matrix::zeros(3, 1);
    earth.wen = my_eigen::Matrix::zeros(3, 1);
    earth.wn = my_eigen::Matrix::zeros(3, 1);
    earth.Rm = a * (1 - e * e) / pow((1 - e * e * sin(lat) * sin(lat)), 1.5);
    earth.Rn = a / sqrt(1 - e * e * sin(lat) * sin(lat));
    earth.wn(0, 0) = we * cos(lat);
    earth.wn(2, 0) = -we * sin(lat);
    earth.wen(0, 0) = ve / (earth.Rn + h);
    earth.wen(1, 0) = -vn / (earth.Rm + h);
    earth.wen(2, 0) = -ve * tan(lat) / (earth.Rn + h);
    //示例数据 GRS80
    //double g0 = g1 * (1 + g2 * pow(sin(lat), 2) + g3 * pow(sin(lat), 4) + 0.0000001262 * pow(sin(lat), 6) + 7 * pow(10, -10) * pow(sin(lat), 8));
    //double g = g0 - (3.087691089 * pow(10, -6) - 4.397731 * pow(10, -9) * pow(sin(lat), 2)) * h + 0.721 * pow(10, -12) * h * h;
    //小车数据 WGS84
    double g0 = (a * gama * pow(cos(lat), 2) + b * gamb * pow(sin(lat), 2)) / sqrt(a * a * cos(lat) * cos(lat) + b * b * sin(lat) * sin(lat));
    double g = g0 * (1 - h * (2 / a) * (1 + f + we * we * a * a * b / GM - 2 * f * sin(lat) * sin(lat)) + 3 * h * h / (a * a));
    earth.gn(2, 0) = g;
    return earth;
}