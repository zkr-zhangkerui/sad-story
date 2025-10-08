#include "SatePos.h"

//实时的，解码出来一个导航电文就计算该时刻的卫星位置 //tk需要估算的时刻
int satePos(Eph* eph, GPSTIME tk, SateResult* result, FILE* sate_pos)//const char* infilename
{
	//SateResult result;
	//eph_parameters eph;
	double n0=0.0, m=0.0, m0 = 0.0, t=0.0, E=0.0, V = 0.0, u=0.0, omage1 = 0.0, r=0.0,
		delta_u=0.0, delta_r=0.0, delta_i=0.0, U=0.0, R=0.0, I=0.0, L=0.0, F=0.0;
	double x=0.0, y=0.0, delta_omage_e,tr=0.0, d_tr=0.0, d_E=0.0, d_V=0.0, d_i=0.0,
		d_u=0.0, d_r=0.0, d_omage=0.0, d_x=0.0, d_y=0.0;
	double X_GK = 0.0, Y_GK = 0.0, Z_GK = 0.0, X_GK_v = 0.0, Y_GK_v = 0.0, Z_GK_v = 0.0;
	Eigen::MatrixXd R_1; Eigen::MatrixXd R_x;

	GPSTIME t_oe,t_oc;
	t_oe.GPSweek = eph->week;
	t_oe.secsofweek = int(eph->toe);
	t_oe.fracofsec = eph->toe - t_oe.secsofweek;
	t_oc.GPSweek = eph->week;
	t_oc.secsofweek = int(eph->toc);
	t_oc.fracofsec = eph->toc - t_oc.secsofweek;

	/*输出时间
	* MJD mjd = gpstime2mjd(tk);
	YMDHMS ymdhms = mjd2ymdhms(mjd);
	fprintf(sate_pos, "%2c %4d %02d %02d %02d %02d %.7f\n", '*', ymdhms.year, ymdhms.month,
		ymdhms.day, ymdhms.hour, ymdhms.min, ymdhms.sec);
	fprintf(sate_pos, "%2c %4d %6d\n", '*', tk.GPSweek, tk.secsofweek);*/

	//判断卫星健康状态 0：好 1：不好
	if (eph->health == 1) return -1;
	//判断卫星系统
	char sys = ' '; double GM = 0.0; double omage_e = 0.0;
	if (eph->sys == 0)//自己设置的
	{
		sys = 'G'; GM = GM_GPS; omage_e = omage_e_GPS;
	}
	if (eph->sys == 1)
	{
		sys = 'C'; GM = GM_BDS; omage_e = omage_e_BDS;
		//t_oe = bdstime2gpstime(t_oe);
	}
	//判断是否在有效期内
	if (sys == 'G' && abs(tk - t_oe) > 7200) return -1;
	if (sys == 'C' && abs(tk - t_oe) > 3600) return -1;
	t = tk - t_oe;//需计算时刻相对于星历参考时刻的时间差
	//同一个历元的不同卫星
	//计算各种数据
	//平均角速度
	n0 = sqrt(GM) / pow(eph->sqrt_A, 3);//弧度
	//计算平近点角
	m = eph->M0 + (n0 + eph->d_N) * t;
	m = fmod(m, 2 * PI);
	//迭代计算E偏近点角
	E = m_to_E(m, eph->e);
	E=result->E = fmod(E, 2 * PI);
	//计算真近点角 atan2把角的范围限制在-pi到pi
	V = atan2(sqrt(1 - eph->e * eph->e) * sin(E), (cos(E) - eph->e));
	//未经摄动改正的升交角距和向径
	u = eph->omage + V;
	u = fmod(u, 2 * PI);
	r = eph->sqrt_A * eph->sqrt_A * (1 - eph->e * cos(E));
	//计算改正量
	delta_u = eph->cuc * cos(2 * u) + eph->cus * sin(2 * u);
	delta_r = eph->crc * cos(2 * u) + eph->crs * sin(2 * u);
	delta_i = eph->cic * cos(2 * u) + eph->cis * sin(2 * u);
	//进行改正
	U = u + delta_u;
	R = r + delta_r;
	I = eph->I0 + eph->d_I * t + delta_i;
	//卫星在轨道平面坐标系的位置
	//U = rad_convert(U);
	//I = fmod(I, 2 * pi);
	x = R * cos(U);
	y = R * sin(U);
	//坐标系转换
	if ((eph->sys == 1)&&(I<PI/6))//北斗GEO卫星
	{
		L = eph->omage0 + eph->d_omage0 * t - omage_e * eph->toe;
		X_GK = x * cos(L) - y * cos(I) * sin(L);
		Y_GK = x * sin(L) + y * cos(I) * cos(L);
		Z_GK = y * sin(I);
		Eigen::MatrixXd R_z; R_z.resize(3, 3);
		R_x.resize(3, 3);
		R_1.resize(3, 3);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				R_z(i,j) = R_x(i, j) = R_1(i, j) = 0.0;
			}
		}
		R_z(0,0) = cos(omage_e * t); R_z(0,1) = sin(omage_e * t);
		R_z(1,0) = -sin(omage_e * t); R_z(1,1) = cos(omage_e * t);
		R_z(2,2) = 1;
		R_x(0,0) = 1;
		R_x(1,1) = cos(-5 * PI / 180); R_x(2,1) = -sin(-5 * PI / 180);
		R_x(1,2) = sin(-5 * PI / 180); R_x(2,2) = cos(-5 * PI / 180);
		R_1 = R_z * R_x;

		result->X = X_GK * R_1(0,0) + Y_GK * R_1(0,1) + Z_GK * R_1(0,2);
		result->Y = X_GK * R_1(1,0) + Y_GK * R_1(1,1) + Z_GK * R_1(1,2);
		result->Z = X_GK * R_1(2,0) + Y_GK * R_1(2,1) + Z_GK * R_1(2,2);
	}
	else
	{
		//计算绕z轴选择的角度
		L = eph->omage0 + (eph->d_omage0 - omage_e) * (t) -omage_e * eph->toe;
		//L = fmod(L, 2 * pi);
		//确保角度的范围正确
		//I = rad_convert(I, PI / 2);
		I = rad_convert(I, PI);
		//L= rad_convert(L,pi);
		result->X = x * cos(L) - y * cos(I) * sin(L);
		result->Y = x * sin(L) + y * cos(I) * cos(L);
		result->Z = y * sin(I);
	}
	//相对论改正
	F = -2 * sqrt(GM) / (c * c);
	result->tr = F * eph->e * eph->sqrt_A * sin(E);
	result->clk = eph->f0 + eph->f1 * (tk-t_oc) + eph->f2 * (tk - t_oc) * (tk - t_oc) + result->tr;
	
	//计算速度
	d_E = (n0 + eph->d_N) / (1 - eph->e * cos(E));
	d_V = d_E * sqrt(1 - eph->e * eph->e) / (1 - eph->e * cos(E));
	d_i = eph->d_I + 2 * d_V * (eph->cis * cos(2 * u) - eph->cic * sin(2 * u));
	d_u = d_V + 2 * d_V * (eph->cus * cos(2 * u) - eph->cuc * sin(2 * u));
	d_r = eph->sqrt_A * eph->sqrt_A * eph->e * sin(E) * d_E + 2 * d_V * (eph->crs * cos(2 * u) - eph->crc * sin(2 * u));//
	d_tr = F * eph->e * eph->sqrt_A * cos(E) * d_E;
	result->clkbias = eph->f1 + 2 * eph->f2 * (tk - t_oc) + d_tr;
	d_x = d_r * cos(U) - R * d_u * sin(U);
	d_y = d_r * sin(U) + R * d_u * cos(U);
	
	if ((eph->sys == 1) && (I < PI / 6))//北斗GEO卫星
	{
		d_omage = eph->d_omage0;
		Eigen::MatrixXd R_z_v; R_z_v.resize(3, 3);
		Eigen::MatrixXd R_v; R_v.resize(3, 3);
		Eigen::MatrixXd R_z_v_0; R_z_v_0.resize(3, 3);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				R_z_v(i, j) = R_v(i, j) = R_z_v_0(i,j) = 0.0;
			}
		}

		R_z_v(0, 0) = -omage_e*sin(omage_e * t); R_z_v(0, 1) = omage_e * cos(omage_e * t);
		R_z_v(1, 0) = -omage_e * cos(omage_e * t); R_z_v(1, 1) = -omage_e * sin(omage_e * t);
		R_z_v_0(0, 0) = cos(omage_e * t); R_z_v_0(0, 1) = sin(omage_e * t);
		R_z_v_0(1, 0) = -sin(omage_e * t); R_z_v_0(1, 1) = cos(omage_e * t);
		R_z_v_0(2, 2) = 1;
		R_v = R_z_v * R_x;
		R_1 = R_z_v_0 * R_x;
		X_GK_v = -Y_GK * d_omage - sin(L) * (d_y * cos(I) - Z_GK * d_i) + d_x * cos(L);
		Y_GK_v= X_GK * d_omage + (d_y * cos(I) - Z_GK * d_i) * cos(L) + d_x * sin(L);
		Z_GK_v= d_y * sin(I) + Y_GK * d_i * cos(I);
		result->d_X = X_GK * R_v(0, 0) + Y_GK * R_v(0, 1) + Z_GK * R_v(0, 2)+ X_GK_v * R_1(0, 0) + Y_GK_v * R_1(0, 1) + Z_GK_v * R_1(0, 2);
		result->d_Y = X_GK * R_v(1, 0) + Y_GK * R_v(1, 1) + Z_GK * R_v(1, 2)+ X_GK_v * R_1(1, 0) + Y_GK_v * R_1(1, 1) + Z_GK_v * R_1(1, 2);
		result->d_Z = X_GK * R_v(2, 0) + Y_GK * R_v(2, 1) + Z_GK * R_v(2, 2)+ X_GK_v * R_1(2, 0) + Y_GK_v * R_1(2, 1) + Z_GK_v * R_1(2, 2);
	}
	else
	{
		d_omage = eph->d_omage0 - omage_e;
		result->d_X = -result->Y * d_omage - sin(L) * (d_y * cos(I) - result->Z * d_i) + d_x * cos(L);
		result->d_Y = result->X * d_omage + (d_y * cos(I) - result->Z * d_i) * cos(L) + d_x * sin(L);
		result->d_Z = d_y * sin(I) + y * d_i * cos(I);
	}
	return 0;
}