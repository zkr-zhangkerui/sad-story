/**
* @file
* @brief    This file gives the format of comments
*           避免编译Eigen库时出现问题
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/06/26
* @version  1.0.20250626
* @par      History:
*           2025/06/26,kerui zhang
*/

// eigen_safe.h
#pragma once

// 只定义一次，避免重定义
#if !defined(_ENABLE_EXTENDED_ALIGNED_STORAGE)
#define _ENABLE_EXTENDED_ALIGNED_STORAGE
#endif

#if !defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif

#if !defined(EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT)
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#endif

// 添加其他必要定义
#if !defined(EIGEN_NO_BLAS)
#define EIGEN_NO_BLAS
#endif

#if !defined(EIGEN_NO_LAPACK)
#define EIGEN_NO_LAPACK
#endif

// 禁用警告
#pragma warning(push)
#pragma warning(disable:4127)  // 条件表达式常量
#pragma warning(disable:4714)  // __forceinline 未内联
#pragma warning(disable:4800)  // 强制值到布尔值
#pragma warning(disable:4180)  // 限定符无意义

#include <Eigen/Core>
#include <Eigen/Dense>

#pragma warning(pop)

