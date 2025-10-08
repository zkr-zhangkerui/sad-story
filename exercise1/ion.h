#ifndef ION_H
#define ION_H

#include "read_obs.h"
#include "MP.h"


struct Ion {
	std::vector<double> V_ion_1;
	std::vector<double> V_ion_2;
	std::vector<double> d_ion_1;
	std::vector<double> d_ion_2;
	std::string satID;;
};

extern void cal_ion(EpochData epoch, std::vector<Ion>* ion_g, std::vector<Ion>* ion_c, std::vector<Ion>* ion_r, int epochCount);
extern void cal_d_ion(std::vector<Ion>* ion_g, FILE* ion_G, int epochCount);

#endif
