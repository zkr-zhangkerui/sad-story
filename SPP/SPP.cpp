/*获取当前历元的观测数据和对应的广播星历*/
/*计算卫星位置*/

#include "SPP.h"
#include "TropCorrect.h"
#include "klobuchar.h"

//Pos pos;

//观测值数据 out_obs 有没有这颗星的可用星历
//星历数据 out_nav 只保存最新的一组星历（先默认已有的就是最新的）
//pre_result 存储上个历元的解算结果

int pvt_spp(Out_obs*out_obs,Out_nav*out_nav_g, Out_nav* out_nav_b, FILE*sate_pos_file,Pos* pos, Out_obs* out_obs0, int& epochCount)
{
	/*计算卫星位置*/
	SateResult result[4096] = {}, now_sate;
	GPSTIME tk,toc,tk_b,lt1; int prn; int sys;
	tk = out_obs->t_obs;//卫星钟面时
	tk_b = gpstime2bdstime(tk);
	MJD mjd = gpstime2mjd(tk);
	YMDHMS ymdhms = mjd2ymdhms(mjd);
	GPSTIME lt;//信号发射时间
	int Num=0;//当前历元观测值和星历都有数据的卫星数量
	int number = 0;//迭代次数
	//Vel vel;
	double ZTD=0.0; double P0 = 0.0; double th = 0.0; double omage_e = 0.0; double P = 0.0; double clk = 0.0; double PDOP = 0.0; double delta_omage_e = 0.0; double PDOP_V = 0.0;
	int true_num_bds = 0; int true_num_gps = 0;int dim = 0;//成功解算的卫星数 未知数个数
	double high_angle = 0.0; double position_angle = 0.0;//卫星高度角 方位角
	double UT = 0.0;//世界时的秒数
	double T_ion = 0.0;//电离层延迟改正
	Out_nav* out_nav=nullptr;
	pos->true_num = out_obs->num;
	Eigen::MatrixXd R; Eigen::MatrixXd D_enu; Eigen::MatrixXd D_after_pos;
	//文件的基站位置
	pos->origin[0] = -2267804.5260; pos->origin[1] = 5009342.372; pos->origin[2] = 3220991.863;
	pos->origin_blh[1] = 1.995907966; pos->origin_blh[0] = 0.5328272449235; pos->origin_blh[2] = 39.9173343685;
	//数据流的基站位置
	//pos->origin[0] = -2267335.669; pos->origin[1] = 5008649.155; pos->origin[2] = 3222374.973;
	//pos->origin_blh[1] = 1.995880081755; pos->origin_blh[0] = 0.53307534966; pos->origin_blh[2] = 33.4591880944;

	for(int i = 0; i < out_obs->num; i++)
	{
		prn = out_obs->prn[i];
		sys = out_obs->sys[i];
		if (prn == 0) {
			pos->true_num--;
			continue;
		}
		
		//信噪比
		if ((out_obs->obs_t[i][sys * 8 + 3] < 35) || (out_obs->obs_t[i][sys * 8 + 7] < 35)) {
			pos->true_num--;
			continue;
		}

		/*判断观测值是否可用*/
		//GPS
		if (sys == 0)
		{
			if(opt.sys==1)continue;
			if (opt.fre == 1) { if ((out_obs->obs_t[i][0] == 0) || (out_obs->obs_t[i][4] == 0)) { pos->true_num--; continue; } }//单频不参与解算
			if (opt.fre == 0) { if (out_obs->obs_t[i][0] == 0) { pos->true_num--; continue; } }//单频
			if ((out_obs->obs_t[i][0] < 1E-3) || (out_obs->obs_t[i][4] < 1E-3)) { pos->true_num--; continue; }//观测值异常
			out_nav = out_nav_g;
		}
		//北斗
		if (sys == 1)
		{
			if(opt.sys==0)continue;
			if (opt.fre == 1) { if ((out_obs->obs_t[i][8] == 0) || (out_obs->obs_t[i][12] == 0)) { pos->true_num--; continue; } }//单频不参与解算
			if (opt.fre == 0) { if (out_obs->obs_t[i][8] == 0) { pos->true_num--; continue; } }//单频
			if ((out_obs->obs_t[i][8] < 1E-3) || (out_obs->obs_t[i][12] < 1E-3)) { pos->true_num--; continue; }//观测值异常
			out_nav = out_nav_b;
		}

		//查找当前星历里面有没有对应的卫星，有则解算
		for (int j = 0; j < MAXSATE; j++)
		{
			if ((prn == out_nav->out_eph[j].prn) && (sys == out_nav->out_eph[j].sys))
			{
				//一致性检验 跳过第一个历元
				if (opt.is_consistant == 0) {
					if (consistency_test(epochCount, prn, sys, out_obs, out_obs0, i) == false)
						continue;
				}

				if (sys == 0) {
					if(opt.sys==1)continue;
					else {
						if (satePos(&(out_nav->out_eph[j]), tk, &result[i], sate_pos_file) == -1) {
							continue;//卫星位置解算失败
						}
					}
				}
				if (sys == 1) {
					if (opt.sys == 0)continue;
					else {
						if (satePos(&(out_nav->out_eph[j]), tk_b, &result[i], sate_pos_file) == -1) {
							continue;//卫星位置解算失败
						}
					}
				}
				
				/*计算信号发射时刻*/
				//先不考虑钟差
				P = out_obs->obs_t[i][ sys * 8];
				toc.GPSweek = out_nav->out_eph[j].week; toc.secsofweek = int(out_nav->out_eph[j].toc);
				toc.fracofsec = out_nav->out_eph[j].toc - toc.secsofweek;
				if (sys == 0) {
					double lt_double = tk - sec2gpstime((P) / c);
					lt = sec2gpstime(lt_double);
					double d_t = lt - toc;
					double clk = out_nav->out_eph[j].f0 + out_nav->out_eph[j].f1 * d_t + out_nav->out_eph[j].f2 * d_t * d_t + result[i].tr;
					lt = sec2gpstime(lt_double - clk);
				}
				if (sys == 1) {
					//double lt_double = tk_b - gpstime2bdstime(sec2gpstime((P)/ c));
					double lt_double = tk - sec2gpstime((P) / c);
					lt = gpstime2bdstime(sec2gpstime(lt_double));
					double d_t = lt - toc;
					double clk = out_nav->out_eph[j].f0 + out_nav->out_eph[j].f1 * d_t + out_nav->out_eph[j].f2 * d_t * d_t + result[i].tr;
					lt = gpstime2bdstime(sec2gpstime(lt_double - clk)); 
				}

				//信号发射时刻卫星位置 北斗GEO卫星改正 相对论效应 地球自转改正
				if(satePos(&(out_nav->out_eph[j]), lt, &result[i], sate_pos_file)==-1)continue;
				
				//地球自转改正
				if (sys == 0) {
					omage_e = omage_e_GPS;
				}
				if (sys == 1){
					omage_e = omage_e_BDS;
					lt = bdstime2gpstime(lt);
				}
				
				delta_omage_e = omage_e * (tk-lt);//使用lt-tk还是伪距  使用几何距离
				result[i].X1 = delta_omage_e * result[i].Y + result[i].X;
				result[i].Y1 = -delta_omage_e * result[i].X + result[i].Y;
				result[i].Z1 = result[i].Z;

				P0 = sqrt(pow((result[i].X1 - pos->recpos_0(0, 0)), 2) + pow((result[i].Y1 - pos->recpos_0(1, 0)), 2) + pow((result[i].Z1 - pos->recpos_0(2, 0)), 2));
				delta_omage_e = omage_e * (P0/c);//使用lt-tk还是伪距  使用几何距离
				result[i].X1 = delta_omage_e * result[i].Y + result[i].X;
				result[i].Y1 = -delta_omage_e * result[i].X + result[i].Y;
				result[i].Z1 = result[i].Z;

				//计算卫星高度角
				high_angle=SatElev(&result[i], pos->recpos_0);
				if (high_angle < PI / 18) { 
					pos->true_num--;
					continue;
				}

				/*近似值*/
				P0 = sqrt(pow((result[i].X1 - pos->recpos_0(0,0)), 2) + pow((result[i].Y1 - pos->recpos_0(1, 0)), 2) + pow((result[i].Z1 - pos->recpos_0(2, 0)), 2));

				if (sys == 0) true_num_gps++;
				if (sys == 1) true_num_bds++;

				if (opt.fre == 1)
				{/* 构建双频消电离层伪距组合*/
					if (sys == 0) {
						pos->P_IF(Num, 0) = (f1_G * f1_G * (out_obs->obs_t[i][0]) - f2_G * f2_G * (out_obs->obs_t[i][4])) / (f1_G * f1_G - f2_G * f2_G);
						pos->D_obs(Num, 0) = -(out_obs->obs_t[i][2] * c / (f1_G * 1000000));
					}
					if (sys == 1) {
						pos->P_IF(Num, 0) = (((out_obs->obs_t[i][12]) - k_1_3 * (out_obs->obs_t[i][8])) + c * k_1_3 * out_nav->out_eph[j].tgd[0]) / (1 - k_1_3);
						pos->D_obs(Num, 0) = -(out_obs->obs_t[i][10] * c / (B1_I * 1000000));
					}
				}

				//测站高程 对流层改正
				if (XYZ2BLH(pos->recpos_0, pos->recpos_blh)) {
					if ((pos->recpos_blh[2] > 30000) || (pos->recpos_blh[2] < 0))ZTD = 0;
					else {
						trop.h = pos->recpos_blh[2];
						ZTD = hopfield(&trop, high_angle);
					}
				}
				else ZTD = 0.0;

				if (opt.fre == 0)
				{//klobuchar改正
					if ((XYZ2BLH(pos->recpos_0, pos->recpos_blh)) && (pos->recpos_blh[2] > 0))
					{
						UT = gpstime2utc(&tk, ion.deltat_ls);
						position_angle = pos_angle(pos->recpos_blh, pos->recpos_0, result[i]);
						T_ion = klobuchar(position_angle, ion, high_angle, UT, pos->recpos_blh);
					}
					else T_ion = 0;

					if (sys == 0) {
						pos->P_IF(Num, 0) = out_obs->obs_t[i][0];
						pos->D_obs(Num, 0) = -(out_obs->obs_t[i][2] * c / (f1_G * 1000000));
					}
					if (sys == 1) {
						pos->P_IF(Num, 0) = out_obs->obs_t[i][8];
						pos->D_obs(Num, 0) = -(out_obs->obs_t[i][10] * c / (B1_I * 1000000));
					}
				}

				/*最小二乘解算所需矩阵*/
				if (sys == 0) {
					result[i].sys = 0;
					if (opt.fre == 1) {
						if (opt.is_ZTD == 1)pos->Z(Num, 0) = pos->P_IF(Num, 0) + c * result[i].clk - P0 - pos->recpos_0(3, 0);//不改对流层延迟
						else pos->Z(Num, 0) = pos->P_IF(Num, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(3, 0);
					}
					else {
						pos->Z(Num, 0) = pos->P_IF(Num, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(3, 0);
					}
				}
				if (sys == 1) {
					result[i].sys = 1;
					if (opt.sys == 2) { 
						if (opt.fre == 1) {
							if (opt.is_ZTD == 1)pos->Z(Num, 0) = pos->P_IF(Num, 0) + c * result[i].clk - P0 - pos->recpos_0(4, 0);//不改对流层延迟
							else pos->Z(Num, 0) = pos->P_IF(Num, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(4, 0);
						}
						else {
							pos->Z(Num, 0) = pos->P_IF(Num, 0) + c * result[i].clk - ZTD- P0 - pos->recpos_0(4, 0);
						}
					}
					if (opt.sys == 1) {
						if (opt.fre == 1) {
							if (opt.is_ZTD == 1)pos->Z(Num, 0) = pos->P_IF(Num, 0) + c * result[i].clk - P0 - pos->recpos_0(3, 0);//不改对流层延迟
							else pos->Z(Num, 0) = pos->P_IF(Num, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(3, 0);
						}
						else {
							pos->Z(Num, 0) = pos->P_IF(Num, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(3, 0);
						}
					}
				}//残差向量 未考虑接收机钟差的近似值 应该再减去一个接收机钟差的近似值（北斗（需要改TGD？）和GPS不同）
				pos->H(Num, 0) = (pos->recpos_0(0, 0) - result[i].X1) / P0;
				pos->H(Num, 1) = (pos->recpos_0(1, 0) - result[i].Y1) / P0;
				pos->H(Num, 2) = (pos->recpos_0(2, 0) - result[i].Z1) / P0;
				if (opt.sys != 2) {
					pos->H(Num, 3) = 1;
				}
				else {
					if (out_obs->sys[i] == 0) { pos->H(Num, 3) = 1; pos->H(Num, 4) = 0; }
					if (out_obs->sys[i] == 1) { pos->H(Num, 3) = 0; pos->H(Num, 4) = 1; }
				}
				Num++;
				result[Num-1] = result[i];
				//fprintf(sate_pos_file, "%10d %20.10lf %20.10lf %20.10lf\n", lt.secsofweek, lt.fracofsec,ZTD,high_angle*180/PI);
				//if (Num == 8)printf("%.3f %.10f %.3f %.3f %.3f %.3f\n", pos->P_IF(Num-1, 0), result[i].clk, ZTD, P0, pos->recpos_0(4, 0), pos->Z(Num-1, 0));
				break;
			}
			
		}
	}
	
	if (true_num_gps * true_num_bds == 0)
	{
		if (opt.sys == 2) {
			//if(true_num_gps==0)fprintf(sate_pos_file, "only GPS system\n");
			//if (true_num_bds == 0)fprintf(sate_pos_file, "only BDS system\n");
			return -1;//先不考虑单系统
		}
		dim = 4;
		if (Num < 4) {
			return -1;
		}//没有足够的卫星数 判断参与解算的是单系统（4个参数）还是双系统（5个参数）
	}
	else
	{
		if(opt.sys!=2)return -1;//单系统
		dim = 5;
		if (Num < 5) {
			return -1;
		}
	}

	//调整全零行
	remove_zero_rows_and_cols(pos, 1e-12,dim);

	// 协方差矩阵初始化为单位矩阵
	pos->D.resize(pos->row, pos->row);
	for (int i = 0; i < pos->row; i++) {
		for (int j = 0; j < pos->row; j++) {
			pos->D(i,j) = (i == j) ? 1.0 : 0.0;
		}
	}
	
	//realloc_pos_intermediate(pos, &rc);
	/*
	Change(pos->H, pos->H_change, rc.row, rc.col);
	Inv(pos->D, pos->Inv_D, rc.row);
	Mult(pos->H_change, pos->Inv_D,pos->Q1, rc.col, rc.row, rc.row);
	Mult(pos->Q1, pos->H,pos->Q, rc.col, rc.row, rc.col);
	Inv(pos->Q, pos->Inv_Q, rc.col);
	Mult(pos->Inv_Q, pos->H_change, pos->recpos1, rc.col,rc.col, rc.row);
	Mult(pos->recpos1, pos->Inv_D, pos->recpos2, rc.col, rc.row, rc.row);
	Mult(pos->recpos2, pos->Z, pos->recpos3, rc.col, rc.row,1);
	Add(pos->recpos3, pre_recpos, pos->recpos, rc.col,1);
	Minus(pos->recpos, pre_recpos, pos->d_recpos, rc.col, rc.col);
	th = Norm(rc.col, pos->d_recpos);*/

	Eigen::MatrixXd time; time.resize(static_cast<Eigen::Index>(dim) - 3, 1);
	
	pos->H_change = pos->H.transpose();
	pos->Inv_D = pos->D.inverse();
	pos->Q = pos->H_change * pos->Inv_D * pos->H;
	pos->Inv_Q = safeInverse(pos->Q, 1e-8);
	pos->recpos3 = pos->Inv_Q * pos->H_change * pos->Inv_D * pos->Z;
	pos->recpos = pos->recpos3 + pos->recpos_0;
	//time = pos->recpos3.block(3, 0, dim-3, 1);
	//pos->recpos.block(3, 0, dim-3, 1) = time;
	pos->d_recpos = pos->recpos - pos->recpos_0;
	th = pos->d_recpos.norm();

	while (th > 1E-6 && number < 11)
	{
		number++; 
		//Copy(pos->recpos, pos->recpos_0, dim, 1);
		pos->recpos_0 = pos->recpos;
		/*更新最小二乘解算所需矩阵*/
		for (int i = 0; i < pos->row; i++) {
			
			//地球自转改正
			P0 = sqrt(pow((result[i].X1 - pos->recpos_0(0, 0)), 2) + pow((result[i].Y1 - pos->recpos_0(1, 0)), 2) + pow((result[i].Z1 - pos->recpos_0(2, 0)), 2));
			
			if (result[i].sys == 0) {
				omage_e = omage_e_GPS;
			}
			if (result[i].sys == 1) {
				omage_e = omage_e_BDS;
			}
			delta_omage_e = omage_e * (P0 / c);//使用伪距还是计算的几何距离
			result[i].X1 = delta_omage_e * result[i].Y + result[i].X;
			result[i].Y1 = -delta_omage_e * result[i].X + result[i].Y;
			result[i].Z1 = result[i].Z;

			/*重新计算高度角*/
			high_angle = SatElev(&result[i], pos->recpos_0);
			if (high_angle < PI / 18) {
				for (int j = i; j < pos->row-1; j++) {
					result[j] = result[j + 1];
					//cal_eph[j] = cal_eph[j + 1];
					pos->Z.row(j) = pos->Z.row(j + 1);
					pos->H.row(j) = pos->H.row(j + 1);
					pos->P_IF.row(j) = pos->P_IF.row(j + 1);
				}
				pos->Z.row(pos->row - 1).setZero();
				pos->H.row(pos->row - 1).setZero();
				pos->P_IF.row(pos->row - 1).setZero();
				pos->row--;
				i = i - 1;
				continue;
			}
			if (pos->row < dim)
				return -1;
			pos->D.resize(pos->row, pos->row);
			for (int i = 0; i < pos->row; i++) {
				for (int j = 0; j < pos->row; j++) {
					pos->D(i, j) = (i == j) ? 1.0 : 0.0;
				}
			}
			remove_zero_rows_and_cols(pos, 1e-12,dim);

			/*对流层改正*/
			if ((XYZ2BLH(pos->recpos_0, pos->recpos_blh))){
				if ((pos->recpos_blh[2] > 30000) || (pos->recpos_blh[2] < 0))ZTD = 0;
				else {
					trop.h = pos->recpos_blh[2];
					ZTD = hopfield(&trop, high_angle);
				}
			}
			else ZTD = 0.0;

			if (opt.fre == 0) {
				//klobuchar改正
				if ((XYZ2BLH(pos->recpos_0, pos->recpos_blh)) && (pos->recpos_blh[2] > 0))
				{
					UT = gpstime2utc(&tk, ion.deltat_ls);
					position_angle = pos_angle(pos->recpos_blh, pos->recpos_0, result[i]);
					T_ion = klobuchar(position_angle, ion, high_angle, UT, pos->recpos_blh);
				}
				else T_ion = 0;
			}

			P0 = sqrt(pow(result[i].X1 - pos->recpos_0(0, 0), 2) + pow(result[i].Y1 - pos->recpos_0(1, 0), 2) + pow(result[i].Z1 - pos->recpos_0(2, 0), 2));

			if (result[i].sys == 0) {
				result[i].sys = 0;
				if (opt.fre == 1) {
					if (opt.is_ZTD == 1)pos->Z(i, 0) = pos->P_IF(i, 0) + c * result[i].clk - P0 - pos->recpos_0(3, 0);//不改对流层延迟
					else pos->Z(i, 0) = pos->P_IF(i, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(3, 0);
				}
				else {
					pos->Z(i, 0) = pos->P_IF(i, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(3, 0);
				}
			}
			if (result[i].sys == 1) {
				result[i].sys = 1;
				if (opt.sys == 2) {
					if (opt.fre == 0) {
						pos->Z(i, 0) = pos->P_IF(i, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(4, 0);
					}
					else {
						if (opt.is_ZTD == 1)pos->Z(i, 0) = pos->P_IF(i, 0) + c * result[i].clk - P0 - pos->recpos_0(4, 0);//不改对流层延迟
						else pos->Z(i, 0) = pos->P_IF(i, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(4, 0);
					}
				}
				if (opt.sys == 1) {
					if (opt.fre == 0) {
						pos->Z(i, 0) = pos->P_IF(i, 0) + c * result[i].clk - ZTD- P0 - pos->recpos_0(3, 0);
					}
					else {
						if (opt.is_ZTD == 1)pos->Z(i, 0) = pos->P_IF(i, 0) + c * result[i].clk - P0 - pos->recpos_0(3, 0);//不改对流层延迟
						else pos->Z(i, 0) = pos->P_IF(i, 0) + c * result[i].clk - ZTD - P0 - pos->recpos_0(3, 0);
					}
				}
			}//残差向量 未考虑接收机钟差的近似值 应该再减去一个接收机钟差的近似值（北斗（需要改TGD？）和GPS不同）
			pos->H(i, 0) = (pos->recpos_0(0,0) - result[i].X1) / P0;
			pos->H(i,1) = (pos->recpos_0(1,0) - result[i].Y1) / P0;
			pos->H(i,2) = (pos->recpos_0(2,0) - result[i].Z1) / P0;
			if (opt.sys != 2) {
				pos->H(i, 3) = 1;
			}
		
			//fprintf(sate_pos_file, "%02d %-15.6f %-15.6f %-15.6f %-15.15f %-15.10f %-15.10f %-15.10f\n", 
				//result[i].sys, result[i].d_X, result[i].d_Y, result[i].d_Z,result[i].clkbias,ZTD,high_angle*180/PI,pos->P_IF(i,0));
			//if (i == 7)printf("%.3f %.10f %.3f %.3f %.3f %.3f\n", pos->P_IF(i, 0), result[i].clk, ZTD,P0, pos->recpos_0(4, 0),pos->Z(i,0));
		}
		
		pos->H_change = pos->H.transpose();
		pos->Inv_D = pos->D.inverse();
		pos->Q = pos->H_change * pos->Inv_D * pos->H;
		pos->Inv_Q = safeInverse(pos->Q, 1e-8);
		pos->recpos3 = pos->Inv_Q * pos->H_change * pos->Inv_D * pos->Z;
		pos->recpos = pos->recpos3 + pos->recpos_0;
		//time = pos->recpos3.block(3, 0, dim - 3, 1);
		//pos->recpos.block(3, 0, dim - 3, 1) = time;
		pos->d_recpos = pos->recpos - pos->recpos_0;
		th = pos->d_recpos.norm();
	}
	
	//Copy(pos->recpos,pre_recpos, dim, 1);
	pos->recpos_0 = pos->recpos;
	//输出时间
	fprintf(sate_pos_file, "%2c %4d %8d ", '*', tk.GPSweek, tk.secsofweek);
	XYZ2BLH(pos->recpos_0, pos->recpos_blh);
	R=XYZ2ENU(pos->recpos_0, pos->recpos_enu, pos->origin, pos->origin_blh);
	
	
	/*精度评定*/
	pos->V = pos->H * pos->d_recpos - pos->Z;
	pos->std = sqrt((((pos->V.transpose()) * pos->Inv_D * pos->V) / (pos->row - dim))(0,0));
	pos->D_after = pos->std * pos->std * pos->Inv_Q;
	D_after_pos = pos->D_after.block(0, 0, 3, 3);
	PDOP = sqrt(pos->D_after(0, 0) + pos->D_after(1, 1) + pos->D_after(2, 2));
	D_enu = R * D_after_pos * R.transpose();

	/*单点测速*/
	for (int i = 0; i < pos->row; i++) {
		P0 = sqrt(pow(result[i].X1 - pos->recpos_0(0, 0), 2) + pow(result[i].Y1 - pos->recpos_0(1, 0), 2) + pow(result[i].Z1 - pos->recpos_0(2, 0), 2));
		pos->H_V(i, 0) = (pos->recpos_0(0,0)-result[i].X1)/P0;
		pos->H_V(i, 1) = (pos->recpos_0(1, 0) - result[i].Y1) / P0;
		pos->H_V(i, 2) = (pos->recpos_0(2, 0) - result[i].Z1) / P0;
		pos->H_V(i, 3) = 1;

		pos->P_V(i,0) = 
			((result[i].X1 - pos->recpos_0(0, 0)) * result[i].d_X + (result[i].Y1 - pos->recpos_0(1, 0)) * result[i].d_Y + (result[i].Z1 - pos->recpos_0(2, 0)) * result[i].d_Z)/P0;
		pos->Z_V(i, 0) = pos->D_obs(i, 0) - pos->P_V(i, 0) + c * result[i].clkbias;
	}
	remove_zero_rows_and_cols_V(pos, 1e-12);
	pos->H_V_change = pos->H_V.transpose();
	pos->Q_V = pos->H_V_change * pos->Inv_D * pos->H_V;
	pos->Q_V_Inv = pos->Q_V.inverse();
	pos->vel = pos->Q_V_Inv*pos->H_V_change* pos->Inv_D * pos->Z_V;
	pos->V_vel = pos->H_V * pos->vel - pos->Z_V;
	pos->std_after_vel = sqrt(((pos->V_vel.transpose() * pos->Inv_D * pos->V_vel) / (pos->row - 4))(0, 0));
	pos->D_after_vel = pos->std_after_vel * pos->std_after_vel * pos->Q_V_Inv;
	PDOP_V= sqrt(pos->D_after_vel(0, 0) + pos->D_after_vel(1, 1) + pos->D_after_vel(2, 2));

	double a = sqrt(abs(D_enu(1, 1)));

	/*输出解算结果*/
	if (opt.sys == 2) {
		fprintf(sate_pos_file, "%3d %3d %3d %-15.4lf %-15.4lf %-15.4lf %-15.4lf  %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf\n",
			pos->row, true_num_gps, true_num_bds, pos->recpos_0(0, 0), pos->recpos_0(1, 0), pos->recpos_0(2, 0), pos->recpos_blh[0], pos->recpos_blh[1], pos->recpos_blh[2], pos->recpos_enu[0], pos->recpos_enu[1], pos->recpos_enu[2],
			pos->recpos(3, 0), pos->recpos(4, 0), pos->std, PDOP, sqrt(pos->D_after(0, 0)), sqrt(pos->D_after(1, 1)), sqrt(pos->D_after(2, 2)),
			sqrt(abs(D_enu(0, 0))), sqrt(abs(D_enu(1, 1))), sqrt(abs(D_enu(2, 2))), sqrt(pos->D_after(3, 3)), sqrt(pos->D_after(4, 4)),
			pos->vel(0, 0), pos->vel(1, 0), pos->vel(2, 0), PDOP_V, sqrt(pos->D_after_vel(0, 0)), sqrt(pos->D_after_vel(1, 1)), sqrt(pos->D_after_vel(2, 2)), sqrt(pos->D_after_vel(3, 3)));
	}
	if (opt.sys != 2) {
		fprintf(sate_pos_file, "%3d %3d %3d %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf %-15.4lf\n",
			pos->row, true_num_gps, true_num_bds, pos->recpos_0(0, 0), pos->recpos_0(1, 0), pos->recpos_0(2, 0), pos->recpos_blh[0], pos->recpos_blh[1], pos->recpos_blh[2], pos->recpos_enu[0], pos->recpos_enu[1], pos->recpos_enu[2],
			pos->recpos(3, 0), pos->std, PDOP, sqrt(pos->D_after(0, 0)), sqrt(pos->D_after(1, 1)), sqrt(pos->D_after(2, 2)),
			sqrt(abs(D_enu(0))), sqrt(abs(D_enu(4))), sqrt(abs(D_enu(8))), sqrt(pos->D_after(3, 3)), 
			pos->vel(0, 0), pos->vel(1, 0), pos->vel(2, 0), PDOP_V, sqrt(pos->D_after_vel(0, 0)), sqrt(pos->D_after_vel(1, 1)), sqrt(pos->D_after_vel(2, 2)), sqrt(pos->D_after_vel(3, 3)));
	}
	
	return 0;

}
