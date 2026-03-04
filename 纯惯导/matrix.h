/**
* @file
* @brief    使用Eigen库定义矩阵运算
* @details  包括矩阵定义、基本运算、访问元素、向量运算、block功能等
* @author   Kerui Zhang. Email: krzhang@whu.edu.cn
* @date     2025/12/03
* @version  1.1.20251203
* @par      无
* @par      History:
*           2025/12/03,Kerui Zhang, 定义类和函数，初步完成封装
*/

#ifndef MATRIX_H
#define MATRIX_H

#include <Eigen/Dense>
#include <iostream>
#include <vector>
//#include <stdexcept>

namespace my_eigen {

    class Matrix {
    private:
        Eigen::MatrixXd mat_;
        bool is_block_ = false;  // 标记是否是块视图
        std::shared_ptr<Matrix> parent_ = nullptr;  // 如果是块，指向父矩阵
        int start_row_ = 0, start_col_ = 0;  // 块在父矩阵中的起始位置
        // 添加块引用标记
        bool is_ref_ = false;
        Matrix* ref_parent_ = nullptr;
        int ref_start_row_ = 0, ref_start_col_ = 0;

    public:
        // ============= 构造函数 =============
        Matrix() : mat_(0, 0) {}

        explicit Matrix(int rows, int cols, double init_val = 0.0)
            : mat_(rows, cols) {
            mat_.setConstant(init_val);
            is_ref_ = false;
        }

        //从花括号列表构造
        Matrix(const std::initializer_list<std::initializer_list<double>>& data) {
            if (data.size() == 0) {
                mat_.resize(0, 0);
                return;
            }

            size_t rows = data.size();
            size_t cols = data.begin()->size();
            mat_.resize(rows, cols);

            int i = 0;
            for (const auto& row : data) {
                if (row.size() != cols) {
                    throw std::invalid_argument("All rows must have the same number of columns");
                }
                int j = 0;
                for (double val : row) {
                    mat_(i, j) = val;
                    ++j;
                }
                ++i;
            }
        }

        // 从 Eigen 矩阵构造
        Matrix(const Eigen::MatrixXd& eigen_mat) : mat_(eigen_mat) {}

        // 从二维 vector 构造
        Matrix(const std::vector<std::vector<double>>& data) {
            if (data.empty()) {
                mat_.resize(0, 0);
                return;
            }

            size_t rows = data.size();
            size_t cols = data[0].size();
            mat_.resize(rows, cols);

            for (size_t i = 0; i < rows; ++i) {
                if (data[i].size() != cols) {
                    throw std::invalid_argument("All rows must have the same number of columns");
                }
                for (size_t j = 0; j < cols; ++j) {
                    mat_(i, j) = data[i][j];
                }
            }
        }


        // ============= 基本属性 =============
        int rows() const { return mat_.rows(); }
        int cols() const { return mat_.cols(); }
        size_t size() const { return mat_.size(); }
        bool empty() const { return mat_.size() == 0; }
        bool is_square() const { return mat_.rows() == mat_.cols(); }

        // ============= 元素访问 =============
        double& operator()(int i, int j) {
            check_bounds(i, j);
            return mat_(i, j);
        }

        double operator()(int i, int j) const {
            check_bounds(i, j);
            return mat_(i, j);
        }

        // 获取行/列向量
        Matrix row(int i) const {
            check_bounds(i, 0);
            return Matrix(mat_.row(i));
        }

        Matrix col(int j) const {
            check_bounds(0, j);
            return Matrix(mat_.col(j));
        }

        // ============= 矩阵运算 =============
        // 加法
        Matrix operator+(const Matrix& other) const {
            check_dimensions_match(other, "addition");
            return Matrix(mat_ + other.mat_);
        }

        Matrix& operator+=(const Matrix& other) {
            check_dimensions_match(other, "addition");
            mat_ += other.mat_;
            return *this;
        }

        // 减法
        Matrix operator-(const Matrix& other) const {
            check_dimensions_match(other, "subtraction");
            return Matrix(mat_ - other.mat_);
        }

        Matrix& operator-=(const Matrix& other) {
            check_dimensions_match(other, "subtraction");
            mat_ -= other.mat_;
            return *this;
        }

        // 矩阵乘法
        Matrix operator*(const Matrix& other) const {
            if (cols() != other.rows()) {
                throw std::invalid_argument("Matrix dimensions don't match for multiplication");
            }
            return Matrix(mat_ * other.mat_);
        }

        // 标量乘法
        Matrix operator*(double scalar) const {
            return Matrix(mat_ * scalar);
        }

        friend Matrix operator*(double scalar, const Matrix& mat) {
            return mat * scalar;
        }

        Matrix& operator*=(double scalar) {
            mat_ *= scalar;
            return *this;
        }

        // 矩阵转置
        Matrix transpose() const {
            return Matrix(mat_.transpose());
        }

        // 矩阵求逆（仅限方阵）
        Matrix inverse() const {
            if (!is_square()) {
                throw std::runtime_error("Matrix must be square for inversion");
            }
            return Matrix(mat_.inverse());
        }

        // 矩阵行列式（仅限方阵）
        double determinant() const {
            if (!is_square()) {
                throw std::runtime_error("Matrix must be square for determinant");
            }
            return mat_.determinant();
        }

        // ============= 特殊矩阵创建 =============
        static Matrix zeros(int rows, int cols) {
            return Matrix(rows, cols, 0.0);
        }

        static Matrix ones(int rows, int cols) {
            return Matrix(rows, cols, 1.0);
        }

        static Matrix identity(int n) {
            Matrix result(n, n, 0.0);
            for (int i = 0; i < n; ++i) {
                result(i, i) = 1.0;
            }
            return result;
        }

        // ============= 向量运算（当矩阵为向量时） =============
        double dot(const Matrix& other) const {
            if (!is_vector() || !other.is_vector() || size() != other.size()) {
                throw std::runtime_error("Vectors must have same dimensions for dot product");
            }

            // 对于向量点积，可以转换为矩阵乘法
            if (rows() == 1) {
                // 行向量
                if (other.rows() == 1) {
                    // 行向量·行向量（需要转置一个）
                    return (mat_.row(0) * other.mat_.row(0).transpose())(0, 0);
                }
                else {
                    // 行向量·列向量
                    return (mat_.row(0) * other.mat_.col(0))(0, 0);
                }
            }
            else {
                // 列向量
                if (other.cols() == 1) {
                    // 列向量·列向量（需要转置一个）
                    //std::cout << mat_.col(0) << std::endl;
                    return (mat_.col(0).transpose() * other.mat_.col(0))(0, 0);
                }
                else {
                    // 列向量·行向量
                    return (mat_.col(0).transpose() * other.mat_.row(0).transpose())(0, 0);
                }
            }
        }

        double norm() const {
            return mat_.norm();
        }

        Matrix normalized() const {
            double n = norm();
            if (n == 0.0) {
                throw std::runtime_error("Cannot normalize zero vector");
            }
            return Matrix(mat_ / n);
        }

        // 方法成员函数形式
        Matrix cross(const Matrix& other) const {
            // 检查条件：两个都必须是3维向量
            if (!is_3d_vector() || !other.is_3d_vector()) {
                throw std::invalid_argument(
                    "叉积只适用于3维向量。当前维度: " +
                    std::to_string(rows()) + "x" + std::to_string(cols()) +
                    " 和 " +
                    std::to_string(other.rows()) + "x" + std::to_string(other.cols())
                );
            }

            // 提取向量元素
            double a1 = get_element(0);
            double a2 = get_element(1);
            double a3 = get_element(2);

            double b1 = other.get_element(0);
            double b2 = other.get_element(1);
            double b3 = other.get_element(2);

            // 计算叉积
            Matrix result = create_vector_shape();
            result.set_element(0, a2 * b3 - a3 * b2);  // x分量
            result.set_element(1, a3 * b1 - a1 * b3);  // y分量
            result.set_element(2, a1 * b2 - a2 * b1);  // z分量

            return result;
        }

        // ============= 工具函数 =============
        bool is_vector() const {
            return rows() == 1 || cols() == 1;
        }

        // 打印矩阵
        void print(const std::string& name = "") const {
            if (!name.empty()) {
                std::cout << name << " = " << std::endl;
            }
            std::cout << mat_ << std::endl;
        }

        // 转换为 std::vector<std::vector<double>>
        std::vector<std::vector<double>> to_vector() const {
            std::vector<std::vector<double>> result(rows(), std::vector<double>(cols()));
            for (int i = 0; i < rows(); ++i) {
                for (int j = 0; j < cols(); ++j) {
                    result[i][j] = mat_(i, j);
                }
            }
            return result;
        }

        // 访问内部 Eigen 矩阵（用于高级操作）
        const Eigen::MatrixXd& eigen() const { return mat_; }
        Eigen::MatrixXd& eigen() { return mat_; }

        // ============= 块视图构造函数 =============
    // 创建块视图（不拷贝数据）
        Matrix(Matrix& parent, int start_row, int start_col, int block_rows, int block_cols)
            : mat_(parent.mat_.block(start_row, start_col, block_rows, block_cols)),
            is_block_(true),
            parent_(std::make_shared<Matrix>(parent)),
            start_row_(start_row),
            start_col_(start_col) {}

        // 常量块视图
        Matrix(const Matrix& parent, int start_row, int start_col, int block_rows, int block_cols)
            : mat_(parent.mat_.block(start_row, start_col, block_rows, block_cols)),
            is_block_(true),
            parent_(std::make_shared<Matrix>(parent)),
            start_row_(start_row),
            start_col_(start_col) {}

        // ============= 块操作方法 =============
        // 获取子块（作为视图）
        Matrix block(int start_row, int start_col, int block_rows, int block_cols) {
            check_block_bounds(start_row, start_col, block_rows, block_cols);
            if (is_block_) {
                // 如果是块视图，需要调整相对于原始矩阵的坐标
                return Matrix(*parent_,
                    start_row_ + start_row,
                    start_col_ + start_col,
                    block_rows,
                    block_cols);
            }
            return Matrix(*this, start_row, start_col, block_rows, block_cols);
        }

        const Matrix block(int start_row, int start_col, int block_rows, int block_cols) const {
            check_block_bounds(start_row, start_col, block_rows, block_cols);
            if (is_block_) {
                return Matrix(*parent_,
                    start_row_ + start_row,
                    start_col_ + start_col,
                    block_rows,
                    block_cols);
            }
            return Matrix(*this, start_row, start_col, block_rows, block_cols);
        }

        // 获取行块
        Matrix row_block(int start_row, int block_rows) {
            return block(start_row, 0, block_rows, cols());
        }

        const Matrix row_block(int start_row, int block_rows) const {
            return block(start_row, 0, block_rows, cols());
        }

        // 获取列块
        Matrix col_block(int start_col, int block_cols) {
            return block(0, start_col, rows(), block_cols);
        }

        const Matrix col_block(int start_col, int block_cols) const {
            return block(0, start_col, rows(), block_cols);
        }

        // 获取顶部 rows 行
        Matrix top_rows(int rows) {
            return row_block(0, rows);
        }

        const Matrix top_rows(int rows) const {
            return row_block(0, rows);
        }

        // 获取底部 rows 行
        Matrix bottom_rows(int rows) {
            return row_block(this->rows() - rows, rows);
        }

        const Matrix bottom_rows(int rows) const {
            return row_block(this->rows() - rows, rows);
        }

        // 获取左侧 cols 列
        Matrix left_cols(int cols) {
            return col_block(0, cols);
        }

        const Matrix left_cols(int cols) const {
            return col_block(0, cols);
        }

        // 获取右侧 cols 列
        Matrix right_cols(int cols) {
            return col_block(this->cols() - cols, cols);
        }

        const Matrix right_cols(int cols) const {
            return col_block(this->cols() - cols, cols);
        }

        // 获取单行（作为视图）
        Matrix row_as_matrix(int i) {
            return block(i, 0, 1, cols());
        }

        // 获取单列（作为视图）
        Matrix col_as_matrix(int j) {
            return block(0, j, rows(), 1);
        }

        // ============= 块赋值操作 =============
        // 设置块的值
        Matrix& set_block(int start_row, int start_col, const Matrix& block_mat) {
            check_block_bounds(start_row, start_col, block_mat.rows(), block_mat.cols());
            mat_.block(start_row, start_col, block_mat.rows(), block_mat.cols()) = block_mat.mat_;
            return *this;
        }

        // 设置左上角块
        Matrix& set_top_left(const Matrix& block_mat) {
            return set_block(0, 0, block_mat);
        }

        // 设置右上角块
        Matrix& set_top_right(const Matrix& block_mat) {
            return set_block(0, cols() - block_mat.cols(), block_mat);
        }

        // 设置左下角块
        Matrix& set_bottom_left(const Matrix& block_mat) {
            return set_block(rows() - block_mat.rows(), 0, block_mat);
        }

        // 设置右下角块
        Matrix& set_bottom_right(const Matrix& block_mat) {
            return set_block(rows() - block_mat.rows(), cols() - block_mat.cols(), block_mat);
        }

        // ============= 矩阵分块操作 =============
        // 水平拼接矩阵
        static Matrix hstack(const std::vector<Matrix>& matrices) {
            if (matrices.empty()) {
                return Matrix();
            }

            // 检查所有矩阵行数相同
            int total_cols = 0;
            int rows = matrices[0].rows();

            for (const auto& mat : matrices) {
                if (mat.rows() != rows) {
                    throw std::invalid_argument("All matrices must have same number of rows for hstack");
                }
                total_cols += mat.cols();
            }

            Matrix result(rows, total_cols);
            int current_col = 0;

            for (const auto& mat : matrices) {
                result.set_block(0, current_col, mat);
                current_col += mat.cols();
            }

            return result;
        }

        // 垂直拼接矩阵
        static Matrix vstack(const std::vector<Matrix>& matrices) {
            if (matrices.empty()) {
                return Matrix();
            }

            // 检查所有矩阵列数相同
            int total_rows = 0;
            int cols = matrices[0].cols();

            for (const auto& mat : matrices) {
                if (mat.cols() != cols) {
                    throw std::invalid_argument("All matrices must have same number of columns for vstack");
                }
                total_rows += mat.rows();
            }

            Matrix result(total_rows, cols);
            int current_row = 0;

            for (const auto& mat : matrices) {
                result.set_block(current_row, 0, mat);
                current_row += mat.rows();
            }

            return result;
        }

        // 对角块矩阵
        static Matrix block_diag(const std::vector<Matrix>& matrices) {
            if (matrices.empty()) {
                return Matrix();
            }

            int total_rows = 0;
            int total_cols = 0;

            for (const auto& mat : matrices) {
                total_rows += mat.rows();
                total_cols += mat.cols();
            }

            Matrix result(total_rows, total_cols);
            int current_row = 0;
            int current_col = 0;

            for (const auto& mat : matrices) {
                result.set_block(current_row, current_col, mat);
                current_row += mat.rows();
                current_col += mat.cols();
            }

            return result;
        }

        // ============= 判断是否是块视图 =============
        bool is_block_view() const { return is_block_; }

        // ============= 深拷贝（如果需要独立副本） =============
        Matrix clone() const {
            return Matrix(mat_);  // 这会创建数据的独立拷贝
        }

    private:
        void check_bounds(int i, int j) const {
            if (i < 0 || i >= rows() || j < 0 || j >= cols()) {
                throw std::out_of_range("Matrix index out of bounds");
            }
        }

        void check_dimensions_match(const Matrix& other, const std::string& operation) const {
            if (rows() != other.rows() || cols() != other.cols()) {
                throw std::invalid_argument("Matrix dimensions don't match for " + operation);
            }
        }

        // 新增：检查块边界
        void check_block_bounds(int start_row, int start_col, int block_rows, int block_cols) const {
            if (start_row < 0 || start_row >= rows() ||
                start_col < 0 || start_col >= cols() ||
                block_rows <= 0 || block_cols <= 0 ||
                start_row + block_rows > rows() ||
                start_col + block_cols > cols()) {
                throw std::out_of_range("Block indices out of range");
            }
        }

        // 辅助函数：检查是否是3维向量
        bool is_3d_vector() const {
            if (!is_vector()) return false;

            int vector_size = (rows() == 1) ? cols() : rows();
            return vector_size == 3;
        }

        // 辅助函数：获取向量元素（处理行向量和列向量）
        double get_element(int index) const {
            if (!is_vector()) {
                throw std::runtime_error("不是向量");
            }

            if (rows() == 1) {
                // 行向量
                return mat_(0, index);
            }
            else {
                // 列向量
                return mat_(index, 0);
            }
        }

        // 辅助函数：设置向量元素
        void set_element(int index, double value) {
            if (!is_vector()) {
                throw std::runtime_error("不是向量");
            }

            if (rows() == 1) {
                mat_(0, index) = value;
            }
            else {
                mat_(index, 0) = value;
            }
        }

        // 辅助函数：创建与当前向量相同形状的结果向量
        Matrix create_vector_shape() const {
            if (rows() == 1) {
                return Matrix(1, 3);  // 行向量
            }
            else {
                return Matrix(3, 1);  // 列向量
            }
        }
    };
} 

#endif // !MATRIX_H
