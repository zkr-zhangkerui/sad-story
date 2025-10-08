#include "ion.h"

//计算电离层延迟
void cal_ion(EpochData epoch,  std::vector<Ion>*ion_g, std::vector<Ion>* ion_c, std::vector<Ion>* ion_r,int epochCount)
{

	for (int i = 0; i < epoch.numSats; i++) {
		int prn = std::stoi(epoch.satellites[i].satID.substr(1));
		if ((epoch.satellites[i].satID[0] == 'G') && (epoch.satellites[i].observations[3] != 0) && (epoch.satellites[i].observations[4] != 0)) {
			(*ion_g)[prn].satID = epoch.satellites[i].satID;
			(*ion_g)[prn].V_ion_1[epochCount-1] = (epoch.satellites[i].observations[3] * c / (f1_G ) - epoch.satellites[i].observations[4] * c / (f2_G ))
					* f2_G * f2_G / (f1_G * f1_G - f2_G * f2_G);
			(*ion_g)[prn].V_ion_2[epochCount - 1] = (epoch.satellites[i].observations[3] * c / (f1_G) - epoch.satellites[i].observations[4] * c / (f2_G))
					* f1_G * f1_G / (f1_G * f1_G - f2_G * f2_G);
		}
		if ((epoch.satellites[i].satID[0] == 'C') && (epoch.satellites[i].observations[3] != 0) && (epoch.satellites[i].observations[5] != 0)) {
			(*ion_c)[prn].satID = epoch.satellites[i].satID;
			(*ion_c)[prn].V_ion_1[epochCount - 1] = (epoch.satellites[i].observations[3] * c / (f1_C ) - epoch.satellites[i].observations[5] * c / (f3_C ))
					* f3_C * f3_C / (f1_C * f1_C - f3_C * f3_C);
			(*ion_c)[prn].V_ion_2[epochCount - 1] = (epoch.satellites[i].observations[3] * c / (f1_C) - epoch.satellites[i].observations[5] * c / (f3_C ))
					* f1_C * f1_C / (f1_C * f1_C - f3_C * f3_C);
		}
		if ((epoch.satellites[i].satID[0] == 'R') && (epoch.satellites[i].observations[3] != 0) && (epoch.satellites[i].observations[2] != 0)) {
			(*ion_r)[prn].satID = epoch.satellites[i].satID;
			double f1 = GLO_fre(f1_R, prn, 0.5625e6); double f2 = GLO_fre(f2_R, prn, 0.4375e6);
			double GLO_f1 = (f2 * f2) / (f1 * f1 - f2 * f2);
			double GLO_f2 = (f1 * f1) / (f1 * f1 - f2 * f2);
			(*ion_r)[prn].V_ion_1[epochCount - 1] = (epoch.satellites[i].observations[2] * c / (f1 ) - epoch.satellites[i].observations[3] * c / (f2))
					* GLO_f2;
			(*ion_r)[prn].V_ion_2[epochCount - 1] = (epoch.satellites[i].observations[2] * c / (f1 ) - epoch.satellites[i].observations[3] * c / (f2 ))
					* GLO_f1;
		}
		
	}
}

//计算电离层延迟变化率
void cal_d_ion(std::vector<Ion>* ion_g, FILE* ion_G, int epochCount)
{
	for (int j = 0; j < 33; j++) {
		if ((*ion_g)[j].satID.size() != 0) {
			fprintf(ion_G, "%4s %2s", (*ion_g)[j].satID.c_str(),"L1");
			fprintf(ion_G, "%4s %2s", (*ion_g)[j].satID.c_str(),"L2");
		}
	}
	fprintf(ion_G, "\n");
	for (int i = 1; i < epochCount; i++) {
		for (int j = 0; j < ion_g->size(); j++) {
			if (((*ion_g)[j].satID.size() != 0)&&((*ion_g)[j].V_ion_1[i]!=0)&&((*ion_g)[j].V_ion_1[i - 1]!=0)
			&&((*ion_g)[j].V_ion_2[i]!=0)&&((*ion_g)[j].V_ion_2[i]!=0)){
				(*ion_g)[j].d_ion_1[i] = ((*ion_g)[j].V_ion_1[i] - (*ion_g)[j].V_ion_1[i - 1]) / 5;
				(*ion_g)[j].d_ion_2[i] = ((*ion_g)[j].V_ion_2[i] - (*ion_g)[j].V_ion_2[i - 1]) / 5;
				fprintf(ion_G, "%10.3lf %10.3lf ", (*ion_g)[j].d_ion_1[i], (*ion_g)[j].d_ion_2[i]);
			}
		}
		fprintf(ion_G, "\n");
	}
}