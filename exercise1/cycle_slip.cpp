//对于静态 GPS使用L1和L2，北斗使用B1和B3，GLONASS使用G1和G2
//epochcount-1对应当前历元，判断的是epochcount-2历元

#include "cycle_slip.h"
#include "MP.h"
#include "read_obs.h"
#include<cmath>

bool sys_test_MW(EpochData epoch, L_MW* l_mw_G, int epochCount,char sys,double f1,double f2,FILE*slip_MW)
{
	bool is_slip = false; 
	int index1 = 0, index2 = 0, index3 = 0, index4 = 0;
	if (sys == 'G') { index1 = 0; index2 = 1; index3 = 3; index4 = 4; }
	if (sys == 'C') { index1 = 0; index2 = 2; index3 = 3; index4 = 5; }
	if (sys == 'R') { index1 = 0; index2 = 1; index3 = 2; index4 = 3; }
	fprintf(slip_MW, "%20s", epoch.time.c_str());
	for (int i = 0; i < epoch.numSats; i++)
	{
		int prn = std::stoi(epoch.satellites[i].satID.substr(1));
		if (sys == 'R') {
			f1 = GLO_fre(f1_R, prn, 0.5625e6); f2 = GLO_fre(f2_R, prn, 0.4375e6);
		}
		if (epoch.satellites[i].satID[0] == sys) {
			if ((epoch.satellites[i].observations[index2] != 0) && (epoch.satellites[i].observations[index1] != 0)
				&& (epoch.satellites[i].observations[index3] != 0)&& (epoch.satellites[i].observations[index4] != 0))//两个频率都有观测数据
			{
				l_mw_G->sate_num[prn]++;
				//跳过第一,2个历元 默认第一个历元无周跳
				if ((l_mw_G->L_MW_ti[0] == 0) && (l_mw_G->L_MW_ti[1] == 0)) {//第一个历元
					l_mw_G->ave_L_MW_ti[0] = l_mw_G->L_MW_ti[0] = (c * epoch.satellites[i].observations[index3] - c * epoch.satellites[i].observations[index4] -
						f1 * epoch.satellites[i].observations[index1] / 1000000 - f2 * epoch.satellites[i].observations[index2] / 1000000) / (f1 / 1000000 - f2 / 1000000);
					return is_slip;
				}
				if ((l_mw_G->L_MW_ti[0] != 0) && (l_mw_G->L_MW_ti[1] == 0)) {//第2个历元
					l_mw_G->L_MW_ti[1] = (c * epoch.satellites[i].observations[index3] - c * epoch.satellites[i].observations[index4] -
						f1 * epoch.satellites[i].observations[index1] / 1000000 - f2 * epoch.satellites[i].observations[index2] / 1000000) / (f1 / 1000000 - f2 / 1000000);
					l_mw_G->ave_L_MW_ti[1] = ((epochCount - 1) * l_mw_G->ave_L_MW_ti[0] + l_mw_G->L_MW_ti[1]) / epochCount;
					l_mw_G->delat_L_MW_ti[1] = ((epochCount - 1) * l_mw_G->delat_L_MW_ti[0] +
						pow((l_mw_G->ave_L_MW_ti[0] - l_mw_G->L_MW_ti[1]), 2)) / epochCount;
					return is_slip;
				}
				l_mw_G->L_MW_ti[2] = (c * epoch.satellites[i].observations[index3] - c * epoch.satellites[i].observations[index4] -
					f1 * epoch.satellites[i].observations[index1] / 1000000 - f2 * epoch.satellites[i].observations[index2] / 1000000) / (f1 / 1000000 - f2 / 1000000);
				l_mw_G->ave_L_MW_ti[2] = ((epochCount - 1) * l_mw_G->ave_L_MW_ti[1] + l_mw_G->L_MW_ti[2]) / epochCount;
				l_mw_G->delat_L_MW_ti[2] = ((epochCount - 1) * l_mw_G->delat_L_MW_ti[1] +
					pow((l_mw_G->ave_L_MW_ti[1] - l_mw_G->L_MW_ti[2]), 2)) / epochCount;
				double th0 = abs(l_mw_G->delat_L_MW_ti[1] - l_mw_G->ave_L_MW_ti[0]);
				double th2 = abs(l_mw_G->delat_L_MW_ti[2] - l_mw_G->ave_L_MW_ti[1]);
				if (th0 >= 4 * l_mw_G->delat_L_MW_ti[0]) {
					double th1 = abs(l_mw_G->L_MW_ti[2] - l_mw_G->L_MW_ti[1]);
					if (((th2 >= 4 * l_mw_G->delat_L_MW_ti[1]) && (th1 > 1)) || (th2 < 4 * l_mw_G->delat_L_MW_ti[1]))is_slip = false;
					if ((th2 >= 4 * l_mw_G->delat_L_MW_ti[1]) && (th1 <= 1)) {
						is_slip = true; l_mw_G->sate_slip[prn]++;
					}
				}
				l_mw_G->ave_L_MW_ti[0] = l_mw_G->ave_L_MW_ti[1]; l_mw_G->ave_L_MW_ti[1] = l_mw_G->ave_L_MW_ti[2];
				l_mw_G->delat_L_MW_ti[0] = l_mw_G->delat_L_MW_ti[1]; l_mw_G->delat_L_MW_ti[1] = l_mw_G->delat_L_MW_ti[1];
				l_mw_G->L_MW_ti[0] = l_mw_G->L_MW_ti[1]; l_mw_G->L_MW_ti[1] = l_mw_G->L_MW_ti[2];
			}
		}
		fprintf(slip_MW, "%20.3lf", l_mw_G->L_MW_ti[0]);
	}
	fprintf(slip_MW, "\n");
	if (is_slip == true)l_mw_G->sys_slip++;
	return is_slip;
}

bool test_MW(EpochData epoch,L_MW*l_mw_G,L_MW*l_mw_C,L_MW*l_mw_R,int epochCount,FILE*slip_MW)
{
	bool is_slip_G=sys_test_MW(epoch, l_mw_G, epochCount,'G',f1_G,f2_G,slip_MW);
	bool is_slip_C= sys_test_MW(epoch, l_mw_C, epochCount,'C',f1_C,f3_C, slip_MW);
	bool is_slip_R = sys_test_MW(epoch, l_mw_R, epochCount,'R',f1_R,f2_R, slip_MW);
	if (is_slip_G || is_slip_C || is_slip_R)return true;
	else return false;
}

bool sys_test_GF(EpochData epoch, GF* GF_G, int epochCount,char sys,double f1,double f2)
{
	bool is_slip = false; 
	int index1 = 0, index2 = 0, index3 = 0, index4 = 0;
	if (sys == 'G') { index1 = 0; index2 = 1; index3 = 3; index4 = 4; }
	if (sys == 'C') { index1 = 0; index2 = 2; index3 = 3; index4 = 5; }
	if (sys == 'R') { index1 = 0; index2 = 1; index3 = 2; index4 = 3; }
	for (int i = 0; i < epoch.numSats; i++)
	{
		int prn = std::stoi(epoch.satellites[i].satID.substr(1));
		if (epoch.satellites[i].satID[0] == sys) {
			if ((epoch.satellites[i].observations[index2] != 0) && (epoch.satellites[i].observations[index1] != 0) 
				&& (epoch.satellites[i].observations[index3] != 0) && (epoch.satellites[i].observations[index4] != 0))//两个频率都有观测数据
			{
				GF_G->sate_num[prn]++;
				GF_G->P_GF.conservativeResize(epochCount);//保存之前的数据并扩展
				GF_G->P_GF(epochCount - 1) = epoch.satellites[i].observations[index2] - epoch.satellites[i].observations[index1];
				if ((epochCount == 1) || (epochCount == 2)) {
					GF_G->L_GF_ti[epochCount] = c * epoch.satellites[i].observations[index4] / f1 - c * epoch.satellites[i].observations[index3] / f2;
					return is_slip;
				}
				GF_G->L_GF_ti[2] = c * epoch.satellites[i].observations[index4] / f1 - c * epoch.satellites[i].observations[index3] / f2;
				GF_G->Q_GF = cal_Q_GF(epochCount, GF_G->P_GF);
				double th1 = abs(GF_G->L_GF_ti[1] - GF_G->Q_GF(epochCount - 2) - GF_G->L_GF_ti[0] + GF_G->Q_GF(epochCount - 3));
				double th2 = abs(GF_G->L_GF_ti[2] - GF_G->Q_GF(epochCount - 1) - GF_G->L_GF_ti[1] + GF_G->Q_GF(epochCount - 2));
				if ((th1 > 6 * c * (1 / f2 - 1 / f1)) && (th2 > c * (1 / f2 - 1 / f1))) {
					is_slip = true; GF_G->sate_slip[prn]++;
				}
				GF_G->L_GF_ti[1] = GF_G->L_GF_ti[0]; GF_G->L_GF_ti[2] = GF_G->L_GF_ti[1];
			}
		}
	}
	if (is_slip == true)GF_G->sys_slip++;
	return is_slip;
}

bool test_GF(EpochData epoch,GF*GF_G,GF*GF_C,GF*GF_R,int epochCount)
{
	bool is_slip_G = sys_test_GF(epoch, GF_G, epochCount, 'G', f1_G, f2_G);
	bool is_slip_C = sys_test_GF(epoch, GF_C, epochCount, 'C', f1_C, f3_C);
	bool is_slip_R = sys_test_GF(epoch, GF_R, epochCount, 'R', f1_R, f2_R);
	if (is_slip_G || is_slip_C || is_slip_R)return true;
	else return false;
}

//拟合多项式
Eigen::VectorXd cal_Q_GF(int N,Eigen::VectorXd P_GF) {
	
	// 确定多项式阶数q
	int q;
	if (N / 100.0 >= 6) {
		q = 6;
	}
	else {
		q = static_cast<int>(N / 100) + 1; // 整数除法向下取整
	}
	q = std::min(q, N); // 确保q <= N

	// 构建设计矩阵 (范德蒙矩阵)
	Eigen::MatrixXd X(N, q + 1);
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j <= q; ++j) {
			X(i, j) = pow(i, j);
		}
	}

	// 使用QR分解求解最小二乘问题
	Eigen::VectorXd a = X.householderQr().solve(P_GF);

	// 计算理论值Q_GF
	Eigen::VectorXd Q_GF(N);
	for (int i = 0; i < N; ++i) {
		double value = 0;
		for (int j = 0; j <= q; ++j) {
			value += a(j) * pow(i, j);
		}
		Q_GF(i) = value;
	}
	return Q_GF;
}

//统计每颗卫星周跳比并输出
void cal_sate_slip(GF GF_G, FILE* file)
{
	for (int i = 0; i < GF_G.sate_num.size(); i++) {
		if (GF_G.sate_num[i] != 0) {
			fprintf(file, "%3d %10.4lf\n", i, static_cast<double>(GF_G.sate_slip[i]) / GF_G.sate_num[i]);
		}
	}
}
