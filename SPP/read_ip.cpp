//读取实时数据

#include"read_ip.h"
#include"ome_rinex.h"
#include "SPP.h"

SOCKET    NetGps;
Pos pos1;
//READ_IP readIp;

int lenR;//每次读取数据的长度
int lenD;//缓冲区的总长度
unsigned char curbuff[MAXOBS];//接收数据的缓冲区
unsigned char decBuff[2 * MAXOBS];//处理数据的缓冲区

bool OpenIp(SOCKET& sock, const char IP[], const unsigned short Port)
{
	WSADATA wsaData;// 用于存储Winsock初始化信息
	SOCKADDR_IN addrSrv;// 服务器地址结构

	// 初始化Winsock库
	if (!WSAStartup(MAKEWORD(1, 1), &wsaData))
	{
		// 创建TCP套接字
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET)
		{
			// 设置服务器地址信息
			addrSrv.sin_addr.S_un.S_addr = inet_addr(IP);// 将IP字符串转换为二进制格式
			addrSrv.sin_family = AF_INET;// 使用IPv4地址族
			addrSrv.sin_port = htons(Port);// 将端口号转换为网络字节序
			// 连接到服务器
			connect(sock, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
			return true;//连接成功
		}
	}
	return false;//连接失败
}

void CloseSocket(SOCKET& sock)
{
	closesocket(sock);
	WSACleanup();
};

int main_Ip(READ_IP readIp,Raw&raw)
{
	int ifcal =0;//是否解算成功
	int epochCount = 0;//历元数
	memset(&raw, 0, sizeof(Raw));
	
	//定义输出文件
	FILE* ip_fp = nullptr;
	pos1.recpos_0.resize(5, 1);
	pos1.recpos_0 << 0, 0, 0, 0, 0;

	if (OpenIp(NetGps, readIp.NetIP, readIp.NetPort) == false) {
		printf("The ip %s was not opened\n", readIp.NetIP);
		return 0;
	}

	if ((ip_fp = fopen(readIp.ObsDatFile, "wb")) == NULL) //记录网口接收的原始数据到观测文件中
	{
		printf("The obs file %s was not opened\n", readIp.ObsDatFile);
		exit(0);
	}
	FILE* nav_fp_g; FILE* obs_fp; FILE* nav_fp_b; FILE* sate_pos_file;
	/*定义输出文件*/
	obs_fp = fopen("obs_ip.obs", "w");
	if (obs_fp == nullptr)
	{
		printf("Fail to open file: %s to write.\n", "obs_ip");
		return 0;
	}
	else
	{
		outobsh(&raw, obs_fp, &header);//输出头文件
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

	//按照配置的读取方式，读取数据到缓存中
	lenD = lenR = 0;// 初始化数据长度计数器
	do {
		Sleep(980);// 延时约1秒（避免CPU占用过高）
		if ((lenR = recv(NetGps, (char*)curbuff, MAXOBS, 0)) > 0)  //读取数据
		{
			printf("%5d ", lenR);
			fwrite(curbuff, sizeof(unsigned char), lenR, ip_fp);    //记录二进制数据流到文件中

			// 缓冲区管理：防止溢出
			if ((lenD + lenR) > 2 * MAXOBS)lenD = 0;
			memcpy(decBuff + lenD, curbuff, lenR);                      //缓存拼接
			lenD += lenR; // 更新缓冲区有效数据长度

			if (header_oem4_ip(decBuff, lenD, &raw,obs_fp,sate_pos_file,nav_fp_g,nav_fp_b,epochCount) != 1)//解码
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

//解码实时数据流读取的数据
//把实时数据流中需要解码的部分复制到raw的buff缓存区中，进行后续的处理
int header_oem4_ip(unsigned char decBuff[], int&lenD, Raw* raw,FILE*obs_fp,FILE* sate_pos_file,FILE*nav_fp_g,FILE*nav_fp_b,int &epochCount)
{
	int i = 0; int j = 0; int out = 0; int len = 0;

	while (1) {
		out = 0;
		for (; i < lenD - 2; i++) {
			if ((decBuff[i] == OEM4SYNC1) && (decBuff[1 + i] == OEM4SYNC2) && (decBuff[2 + i] == OEM4SYNC3)) {
				//同步成功
				break;
			}
		}
		if ((i + OEM4HEADLEN) >= lenD)break;//当前消息头没有全部在已经读取的缓冲区中
		for (j=0; j < OEM4HEADLEN; j++) {
			raw->buff[j] = decBuff[i + j];//把消息体复制到用于解码的结构体中
		}
		len = U2(raw->buff + 8) + OEM4HEADLEN;//消息包的长度
		if (((len + 4 + i) > lenD) || len > MAXRAWLEN)break;//当前缓冲区中未能包含完整的消息
		for ( j = OEM4HEADLEN; j < len + 4; j++) {
			raw->buff[j] = decBuff[i + j];//获取消息体
		}
		/*CRC校验*/
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
			out=outobs(&out_obs, obs_fp);//1表示解码输出成功
			if (out == 1) {
				/*初始化变量*/
				allocate_pos_memory(&pos1, out_obs.num, 5);
				/*单点定位解算*/
				pvt_spp(&out_obs, &out_nav_g, &out_nav_b, sate_pos_file, &pos1,&out_obs0,epochCount);//0：解算成功 -1：解算失败
			}
			break;
		case GPSEPHID:
			decode_gpseph(raw, &out_nav_g);
			outnav(raw, nav_fp_g, &header, &out_nav_g);//0表示GPS星历
			out = 2;
			break;
		case BDSEPHID:
			decode_bdseph(raw, &out_nav_b);
			outnav(raw, nav_fp_b, &header, &out_nav_b);//1表示北斗星历 -1表示失败
			out = 3;
			break;
		}

		i += len + 4;
		if (out == 1) {
			epochCount++; break;
		}//解码成功 直到读取到观测数据才跳出循环读取新的数据
	}
	for (j=0; j < lenD - i; j++)
		decBuff[j] = decBuff[i + j];
	lenD = j; //解码后，缓存中剩余的尚未解码的字节数

	return out;
}