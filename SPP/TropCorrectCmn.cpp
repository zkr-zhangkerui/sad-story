#include "TropCorrect.h"
using namespace std;

Trop trop;



//hopfield模型计算ZTD 
double hopfield(Trop*trop,double E)
{
	//标准气象参数外推
	trop->TD = TD0 - 0.0065 * trop->h;//开尔文
	trop->PR = PR0 * pow((1 - 0.0000226 * trop->h), 5.225);
	trop->HR = HR0 * exp(-0.0006396 * trop->h);

	double hd = 40136 + 148.72 * (trop->TD-273.15);
	double e = trop->HR * exp(-37.2465 + 0.213166 * (trop->TD) - 0.000256908 * (trop->TD ) * (trop->TD ));
	double kd = 155.2 * pow(10, -7) * trop->PR * (hd-trop->h) / (trop->TD);
	double kw = 155.2 * pow(10, -7) * 4810 * e * (hw - trop->h) / ((trop->TD) * (trop->TD));
	if (E == 0 || E == 90*PI/180)
	{
		double ZTD = kd + kw;
		return ZTD;
	}
	else
	{
		double ZTD = kd / sin(sqrt(E * E + pow(2.5*PI/180,2))) + kw / sin(sqrt(E * E + pow(1.5 * PI / 180, 2)));
		return ZTD;
	}
}

//Saastaminen模型计算STD
double Saastaminen(Trop*trop, double E)
{
	trop->TD = TD0 - 0.0065 * trop->h;//开尔文
	trop->PR = PR0 * pow((1 - 0.0000226 * trop->h), 5.225);
	trop->HR = HR0 * exp(-0.0006396 * trop->h);

	double a = 1.16 - 0.15 * pow(10, -3) * trop->h / 1000 + 0.716 * pow(10, -3) * 
		(trop->h / 1000) * (trop->h / 1000);
	double e = trop->HR * exp(-37.2465 + 0.213166 * (trop->TD) - 0.000256908 * 
		(trop->TD) * (trop->TD));
	if (E == 0 || E == 90)
	{
		double STD = 0.002277 * (trop->PR + (1255 / (trop->TD ) + 0.05) * e);
		return STD;
	}
	else
	{
		double E1 = E + (16 * (trop->PR + 4810 * e / (trop->TD )) / 
			(3600 * (trop->TD ) * tan(E)))*PI/180;//弧度
		double STD = 0.002277 * (trop->PR + (1255 / (trop->TD) + 0.05) * e -
			a / pow(tan(E1), 2)) / sin(E1);
		return STD;
	}
}