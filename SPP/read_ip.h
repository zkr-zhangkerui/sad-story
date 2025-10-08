/**
* @file
* @brief    This file gives the format of comments
* @details  This file gives the comment rules in C++ format.\n
*           All comments in the project should follow these rules\n
*           Chinese is supported in the comments.
* @author   kerui zhang
* @date     2025/07/12
* @version  1.0.20250712
* @par      History:
*           2025/07/12,kerui zhang
*/

#ifndef READ_IP_H
#define READ_IP_H

#include"ome_rinex.h"
#include<stdio.h>
#include<windows.h>

#pragma comment(lib,"WS2_32.lib")
#pragma warning(disable:4996)

/**
* @brief       设置IP地址，端口和输出文件路径
* 
* IP地址
* 端口
* 输出文件路径
*/
struct READ_IP
{
    char NetIP[256];
    unsigned short NetPort;
    char ObsDatFile[256];
};

/**
* @brief       连接网口
* @param[in]   sock   SOCKET&   套接字句柄
*			   IP[]   char      IP地址
*              Port   unsigned short  端口
* @return      bool    返回是否连接成功
* @note
*/
extern bool OpenIp(SOCKET& sock, const char IP[], const unsigned short Port);

/**
* @brief      数据流数据处理入口
* @param[in]  read_ip    READ_IP    数据流参数
*             raw        Raw&       存储一次读到的数据
* @return     int
*/
extern int main_Ip(READ_IP raed_ip,Raw&raw);

/**
* @brief      解码函数
* @param[in]  decBuff[]      char     数据处理缓冲区
*             lenD           &int     缓冲区总长度
*             obs_fp,nav_fp_g,nav_fp_b
*                            FILE*    输出文件指针
*             raw            Raw&     存储一次读到的数据
*             epochCount     int&     历元数
* @return     int   解码是否成功
*/
extern int header_oem4_ip(unsigned char decBuff[], int&lenD, Raw* raw, FILE* obs_fp, FILE* sate_pos_file, FILE* nav_fp_g, FILE* nav_fp_b,int&epochCount);

/**
* @brief       断开网口
* @param[in]   sock   SOCKET&   套接字句柄
* @return     
* @note
*/
extern void CloseSocket(SOCKET& sock);

#endif // !READ_IP_H
