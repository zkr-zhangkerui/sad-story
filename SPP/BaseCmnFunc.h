/**
* @file
* @brief    This file gives the format of comments
*           ������ͬʱ��ϵͳת���ĺ���
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

//ʱ��ת������

/**
* @brief       ��������ת����������ʱ����
* @param[in]   mjd   MJD  ������
* @return      YMDHMS ����������ʱ����Ľṹ��
* @note        
*/
YMDHMS mjd2ymdhms(MJD mjd);

/**
* @brief       ������ʱ����ת������������
* @param[in]   ymdhms   YMDHMS  ������ʱ����
* @return      MJD ���ؼ��������Ľṹ��
* @note
*/
MJD ymdhms2mjd(YMDHMS ymdhms);

/**
* @brief       ������ת����GPSʱ
* @param[in]   mjd   MJD  ������
* @return      YMDHMS ����GPSʱ�Ľṹ��
* @note
*/
GPSTIME mjd2gpstime(MJD mjd);

/**
* @brief       GPSʱת����������
* @param[in]   gpstime   GPSTIME  GPSʱ
* @return      MJD �����������Ľṹ��
* @note
*/
MJD gpstime2mjd(GPSTIME gpstime);

/**
* @brief       GPSʱת����������ʱ����
* @param[in]   gpstime   GPSTIME  GPSʱ
* @return      YMDHMS ����������ʱ����Ľṹ��
* @note
*/
YMDHMS gpstime2ymdhms(GPSTIME gpstime);

/**
* @brief       ������ʱ����ת����GPSʱ
* @param[in]   ymdhms   YMDHMS  ������ʱ����
* @return      YMDHMS ����GPSʱ�Ľṹ��
* @note
*/
GPSTIME ymdhms2gpstime(YMDHMS ymdhms);

/**
* @brief       GPSʱת����BDSʱ
* @param[in]   gpstime   GPSTIME   GPSʱ
* @return      YMDHMS ���ر���ʱ�Ľṹ��
* @note
*/
GPSTIME gpstime2bdstime(GPSTIME gpstime);

/**
* @brief       BDSʱת����GPSʱ
* @param[in]   bdstime   GPSTIME   ����ʱ
* @return      YMDHMS ����GPSʱ�Ľṹ��
* @note
*/
GPSTIME bdstime2gpstime(GPSTIME bdstime);

/**
* @brief       ����תGPSʱ
* @param[in]   sec   double   ����
* @return      GPSTIME ����GPSʱ�Ľṹ��
* @note
*/
GPSTIME sec2gpstime(double sec);

/**
* @brief       GPSʱתЭ������ʱUTC
* @param[in]   gpstime   GPSTIME   GPSʱ
*              deltat_le double    ������
* @return      double ����utc����
* @note
*/
double gpstime2utc(GPSTIME*gpstime, double deltat_ls);

#endif