#include"Matrix.h" //先包含自己的头文件 include"SateNavDLL/BASEDLL/Matrix.h"
#include"..\GNSSDLL\SPP.h"
/**
* @par History:
*              2025/4/17,Kerui Zhang, 第一次写\n
* @internals   初始化一个矩阵，但不分配任何内存空间，不指定行列
*/
Mat::Mat()
{
	//初始化
	x = NULL;//空指针
	row = col = 0;
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, 第一次写\n
* @internals   指定矩阵大小 初始化矩阵
*/
Mat::Mat(unsigned int nrow, unsigned int ncol)
{
	this->row = nrow;
	this->col = ncol;
	x = (double*)calloc(nrow * ncol, sizeof(double));//分配大小 x中所有值赋值为0
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, 第一次写\n
* @internals   
*/
void Mat::newMat(int nrow, int ncol,const double* M)
{
	this->row = nrow;
	this->col = ncol;
	x = (double*)calloc(nrow * ncol, sizeof(double));//分配大小 x中所有值赋值为0

	if (M == NULL)return;
	
	for (int i = 0; i < nrow * ncol; i++)
	{
		x[i] = M[i];
	}
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, 第一次写\n
* @internals   释放掉这个矩阵
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
*              2025/4/17,Kerui Zhang, 第一次写\n
* @internals   一般矩阵的相加
*/
void Mat::add(Mat M1, Mat M2)
{
	this->row = M1.row;
	this->col = M1.col;
	x = (double*)calloc(row * col, sizeof(double));//分配大小 x中所有值赋值为0
	for (int i = 0; i < row * col; i++)x[i] = M1.x[i] + M2.x[i];
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, 第一次写\n
* @internals   一般矩阵的相减
*/
void Mat::Mul(Mat M1, Mat M2)
{
	this->row = M1.row;
	this->col = M1.col;
	x = (double*)calloc(row * col, sizeof(double));//分配大小 x中所有值赋值为0
	for (int i = 0; i < row * col; i++)x[i] = M1.x[i] - M2.x[i];
}

/**
* @par History:
*              2025/4/17,Kerui Zhang, 第一次写\n
* @internals   一般矩阵的相乘
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
*              2025/4/17,Kerui Zhang, 第一次写\n 对于奇异矩阵的处理有问题
*       
* @internals   一般矩阵的求逆
*/
void Mat::InvSP(Mat M)
{
	// TODO: 程序前面需要先判断矩阵M是否为对称正定阵
		//       这部分判断的代码，请大家自行思考并补充下

	
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
		
		if (M.x[k * r + k] < 1e-15) M.x[k * r + k] += 1e-15; // M矩阵接近奇异时,主对角线加上一个小量*/

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
*              2025/4/17,Kerui Zhang, 第一次写\n
* @internals   
*/
double Mat::at(int i, int j)
{
	return this->x[i * col + j];
}
/**
* @par History:
*              2025/4/17,Kerui Zhang, 第一次写\n
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
/*矩阵乘法*/
extern void Mult(double** mat1, double** mat2, double** mat,int M,int N,int P)
{
	if (!mat1 || !mat||!mat2) return; // 空指针检查
	// 初始化结果矩阵为0
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < P; ++j) {
			mat[i][j] = 0;
		}
	}
	// 执行矩阵乘法
	for (int i = 0; i < M; ++i) {
		for (int k = 0; k < N; ++k) {
			if (mat1[i][k] != 0) { // 跳过零元素
				for (int j = 0; j < P; ++j) {
					mat[i][j] += mat1[i][k] * mat2[k][j];
				}
			}
		}
	}
}
/*矩阵转置*/
extern void Change(double **mat1, double **mat,int N,int M)
{
	if (!mat1 || !mat) return; // 空指针检查
	//转置
	for (int i = 0; i < M; i++){
		for (int k = 0; k < N; k++){
			mat[i][k] = mat1[k][i];
		}
	}
}
/*矩阵求逆*/
extern bool Inv(double** mat, double** inv,int N)
{
	if (!inv || !mat) return false; // 空指针检查
	// 初始化逆矩阵为单位矩阵
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			inv[i][j] = (i == j) ? 1.0 : 0.0;
		}
	}
	// 创建矩阵副本用于计算
	double **temp=nullptr;
	temp = alloc_2d_double(N, N);
	if (!temp) return false; // 分配失败

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			temp[i][j] = mat[i][j];
		}
	}
	// 高斯-约当消元法
	for (int i = 0; i < N; ++i) {
		// 寻找主元
		double maxEl = fabs(temp[i][i]);
		int maxRow = i;
		for (int k = i + 1; k < N; ++k) {
			if (fabs(temp[k][i]) > maxEl) {
				maxEl = fabs(temp[k][i]);
				maxRow = k;
			}
		}

		// 交换行（主元行和当前行）
		if (maxRow != i) {
			for (int k = 0; k < N; ++k) {
				std::swap(temp[i][k], temp[maxRow][k]);
				std::swap(inv[i][k], inv[maxRow][k]);
			}
		}

		// 检查主元是否为0（奇异矩阵）
		if (fabs(temp[i][i]) < 1e-15) {
			return false; // 矩阵不可逆
		}

		// 归一化当前行
		double divisor = temp[i][i];
		for (int j = 0; j < N; ++j) {
			temp[i][j] /= divisor;
			inv[i][j] /= divisor;
		}

		// 消元：使其他行的当前列为0
		for (int k = 0; k < N; ++k) {
			if (k == i) continue;

			double factor = temp[k][i];
			for (int j = 0; j < N; ++j) {
				temp[k][j] -= factor * temp[i][j];
				inv[k][j] -= factor * inv[i][j];
			}
		}
	}
	// 释放临时内存
	free_2d_double(temp); temp = nullptr;
}

/*矩阵相加*/
extern void Add(double** mat1, double** mat2, double** mat, int N, int M)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			mat[i][j] = mat1[i][j] + mat2[i][j];
		}
	}
}

/*一个矩阵的值赋值为另一个矩阵*/
extern void Copy(double** mat1, double** mat, int N, int M)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			mat[i][j] = mat1[i][j];
		}
	}
}

/*矩阵相减*/
extern void Minus(double** mat1, double** mat2, double** mat, int N, int M)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			mat[i][j] = mat1[i][j] - mat2[i][j];
		}
	}
}

/*矩阵和数相乘*/
extern void Mult1(double mat1, double** mat2, double** mat, int M, int P)
{
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < P; j++) {
			mat[i][j] = mat1 * mat2[i][j];
		}
	}
}
