#include "TropCorrect.h"
using namespace std;

Trop trop;
//Trop_S trop_s;

double SMP_ZTD = 0;//hopfield ��׼����Ԫ�ط������ZTD Standard Meteorological Parameter
double MMP_ZTD = 0;//hopfield ʵ������Ԫ�ؼ����ZTD measured meteorological parameters
double SMP_ZTD_S = 0;//Saastamoinen ��׼����Ԫ�ط������ZTD Standard Meteorological Parameter
double MMP_ZTD_S = 0;//Saastamoinen ʵ������Ԫ�ؼ����ZTD Standard Meteorological Parameter
double IGS_ZTD = 0;//IGS��Ʒ�и�����ZTD
double IGS_STD = 0;//IGS��Ʒб·���ӳ�
double MMP_STD = 0;//hopfield ʵ������Ԫ�ؼ����STD measured meteorological parameters
double SMP_STD = 0;//hopfield ��׼����Ԫ�ط������STD Standard Meteorological Parameter
double MMP_STD_S = 0;//Saastamoinen ʵ������Ԫ�ؼ����STD measured meteorological parameters
double SMP_STD_S = 0;//Saastamoinen ʵ������Ԫ�ؼ����STD measured meteorological parameters
double Time[6];//ʱ��

extern int trop_correct(Trop*trop)
{
	//����ļ�
	//����ļ�1
	ofstream outfile;
	outfile.open("output1.txt");
	if (!outfile.is_open())
	{
		cout << "can not open output1.txt" << endl;
		return -1;
	}
	//����ļ�2
	ofstream outfile1;
	outfile1.open("output2.txt");
	if (!outfile1.is_open())
	{
		cout << "can not open output2.txt" << endl;
		return -1;
	}
	//��ȡ����
	//ʵ�����������ļ�
	ifstream file("E:/�½��ļ���/�����/����ԭ��/�����ҵ2_�����/WUH200CHN_R_20250010000_01D_05M_MM.rnx");
	if (!file)
	{
		cout << "can not open file" << endl;
		return 1;
	}
	string line;
	vector<vector<double>> Line;
	int num = 0;
	while (getline(file, line))
	{
		num++;
		//ȥ����ͷ�ͽ�β�Ŀո�
		line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
		line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
		if (line == "END OF HEADER")
		{
			break;
		}
	}
	while (getline(file, line))
	{
		vector<double> double_line = string_double_trop(line);
		Line.push_back(double_line);
	}
	//IGS�������Ʒ�ļ�
	ifstream file1("E:/�½��ļ���/�����/����ԭ��/�����ҵ2_�����/IGS0OPSFIN_20250010000_01D_05M_WUH200CHN_TRO.TRO");
	if (!file1)
	{
		cout << "can not open file1" << endl;
		return 1;
	}
	string line1;
	vector<vector<double>> Line1;
	int num1 = 0;
	while (getline(file1, line1))
	{
		num1++;
		//ȥ����ͷ�ͽ�β�Ŀո�
		line1.erase(0, line1.find_first_not_of(" \t\n\r\f\v"));
		line1.erase(line1.find_last_not_of(" \t\n\r\f\v") + 1);
		//��վ�߳�
		if (line1 == "+SITE/ID")
		{
			getline(file1, line1);
			getline(file1, line1);
			vector<double> double_line1 = string_double_trop(line1);
			Line1.push_back(double_line1);
		}
		else if (line1 == "+TROP/SOLUTION")
		{
			getline(file1, line1);
			break;
		}
	}
	while (getline(file1, line1))
	{
		vector<double> double_line1 = string_double_trop(line1);
		Line1.push_back(double_line1);
	}
	outfile << left << setw(30) << "Time" << setw(30) << "ZTD(Hopfield,Standard)(mm)" << setw(30) << "ZTD(Hopfield, Measured)(mm)";
	outfile << left << setw(30) << "ZTD(Saastamoinen,Standard)(mm)" << setw(30) << "ZTD(Saastamoinen, Measured)(mm)" << setw(30) << "ZTD(IGS product)(mm)" << endl;

	trop->h = Line1[0][8];
	//��ƽ���׼�����������ZTD
	//StandardMP(trop->h, trop->TD_S, trop->PR_S, trop->HR_S);
	trop->TD = TD0 - 0.0065 * trop->h;
	trop->PR = PR0 * pow((1 - 0.0000266 * trop->h), 5.225);
	trop->HR = HR0 * exp(-0.0006396 * trop->h);
	SMP_ZTD = hopfield(trop, 0);
	SMP_ZTD_S = Saastaminen(trop, 0);
	//ʵ�������������ZTD
	for (int i = 0; i < Line.size(); i++)
	{
		trop->PR = Line[i][8];
		trop->TD = Line[i][10];
		trop->HR = Line[i][7] / 100;
		MMP_ZTD = hopfield(trop, 0);
		MMP_ZTD_S = Saastaminen(trop, 0);
		for (int j = 0; j < 6; j++)
		{
			Time[j] = Line[i][j];
		}
		IGS_ZTD = Line1[i + 1][2];
		for (int j = 0; j < 6; j++)
		{
			outfile << left << setw(5) << Time[j];
		}
		outfile << setw(30) << SMP_ZTD * 1000 << setw(30) << MMP_ZTD * 1000 << setw(30) << SMP_ZTD_S * 1000 << setw(30) << MMP_ZTD_S * 1000 << setw(30) << IGS_ZTD << endl;
	}

	//ͬһʱ�̲�ͬ�߶Ƚ�5-90,ѡȡ���һ��ʱ��
	outfile1 << "Epoch:";
	for (int j = 0; j < 6; j++)
	{
		outfile1 << left << setw(5) << Time[j];
	}
	outfile1 << endl;
	outfile1 << left << setw(20) << "Elevation(degrees)" << setw(30) << "STD(Hopfield,Standard)(mm)" << setw(30) << "STD(Hopfield, Measured)(mm)";
	outfile1 << left << setw(30) << "STD(Saastamoinen,Standard)(mm)" << setw(30) << "STD(Saastamoinen, Measured)(mm)" << setw(30) << "STD(IGS product)(mm)" << endl;
	//IGS
	for (double i = 5; i <= 90; i = i + 5)
	{
		IGS_STD = IGS_ZTD / sin(i * PI / 180);
		MMP_STD = hopfield(trop, i);
		SMP_STD = hopfield(trop, i);
		MMP_STD_S = Saastaminen(trop, i);
		SMP_STD_S = Saastaminen(trop, i);
		outfile1 << left << setw(20) << i << setw(30) << SMP_STD * 1000 << setw(30) << MMP_STD * 1000 << setw(30) << SMP_STD_S * 1000 << setw(30) << MMP_STD_S * 1000 << setw(30) << IGS_STD << endl;
	}

	return 0;
}