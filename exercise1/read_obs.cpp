#include "read_obs.h"

// 解析文件头
RinexHeader parseRinexHeader(std::ifstream& file) {
    RinexHeader header;
    std::string line;

    // 读取第一行获取版本和文件类型
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> header.version >> header.fileType;
    }

    while (std::getline(file, line)) {
        // 检查是否到达文件头结束
        if (line.find("END OF HEADER") != std::string::npos) {
            break;
        }

        std::string label = line.substr(60, 20);
        /*if (label.find("PGM / RUN BY / DATE") != std::string::npos) {
            header.program = line.substr(0, 20);
        }
        else if (label.find("OBSERVER / AGENCY") != std::string::npos) {
            header.observer = line.substr(0, 20);
        }
        else if (label.find("REC # / TYPE / VERS") != std::string::npos) {
            header.receiver = line.substr(0, 20);
        }
        else if (label.find("ANT # / TYPE") != std::string::npos) {
            header.antenna = line.substr(0, 20);
        }*/
        if (label.find("APPROX POSITION XYZ") != std::string::npos) {
            std::istringstream iss(line.substr(0, 60));
            iss >> header.approxPos[0] >> header.approxPos[1] >> header.approxPos[2];
        }
        else if (label.find("ANTENNA: DELTA H/E/N") != std::string::npos) {
            std::istringstream iss(line.substr(0, 60));
            iss >> header.antennaDelta[0] >> header.antennaDelta[1] >> header.antennaDelta[2];
        }
        else if (label.find("TIME OF FIRST OBS") != std::string::npos) {
            header.timeFirstObs = line.substr(0, 43);
        }
        else if (label.find("LEAP SECONDS") != std::string::npos) {
            std::istringstream iss(line.substr(0, 6));
            iss >> header.leapSeconds;
        }
        else if (label.find("SYS / # / OBS TYPES") != std::string::npos) {
            std::string sys = line.substr(0, 1);
            int numTypes;
            std::istringstream iss(line.substr(3, 3));
            iss >> numTypes;

            // 解析观测类型
            int pos = 7;
            for (int i = 0; i < numTypes; i++) {
                if (pos + 3 > 60) {  // 需要换行
                    std::getline(file, line);
                    pos = 7;
                }
                std::string type = line.substr(pos, 3);
                header.obsInfo.types.push_back(type);
                header.obsInfo.systems.push_back(sys);
                pos += 4;
            }
        }
    }
    return header;
}

// 解析历元数据
EpochData parseEpoch(std::ifstream& file, const RinexHeader& header) {
    EpochData epoch;
    std::string line;

    // 保存当前文件位置
    std::streampos prevPos = file.tellg();

    if (!std::getline(file, line)) return epoch;

    // 判断是否为新历元行
    // 检查行长度和关键特征
    if (line.length() < 32 ||
        !isdigit(line[2]) ||   // 年份的第一位应该是数字
        line[0] != '>') {     // 第29位通常是历元标志（0表示正常）
        // 不是有效的历元头行，回退文件指针
        //file.seekg(prevPos);
        return epoch;
    }

    // 解析时间
    int year = std::stoi(line.substr(2, 4));
    int month = std::stoi(line.substr(7, 2));
    int day = std::stoi(line.substr(10, 2));
    int hour = std::stoi(line.substr(13, 2));
    int minute = std::stoi(line.substr(16, 2));
    double second = std::stod(line.substr(19, 11));

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << year << "-"
        << std::setw(2) << month << "-"
        << std::setw(2) << day << " "
        << std::setw(2) << hour << ":"
        << std::setw(2) << minute << ":"
        << std::fixed << std::setprecision(6) << std::setw(10) << second;
    epoch.time = oss.str();

    // 解析历元标志和卫星数量
    epoch.epochFlag = std::stoi(line.substr(31, 1));
    epoch.numSats = std::stoi(line.substr(32, 3));

    // 读取卫星观测值
    for (int i = 0; i < epoch.numSats; i++) {
        if (!std::getline(file, line)) break;

        SatelliteData sat;
        // 卫星ID在每行的前3个字符
        sat.satID = line.substr(0, 3);

        // 解析观测值
        int pos = 3; // 从第3列开始（跳过卫星ID）
        for (size_t j = 0; j < header.obsInfo.types.size(); j++) {
            if (pos + 16 > line.length()) break;

            // 观测值格式：14位观测值 + 1位LLI + 1位信号强度
            std::string obsStr = line.substr(pos, 14);

            // 检查数据有效性
            if (obsStr.find_first_not_of(' ') != std::string::npos) {
                try {
                    sat.observations.push_back(std::stod(obsStr));
                }
                catch (...) {
                    sat.observations.push_back(0.0);  // 无效数据用0代替
                }
            }
            else {
                sat.observations.push_back(0.0);
            }

            // 移动到下一组观测值（每组16字符）
            pos += 16;
        }
        epoch.satellites.push_back(sat);
    }
    return epoch;
}