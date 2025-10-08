#include"Matrix.h" //�Ȱ����Լ���ͷ�ļ� include"SateNavDLL/BASEDLL/Matrix.h"
#include"..\GNSSDLL\SPP.h"
/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n
* @internals   ��ʼ��һ�����󣬵��������κ��ڴ�ռ䣬��ָ������
*/
Mat::Mat()
{
	//��ʼ��
	x = NULL;//��ָ��
	row = col = 0;
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n
* @internals   ָ�������С ��ʼ������
*/
Mat::Mat(unsigned int nrow, unsigned int ncol)
{
	this->row = nrow;
	this->col = ncol;
	x = (double*)calloc(nrow * ncol, sizeof(double));//�����С x������ֵ��ֵΪ0
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n
* @internals   
*/
void Mat::newMat(int nrow, int ncol,const double* M)
{
	this->row = nrow;
	this->col = ncol;
	x = (double*)calloc(nrow * ncol, sizeof(double));//�����С x������ֵ��ֵΪ0

	if (M == NULL)return;
	
	for (int i = 0; i < nrow * ncol; i++)
	{
		x[i] = M[i];
	}
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n
* @internals   �ͷŵ��������
*/
void Mat::freeMat()
{
	if (x == NULL)return;

	free(x);

	x = NULL;;
	row = col = 0;
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n
* @internals   һ���������
*/
void Mat::add(Mat M1, Mat M2)
{
	this->row = M1.row;
	this->col = M1.col;
	x = (double*)calloc(row * col, sizeof(double));//�����С x������ֵ��ֵΪ0
	for (int i = 0; i < row * col; i++)x[i] = M1.x[i] + M2.x[i];
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n
* @internals   һ���������
*/
void Mat::Mul(Mat M1, Mat M2)
{
	this->row = M1.row;
	this->col = M1.col;
	x = (double*)calloc(row * col, sizeof(double));//�����С x������ֵ��ֵΪ0
	for (int i = 0; i < row * col; i++)x[i] = M1.x[i] - M2.x[i];
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n
* @internals   һ���������
*/
void Mat::Minus(Mat M1,Mat M2)
{
	this->row = M1.row;
	this->col = M1.col;
	int xrow = MIN(row, col);
	int xcol = MAX(row, col);
	int xnum = xrow*xrow;
	for (int i = 0; i < xnum; i++)
	{
		for (int j = 0; j < xcol; j++)
		{
			x[i] = x[i] + M1.x[int(j + floor(i / xcol) * xcol)] * M2.x[int(i %xcol  + j * xcol)];
		}
	}
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n �����������Ĵ���������
*       
* @internals   һ����������
*/
void Mat::InvSP(Mat M)
{
	// TODO: ����ǰ����Ҫ���жϾ���M�Ƿ�Ϊ�Գ�������
		//       �ⲿ���жϵĴ��룬��������˼����������

	
	int j = 0, k = 0;

	double a = 0;
	int r = M.row;

	for (k = 0; k < r; k++)
	{
		/*if (M.x[k * r + k] <= 0.0 || M.col != M.row)
		{ 
			printf("MatrixInvSP: singular matrix.");
			return;
		}
		
		if (M.x[k * r + k] < 1e-15) M.x[k * r + k] += 1e-15; // M����ӽ�����ʱ,���Խ��߼���һ��С��*/

		for (int i = 0; i < r; i++)
		{
			if (i != k)   M.x[i * r + k] = -M.x[i * r + k] / M.x[k * r + k];
		}
		M.x[k * r + k] = 1.0 / M.x[k * r + k];

		for (int i = 0; i < r; i++)
		{
			if (i != k)
			{
				for (j = 0; j < r; j++)
				{
					if (j != k)  M.x[i * r + j] += M.x[k * r + j] * M.x[i * r + k];
				}
			}
		}
		for (j = 0; j < r; j++)
		{
			if (j != k)  M.x[k * r + j] *= M.x[k * r + k];
		}
	}

}

/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n
* @internals   
*/
double Mat::at(int i, int j)
{
	return this->x[i * col + j];
}
/**
* @par History:
*              2025/4/17,Kerui Zhang, ��һ��д\n
* @internals   
*/
void Mat::at(int i, int j, double val)
{
	this->x[i * col + j] = val;
}

void read(std::string str)
{
	std::ifstream file;
	file.open("str");
	if (!file.is_open())
	{
		std::cout << "can not open file.txt" << std::endl;
		return;
	}
}

void write(FILE* fp)
{
	std::ofstream outfile;
	outfile.open("str");
	if (!outfile.is_open())
	{
		std::cout << "can not open output1.txt" << std::endl;
		return;
	}
}
/*����˷�*/
extern void Mult(double** mat1, double** mat2, double** mat,int M,int N,int P)
{
	if (!mat1 || !mat||!mat2) return; // ��ָ����
	// ��ʼ���������Ϊ0
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < P; ++j) {
			mat[i][j] = 0;
		}
	}
	// ִ�о���˷�
	for (int i = 0; i < M; ++i) {
		for (int k = 0; k < N; ++k) {
			if (mat1[i][k] != 0) { // ������Ԫ��
				for (int j = 0; j < P; ++j) {
					mat[i][j] += mat1[i][k] * mat2[k][j];
				}
			}
		}
	}
}
/*����ת��*/
extern void Change(double **mat1, double **mat,int N,int M)
{
	if (!mat1 || !mat) return; // ��ָ����
	//ת��
	for (int i = 0; i < M; i++){
		for (int k = 0; k < N; k++){
			mat[i][k] = mat1[k][i];
		}
	}
}
/*��������*/
extern bool Inv(double** mat, double** inv,int N)
{
	if (!inv || !mat) return false; // ��ָ����
	// ��ʼ�������Ϊ��λ����
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			inv[i][j] = (i == j) ? 1.0 : 0.0;
		}
	}
	// �������󸱱����ڼ���
	double **temp=nullptr;
	temp = alloc_2d_double(N, N);
	if (!temp) return false; // ����ʧ��

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			temp[i][j] = mat[i][j];
		}
	}
	// ��˹-Լ����Ԫ��
	for (int i = 0; i < N; ++i) {
		// Ѱ����Ԫ
		double maxEl = fabs(temp[i][i]);
		int maxRow = i;
		for (int k = i + 1; k < N; ++k) {
			if (fabs(temp[k][i]) > maxEl) {
				maxEl = fabs(temp[k][i]);
				maxRow = k;
			}
		}

		// �����У���Ԫ�к͵�ǰ�У�
		if (maxRow != i) {
			for (int k = 0; k < N; ++k) {
				std::swap(temp[i][k], temp[maxRow][k]);
				std::swap(inv[i][k], inv[maxRow][k]);
			}
		}

		// �����Ԫ�Ƿ�Ϊ0���������
		if (fabs(temp[i][i]) < 1e-15) {
			return false; // ���󲻿���
		}

		// ��һ����ǰ��
		double divisor = temp[i][i];
		for (int j = 0; j < N; ++j) {
			temp[i][j] /= divisor;
			inv[i][j] /= divisor;
		}

		// ��Ԫ��ʹ�����еĵ�ǰ��Ϊ0
		for (int k = 0; k < N; ++k) {
			if (k == i) continue;

			double factor = temp[k][i];
			for (int j = 0; j < N; ++j) {
				temp[k][j] -= factor * temp[i][j];
				inv[k][j] -= factor * inv[i][j];
			}
		}
	}
	// �ͷ���ʱ�ڴ�
	free_2d_double(temp); temp = nullptr;
}

/*�������*/
extern void Add(double** mat1, double** mat2, double** mat, int N, int M)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			mat[i][j] = mat1[i][j] + mat2[i][j];
		}
	}
}

/*һ�������ֵ��ֵΪ��һ������*/
extern void Copy(double** mat1, double** mat, int N, int M)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			mat[i][j] = mat1[i][j];
		}
	}
}

/*�������*/
extern void Minus(double** mat1, double** mat2, double** mat, int N, int M)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			mat[i][j] = mat1[i][j] - mat2[i][j];
		}
	}
}

/*����������*/
extern void Mult1(double mat1, double** mat2, double** mat, int M, int P)
{
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < P; j++) {
			mat[i][j] = mat1 * mat2[i][j];
		}
	}
}
