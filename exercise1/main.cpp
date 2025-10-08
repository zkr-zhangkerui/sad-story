#include "cycle_slip.h"
#include "Intergrity.h"
#include "read_obs.h"
#include "MP.h"
#include "ion.h"

//��ȡ�۲�ֵ�ļ�
int main() {
    /*��������ļ� ��·�����*/
    FILE* MP_G = nullptr;
    errno_t err = fopen_s(&MP_G, "MP_GPS.txt", "w");
    if (err != 0 || MP_G == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", err);
        return 0;
    }
    FILE* MP_C = nullptr;
    errno_t err_C = fopen_s(&MP_C, "MP_BDS.txt", "w");
    if (err_C != 0 || MP_C == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", err_C);
        return 0;
    }
    FILE* MP_R = nullptr;
    errno_t err_R = fopen_s(&MP_R, "MP_GLO.txt", "w");
    if (err_R != 0 || MP_R == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", err_R);
        return 0;
    }
    FILE* MP = nullptr;
    errno_t err_MP = fopen_s(&MP, "MP.txt", "w");
    if (err_MP != 0 || MP == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", err_MP);
        return 0;
    }

    /*��������ļ� ÿ������������*/
    FILE* slip_G = nullptr;
    errno_t slip_err = fopen_s(&slip_G, "slip_GPS.txt", "w");
    if (slip_err != 0 || slip_G == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", slip_err);
        return 0;
    }
    FILE* slip_C = nullptr;
    errno_t slip_err_C = fopen_s(&slip_C, "slip_BDS.txt", "w");
    if (slip_err_C != 0 || slip_C == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", slip_err_C);
        return 0;
    }
    FILE* slip_R = nullptr;
    errno_t slip_err_R = fopen_s(&slip_R, "slip_GLO.txt", "w");
    if (slip_err_R != 0 || slip_R == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", slip_err_R);
        return 0;
    }
    FILE* slip_MW = nullptr;
    errno_t slip_err_MW = fopen_s(&slip_MW, "slip_MW.txt", "w");
    if (slip_err_MW != 0 || slip_MW == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", slip_err_MW);
        return 0;
    }

    /*��������ļ� ������ӳ�*/
    FILE* ion_G = nullptr;
    errno_t err_ion_g = fopen_s(&ion_G, "ion_GPS.txt", "w");
    if (err_ion_g != 0 || MP_G == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "ion.txt", err_ion_g);
        return 0;
    }
    FILE* ion_C = nullptr;
    errno_t err_ion_C = fopen_s(&ion_C, "ion_BDS.txt", "w");
    if (err_ion_C != 0 || ion_C == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", err_ion_C);
        return 0;
    }
    FILE* ion_R = nullptr;
    errno_t err_ion_R = fopen_s(&ion_R, "ion_GLO.txt", "w");
    if (err_ion_R != 0 || ion_R == nullptr) {
        printf("Fail to open file: %s to write. Error code: %d\n", "MP.txt", err_ion_R);
        return 0;
    }

    //std::string filename = "E:\\�½��ļ���\\�����\\ʵϰ\\��̬����\\4��-20250627\\4��-20250627\\x001\\Rinex\\x001178B5.25o";�����Ͷ�·��ֻ������̬����
   // std::string filename = "E:\\�½��ļ���\\�����\\ʵϰ\\�ڶ���ӻ���\\�ڶ���ӻ���\\data\\time1\\D075\\hcn\\Rinex\\D075176A.25o";
    std::string filename = "E:\\myproject\\�ļ����\\NovatelOEM20211114obs.obs";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    // �����ļ�ͷ
    RinexHeader header = parseRinexHeader(file);

    // �����۲�����
    int epochCount = 0;//����Ԫ��
    int slip_num = 0;//������������Ԫ��
    double epoch_slip_ratio;//��Ԫ������
    double sys_slip_ratio[3];//ϵͳ������ 0:GPS 1:BDS 2:GLO
    sat_fre_obsnum GPS, BDS, GLO;//, SBAS;
    L_MW l_mw_G,l_mw_C,l_mw_R;
    GF GF_G, GF_C, GF_R;
    std::vector<epoch_MP> epoch_MP_G, epoch_MP_C, epoch_MP_R;
    std::vector<int> slip_epoch;//���������������Ԫ
    std::vector<Ion> ion_g, ion_c, ion_r;
    //��ʼ���Լ������ڴ�
    l_mw_G.sate_num.resize(33, 0); l_mw_C.sate_num.resize(33, 0); l_mw_R.sate_num.resize(33, 0);
    l_mw_G.sate_slip.resize(33, 0); l_mw_C.sate_slip.resize(33, 0); l_mw_R.sate_slip.resize(33, 0);
    GF_G.sate_num.resize(33, 0); GF_C.sate_num.resize(33, 0); GF_R.sate_num.resize(33, 0);
    GF_G.sate_slip.resize(33, 0); GF_C.sate_slip.resize(33, 0); GF_R.sate_slip.resize(33, 0);
    GF_G.P_GF = Eigen::MatrixXd::Zero(1, 1); GF_C.P_GF = Eigen::MatrixXd::Zero(1, 1); GF_R.P_GF = Eigen::MatrixXd::Zero(1, 1);
    epoch_MP_G.resize(33); epoch_MP_C.resize(33); epoch_MP_R.resize(33);
    ion_g.resize(33); ion_c.resize(33); ion_r.resize(33);
    for (int i = 0; i < epoch_MP_G.size(); i++) {//Ϊÿ�����ǵ�ÿ����Ԫ�����ڴ�
        epoch_MP_G[i].MP1.resize(800, 0.0); ion_g[i].d_ion_1.resize(800, 0.0); ion_g[i].V_ion_1.resize(800, 0.0);
        epoch_MP_G[i].MP2.resize(800, 0.0); ion_g[i].d_ion_2.resize(800, 0.0); ion_g[i].V_ion_2.resize(800, 0.0);
        epoch_MP_C[i].MP1.resize(800, 0.0); ion_c[i].d_ion_1.resize(800, 0.0); ion_c[i].V_ion_1.resize(800, 0.0);
        epoch_MP_C[i].MP2.resize(800, 0.0); ion_c[i].d_ion_2.resize(800, 0.0); ion_c[i].V_ion_2.resize(800, 0.0);
        epoch_MP_R[i].MP1.resize(800, 0.0); ion_r[i].d_ion_1.resize(800, 0.0); ion_r[i].V_ion_1.resize(800, 0.0);
        epoch_MP_R[i].MP2.resize(800, 0.0); ion_r[i].d_ion_2.resize(800, 0.0); ion_r[i].V_ion_2.resize(800, 0.0);
    }
    slip_epoch.resize(11, 0);
   
    while (!file.eof()) {
        EpochData epoch = parseEpoch(file, header);
        if (epoch.time.empty()) { 
            continue; 
        }
        epochCount++;
        int_count(epoch, &GPS, &BDS, &GLO);
        cal_MP(epoch, MP_G,MP_C,MP_R, &epoch_MP_G,&epoch_MP_C, &epoch_MP_R,epochCount);
        cal_ion(epoch, &ion_g, &ion_c, &ion_r,epochCount);
        if (test_MW(epoch, &l_mw_G, &l_mw_C, &l_mw_R, epochCount,slip_MW)) {
            slip_num++; slip_epoch[slip_num-1]=epochCount;
        }
        else
        {
            //GF���
            if (test_GF(epoch, &GF_G, &GF_C, &GF_R, epochCount)) {
                slip_num++; slip_epoch[slip_num-1] = epochCount;
            }
        }
    }
    epoch_slip_ratio = static_cast<double>(slip_num) / epochCount;
    sys_slip_ratio[0] = GF_G.sys_slip / static_cast<double>(epochCount);//ÿ����Ԫ������Щϵͳ������ֱ����epochCOunt
    sys_slip_ratio[1] = GF_C.sys_slip / static_cast<double>(epochCount);
    sys_slip_ratio[2] = GF_R.sys_slip / static_cast<double>(epochCount);
    //ÿ�����ǵ�������
    cal_sate_slip(GF_G, slip_G);
    cal_sate_slip(GF_C, slip_C);
    cal_sate_slip(GF_R, slip_R);
    std::cout << epoch_slip_ratio << "    " << sys_slip_ratio[0] << "    " << sys_slip_ratio[1] << "    " << sys_slip_ratio[2] << std::endl;
    //������������Ԫ
    for (int i = 0; i < slip_num; i++) {
        if(slip_epoch[i]!=0)std::cout << slip_epoch[i] << " ";
    }
    std::cout << std::endl;

    //�������������ʲ����
    int_cal(&GPS);
    int_cal(&BDS);
    int_cal(&GLO);
    //int_cal(&SBAS);

    //������������Ԫ��MP
    diff_MP(&epoch_MP_G,epochCount);
    diff_MP(&epoch_MP_C,epochCount);
    diff_MP(&epoch_MP_R, epochCount);
    cal_MP_ave(epoch_MP_G,slip_epoch, MP);
    cal_MP_ave(epoch_MP_C, slip_epoch, MP);
    cal_MP_ave(epoch_MP_R, slip_epoch, MP);

    //���������ӳٱ仯��
    cal_d_ion(&ion_g, ion_G, epochCount);
    cal_d_ion(&ion_c, ion_C, epochCount);
    cal_d_ion(&ion_r, ion_R, epochCount);

    file.close();
    fclose(MP_G);
    fclose(MP_C);
    fclose(MP_R);
    return 0;
}