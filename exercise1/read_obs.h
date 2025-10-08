#ifndef READ_OBS_H
#define READ_OBS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <map>

// 存储观测值类型信息
struct ObservationType {
    std::vector<std::string> types;  // 观测类型代码 (e.g., "C1C", "L1C", "D1C")
    std::vector<std::string> systems; // 卫星系统 (G, R, E, C, S)
};

// 存储文件头信息
struct RinexHeader {
    double version;
    std::string fileType;
    std::string satSystem;
    //std::string program;项目
    //std::string observer;观测者
    //std::string receiver;接收机
    ///std::string antenna;天线
    double approxPos[3];  // 近似位置 XYZ
    double antennaDelta[3]; // 天线偏移量
    std::string timeFirstObs;//观测开始时间
    std::string timeSystem;//时间系统
    ObservationType obsInfo;//观测类型
    int leapSeconds;
};

// 存储单个卫星的观测值
struct SatelliteData {
    std::string satID;  // 卫星标识 (e.g., "G01")
    std::vector<double> observations; // 观测值数据
};

// 存储一个历元的数据
struct EpochData {
    std::string time;  // 历元时间
    int epochFlag;     // 历元标志
    int numSats;       // 卫星数量
    std::vector<SatelliteData> satellites;
};

extern RinexHeader parseRinexHeader(std::ifstream& file);
extern EpochData parseEpoch(std::ifstream& file, const RinexHeader& header);

#endif
