#include "klobuchar.h"


//计算卫星方位角
double pos_angle(double blh[3], Eigen::MatrixXd pos, SateResult result)
{
	double d[3] = {};
	d[0] = result.X1 - pos(0, 0);
	d[1] = result.Y1 - pos(1, 0);
	d[2] = result.Z1 - pos(2, 0);

	double E = 0.0, N = 0.0;
	E = -sin(blh[1]) * d[0] + cos(blh[1]) * d[1];
	N = -sin(blh[0]) * cos(blh[1]) * d[0] - sin(blh[0]) * sin(blh[1]) * d[1] + cos(blh[0]) * d[2];

	double pos_angle = 0.0;//方位角
	pos_angle = atan2(E, N);//弧度
	pos_angle = pos_angle * 180 / PI;
	if (pos_angle < 0)pos_angle = pos_angle + 360;
	return pos_angle;//度
}

//计算电离层延迟改正
double klobuchar(double pos_angle, Ion ion, double high_angle, double UT, double blh[3])
{
	//电离层八参数
	ion.a1 = 3.073 * pow(10, -8);
	ion.a2 = 0;
	ion.a3 = -0.1192 * pow(10, -6);
	ion.a4= 0.1788 * pow(10, -6);
	ion.b1= 0.1413* pow(10, 6);
	ion.b2 = -0.9838 * pow(10, 5);
	ion.b3 = 0.6554 * pow(10, 5);
	ion.b4 = -0.3932 * pow(10, 6);
	//把高度角转成度
	double high_angle_degree = 0.0;
	high_angle_degree = high_angle * 180 / PI;

	double secZ = 0.0;//天顶距
	secZ = 1 + 2 * pow(((96 - high_angle_degree) / 90), 3);

	double EA = 0.0;//测站和穿刺点在地心的夹角
	EA = 445 / (high_angle_degree + 20) - 4;

	double B_IP = 0.0,L_IP=0.0;//穿刺点地磁纬度，地磁经度 度
	L_IP = blh[1]*180/PI + EA * sin(pos_angle * PI / 180) / cos(blh[0]);//经度
	B_IP = blh[0]*180/PI + EA * cos(pos_angle * PI / 180);//纬度

	double B_m = 0.0;
	B_m = B_IP + 10.07 * cos((L_IP - 288.04) * PI / 180);

	double t = 0.0;//地方时 h
	t = fmod(UT/3600,24) + L_IP / 15;
	if (t < 0)t += 24;
	else if (t > 24)t -= 24;

	double A = 0.0, P = 0.0;
	A = ion.a1 + ion.a2 * B_m/180 + ion.a3 * B_m * B_m/(180*180) + ion.a4 * pow(B_m, 3)/pow(180,3);
	P= ion.b1 + ion.b2 * B_m/180 + ion.b3 * B_m * B_m / (180 * 180) + ion.b4 * pow(B_m, 3) / pow(180, 3);
	if (A < 0)A = 0;
	if (P < 72000)P = 72000;

	double T_ion = 0.0;//电离层延迟 秒
	T_ion = secZ * (5 * pow(10, -9) + A * cos((2 * PI *3600/ P) * (t-14)));

	return T_ion*c;
}
