//��ں���
//�鿴ָ����� ָ�����ֺ�ӣ���Ҫ��������

#include "..\SateNavDLL\GNSSDLL\ome_rinex.h"
#include "..\SateNavDLL\GNSSDLL\read_ip.h"

option opt = {
		1,//˫Ƶ
		2,//˫ϵͳ
		0,//�Ķ�����
		1//������һ���Լ���
};


int main(int argc, char* argv[])
{
	const char* str = "dbwicbeh";
	char filename[] = "E:\\myproject\\SateNav\\NovatelOEM20211114-01.log";
	char filename1[] = "E:\\myproject\\SateNav\\����\\NovatelOEM20211114-01.nav";

	READ_IP readIp = {
		"47.114.134.129",  // �Զ����NetIP����
		7190,              // ��ֵ��NetPort
		"NovatelOEM20211114-01_ip.log" // �Զ����ObsDatFile
	};

	int isReadFile=0;//1��ʵʱ������ 0���ļ�
	if (isReadFile == 0)return main_ome4(filename);
	else return  main_Ip(readIp,raw);

}