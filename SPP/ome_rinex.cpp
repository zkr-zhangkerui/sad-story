/*读取log文件*/
/*读取头文件，找到消息类型*/
/*解码该类型的消息*/

//#include <cstdio>
//#include "GNSSSDC.h"
#include "SPP.h"

Pos pos;

int main_ome4(char* filename)
{
	/*初始化变量*/
	int epochCount = 0;//历元数
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
	/*定义输出文件名*/
	char nav_name_g[256];
	char nav_name_b[256];
	char obs_name[256];
	strcpy(nav_name_g, filename);
	strcpy(nav_name_b, filename);
	strcpy(obs_name, filename);
	// 修改 nav_name_g
	char* dot = strrchr(nav_name_g, '.');
	if (dot != NULL && (dot - nav_name_g) >= 3) {
		dot[-3] = '-';dot[-2] = '0';dot[-1] = '1';*dot = '\0';
		strcat(nav_name_g, ".21N  ");
	}
	// 修改 nav_name_b
	dot = strrchr(nav_name_b, '.');
	if (dot != NULL && (dot - nav_name_b) >= 3) {
		dot[-3] = '-'; dot[-2] = '0'; dot[-1] = '1'; *dot = '\0';
		strcat(nav_name_b, ".21F  ");
	}
	// 修改 obs_name
	dot = strrchr(obs_name, '.');
	if (dot != NULL && (dot - obs_name) >= 3) {
		dot[-3] = 'o';dot[-2] = 'b';dot[-1] = 's';*dot = '\0';
		strcat(obs_name, ".obs");
	}

	/*定义输出文件*/
	obs_fp = fopen(obs_name, "w");
	if (obs_fp == nullptr)
	{
		printf("Fail to open file: %s to write.\n", obs_name);
		return 0;
	}
	else
	{
		outobsh(&raw, obs_fp, &header);//输出头文件
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
	
	/*读取log文件*/
	// 打开文件用于读取
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Error opening file");
		return(-1);
	}

	int out = 0;
	fprintf(sate_pos_file, "number,GPS_num,BDS_num,GPSweek,GPSsec,ECEF_X,ECEF_Y,ECEF_Z,BLH_B,BLH_L,BLH_H,ENU_E,ENU_N,ENU_U,t_GPS,t_BDS,"
		"std, PDOP, std_X, std_Y, std_Z, std_E, std_N, std_U, std_t_GPS, std_t_BDS, V_X, V_Y, V_Z, PDOP_V, std_V_X, std_V_Y, std_V_Z, std_d_t\n");
	// 读取数据 一次一个字节
	while (!feof(fp))
	{
		//一次一个字节
		fread(&data, sizeof(uint8_t), 1, fp);

		/*存储数据到缓冲区并解码*/
		if (header_oem4(data,&raw,sate_pos_file) == -1) continue;

		/*读取的数据输出到文件*/
		if(raw.msgID==RANGEID) out=outobs(&out_obs, obs_fp);//1表示完成输出
		if(raw.msgID ==GPSEPHID) outnav(&raw, nav_fp_g, &header,&out_nav_g);//0表示GPS星历
		if (raw.msgID == BDSEPHID) outnav(&raw, nav_fp_b, &header,&out_nav_b);//1表示北斗星历 -1表示失败

		if(out==1){
		/*初始化变量*/
		allocate_pos_memory(&pos, out_obs.num, 5);
		/*单点定位解算*/
		if(pvt_spp(&out_obs, &out_nav_g, &out_nav_b, sate_pos_file,&pos,&out_obs0,epochCount)==0)epochCount++;
		out = 0;
		copy_obs(&out_obs0, &out_obs);
		/*释放内存*/
		//free_pos_memory(&pos);
		}
		memset(raw.buff, 0, MAXOBS);//清空缓存区
		memset(&out_obs, 0, sizeof(out_obs));
	}

	// 关闭文件
	fclose(fp);
	fclose(obs_fp);
	fclose(nav_fp_b);
	fclose(nav_fp_g);

	return 0;
}