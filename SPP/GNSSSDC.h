/**
* @file
* @brief    This file gives the format of comments
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/04/17
* @version  2.3.20250417
* @par      Copyright(c) 2012-2021 School of Geodesy and Geomatics, University of Wuhan. All Rights Reserved.
*           POSMind Software Kernel, named IPS (Inertial Plus Sensing), this file is a part of IPS.
* @par      History:
*           2025/04/17,kerui zhang, new
*/

#ifndef GNSS_SDC_H
#define GNSS_SDC_H

//跨目录引用别的文件 ..：当前目录的上一层
//第三库不在工程目录下 可以在工程-属性中进行设置

#include "../BASEDLL/BaseSDC.h"
//#include "../../thirdParty/opencv.h"
typedef unsigned char      uint8_t;

#define OEM4HLEN        28      /* oem7/6/4 message header length (bytes) */
#define MAXRAWLEN   16384               /* max length of receiver raw message 哪里得到的？？？*/


#endif