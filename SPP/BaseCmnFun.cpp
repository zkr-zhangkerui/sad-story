#include "BaseCmnFunc.h"
#include <time.h>//获得系统的时间，获得程序运行的时间


YMDHMS mjd2ymdhms(MJD mjd)
{
	YMDHMS ymdhms;
	double JD = mjd.mjd + 2400000.5+mjd.fracofday;
	// 整数部分是JD的日期部分（中午12点对应的日期）
	int JDN = static_cast<int>(JD + 0.5); // 四舍五入到整数
	// 分数天数为当天剩余时间（范围 [-0.5, 0.5)）
	double frac = JD + 0.5 - JDN; // 将正午起点对齐到午夜
	
	int a = JDN + 32044;
	int b = (4 * a + 3) / 146097;
	int c = a - (146097 * b) / 4;
	int d = (4 * c + 3) / 1461;
	int e = c - (1461 * d) / 4;
	int m = (5 * e + 2) / 153;

	ymdhms.day = e - (153 * m + 2) / 5 + 1;
	ymdhms.month = m + 3 - 12 * (m / 10);
	ymdhms.year = 100 * b + d - 4800 + (m / 10);

	double total_seconds = frac * 86400;
	ymdhms.hour = static_cast<int>(total_seconds / 3600);
	int remaining = static_cast<int>(total_seconds) % 3600;
	ymdhms.min = remaining / 60;
	ymdhms.sec = remaining % 60 + (total_seconds - static_cast<int>(total_seconds));
	return ymdhms;
}

MJD ymdhms2mjd(YMDHMS ymdhms)
{
	if (ymdhms.month <= 2)
	{
		ymdhms.year = ymdhms.year - 1;
		ymdhms.month = ymdhms.month + 12;
	}
	MJD mjd;
	int A = ymdhms.year / 100;
	int B = 2 - A + (A / 4);
	double JD = 365.25 * (ymdhms.year + 4716) + 30.6001 * (ymdhms.month + 1) + ymdhms.day + B - 1524.5;
	mjd.mjd = static_cast<unsigned long>(JD-2400000.5);
	mjd.fracofday = ymdhms.hour / 24.0 + ymdhms.min / 1440.0 + ymdhms.sec / 86400;
	return mjd;
}


GPSTIME mjd2gpstime(MJD mjd)
{
	GPSTIME gpst;
	gpst.GPSweek = int((mjd.mjd - 44244) / 7);
	gpst.secsofweek = int((mjd.mjd - 44244 - gpst.GPSweek * 7 + mjd.fracofday) * 86400);
	gpst.fracofsec = (mjd.mjd - 44244 - gpst.GPSweek * 7+ mjd.fracofday) * 86400 - gpst.secsofweek;
	return gpst;
}

MJD gpstime2mjd(GPSTIME gpstime)
{
	MJD mjd;
	double total_seconds = gpstime.secsofweek + gpstime.fracofsec;
	double total_days = 44244.0 + gpstime.GPSweek * 7 + total_seconds / 86400;
	mjd.mjd = int(floor(total_days));
	mjd.fracofday = total_days - mjd.mjd;
	return mjd;
}


YMDHMS gpstime2ymdhms(GPSTIME gpstime)
{
	return mjd2ymdhms(gpstime2mjd(gpstime));

}

GPSTIME ymdhms2gpstime(YMDHMS ymdhms)
{
	return mjd2gpstime(ymdhms2mjd(ymdhms));
}



GPSTIME gpstime2bdstime(GPSTIME gpstime)
{
	GPSTIME bdstime;
	// 计算GPST总秒数（含小数）
	double gpst_total = gpstime.GPSweek * SEC_PER_WEEK + gpstime.secsofweek + gpstime.fracofsec;

	// 计算BDT总秒数: BDT = GPST - (1356周 + 14秒)
	double bdt_total = gpst_total - (GPS_BDS_WEEK_OFFSET * SEC_PER_WEEK + (-GPS_BDS_SECONDS_OFFSET));

	// 计算BDT周数和周内秒
	bdstime.GPSweek = static_cast<int>(bdt_total / SEC_PER_WEEK);
	double remainder = bdt_total - bdstime.GPSweek * SEC_PER_WEEK;

	bdstime.secsofweek = static_cast<int>(remainder);
	bdstime.fracofsec = remainder - bdstime.secsofweek;

	return bdstime;
}

GPSTIME bdstime2gpstime(GPSTIME bdstime)
{
	GPSTIME gpstime;
	// 计算BDT总秒数（含小数）
	double bdt_total = bdstime.GPSweek * SEC_PER_WEEK + bdstime.secsofweek + bdstime.fracofsec;

	// 计算GPST总秒数: GPST = BDT + (1356周 + 14秒)
	double gpst_total = bdt_total + (GPS_BDS_WEEK_OFFSET * SEC_PER_WEEK + (-GPS_BDS_SECONDS_OFFSET));

	// 计算GPST周数和周内秒
	gpstime.GPSweek = static_cast<int>(gpst_total / SEC_PER_WEEK);
	double remainder = gpst_total - gpstime.GPSweek * SEC_PER_WEEK;

	gpstime.secsofweek = static_cast<int>(remainder);
	gpstime.fracofsec = remainder - gpstime.secsofweek;

	return gpstime;
}

GPSTIME sec2gpstime(double sec)
{
	GPSTIME P_c;
	P_c.GPSweek = int((sec) / 604800);
	double remainder = sec - P_c.GPSweek * 604800;
	P_c.secsofweek = int(remainder);
	P_c.fracofsec = remainder - P_c.secsofweek;

	// 精度补偿
	if (P_c.fracofsec >= 1.0) {
		P_c.secsofweek += 1;
		P_c.fracofsec -= 1.0;
	}

	return P_c;
}


/*GPS转UTC*/
double gpstime2utc(GPSTIME*gpstime,double deltat_ls)
{
	deltat_ls = 18;
	double utc_sec = 0.0;
	utc_sec = gpstime->secsofweek + gpstime->fracofsec - deltat_ls;
	if (utc_sec < 0) {
		utc_sec += 604800;
		gpstime->GPSweek -= 1;
	}
	return utc_sec;
}

