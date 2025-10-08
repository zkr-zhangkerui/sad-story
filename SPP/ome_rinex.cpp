/*��ȡlog�ļ�*/
/*��ȡͷ�ļ����ҵ���Ϣ����*/
/*��������͵���Ϣ*/

//#include <cstdio>
//#include "GNSSSDC.h"
#include "SPP.h"

Pos pos;

int main_ome4(char* filename)
{
	/*��ʼ������*/
	int epochCount = 0;//��Ԫ��
	FILE* fp; FILE* nav_fp_g; FILE* obs_fp; FILE* nav_fp_b; FILE* sate_pos_file;// double** pre_recpos=nullptr;
	if (opt.sys == 2){
		pos.recpos_0.resize(5, 1);
		pos.recpos_0 << 0, 0, 0, 0, 0;
	}
	if (opt.sys != 2) {
		pos.recpos_0.resize(4, 1);
		pos.recpos_0 << 0, 0, 0, 0;
	}
	
	char data;
	/*��������ļ���*/
	char nav_name_g[256];
	char nav_name_b[256];
	char obs_name[256];
	strcpy(nav_name_g, filename);
	strcpy(nav_name_b, filename);
	strcpy(obs_name, filename);
	// �޸� nav_name_g
	char* dot = strrchr(nav_name_g, '.');
	if (dot != NULL && (dot - nav_name_g) >= 3) {
		dot[-3] = '-';dot[-2] = '0';dot[-1] = '1';*dot = '\0';
		strcat(nav_name_g, ".21N  ");
	}
	// �޸� nav_name_b
	dot = strrchr(nav_name_b, '.');
	if (dot != NULL && (dot - nav_name_b) >= 3) {
		dot[-3] = '-'; dot[-2] = '0'; dot[-1] = '1'; *dot = '\0';
		strcat(nav_name_b, ".21F  ");
	}
	// �޸� obs_name
	dot = strrchr(obs_name, '.');
	if (dot != NULL && (dot - obs_name) >= 3) {
		dot[-3] = 'o';dot[-2] = 'b';dot[-1] = 's';*dot = '\0';
		strcat(obs_name, ".obs");
	}

	/*��������ļ�*/
	obs_fp = fopen(obs_name, "w");
	if (obs_fp == nullptr)
	{
		printf("Fail to open file: %s to write.\n", obs_name);
		return 0;
	}
	else
	{
		outobsh(&raw, obs_fp, &header);//���ͷ�ļ�
	}
	nav_fp_g = fopen(nav_name_g, "w");
	if (nav_fp_g == nullptr)
	{
		printf("Fail to open file: %s to write.\n", nav_name_g);
		return 0;
	}
	else
	{
		header.sate_sys = 'G';
		outnavh(&raw, nav_fp_g, &header);
	}
	nav_fp_b = fopen(nav_name_b, "w");
	if (nav_fp_b == nullptr)
	{
		printf("Fail to open file: %s to write.\n", nav_name_b);
		return 0;
	}
	else
	{
		header.sate_sys = 'C';
		outnavh(&raw, nav_fp_b, &header);
	}
	sate_pos_file = fopen("satepos_outfile.txt", "w");
	if (sate_pos_file == nullptr)
	{
		printf("Fail to open file: %s to write.\n", nav_name_b);
		return 0;
	}
	
	/*��ȡlog�ļ�*/
	// ���ļ����ڶ�ȡ
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Error opening file");
		return(-1);
	}

	int out = 0;
	fprintf(sate_pos_file, "number,GPS_num,BDS_num,GPSweek,GPSsec,ECEF_X,ECEF_Y,ECEF_Z,BLH_B,BLH_L,BLH_H,ENU_E,ENU_N,ENU_U,t_GPS,t_BDS,"
		"std, PDOP, std_X, std_Y, std_Z, std_E, std_N, std_U, std_t_GPS, std_t_BDS, V_X, V_Y, V_Z, PDOP_V, std_V_X, std_V_Y, std_V_Z, std_d_t\n");
	// ��ȡ���� һ��һ���ֽ�
	while (!feof(fp))
	{
		//һ��һ���ֽ�
		fread(&data, sizeof(uint8_t), 1, fp);

		/*�洢���ݵ�������������*/
		if (header_oem4(data,&raw,sate_pos_file) == -1) continue;

		/*��ȡ������������ļ�*/
		if(raw.msgID==RANGEID) out=outobs(&out_obs, obs_fp);//1��ʾ������
		if(raw.msgID ==GPSEPHID) outnav(&raw, nav_fp_g, &header,&out_nav_g);//0��ʾGPS����
		if (raw.msgID == BDSEPHID) outnav(&raw, nav_fp_b, &header,&out_nav_b);//1��ʾ�������� -1��ʾʧ��

		if(out==1){
		/*��ʼ������*/
		allocate_pos_memory(&pos, out_obs.num, 5);
		/*���㶨λ����*/
		if(pvt_spp(&out_obs, &out_nav_g, &out_nav_b, sate_pos_file,&pos,&out_obs0,epochCount)==0)epochCount++;
		out = 0;
		copy_obs(&out_obs0, &out_obs);
		/*�ͷ��ڴ�*/
		//free_pos_memory(&pos);
		}
		memset(raw.buff, 0, MAXOBS);//��ջ�����
		memset(&out_obs, 0, sizeof(out_obs));
	}

	// �ر��ļ�
	fclose(fp);
	fclose(obs_fp);
	fclose(nav_fp_b);
	fclose(nav_fp_g);

	return 0;
}