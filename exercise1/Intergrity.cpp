//每颗卫星每个频率有效观测数据的历元数

//每颗卫星所有频率有效观测数据的历元数

#include "Intergrity.h"

void int_count(EpochData epoch, sat_fre_obsnum*GPS, sat_fre_obsnum*BDS, sat_fre_obsnum*GLO)
{
	GPS->fre_valid_num.resize(6,0.0);
	GLO->fre_valid_num.resize(4, 0.0);
	BDS->fre_valid_num.resize(6, 0.0);
	//SBAS->fre_valid_num.resize(2, 0.0);
	for (int i = 0; i < epoch.numSats; i++)
	{
		if (epoch.satellites[i].satID[0] == 'G') {
			GPS->num++;
			bool all_fre = true;
			if ((epoch.satellites[i].observations[0] != 0) && (epoch.satellites[i].observations[3] != 0) && (epoch.satellites[i].observations[9] != 0)) {
				GPS->fre_valid_num[0]++;
			}
			else all_fre = false;
			if ((epoch.satellites[i].observations[1] != 0) && (epoch.satellites[i].observations[4] != 0) && (epoch.satellites[i].observations[10] != 0)) {
				GPS->fre_valid_num[1]++;
			}
			else all_fre = false;
			if ((epoch.satellites[i].observations[2] != 0) && (epoch.satellites[i].observations[5] != 0) && (epoch.satellites[i].observations[11] != 0)) {
				GPS->fre_valid_num[2]++;
			}
			else all_fre = false;
			if (all_fre == true)GPS->sys_valid_num++;
		}
		if (epoch.satellites[i].satID[0] == 'R') {
			GLO->num++; bool all_fre = true;
			if ((epoch.satellites[i].observations[0] != 0) && (epoch.satellites[i].observations[2] != 0) && (epoch.satellites[i].observations[6] != 0)) {
				GLO->fre_valid_num[0]++;
			}
			else all_fre = false;
			if ((epoch.satellites[i].observations[1] != 0) && (epoch.satellites[i].observations[3] != 0) && (epoch.satellites[i].observations[7] != 0)) {
				GLO->fre_valid_num[1]++;
			}
			else all_fre = false;
			if (all_fre == true)GLO->sys_valid_num++;
		}
		if (epoch.satellites[i].satID[0] == 'C') {
			BDS->num++; bool all_fre = true;
			if ((epoch.satellites[i].observations[0] != 0) && (epoch.satellites[i].observations[3] != 0) && (epoch.satellites[i].observations[9] != 0)) {
				BDS->fre_valid_num[0]++;
			}
			else all_fre = false;
			if ((epoch.satellites[i].observations[1] != 0) && (epoch.satellites[i].observations[4] != 0) && (epoch.satellites[i].observations[10] != 0)) {
				BDS->fre_valid_num[1]++;
			}
			else all_fre = false;
			if ((epoch.satellites[i].observations[2] != 0) && (epoch.satellites[i].observations[5] != 0) && (epoch.satellites[i].observations[11] != 0)) {
				BDS->fre_valid_num[2]++;
			}
			else all_fre = false;
			if (all_fre == true)BDS->sys_valid_num++;
		}
		/*if (epoch.satellites[i].satID[0] == 'S') {
			SBAS->num++;
			if ((epoch.satellites[i].observations[0] != 0) && (epoch.satellites[i].observations[1] != 0)) {
				SBAS->fre_valid_num[0]++;
				SBAS->sys_valid_num++;
			}
		}*/
	}
}

void int_cal(sat_fre_obsnum* GPS)
{
	int index = GPS->fre_valid_num.size();
	std::cout << "fre_intergrity  sys_intergrity" << std::endl;
	for (int i = 0; i < index / 2; i++)
	{
		GPS->fre_valid_num[index/2+i] = GPS->fre_valid_num[i] / GPS->num;
		std::cout << GPS->fre_valid_num[index / 2 + i] << std::endl;
	}
	GPS->sys_intergrity = GPS->sys_valid_num / GPS->num;
	std::cout << GPS->sys_intergrity << std::endl;
	std::cout << std::endl;
	
}