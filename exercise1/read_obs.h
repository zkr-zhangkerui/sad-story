#ifndef READ_OBS_H
#define READ_OBS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <map>

// �洢�۲�ֵ������Ϣ
struct ObservationType {
    std::vector<std::string> types;  // �۲����ʹ��� (e.g., "C1C", "L1C", "D1C")
    std::vector<std::string> systems; // ����ϵͳ (G, R, E, C, S)
};

// �洢�ļ�ͷ��Ϣ
struct RinexHeader {
    double version;
    std::string fileType;
    std::string satSystem;
    //std::string program;��Ŀ
    //std::string observer;�۲���
    //std::string receiver;���ջ�
    ///std::string antenna;����
    double approxPos[3];  // ����λ�� XYZ
    double antennaDelta[3]; // ����ƫ����
    std::string timeFirstObs;//�۲⿪ʼʱ��
    std::string timeSystem;//ʱ��ϵͳ
    ObservationType obsInfo;//�۲�����
    int leapSeconds;
};

// �洢�������ǵĹ۲�ֵ
struct SatelliteData {
    std::string satID;  // ���Ǳ�ʶ (e.g., "G01")
    std::vector<double> observations; // �۲�ֵ����
};

// �洢һ����Ԫ������
struct EpochData {
    std::string time;  // ��Ԫʱ��
    int epochFlag;     // ��Ԫ��־
    int numSats;       // ��������
    std::vector<SatelliteData> satellites;
};

extern RinexHeader parseRinexHeader(std::ifstream& file);
extern EpochData parseEpoch(std::ifstream& file, const RinexHeader& header);

#endif
