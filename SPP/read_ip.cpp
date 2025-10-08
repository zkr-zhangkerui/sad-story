//��ȡʵʱ����

#include"read_ip.h"
#include"ome_rinex.h"
#include "SPP.h"

SOCKET    NetGps;
Pos pos1;
//READ_IP readIp;

int lenR;//ÿ�ζ�ȡ���ݵĳ���
int lenD;//���������ܳ���
unsigned char curbuff[MAXOBS];//�������ݵĻ�����
unsigned char decBuff[2 * MAXOBS];//�������ݵĻ�����

bool OpenIp(SOCKET& sock, const char IP[], const unsigned short Port)
{
	WSADATA wsaData;// ���ڴ洢Winsock��ʼ����Ϣ
	SOCKADDR_IN addrSrv;// ��������ַ�ṹ

	// ��ʼ��Winsock��
	if (!WSAStartup(MAKEWORD(1, 1), &wsaData))
	{
		// ����TCP�׽���
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET)
		{
			// ���÷�������ַ��Ϣ
			addrSrv.sin_addr.S_un.S_addr = inet_addr(IP);// ��IP�ַ���ת��Ϊ�����Ƹ�ʽ
			addrSrv.sin_family = AF_INET;// ʹ��IPv4��ַ��
			addrSrv.sin_port = htons(Port);// ���˿ں�ת��Ϊ�����ֽ���
			// ���ӵ�������
			connect(sock, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
			return true;//���ӳɹ�
		}
	}
	return false;//����ʧ��
}

void CloseSocket(SOCKET& sock)
{
	closesocket(sock);
	WSACleanup();
};

int main_Ip(READ_IP readIp,Raw&raw)
{
	int ifcal =0;//�Ƿ����ɹ�
	int epochCount = 0;//��Ԫ��
	memset(&raw, 0, sizeof(Raw));
	
	//��������ļ�
	FILE* ip_fp = nullptr;
	pos1.recpos_0.resize(5, 1);
	pos1.recpos_0 << 0, 0, 0, 0, 0;

	if (OpenIp(NetGps, readIp.NetIP, readIp.NetPort) == false) {
		printf("The ip %s was not opened\n", readIp.NetIP);
		return 0;
	}

	if ((ip_fp = fopen(readIp.ObsDatFile, "wb")) == NULL) //��¼���ڽ��յ�ԭʼ���ݵ��۲��ļ���
	{
		printf("The obs file %s was not opened\n", readIp.ObsDatFile);
		exit(0);
	}
	FILE* nav_fp_g; FILE* obs_fp; FILE* nav_fp_b; FILE* sate_pos_file;
	/*��������ļ�*/
	obs_fp = fopen("obs_ip.obs", "w");
	if (obs_fp == nullptr)
	{
		printf("Fail to open file: %s to write.\n", "obs_ip");
		return 0;
	}
	else
	{
		outobsh(&raw, obs_fp, &header);//���ͷ�ļ�
	}
	nav_fp_g = fopen("nav_g_ip.nav", "w");
	if (nav_fp_g == nullptr)
	{
		printf("Fail to open file: %s to write.\n", "nav_g_ip");
		return 0;
	}
	else
	{
		header.sate_sys = 'G';
		outnavh(&raw, nav_fp_g, &header);
	}
	nav_fp_b = fopen("nav_b_ip.nav", "w");
	if (nav_fp_b == nullptr)
	{
		printf("Fail to open file: %s to write.\n", "nav_b_ip");
		return 0;
	}
	else
	{
		header.sate_sys = 'C';
		outnavh(&raw, nav_fp_b, &header);
	}
	sate_pos_file = fopen("satepos_outfile_ip.txt", "w");
	if (sate_pos_file == nullptr)
	{
		printf("Fail to open file: %s to write.\n", "satepos_outfile_ip.txt");
		return 0;
	}

	//�������õĶ�ȡ��ʽ����ȡ���ݵ�������
	lenD = lenR = 0;// ��ʼ�����ݳ��ȼ�����
	do {
		Sleep(980);// ��ʱԼ1�루����CPUռ�ù��ߣ�
		if ((lenR = recv(NetGps, (char*)curbuff, MAXOBS, 0)) > 0)  //��ȡ����
		{
			printf("%5d ", lenR);
			fwrite(curbuff, sizeof(unsigned char), lenR, ip_fp);    //��¼���������������ļ���

			// ������������ֹ���
			if ((lenD + lenR) > 2 * MAXOBS)lenD = 0;
			memcpy(decBuff + lenD, curbuff, lenR);                      //����ƴ��
			lenD += lenR; // ���»�������Ч���ݳ���

			if (header_oem4_ip(decBuff, lenD, &raw,obs_fp,sate_pos_file,nav_fp_g,nav_fp_b,epochCount) != 1)//����
			{
				printf("\n");
				continue;
			}
			else
			{
				printf("obs GPS:%d %d X:%.3f Y:%.3f Z:%.3f t_GPS:%.3f t_BDS:%.3f std:%.3f d_X:%.3f d_Y:%.3f d_Z:%.3f d_t_g:%.3f d_t_b:%.3f\n", out_obs.t_obs.GPSweek, out_obs.t_obs.secsofweek,
						pos1.recpos_0(0, 0), pos1.recpos_0(1, 0), pos1.recpos_0(2, 0), pos1.recpos_0(3, 0), pos1.recpos_0(4, 0), pos1.std, 
						pos1.D_after(0, 0), pos1.D_after(1, 1), pos1.D_after(2, 2), pos1.D_after(3, 3), pos1.D_after(4, 4));
				printf("vel_X:%.3f vel_Y:%.3f vel_Z:%.3f vel_t:%.3f vel_std:%.3f d_vel_X:%.3f d_vel_Y:%.3f d_vel_Z:%.3f d_vel_t:%.3f\n", pos1.vel(0, 0), pos1.vel(1, 0), pos1.vel(2, 0), pos1.vel(3, 0),
						pos1.std_after_vel, pos1.D_after_vel(0, 0), pos1.D_after_vel(1, 1), pos1.D_after_vel(2, 2), pos1.D_after_vel(3, 3));
				continue;
			}
		}
	} while(1);

	CloseSocket(NetGps);
	fclose(ip_fp);
	fclose(obs_fp);
	fclose(nav_fp_b);
	fclose(nav_fp_g);

	return 0;
}

//����ʵʱ��������ȡ������
//��ʵʱ����������Ҫ����Ĳ��ָ��Ƶ�raw��buff�������У����к����Ĵ���
int header_oem4_ip(unsigned char decBuff[], int&lenD, Raw* raw,FILE*obs_fp,FILE* sate_pos_file,FILE*nav_fp_g,FILE*nav_fp_b,int &epochCount)
{
	int i = 0; int j = 0; int out = 0; int len = 0;

	while (1) {
		out = 0;
		for (; i < lenD - 2; i++) {
			if ((decBuff[i] == OEM4SYNC1) && (decBuff[1 + i] == OEM4SYNC2) && (decBuff[2 + i] == OEM4SYNC3)) {
				//ͬ���ɹ�
				break;
			}
		}
		if ((i + OEM4HEADLEN) >= lenD)break;//��ǰ��Ϣͷû��ȫ�����Ѿ���ȡ�Ļ�������
		for (j=0; j < OEM4HEADLEN; j++) {
			raw->buff[j] = decBuff[i + j];//����Ϣ�帴�Ƶ����ڽ���Ľṹ����
		}
		len = U2(raw->buff + 8) + OEM4HEADLEN;//��Ϣ���ĳ���
		if (((len + 4 + i) > lenD) || len > MAXRAWLEN)break;//��ǰ��������δ�ܰ�����������Ϣ
		for ( j = OEM4HEADLEN; j < len + 4; j++) {
			raw->buff[j] = decBuff[i + j];//��ȡ��Ϣ��
		}
		/*CRCУ��*/
		if (rtk_crc32(raw->buff, len) != U4(raw->buff + len))
		{
			i += len + 4;
			continue;
		}

		raw->msgID = U2(raw->buff + 4);
		switch (raw->msgID)
		{
		case RANGEID:
			memset(&out_obs, 0, sizeof(out_obs));
			decode_range(raw, &out_obs);
			out=outobs(&out_obs, obs_fp);//1��ʾ��������ɹ�
			if (out == 1) {
				/*��ʼ������*/
				allocate_pos_memory(&pos1, out_obs.num, 5);
				/*���㶨λ����*/
				pvt_spp(&out_obs, &out_nav_g, &out_nav_b, sate_pos_file, &pos1,&out_obs0,epochCount);//0������ɹ� -1������ʧ��
			}
			break;
		case GPSEPHID:
			decode_gpseph(raw, &out_nav_g);
			outnav(raw, nav_fp_g, &header, &out_nav_g);//0��ʾGPS����
			out = 2;
			break;
		case BDSEPHID:
			decode_bdseph(raw, &out_nav_b);
			outnav(raw, nav_fp_b, &header, &out_nav_b);//1��ʾ�������� -1��ʾʧ��
			out = 3;
			break;
		}

		i += len + 4;
		if (out == 1) {
			epochCount++; break;
		}//����ɹ� ֱ����ȡ���۲����ݲ�����ѭ����ȡ�µ�����
	}
	for (j=0; j < lenD - i; j++)
		decBuff[j] = decBuff[i + j];
	lenD = j; //����󣬻�����ʣ�����δ������ֽ���

	return out;
}