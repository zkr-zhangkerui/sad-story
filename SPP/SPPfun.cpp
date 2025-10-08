#include "SPP.h"

/*����������ģ��
double Norm(int dim, double** M)
{
	double sum = 0;
	for (int i = 0; i < dim; i++)
	{
		sum = sum + M[i][0] * M[i][0];
	}
	return sqrt(sum);
}
double VectDot(int dim, double** M1, double** M2)
{
	double sum = 0;
	for (int i = 0; i < dim; i++)
	{
		sum = sum + M1[i][0] * M2[i][0];
	}
	return sum;
}*/

/****************************************************************************
 SatElev
 Ŀ�ģ�  �������Ǹ߶Ƚ�, ������ջ�λ��û�г�ʼ��, ����Pai/2.0
 ����:
 SatPos[3]  ����λ��[m]
 RCVPos[3]  ���ջ�λ��[m]
 ����ֵ
 ���Ǹ߶Ƚ�[Rad]
 ****************************************************************************/
double SatElev(SateResult* SatPos, Eigen::MatrixXd RCVPos)
{
    double Elev = 0;
	double RcvR, SatRcvR;
    Eigen::VectorXd dPos; dPos.resize(3, 1);
    Eigen::VectorXd satpos; satpos.resize(3, 1);
    Eigen::VectorXd XYZ; 
    XYZ = RCVPos.block(0, 0, 3, 1);
    satpos(0, 0) = SatPos->X1;
    satpos(1, 0) = SatPos->Y1;
    satpos(2, 0) = SatPos->Z1;
    dPos = satpos - XYZ;
	//double** dPos = nullptr;       /* ����λ������ջ���λ�ò�ֵ */

    /*dPos = alloc_2d_double(3, 1);
	dPos[0][0] = SatPos->X - RCVPos[0][0];
	dPos[1][0] = SatPos->Y - RCVPos[1][0];
	dPos[2][0] = SatPos->Z - RCVPos[2][0];*/

	//RcvR = Norm(3, RCVPos);//������ά������ģ��
	//SatRcvR = Norm(3, dPos);
    RcvR = XYZ.norm();
    SatRcvR = dPos.norm();

	if (fabs(RcvR * SatRcvR) < 1.0) //��������λ�û���ջ�λ���Ƿ�Ϊ0 
	{
		Elev = PI / 2.0;
	}
	else
	{
		//Elev = VectDot(3, RCVPos, dPos) / (RcvR * SatRcvR);//�������������ĵ��
        //Elev = (XYZ.cwiseProduct(dPos).sum())/ (RcvR * SatRcvR);
        Elev = (XYZ.dot(dPos) / (RcvR * SatRcvR));
		Elev = PI / 2.0 - acos(Elev);
	}
	return (Elev);
}

/*
// Ϊ��ά������������ڴ�
double** alloc_2d_double(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        std::cerr << "Invalid allocation size: " << rows << "x" << cols << std::endl;
        return nullptr;
    }
    double** ptr = new double* [rows];  // ������ָ������
    if (!ptr) return nullptr;
    ptr[0] = new double[rows * cols](); // ������������Ԫ�ز���ʼ��Ϊ0
    if (!ptr[0]) {
        delete[] ptr;
        return nullptr;
    }
    for (int i = 1; i < rows; ++i) {
        ptr[i] = ptr[i - 1] + cols;    // ����ÿ����ʼ��ַ
    }

    return ptr;
}

// �ͷŶ�ά�����ڴ�
void free_2d_double(double** ptr) {
    if (ptr) {
        delete[] ptr[0];// �ͷ������ڴ��
        delete[] ptr;    // �ͷ���ָ������
    }
}
*/
// ΪPos�ṹ������ڴ� n�۲�ֵ���� mδ֪������
void allocate_pos_memory(Pos* pos, int n, int m) {
    
    // ��ά������䣨�����������ڴ棩 ���ͷž��ڴ�
    pos->P_IF = Eigen::MatrixXd(0, 0);
    pos->P_IF= Eigen::MatrixXd::Zero(n, 1);

    pos->Z = Eigen::MatrixXd(0, 0);
    pos->Z= Eigen::MatrixXd::Zero(n, 1);     // �в�����

    pos->H = Eigen::MatrixXd(0, 0);
    pos->H = Eigen::MatrixXd::Zero(n, m);        // ��ƾ���

    pos->recpos = Eigen::MatrixXd(0, 0);
    pos->recpos = Eigen::MatrixXd::Zero(m, 1);    // λ������

    pos->D_obs = Eigen::MatrixXd(0, 0);
    pos->D_obs = Eigen::MatrixXd::Zero(m, 1);

    pos->H_V = Eigen::MatrixXd(0, 0);
    pos->H_V= Eigen::MatrixXd::Zero(n, 4);

    pos->P_V = Eigen::MatrixXd(0, 0);
    pos->P_V= Eigen::MatrixXd::Zero(n, 1);

    pos->vel = Eigen::MatrixXd(0, 0);
    pos->vel= Eigen::MatrixXd::Zero(4,1);

    pos->Z_V = Eigen::MatrixXd(0, 0);
    pos->Z_V = Eigen::MatrixXd::Zero(n, 1);

    pos->D_obs = Eigen::MatrixXd(0, 0);
    pos->D_obs = Eigen::MatrixXd::Zero(n, 1);

    pos->D_after = Eigen::MatrixXd(0, 0);
    pos->D_after= Eigen::MatrixXd::Zero(n, n);

    pos->D_after_vel = Eigen::MatrixXd(0, 0);
    pos->D_after_vel= Eigen::MatrixXd::Zero(n, n);
    
   
    // ��ʼ��ά����Ϣ
    pos->row = 0;
    pos->col = 0;

}

/*
// �ͷ��ڴ溯�� n�۲�ֵ���� mδ֪������
void free_pos_memory(Pos* pos) {

    // ͳһ�ͷ����ж�ά����
    if (pos->P_IF) free_2d_double(pos->P_IF);
    // �ͷź��ÿ�ָ��
    pos->P_IF = nullptr;
    if (pos->Z) free_2d_double(pos->Z); pos->Z = nullptr;
    if (pos->H) free_2d_double(pos->H); pos->H = nullptr;
    if (pos->D) free_2d_double(pos->D); pos->D = nullptr;
    if (pos->recpos) free_2d_double(pos->recpos); pos->recpos = nullptr;
    if (pos->recpos_0) free_2d_double(pos->recpos_0); pos->recpos_0 = nullptr;

    // �ͷ��м����
    if (pos->H_change) free_2d_double(pos->H_change); pos->H_change = nullptr;
    if (pos->Inv_D) free_2d_double(pos->Inv_D); pos->Inv_D = nullptr;
    if (pos->Q) free_2d_double(pos->Q); pos->Q = nullptr;
    if (pos->Inv_Q) free_2d_double(pos->Inv_Q); pos->Inv_Q = nullptr;
    if (pos->d_recpos) free_2d_double(pos->d_recpos); pos->d_recpos = nullptr;
    if (pos->recpos1) free_2d_double(pos->recpos1); pos->recpos1 = nullptr;
    if (pos->recpos2) free_2d_double(pos->recpos2); pos->recpos2 = nullptr;
    if (pos->recpos3) free_2d_double(pos->recpos3); pos->recpos3 = nullptr;
    if(pos->Q1)free_2d_double(pos->Q1); pos->Q1 = nullptr;

}

//��ʼ������
void init_matrix(double** mat, int rows, int cols, double value) {
    if (!mat) return;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mat[i][j] = value;
        }
    }
}

// �м�������ڴ������
void realloc_pos_intermediate(Pos* pos, Row_Col*rc) {
    if (!pos) return;
    // �ͷſ��ܴ��ڵľ��ڴ�
    if (pos->H_change) free_2d_double(pos->H_change); pos->H_change = nullptr;
    if (pos->Inv_D) free_2d_double(pos->Inv_D); pos->Inv_D = nullptr;
    if (pos->Q) free_2d_double(pos->Q); pos->Q = nullptr;
    if (pos->Inv_Q) free_2d_double(pos->Inv_Q); pos->Inv_Q = nullptr;
    if (pos->d_recpos) free_2d_double(pos->d_recpos); pos->d_recpos = nullptr;
    if (pos->recpos1) free_2d_double(pos->recpos1); pos->recpos1= nullptr;
    if (pos->recpos2) free_2d_double(pos->recpos2); pos->recpos2 = nullptr;
    if (pos->recpos3) free_2d_double(pos->recpos3); pos->recpos3 = nullptr;
    if(pos->Q1)free_2d_double(pos->Q1); pos->Q1 = nullptr;

    //Ϊ�м���������ڴ�
    pos->H_change = alloc_2d_double(rc->col, rc->row);
    pos->Inv_D = alloc_2d_double(rc->row, rc->row);
    pos->Q = alloc_2d_double(rc->col, rc->col);
    pos->Q1 = alloc_2d_double(rc->col, rc->row);
    pos->Inv_Q = alloc_2d_double(rc->col, rc->col);
    pos->d_recpos = alloc_2d_double(rc->col, 1);
    pos->recpos1 = alloc_2d_double(rc->col, rc->row);
    pos->recpos2 = alloc_2d_double(rc->col, rc->row);
    pos->recpos3 = alloc_2d_double(rc->col, 1);

    // �洢��ǰά����Ϣ
    pos->row = rc->row;
    pos->col = rc->col;

    // ��ʼ���м����
    init_matrix(pos->H_change, rc->col, rc->row, 0.0);
    init_matrix(pos->Inv_D, rc->row, rc->row, 0.0);
    init_matrix(pos->Inv_Q, rc->col, rc->col, 0.0);
    init_matrix(pos->Q, rc->col, rc->col, 0.0);
    init_matrix(pos->Q1, rc->col, rc->row, 0.0);
    init_matrix(pos->d_recpos, rc->col, 1, 0.0);
    init_matrix(pos->recpos1, rc->col, rc->row, 0.0);
    init_matrix(pos->recpos2, rc->col, rc->row, 0.0);
    init_matrix(pos->recpos3, rc->col, 1, 0.0);
}*/

//ȥ��ȫ0��
Eigen::MatrixXd removeZeroRows(Eigen::MatrixXd& matrix, double threshold) {
    std::vector<int> keepRows;

    // ����ÿһ�У�����Ƿ�ȫ��
    for (int i = 0; i < matrix.rows(); ++i) {
        if (matrix.row(i).norm() > threshold) {
            keepRows.push_back(i);
        }
    }

    // �����¾���
    Eigen::MatrixXd result(keepRows.size(), matrix.cols());
    for (int i = 0; i < keepRows.size(); ++i) {
        result.row(i) = matrix.row(keepRows[i]);
    }

    return result;
}

//ȥ��ȫ0��
Eigen::MatrixXd removeZeroCols(Eigen::MatrixXd& matrix, double threshold) {
    std::vector<int> keepCols;

    // ����ÿһ�У�����Ƿ�ȫ��
    for (int j = 0; j < matrix.cols(); ++j) {
        if (matrix.col(j).cwiseAbs().maxCoeff() > threshold) {
            keepCols.push_back(j);
        }
    }

    // �����¾���
    Eigen::MatrixXd result(matrix.rows(), keepCols.size());
    for (int j = 0; j < keepCols.size(); ++j) {
        result.col(j) = matrix.col(keepCols[j]);
    }

    return result;
}

//ɾ��H��Z��P_IF��ȫ���У�����������
extern void remove_zero_rows_and_cols(Pos* pos, double threshold,int dim)
{
    threshold = 1e-12;
    pos->P_IF = removeZeroRows(pos->P_IF, threshold);
    int target_rows = pos->P_IF.rows();  // Ŀ������ = IF������

    // ����H����
    if (pos->H.rows() > target_rows) {
        Eigen::MatrixXd new_H = pos->H.topRows(target_rows).eval();
        pos->H = new_H;
    }
    if(pos->H.cols()>dim)pos->H = removeZeroCols(pos->H, threshold);

    // ����P_IF����
    if (pos->P_IF.rows() > target_rows) {
        Eigen::MatrixXd new_P = pos->P_IF.topRows(target_rows).eval();
        pos->P_IF = new_P;
    }
    if(pos->P_IF.cols()>1)pos->P_IF = removeZeroCols(pos->P_IF, threshold);

    // ����Z����
    if (pos->Z.rows() > target_rows) {
        Eigen::MatrixXd new_Z = pos->Z.topRows(target_rows).eval();
        pos->Z = new_Z;
    }
    if (pos->Z.cols() > 1)pos->Z = removeZeroCols(pos->Z, threshold);

    // ȷ�����о���ά��һ��
    pos->H.conservativeResize(pos->Z.rows(), pos->H.cols());
    pos->P_IF.conservativeResize(pos->Z.rows(), pos->P_IF.cols());

    // ����ά����Ϣ
    pos->row = pos->Z.rows();
    pos->col = pos->H.cols();
}

extern void remove_zero_rows_and_cols_V(Pos* pos, double threshold)
{
    threshold = 1e-12;
    pos->P_V = removeZeroRows(pos->P_V, threshold);
    int target_rows = pos->P_V.rows();  // Ŀ������ = IF������

    // ����H����
    if (pos->H_V.rows() > target_rows) {
        Eigen::MatrixXd new_H = pos->H_V.topRows(target_rows).eval();
        pos->H_V = new_H;
    }
    pos->H_V = removeZeroCols(pos->H_V, threshold);

    // ����P_IF����
    if (pos->P_V.rows() > target_rows) {
        Eigen::MatrixXd new_P = pos->P_V.topRows(target_rows).eval();
        pos->P_V = new_P;
    }
    pos->P_V = removeZeroCols(pos->P_V, threshold);

    // ����Z����
    if (pos->Z_V.rows() > target_rows) {
        Eigen::MatrixXd new_Z = pos->Z_V.topRows(target_rows).eval();
        pos->Z_V = new_Z;
    }
    pos->Z_V = removeZeroCols(pos->Z_V, threshold);

    // ȷ�����о���ά��һ��
    pos->H_V.conservativeResize(pos->Z_V.rows(), pos->H_V.cols());
    pos->P_V.conservativeResize(pos->Z_V.rows(), pos->P_V.cols());

    // ����ά����Ϣ
    pos->row = pos->Z_V.rows();
    pos->col = pos->H_V.cols();
}

//����ȫ������
Eigen::MatrixXd safeInverse(const Eigen::MatrixXd& Q, double lambda) {
    // ��������Q + ��I
    Eigen::MatrixXd regMatrix = Q + lambda * Eigen::MatrixXd::Identity(Q.rows(), Q.cols());
    return regMatrix.inverse();
}

//�洢��һ����Ԫ�Ĺ۲�����
void copy_obs(Out_obs* out_obs0, Out_obs* out_obs)
{
    for (int i = 0; i < out_obs->num; i++) {
        out_obs0->prn[i] = out_obs->prn[i];
        out_obs0->sys[i] = out_obs->sys[i];
        out_obs0->num = out_obs->num;
        for (int j = 0; j < 16; j++) {
            out_obs0->obs_t[i][j] = out_obs->obs_t[i][j];
        }
    }
}

//һ���Լ���
bool consistency_test(int epochCount,int prn,int sys,Out_obs*out_obs,Out_obs*out_obs0,int i)
{
    int index = 0; 
    double Cd1 = 0.0, Cd2 = 0.0, Cp1 = 0.0, Cp2 = 0.0;
    if ((epochCount != 0) && (find(prn, out_obs0->prn, out_obs0->num) >= 0) && (find(sys, out_obs0->sys, out_obs0->num) >= 0)) {
        index = find(prn, out_obs0->prn, out_obs0->num);
        if (out_obs0->sys[index]!=sys) {
            out_obs0->prn[index] = 0;
            index = find(prn, out_obs0->prn, out_obs0->num);
        }
        //������
        if (sys == 0) {
            if ((out_obs->obs_t[i][2] != 0) && (out_obs->obs_t[i][6] != 0)&& (out_obs0->obs_t[i][2] != 0) && (out_obs0->obs_t[i][6] != 0)) {
                Cd1 = (c / (f1_G * 1000000)) * abs(out_obs->obs_t[i][2] - out_obs0->obs_t[index][2]);
                Cd2 = (c / (f2_G * 1000000)) * abs(out_obs->obs_t[i][6] - out_obs0->obs_t[index][6]);
                Cp1 = abs(out_obs->obs_t[i][0] - out_obs0->obs_t[index][0] + (out_obs0->obs_t[index][2]+ out_obs->obs_t[i][2]) * c / (2*f1_G * 1000000));
                Cp2 = abs(out_obs->obs_t[i][4] - out_obs0->obs_t[index][4] + (out_obs0->obs_t[index][6] + out_obs->obs_t[i][6]) * c / (2*f2_G * 1000000));
            }
            else return true;
        }
        if (sys == 1) {
            if ((out_obs->obs_t[i][10] != 0) && (out_obs->obs_t[i][14] != 0)&& (out_obs0->obs_t[i][10] != 0) && (out_obs0->obs_t[i][14] != 0)) {
                Cd1 = (c / (B1_I * 1000000)) * abs(out_obs->obs_t[i][10] - out_obs0->obs_t[index][10]);
                Cd2 = (c / (B3_I * 1000000)) * abs(out_obs->obs_t[i][14] - out_obs0->obs_t[index][14]);
                Cp1 = abs(out_obs->obs_t[i][8] - out_obs0->obs_t[index][8] + (out_obs0->obs_t[index][10]+out_obs->obs_t[i][10]) * c / (2*B1_I * 1000000));
                Cp2 = abs(out_obs->obs_t[i][12] - out_obs0->obs_t[index][12] + (out_obs0->obs_t[index][14] + out_obs->obs_t[i][14]) * c / (2*B3_I * 1000000));
            }
            else return true;
        }
        if (Cd1 > 20 || Cd2 > 20) { 
            return false;
        }
        else { 
            if(Cp1 > 8 || Cp2 > 8) return false;
            else return true;
        }
    }
    else {
        return true;
    }
}
