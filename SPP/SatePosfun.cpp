#include "SatePos.h"

using namespace std;

//��������ƫ�����
extern double m_to_E(double m, double e)
{
	double E0 = m;
	double E;
	E = m + e * sin(E0);
	while (abs(E - E0) > 0.00001)
	{
		E0 = E;
		E = m + e * sin(E0);
	}
	return E;
}

//���ַ���������double
extern vector<double> string_double(string line)
{
	//��D�滻��E
	string modified_line;
	for (char C : line)
	{
		if (C == 'D')
		{
			modified_line += 'E';
		}
		else if (C == '*' || C == 'P' || C == 'G')
		{
			continue;
		}
		else
		{
			modified_line += C;
		}
	}
	//ʹ��istringstream�����ַ���
	istringstream iss(modified_line);
	vector<double> numbers;
	double num;
	while (iss >> num)
	{

		numbers.push_back(num);
	}
	return numbers;
}



//�ѹ����ǵķ�Χ������0��pi
extern double rad_convert(double I, double pai)
{
	I = fmod(I, PI * 2);
	if (I < 0)
	{
		I = I + 2 * PI;
	}
	//ȷ���Ƕȵķ�Χ��ȷ
	if (pai == PI)
	{
		if (I > PI && I < 2 * PI)
		{
			I = I - PI;
		}
		else
		{
			I = I;
		}
	}
	else if (pai == PI / 2)
	{
		if (I > PI / 2 && I < PI)
		{
			I = PI - I;
		}
		else if (I > PI && I < 3 * PI / 2)
		{
			I = I - PI;
		}
		else if (I > 3 * PI / 2 && I < 2 * PI)
		{
			I = 2 * PI - I;
		}
		else
		{
			I = I;
		}
	}
	return I;
}


