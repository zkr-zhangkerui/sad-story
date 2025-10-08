/**
* @file
* @brief    This file gives the format of comments
*           声明不同时间系统转化的函数
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/04/17
* @version  1.0 20250417
* @par      History:
*           2025/04/17,kerui zhang
*/
#ifndef BASE_CMN_FUNC_H
#define BASE_CMN_FUNC_H

#include "BaseSDC.h"

//时间转换函数

/**
* @brief       简化儒略历转换到年月日时分秒
* @param[in]   mjd   MJD  儒略历
* @return      YMDHMS 返回年月日时分秒的结构体
* @note        
*/
YMDHMS mjd2ymdhms(MJD mjd);

/**
* @brief       年月日时分秒转化到简化儒略历
* @param[in]   ymdhms   YMDHMS  年月日时分秒
* @return      MJD 返回简化儒略历的结构体
* @note
*/
MJD ymdhms2mjd(YMDHMS ymdhms);

/**
* @brief       儒略历转换到GPS时
* @param[in]   mjd   MJD  儒略历
* @return      YMDHMS 返回GPS时的结构体
* @note
*/
GPSTIME mjd2gpstime(MJD mjd);

/**
* @brief       GPS时转化到儒略历
* @param[in]   gpstime   GPSTIME  GPS时
* @return      MJD 返回儒略历的结构体
* @note
*/
MJD gpstime2mjd(GPSTIME gpstime);

/**
* @brief       GPS时转换到年月日时分秒
* @param[in]   gpstime   GPSTIME  GPS时
* @return      YMDHMS 返回年月日时分秒的结构体
* @note
*/
YMDHMS gpstime2ymdhms(GPSTIME gpstime);

/**
* @brief       年月日时分秒转化到GPS时
* @param[in]   ymdhms   YMDHMS  年月日时分秒
* @return      YMDHMS 返回GPS时的结构体
* @note
*/
GPSTIME ymdhms2gpstime(YMDHMS ymdhms);

/**
* @brief       GPS时转化到BDS时
* @param[in]   gpstime   GPSTIME   GPS时
* @return      YMDHMS 返回北斗时的结构体
* @note
*/
GPSTIME gpstime2bdstime(GPSTIME gpstime);

/**
* @brief       BDS时转化到GPS时
* @param[in]   bdstime   GPSTIME   北斗时
* @return      YMDHMS 返回GPS时的结构体
* @note
*/
GPSTIME bdstime2gpstime(GPSTIME bdstime);

/**
* @brief       秒数转GPS时
* @param[in]   sec   double   秒数
* @return      GPSTIME 返回GPS时的结构体
* @note
*/
GPSTIME sec2gpstime(double sec);

/**
* @brief       GPS时转协调世界时UTC
* @param[in]   gpstime   GPSTIME   GPS时
*              deltat_le double    闰秒数
* @return      double 返回utc秒数
* @note
*/
double gpstime2utc(GPSTIME*gpstime, double deltat_ls);

#endif