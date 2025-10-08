#include "read_obs.h"

// �����ļ�ͷ
RinexHeader parseRinexHeader(std::ifstream& file) {
    RinexHeader header;
    std::string line;

    // ��ȡ��һ�л�ȡ�汾���ļ�����
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> header.version >> header.fileType;
    }

    while (std::getline(file, line)) {
        // ����Ƿ񵽴��ļ�ͷ����
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

            // �����۲�����
            int pos = 7;
            for (int i = 0; i < numTypes; i++) {
                if (pos + 3 > 60) {  // ��Ҫ����
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

// ������Ԫ����
EpochData parseEpoch(std::ifstream& file, const RinexHeader& header) {
    EpochData epoch;
    std::string line;

    // ���浱ǰ�ļ�λ��
    std::streampos prevPos = file.tellg();

    if (!std::getline(file, line)) return epoch;

    // �ж��Ƿ�Ϊ����Ԫ��
    // ����г��Ⱥ͹ؼ�����
    if (line.length() < 32 ||
        !isdigit(line[2]) ||   // ��ݵĵ�һλӦ��������
        line[0] != '>') {     // ��29λͨ������Ԫ��־��0��ʾ������
        // ������Ч����Ԫͷ�У������ļ�ָ��
        //file.seekg(prevPos);
        return epoch;
    }

    // ����ʱ��
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

    // ������Ԫ��־����������
    epoch.epochFlag = std::stoi(line.substr(31, 1));
    epoch.numSats = std::stoi(line.substr(32, 3));

    // ��ȡ���ǹ۲�ֵ
    for (int i = 0; i < epoch.numSats; i++) {
        if (!std::getline(file, line)) break;

        SatelliteData sat;
        // ����ID��ÿ�е�ǰ3���ַ�
        sat.satID = line.substr(0, 3);

        // �����۲�ֵ
        int pos = 3; // �ӵ�3�п�ʼ����������ID��
        for (size_t j = 0; j < header.obsInfo.types.size(); j++) {
            if (pos + 16 > line.length()) break;

            // �۲�ֵ��ʽ��14λ�۲�ֵ + 1λLLI + 1λ�ź�ǿ��
            std::string obsStr = line.substr(pos, 14);

            // ���������Ч��
            if (obsStr.find_first_not_of(' ') != std::string::npos) {
                try {
                    sat.observations.push_back(std::stod(obsStr));
                }
                catch (...) {
                    sat.observations.push_back(0.0);  // ��Ч������0����
                }
            }
            else {
                sat.observations.push_back(0.0);
            }

            // �ƶ�����һ��۲�ֵ��ÿ��16�ַ���
            pos += 16;
        }
        epoch.satellites.push_back(sat);
    }
    return epoch;
}