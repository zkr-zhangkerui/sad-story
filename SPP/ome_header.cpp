#include "ome_rinex.h"

Range range;
Raw raw;
Header header;
Out_obs out_obs,out_obs0;
Out_nav out_nav_g, out_nav_b;
Ion ion;

/* crc-32 parity ---------------------------------------------------------------
* compute crc-32 parity for novatel raw
* args   : uint8_t *buff    I   data
*          int    len       I   data length (bytes)
* return : crc-32 parity
* notes  : see NovAtel OEMV firmware manual 1.7 32-bit CRC
*-----------------------------------------------------------------------------*/
extern uint32_t rtk_crc32(const uint8_t* buff, int len)
{
	uint32_t crc = 0;
	int i, j;

	for (i = 0; i < len; i++) {
		crc ^= buff[i];
		for (j = 0; j < 8; j++) {
			if (crc & 1) crc = (crc >> 1) ^ POLYCRC32; else crc >>= 1;
		}
	}
	return crc;
}


int header_oem4(char data, Raw *raw,FILE*sate_pos_file)
{
	/*找到起始同步字节*/
	//是否已经找到同步字节
	if (raw->len == 0)
	{
		raw->buff[0] = raw->buff[1];
		raw->buff[1] = raw->buff[2];
		raw->buff[2] = data;
		if ((raw->buff[0] == OEM4SYNC1) && (raw->buff[1] == OEM4SYNC2) && (raw->buff[2] == OEM4SYNC3))
		{
			raw->len = 3;
			return -1;
		}
		else return -1;
	}
	//将数据存入缓冲区
	raw->buff[raw->len++] = data;

	/*判断消息的长度是否合法*/
	if (raw->len == 10)//当读到第10个字节时，已经获得了消息体的长度
	{
		if ((OEM4HEADLEN + U2(raw->buff + 8)) > MAXRAWLEN - 4)//消息头消息体长度大于最大长度-CRC
		{
			raw->len = 0;
			return -1;
		}
	}

	/*判断是否获得完整的一帧数据*/
	if (raw->len < 10 || (raw->len - 4) < (OEM4HEADLEN + U2(raw->buff + 8)))//未获取到消息体的长度或者实际长度小于应有的长度
	{
		return -1;
	}

	/*CRC校验*/
	if (rtk_crc32(raw->buff, raw->len) != U4(raw->buff + raw->len)) 
	{
		raw->len = 0;
		//fprintf(sate_pos_file, "CRC failed\n");
		return -1;
	}
	raw->len = 0;

	raw->msgID = U2(raw->buff + 4);
	raw->msg_len = U2(raw->buff + 8);

	//if(raw->msgID==8)printf("%4d\n",raw->msgID);

	switch (raw->msgID)
	{
	case RANGEID:return decode_range(raw,&out_obs);
	case GPSEPHID:return decode_gpseph(raw,&out_nav_g);
	case BDSEPHID:return decode_bdseph(raw, &out_nav_b);
	case ION:return decode_ion(raw, &ion);
	}

	return 0;
}

int decode_range(Raw* raw, Out_obs*out_obs)//要存储到一个非实时的历元观测数据结构体中 用于实时spv解算
{
	//init_out_obs(out_obs);
	int num=raw->range.num = U4(raw->buff + OEM4HEADLEN);//这个数据包 包括的观测值的数量
	out_obs->prn_num = 0;
	out_obs->num = num;
	for (int i = 0; i < num; i++) out_obs->prn[i] = out_obs->sys[i] = 0;
	out_obs->t_obs.GPSweek = U2(raw->buff + 14);
	out_obs->t_obs.secsofweek = int(U4(raw->buff + 16)*0.001);
	out_obs->t_obs.fracofsec = U4(raw->buff + 16) *0.001 - out_obs->t_obs.secsofweek;
	//int prn_find[4096];
	int* prn_find = (int*)malloc(MAXOBS * sizeof(int));
	if (prn_find == NULL) {
		// 处理内存分配失败
		return -1;
	}
	memset(prn_find, 0, MAXOBS * sizeof(int));
	for (int i = 0; i < num; i++)
	{
		int prn; int index; 
		//int* prn_find = (int*)malloc(MAXOBS * sizeof(int));
		//memcpy(prn_find, out_obs->prn, MAXOBS);
		int sys = get_sys(raw, i);
		int singla = get_singla(raw, i);
		prn = U2(raw->buff + i * LENOFOBS + OEM4HEADLEN + 4);
		if (sys < 0 || singla < 0)
		{
			out_obs->prn[i] = 0;
			continue;//不是需要的系统
		}

		//查找这个卫星之前有没有
		if (find(prn, out_obs->prn,i+1) == -1)//没有
		{
			prn = out_obs->prn[i]=prn_find[i] = U2(raw->buff + i * LENOFOBS + OEM4HEADLEN + 4);
			index = i;
		}
		else
		{
			prn = out_obs->prn[find(prn, prn_find, i+1)];
			index = find(prn, prn_find,i+1);//这个卫星之前出现的索引值
			if (out_obs->sys[index] == sys)//系统也相同
			{
				out_obs->prn[i] = 0;
			}
			else//系统不同
			{
				out_obs->prn[i] = prn;
				prn_find[i] = prn;
				prn_find[index] = 0;
				index = i;
			}
		}
		out_obs->sys[i] = sys;//0GPS 1BDS
		out_obs->singla[i] = singla;
		raw->range.singla[sys][index] = singla;
		/*raw->range.psr[prn][singla] = U8(raw->buff + i * LENOFOBS + OEM4HEADLEN + 8);
		raw->range.adr[prn][singla] = U8(raw->buff + i * LENOFOBS + 20 + OEM4HEADLEN);
		raw->range.dopp[prn][singla] = U4(raw->buff + i * LENOFOBS + 32 + OEM4HEADLEN);
		raw->range.c_no[prn][singla] = U4(raw->buff + i * LENOFOBS + 36 + OEM4HEADLEN);*/
		out_obs->obs_t[index][singla*4+0+sys*8]= R8(raw->buff + i * LENOFOBS + OEM4HEADLEN + 8);
		out_obs->obs_t[index][singla * 4 + 1 + sys * 8]= -R8(raw->buff + i * LENOFOBS + 20 + OEM4HEADLEN);//我的载波相位为什么是负的
		out_obs->obs_t[index][singla * 4 + 2 + sys * 8]= R4(raw->buff + i * LENOFOBS + 32 + OEM4HEADLEN);
		out_obs->obs_t[index][singla * 4 + 3 + sys * 8]=R4(raw->buff + i * LENOFOBS + 36 + OEM4HEADLEN);
	}
	//获得该历元的卫星数量
	for (int i = 0; i < num; i++)
	{
		if (out_obs->prn[i] > 0)out_obs->prn_num++;
	}
	//std::cout << out_obs->prn_num << std::endl;
	//raw->range.glofreq = U2(raw->buff + OEM4HEADLEN+6);
	//raw->range.d_psr = U4(raw->buff +16+ OEM4HEADLEN);
	//raw->range.d_adr = U4(raw->buff + 28 + OEM4HEADLEN);
	//raw->range.locktime = U4(raw->buff + 40 + OEM4HEADLEN);
	//raw->range.ch_tr_states =raw->buff + 44 + OEM4HEADLEN;
	//memset(raw->buff, 0, raw->buff_len);//清空缓存区
	// 
	//memset(raw->buff, 0, MAXOBS);//清空缓存区
	//raw->len = 0;
	//std::cout << raw->msgID << std::endl;
	free(prn_find);
	//out_obs=free_out_obs();
	return 1;
}

//一个数据包只有一颗星的星历 去重复：两个参考时间，卫星号，健康状态都相同就是同一组星历 
//每个卫星保存一组最新的星历
int decode_gpseph(Raw* raw,Out_nav*out_nav)
{
	int index = 0;
	raw->eph.prn = U4(raw->buff + OEM4HEADLEN);
	raw->eph.toc = R8(raw->buff + 164 + OEM4HEADLEN);
	raw->eph.week = U4(raw->buff + 24 + OEM4HEADLEN);
	raw->eph.toe = R8(raw->buff + 32 + OEM4HEADLEN);
	raw->eph.health = U4(raw->buff + 12 + OEM4HEADLEN);
	//先找之前有没有这个卫星，如果有的话直接在上面修改，没有的话增加
	int prn_find_index = find(raw->eph.prn, out_nav->prn, out_nav->num - 1);
	if ((prn_find_index >= 0) && ((out_nav->toe[prn_find_index] != raw->eph.toe) || (out_nav->week[prn_find_index] != raw->eph.week)))//同一卫星的一组新的星历
	{
		index = prn_find_index;
		out_nav->is_change = 1;
	}
	else if (prn_find_index < 0) {
		index = out_nav->num;
		out_nav->num++;
	}
	else if ((prn_find_index >= 0) && (out_nav->toe[prn_find_index] == raw->eph.toe) && (out_nav->week[prn_find_index] == raw->eph.week)) {
		return 1;
	}

	//init_out_nav(out_nav);
	out_nav->prn[index] = raw->eph.prn;
	out_nav->toc[index] = raw->eph.toc;
	out_nav->week[index] = raw->eph.week;
	out_nav->toe[index] = raw->eph.toe;
	out_nav->health[index] = raw->eph.health;

	raw->eph.sys = 0;
	//raw->eph.prn= U4(raw->buff + OEM4HEADLEN);
	raw->eph.tow = R8(raw->buff +4+ OEM4HEADLEN);
	//raw->eph.health = U4(raw->buff +12+ OEM4HEADLEN);
	raw->eph.IODE1 = U4(raw->buff + +16+OEM4HEADLEN);
	raw->eph.IODE2 = U4(raw->buff +20+ OEM4HEADLEN);
	//raw->eph.week = U4(raw->buff+24 + OEM4HEADLEN);
	//raw->eph.toe = R8(raw->buff +32+ OEM4HEADLEN);
	raw->eph.sqrt_A = sqrt(R8(raw->buff + 40+OEM4HEADLEN));
	raw->eph.d_N = R8(raw->buff+48 + OEM4HEADLEN);
	raw->eph.M0 = R8(raw->buff +56+ OEM4HEADLEN);
	raw->eph.e = R8(raw->buff+64 + OEM4HEADLEN);
	raw->eph.omage = R8(raw->buff+72 + OEM4HEADLEN);
	raw->eph.cuc = R8(raw->buff+80 + OEM4HEADLEN);
	raw->eph.cus = R8(raw->buff +88+ OEM4HEADLEN);
	raw->eph.crc = R8(raw->buff +96+ OEM4HEADLEN);
	raw->eph.crs = R8(raw->buff+104 + OEM4HEADLEN);
	raw->eph.cic = R8(raw->buff +112+ OEM4HEADLEN);
	raw->eph.cis = R8(raw->buff+120 + OEM4HEADLEN);
	raw->eph.I0 = R8(raw->buff +128+ OEM4HEADLEN);
	raw->eph.d_I = R8(raw->buff +136+ OEM4HEADLEN);
	raw->eph.omage0 = R8(raw->buff+144 + OEM4HEADLEN);
	raw->eph.d_omage0 = R8(raw->buff+152 + OEM4HEADLEN);
	raw->eph.IODC = U4(raw->buff+160 + OEM4HEADLEN);
	//raw->eph.toc = R8(raw->buff +164+ OEM4HEADLEN);
	raw->eph.tgd[0] = R8(raw->buff+172 + OEM4HEADLEN);
	raw->eph.f0 = R8(raw->buff +180+ OEM4HEADLEN);
	raw->eph.f1 = R8(raw->buff+188 + OEM4HEADLEN);
	raw->eph.f2 = R8(raw->buff+196 + OEM4HEADLEN);
	raw->eph.AS = U4(raw->buff +204+ OEM4HEADLEN);
	raw->eph.N = R8(raw->buff+208 + OEM4HEADLEN);
	raw->eph.URA = R8(raw->buff+216 + OEM4HEADLEN);

	out_nav->out_eph[index] = raw->eph;
	//memset(raw->buff, 0, MAXOBS);//清空缓存区
	//out_nav = free_out_nav();
	//std::cout << raw->msgID << std::endl;

	return 1;
}

int decode_bdseph(Raw* raw, Out_nav* out_nav)
{
	int index = 0;
	raw->eph.prn = U4(raw->buff + OEM4HEADLEN);
	raw->eph.toc = U4(raw->buff + 40 + OEM4HEADLEN);
	raw->eph.week = U4(raw->buff + 4 + OEM4HEADLEN);
	raw->eph.toe = U4(raw->buff + 72 + OEM4HEADLEN);
	raw->eph.health = U4(raw->buff + 16 + OEM4HEADLEN) & 1;

	//先找之前有没有这个卫星，如果有的话直接在上面修改，没有的话增加
	int prn_find_index = find(raw->eph.prn, out_nav->prn, out_nav->num - 1);
	if ((prn_find_index >= 0) && ((out_nav->toe[prn_find_index] != raw->eph.toe) || (out_nav->week[prn_find_index] != raw->eph.week)))//同一卫星的一组新的星历
	{
		index = prn_find_index;
		out_nav->is_change = 1;
	}
	else if (prn_find_index < 0) {
		index = out_nav->num;
		out_nav->num++;
	}
	else if ((prn_find_index >= 0) && (out_nav->toe[prn_find_index] == raw->eph.toe) && (out_nav->week[prn_find_index] == raw->eph.week)) {
		return 1;
	}

	out_nav->prn[index] = raw->eph.prn;
	out_nav->toc[index] = raw->eph.toc;
	out_nav->week[index] = raw->eph.week;
	out_nav->toe[index] = raw->eph.toe;
	out_nav->health[index] = raw->eph.health;

	raw->eph.sys = 1;
	raw->eph.singla = get_singla(raw, 0);
	//raw->eph.prn = U4(raw->buff + OEM4HEADLEN);
	//raw->eph.health = U4(raw->buff + 16 + OEM4HEADLEN)&1;
	//raw->eph.week = U4(raw->buff + 4 + OEM4HEADLEN);
	//raw->eph.toe = U4(raw->buff + 72 + OEM4HEADLEN);
	raw->eph.sqrt_A = R8(raw->buff + 76 + OEM4HEADLEN);
	raw->eph.d_N = R8(raw->buff + 100 + OEM4HEADLEN);
	raw->eph.M0 = R8(raw->buff + 108 + OEM4HEADLEN);
	raw->eph.e = R8(raw->buff + 84 + OEM4HEADLEN);
	raw->eph.omage = R8(raw->buff + 92 + OEM4HEADLEN);
	raw->eph.cuc = R8(raw->buff + 148 + OEM4HEADLEN);
	raw->eph.cus = R8(raw->buff + 156 + OEM4HEADLEN);
	raw->eph.crc = R8(raw->buff + 164 + OEM4HEADLEN);
	raw->eph.crs = R8(raw->buff + 172 + OEM4HEADLEN);
	raw->eph.cic = R8(raw->buff + 180 + OEM4HEADLEN);
	raw->eph.cis = R8(raw->buff + 188 + OEM4HEADLEN);
	raw->eph.I0 = R8(raw->buff + 132 + OEM4HEADLEN);
	raw->eph.d_I = R8(raw->buff + 140 + OEM4HEADLEN);
	raw->eph.omage0 = R8(raw->buff + 116 + OEM4HEADLEN);
	raw->eph.d_omage0 = R8(raw->buff + 124 + OEM4HEADLEN);
	//raw->eph.toc = U4(raw->buff + 40 + OEM4HEADLEN);
	raw->eph.tgd[0] = R8(raw->buff + 20 + OEM4HEADLEN);
	raw->eph.tgd[1] = R8(raw->buff + 28 + OEM4HEADLEN);
	raw->eph.f0 = R8(raw->buff + 44 + OEM4HEADLEN);
	raw->eph.f1 = R8(raw->buff + 52 + OEM4HEADLEN);
	raw->eph.f2 = R8(raw->buff + 60 + OEM4HEADLEN);
	raw->eph.URA = R8(raw->buff +8 + OEM4HEADLEN);
	raw->eph.AODC = U4(raw->buff + 36 + OEM4HEADLEN);
	raw->eph.AODE = U4(raw->buff + 68 + OEM4HEADLEN);

	out_nav->out_eph[index] = raw->eph;

	//memset(raw->buff, 0, MAXOBS);//清空缓存区
	//out_nav = free_out_nav();
	//std::cout << raw->msgID << std::endl;
	return 1;
}

int decode_ion(Raw* raw,Ion* ion)
{
	ion->a1 = R8(raw->buff + OEM4HEADLEN);
	ion->a2 = R8(raw->buff + 8 + OEM4HEADLEN);
	ion->a3 = R8(raw->buff + 16 + OEM4HEADLEN);
	ion->a4 = R8(raw->buff + 24 + OEM4HEADLEN);
	ion->b1 = R8(raw->buff + 32 + OEM4HEADLEN);
	ion->b2 = R8(raw->buff + 40 + OEM4HEADLEN);
	ion->b3 = R8(raw->buff + 48 + OEM4HEADLEN);
	ion->b4 = R8(raw->buff + 56 + OEM4HEADLEN);
	ion->deltat_ls = R8(raw->buff + 96 + OEM4HEADLEN);
	return 0;
}

/*获取卫星的系统*/
int get_sys(Raw* raw, int num)
{
	int sys = ((U4(raw->buff + 44 + num * LENOFOBS + OEM4HEADLEN)) >> 16) & 7;
	
	//sys=0:GPS sys=1:BDS
	if (sys == GPS_SYS) { sys = 0; }
	else if (sys == BDS_SYS) { sys = 1; }
	else sys = -1;
	return sys;
}

/*获取卫星观测信号 num:该数据包中第几个观测值*/
int get_singla(Raw* raw,int num)
{
	int sys = get_sys(raw, num);
	int singla = ((U4(raw->buff + 44 + num * LENOFOBS + OEM4HEADLEN)) >> 21) & 0x1F;

	//映射到输出顺序 0:L1C/A或D1 1:L2P(Y)或D2
	if (sys == -1) singla = -1;
	if (sys == 0 || sys == 1)
	{
		if (singla == GPS_L1 || singla == BDS1_D2|| singla == BDS1_D1) { singla = 0; }
		else if (singla == GPS_L2 || singla == BDS3_D1 || singla == BDS3_D2) { singla = 1; }
		else singla = -1;
	}
	return singla;
}

