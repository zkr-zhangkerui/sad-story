#include "ome_rinex.h"
#include "SatePos.h"


/*头文件中的系统时间utc*/
extern void systime_rnx(char*data)
{
	//char* data;
    // 获取当前时间戳（UTC 时间，单位：秒）
    std::time_t now = std::time(nullptr);

    // 转换为本地时间结构体 tm
    std::tm* local_time = std::localtime(&now);

    // 转换为 UTC 时间
    std::tm* utc_time = std::gmtime(&now);

	sprintf(data, "%04d%02d%02d%02d%02d%02d%3s", utc_time->tm_year+1900, 
        utc_time->tm_mon+1,utc_time->tm_mday,utc_time->tm_hour,utc_time->tm_min,utc_time->tm_sec,"UTC");
}

/*导航电文格式转化*/
extern void outnavf_n(FILE* fp, double value, int n)
{
	double e = (fabs(value) < 1E-99) ? 0.0 : floor(log10(fabs(value)) + 1.0);

	fprintf(fp, " %s.%0*.0f%s%+03.0f", value < 0.0 ? "-" : " ", n,
		fabs(value) / pow(10.0, e - n), "D", e);
}

/*输出导航电文数据*/
extern int outnavh(Raw* raw, FILE* nav, Header* header_nav)
{
	/*头文件*/
	char sys_note= '\0';
	if (raw->msgID == BDSEPHID) sys_note = 'C';
	if (raw->msgID == GPSEPHID) sys_note = 'G';
	fprintf(nav, "%9.2f%-11s%-20c%-20s\n", header_nav->version, "N", sys_note, "RINEX VERSION / TYPE");
	systime_rnx(header_nav->date);
	fprintf(nav, "%-20.20s%-20.20s%-20.20s%-20s\n", header_nav->PGM, header_nav->RUN_BY, header_nav->date,
		"PGM / RUN BY / DATE");
	fprintf(nav, "%-60.60s%-20s\n", "", "END OF HEADER");
	return 0;
}

/*输出观测数据头文件*/
extern int outobsh(Raw* raw, FILE* obs, Header* header_obs)
{
	/*头文件*/
	fprintf(obs, "%9.2f%-11s%-20s%-20s\n", header_obs->version, "OBSERVATION DATA",
		"M:Mixed", "RINEX VERSION / TYPE");

	systime_rnx(header_obs->date);
	fprintf(obs, "%-20.20s%-20.20s%-20.20s%-20s\n", header_obs->PGM,header_obs->RUN_BY, header_obs->date,
		"PGM / RUN BY / DATE");
	//测站坐标
	
	//观测信号类型 
	fprintf(obs, "%-c %-4d %-4s%-4s%-4s%-4s%-4s%-4s%-4s%-4s %-20s\n", 'G', 8, obs_types[0][0], obs_types[0][1],
		obs_types[0][2], obs_types[0][3], obs_types[0][4], obs_types[0][5], obs_types[0][6], obs_types[0][7],"SYS / # / OBS TYPES");
	fprintf(obs, "%-c %-4d %-4s%-4s%-4s%-4s%-4s%-4s%-4s%-4s %-20s\n", 'C', 8, obs_types[1][0], obs_types[1][1],
		obs_types[1][2], obs_types[1][3], obs_types[1][4], obs_types[1][5], obs_types[1][6], obs_types[1][7],"SYS / # / OBS TYPES");

	//起止时间
	fprintf(obs, "%-6d%-6d%-6d%-6d%-6d%-13.7f%-5s%-20s\n", raw->range.ymdhms_obs.year, raw->range.ymdhms_obs.month,
		raw->range.ymdhms_obs.day, raw->range.ymdhms_obs.hour, raw->range.ymdhms_obs.min, raw->range.ymdhms_obs.sec,
		"GPS", "TIME OF FIRST OBS");

	fprintf(obs, "%-60.60s%-20s\n", "", "END OF HEADER");
	return 0;
}

/*输出导航电文文件*/
extern int outnav(Raw* raw, FILE* nav, Header* header_nav,Out_nav*out_nav)
{
	//判断是否是同一组星历 大于0表示找到 小于0表示没找到
	int prn_find = find(raw->eph.prn, out_nav->prn, out_nav->num - 1);
	int toc_find = find(raw->eph.toc, out_nav->toc, out_nav->num - 1);
	int week_find = find(raw->eph.week, out_nav->week, out_nav->num - 1);
	int toe_find = find(raw->eph.toe, out_nav->toe, out_nav->num - 1);
	int health_find = find(raw->eph.health, out_nav->health, out_nav->num - 1);
	if ((prn_find >= 0) &&(out_nav->is_change!=1) && (raw->eph.toc == out_nav->toc[prn_find]) && (raw->eph.week == out_nav->week[prn_find]) && (raw->eph.toe == out_nav->toe[prn_find])
		&& (raw->eph.health == out_nav->health[prn_find]))return -1;
	out_nav->is_change = 0;

	if (raw->msgID == GPSEPHID)
	{
		//时间用的是toc转化成年月日时分秒
		GPSTIME toc_gps;
		toc_gps.secsofweek = int(raw->eph.toc);
		toc_gps.fracofsec = raw->eph.toc - toc_gps.secsofweek;
		toc_gps.GPSweek = raw->eph.week;
		YMDHMS toc_ymdhms = gpstime2ymdhms(toc_gps);
		fprintf(nav, "%c%02d %4d %02d %02d %02d %02d %02d", 'G', raw->eph.prn,
			toc_ymdhms.year, toc_ymdhms.month, toc_ymdhms.day,toc_ymdhms.hour, toc_ymdhms.min, int(toc_ymdhms.sec));
		outnavf_n(nav, raw->eph.f0, 12);
		outnavf_n(nav, raw->eph.f1, 12);
		outnavf_n(nav, raw->eph.f2, 12);
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav,raw->eph.IODE1,12); 
		outnavf_n(nav,raw->eph.crs,12); 
		outnavf_n(nav,raw->eph.d_N,12); 
		outnavf_n(nav,raw->eph.M0,12); 
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.cuc, 12); 
		outnavf_n(nav, raw->eph.e, 12); 
		outnavf_n(nav, raw->eph.cus, 12); 
		outnavf_n(nav, raw->eph.sqrt_A, 12); 
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.toe, 12); 
		outnavf_n(nav, raw->eph.cic, 12); 
		outnavf_n(nav, raw->eph.omage0, 12); 
		outnavf_n(nav, raw->eph.cis, 12); 
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.I0, 12); 
		outnavf_n(nav, raw->eph.crc, 12); 
		outnavf_n(nav, raw->eph.omage, 12); 
		outnavf_n(nav, raw->eph.d_omage0, 12); 
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.d_I, 12); 
		outnavf_n(nav, 0.0, 12); 
		outnavf_n(nav, raw->eph.week, 12); 
		outnavf_n(nav,0.0, 12); 
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.URA, 12); 
		outnavf_n(nav, raw->eph.health, 12); 
		outnavf_n(nav, raw->eph.tgd[0], 12); 
		outnavf_n(nav, raw->eph.IODC, 12); 
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, 0.0, 12); 
		outnavf_n(nav,0.0, 12); 
		fprintf(nav, "\n");
;		
		return 0;
	}

	if (raw->msgID == BDSEPHID)
	{
		//没有的数据填充19个空格
		//时间用的是toc转化成年月日时分秒 toc是北斗时
		
		GPSTIME toc_bds;
		toc_bds.secsofweek = int(raw->eph.toc);
		toc_bds.fracofsec = raw->eph.toc - toc_bds.secsofweek;
		toc_bds.GPSweek = raw->eph.week;
		GPSTIME toc_bds2gps = bdstime2gpstime(toc_bds);
		YMDHMS toc_ymdhms = gpstime2ymdhms(toc_bds2gps);
		fprintf(nav, "%c%02d %4d %02d %02d %02d %02d %02d", 'C', raw->eph.prn,
			toc_ymdhms.year, toc_ymdhms.month, toc_ymdhms.day,toc_ymdhms.hour, toc_ymdhms.min, int(toc_ymdhms.sec));
		outnavf_n(nav, raw->eph.f0, 12);
		outnavf_n(nav, raw->eph.f1, 12);
		outnavf_n(nav, raw->eph.f2, 12);
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.AODE, 12);
		outnavf_n(nav, raw->eph.crs, 12);
		outnavf_n(nav, raw->eph.d_N, 12);
		outnavf_n(nav, raw->eph.M0, 12);
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.cuc, 12);
		outnavf_n(nav, raw->eph.e, 12);
		outnavf_n(nav, raw->eph.cus, 12);
		outnavf_n(nav, raw->eph.sqrt_A, 12);
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.toe, 12);
		outnavf_n(nav, raw->eph.cic, 12);
		outnavf_n(nav, raw->eph.omage0, 12);
		outnavf_n(nav, raw->eph.cis, 12);
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.I0, 12);
		outnavf_n(nav, raw->eph.crc, 12);
		outnavf_n(nav, raw->eph.omage, 12);
		outnavf_n(nav, raw->eph.d_omage0, 12);
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.d_I, 12);
		outnavf_n(nav, 0.0, 12);
		outnavf_n(nav, raw->eph.week, 12);
		outnavf_n(nav, 0.0, 12);
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, raw->eph.URA, 12);
		outnavf_n(nav, raw->eph.health, 12);
		outnavf_n(nav, raw->eph.tgd[0], 12);
		outnavf_n(nav, raw->eph.tgd[1], 12);
		fprintf(nav, "\n");

		fprintf(nav, "    ");
		outnavf_n(nav, 0.0, 12);
		outnavf_n(nav, raw->eph.AODC, 12);
		fprintf(nav, "\n");

		out_nav->TGD[0] = raw->eph.tgd[0];
		
		return 1;
	}

}

/*输出观测值文件*/
extern int outobs(Out_obs*out_obs, FILE* obs)
{
	//输出时间
	YMDHMS ymdhms_obs= out_obs->ymdhms_obs = gpstime2ymdhms(out_obs->t_obs);
	fprintf(obs, "%2c%4d %02d %02d %02d %02d %.7f", '>', ymdhms_obs.year, ymdhms_obs.month,
		ymdhms_obs.day, ymdhms_obs.hour, ymdhms_obs.min, ymdhms_obs.sec);
	//卫星状态？
	//该历元观测的卫星数
	fprintf(obs, "%3d\n", out_obs->prn_num);

	//输出观测数据
	for (int i = 0; i < out_obs->num; i++)
	{
		if (out_obs->prn[i] == 0) continue;
		//输出卫星号
		int prn = out_obs->prn[i];
		int sys = out_obs->sys[i];
		if(sys==0)//GPS
		{
			fprintf(obs, "%c%02d", 'G', prn);
			fprintf(obs, "%20.3lf %20.4lf %20.3lf %20.3lf  %20.3lf %20.4lf %20.3lf %20.3lf\n",
				out_obs->obs_t[i][0], out_obs->obs_t[i][1], out_obs->obs_t[i][2], out_obs->obs_t[i][3],
				out_obs->obs_t[i][4], out_obs->obs_t[i][5], out_obs->obs_t[i][6], out_obs->obs_t[i][7]);
		}
		if (sys==1)//BDS
		{
			fprintf(obs, "%c%02d", 'C', prn);
			fprintf(obs, "%20.3lf %20.4lf %20.3lf %20.3lf  %20.3lf %20.4lf %20.3lf %20.3lf\n",
				out_obs->obs_t[i][8], out_obs->obs_t[i][9], out_obs->obs_t[i][10], out_obs->obs_t[i][11],
				out_obs->obs_t[i][12], out_obs->obs_t[i][13], out_obs->obs_t[i][14], out_obs->obs_t[i][15]);
		}
		
	}

	return 1;
}


