#include "MP.h"
#include "read_obs.h"

//对于静态 GPS使用L1和L2，北斗使用B1和B3，GLONASS使用G1和G2
void cal_MP(EpochData epoch,FILE*obs_fp,FILE*MP_C,FILE*MP_R, std::vector<epoch_MP>*epoch_mp_g, std::vector<epoch_MP>*epoch_mp_c, std::vector<epoch_MP>*epoch_mp_r,int epochCount)
{

	for (int i = 0; i < epoch.numSats; i++)
	{
		int prn = std::stoi(epoch.satellites[i].satID.substr(1));
		if ((epoch.satellites[i].satID[0] == 'G')&&(epoch.satellites[i].observations[0]!=0)&&(epoch.satellites[i].observations[1]!=0)
			&& (epoch.satellites[i].observations[3] != 0) && (epoch.satellites[i].observations[4] != 0))
		{
			(*epoch_mp_g)[prn].satID = epoch.satellites[i].satID;
			(*epoch_mp_g)[prn].MP1[epochCount-1]=(epoch.satellites[i].observations[0] - G_f1 * epoch.satellites[i].observations[3] * c / f1_G + G_f2 * epoch.satellites[i].observations[4] * c / f2_G);
			(*epoch_mp_g)[prn].MP2[epochCount - 1] = (epoch.satellites[i].observations[1] + G_f1 * epoch.satellites[i].observations[4] * c / f2_G - G_f3 * epoch.satellites[i].observations[3] * c / f1_G);
		}
		if ((epoch.satellites[i].satID[0] == 'C')&&(epoch.satellites[i].observations[0]!=0)&&(epoch.satellites[i].observations[2]!=0)
			&& (epoch.satellites[i].observations[3] != 0) && (epoch.satellites[i].observations[5] != 0))
		{
			(*epoch_mp_c)[prn].satID = epoch.satellites[i].satID;
			(*epoch_mp_c)[prn].MP1[epochCount - 1] = (epoch.satellites[i].observations[0] - C_f1 * epoch.satellites[i].observations[3] * c / f1_C + C_f2 * epoch.satellites[i].observations[5] * c / f3_C);
			(*epoch_mp_c)[prn].MP2[epochCount - 1] = (epoch.satellites[i].observations[2] + C_f1 * epoch.satellites[i].observations[5] * c / f3_C - C_f3 * epoch.satellites[i].observations[3] * c / f1_C);
		}
		if ((epoch.satellites[i].satID[0] == 'R')&&(epoch.satellites[i].observations[0]!=0)&&(epoch.satellites[i].observations[1]!=0)
			&& (epoch.satellites[i].observations[2] != 0) && (epoch.satellites[i].observations[3] != 0))
		{
			(*epoch_mp_r)[prn].satID = epoch.satellites[i].satID;
			double f1 = GLO_fre(f1_R, prn, 0.5625e6); double f2 = GLO_fre(f2_R, prn, 0.4375e6);
			double GLO_f1= (f1 * f1 + f2 * f2) / (f1 * f1 - f2 * f2);
			double GLO_f2 = (2 * f2 * f2) / (f1 * f1 - f2 * f2);
			double GLO_f3 = (2 * f1 * f1) / (f1 * f1 - f2 * f2);
			(*epoch_mp_r)[prn].MP1[epochCount - 1] = (epoch.satellites[i].observations[0] - GLO_f1 * epoch.satellites[i].observations[2] * c / f1 + GLO_f2 * epoch.satellites[i].observations[3] * c / f2);
			(*epoch_mp_r)[prn].MP2[epochCount - 1] = (epoch.satellites[i].observations[1] + GLO_f1 * epoch.satellites[i].observations[3] * c / f2 - GLO_f3 * epoch.satellites[i].observations[2] * c / f1);
		}
	}
	//输出该历元每个卫星的多路径误差（包含模糊度）
	fprintf(obs_fp, "%2d\n", epochCount);
	for (int i = 0; i < 33; i++) {
		if ((*epoch_mp_g)[i].satID.size()!=0) {
			fprintf(obs_fp, "%4s %15.3lf %15.3lf\n", (*epoch_mp_g)[i].satID.c_str(), (*epoch_mp_g)[i].MP1[epochCount-1], (*epoch_mp_g)[i].MP2[epochCount-1]);
		}
	}
	fprintf(MP_C, "%2d\n", epochCount);
	for (int i = 0; i < 33; i++) {
		if ((*epoch_mp_c)[i].satID.size() != 0) {
			fprintf(MP_C, "%4s %15.3lf %15.3lf\n", (*epoch_mp_c)[i].satID.c_str(), (*epoch_mp_c)[i].MP1[epochCount-1], (*epoch_mp_c)[i].MP2[epochCount-1]);
		}
	}
	fprintf(MP_R, "%2d\n", epochCount);
	for (int i = 0; i < 33; i++) {
		if ((*epoch_mp_r)[i].satID.size() != 0) {
			fprintf(MP_R, "%4s %15.3lf %15.3lf\n", (*epoch_mp_r)[i].satID.c_str(), (*epoch_mp_r)[i].MP1[epochCount-1], (*epoch_mp_r)[i].MP2[epochCount-1]);
		}
	}
}

//计算差分MP
void diff_MP(std::vector<epoch_MP>* epoch_mp_g,int epochCount)
{
	for (int i = 0; i < epoch_mp_g->size(); i++)
	{
		for (int j = 0; j < epochCount; j++) {
			if (((*epoch_mp_g)[i].MP1[j]!=0)&& ((*epoch_mp_g)[i].MP1[j+1] != 0)) {
				(*epoch_mp_g)[i].MP1[j] = (*epoch_mp_g)[i].MP1[j + 1] - (*epoch_mp_g)[i].MP1[j];
			}
			else {
				(*epoch_mp_g)[i].MP1[j] = 0; (*epoch_mp_g)[i].MP1[j + 1] = 0;
			}
		}
		for (int j = 0; j < epochCount; j++) {
			if (((*epoch_mp_g)[i].MP2[j] != 0) && ((*epoch_mp_g)[i].MP2[j + 1] != 0)) {
				(*epoch_mp_g)[i].MP2[j] = (*epoch_mp_g)[i].MP2[j + 1] - (*epoch_mp_g)[i].MP2[j];
			}
			else {
				(*epoch_mp_g)[i].MP2[j] = 0; (*epoch_mp_g)[i].MP2[j + 1] = 0;
			}
		}
	}
}

void cal_MP_ave(std::vector<epoch_MP> epoch_mp_g,std::vector<int> slip_epoch,FILE*MP)
{
	int start = 0, end = 0;
	for (int i = 0; i < slip_epoch.size()+1; i++) {
		if (i == slip_epoch.size()) {
			start = end;
			end = 758;
		}
		else { start = end; end = slip_epoch[i] - 1; }
		if ((end - start) < 50)continue;
		
		for (int j = 0; j < 33; j++) {
			if (epoch_mp_g[j].satID.size() != 0) {
				fprintf(MP, "%4s ", epoch_mp_g[j].satID.c_str());
			}
		}
		fprintf(MP, "\n");

		for (int k = start; k < end - 49; k++) {
			fprintf(MP, "%4d %4d  ", k, k + 50);
			for (int j = 0; j < 33; j++) {
				if (epoch_mp_g[j].satID.size() != 0) {
					double sum1 = 0.0, sum2 = 0.0, ave1 = 0.0, ave2 = 0.0;
					double SUM1 = 0.0, SUM2 = 0.0, AVE1 = 0.0, AVE2 = 0.0;
					int valid_num = 0;
					for (int m = k; m < k + 50; m++) {
						if (epoch_mp_g[j].MP1[m] != 0) {
							sum1 = sum1 + epoch_mp_g[j].MP1[m];
							sum2 = sum2 + epoch_mp_g[j].MP2[m];
							valid_num++;
						}
					}
					ave1 = sum1 /valid_num ;
					ave2 = sum2 / valid_num;
					if ((valid_num > 1) && (ave1 != 0)) {
						for (int m = k; m < k + 50; m++) {
							if (epoch_mp_g[j].MP1[m] != 0) {
								SUM1 = SUM1 + pow((epoch_mp_g[j].MP1[m] - ave1), 2);
								SUM2 = SUM2 + pow((epoch_mp_g[j].MP2[m] - ave2), 2);
							}
						}
						AVE1 = sqrt(SUM1 / (valid_num - 1));
						AVE2 = sqrt(SUM2 / (valid_num - 1));
						
					}

					fprintf(MP, "%15.4lf %15.4lf", AVE1, AVE2);
				}
			}
			fprintf(MP, "\n");
		}
		
		
	}
}


//获得GLONASS的正确频率
double GLO_fre(double center_fre, int prn,double d_fre)
{
	return center_fre + (prn-8) * d_fre;
}