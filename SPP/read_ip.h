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
* @brief       ����IP��ַ���˿ں�����ļ�·��
* 
* IP��ַ
* �˿�
* ����ļ�·��
*/
struct READ_IP
{
    char NetIP[256];
    unsigned short NetPort;
    char ObsDatFile[256];
};

/**
* @brief       ��������
* @param[in]   sock   SOCKET&   �׽��־��
*			   IP[]   char      IP��ַ
*              Port   unsigned short  �˿�
* @return      bool    �����Ƿ����ӳɹ�
* @note
*/
extern bool OpenIp(SOCKET& sock, const char IP[], const unsigned short Port);

/**
* @brief      ���������ݴ������
* @param[in]  read_ip    READ_IP    ����������
*             raw        Raw&       �洢һ�ζ���������
* @return     int
*/
extern int main_Ip(READ_IP raed_ip,Raw&raw);

/**
* @brief      ���뺯��
* @param[in]  decBuff[]      char     ���ݴ�������
*             lenD           &int     �������ܳ���
*             obs_fp,nav_fp_g,nav_fp_b
*                            FILE*    ����ļ�ָ��
*             raw            Raw&     �洢һ�ζ���������
*             epochCount     int&     ��Ԫ��
* @return     int   �����Ƿ�ɹ�
*/
extern int header_oem4_ip(unsigned char decBuff[], int&lenD, Raw* raw, FILE* obs_fp, FILE* sate_pos_file, FILE* nav_fp_g, FILE* nav_fp_b,int&epochCount);

/**
* @brief       �Ͽ�����
* @param[in]   sock   SOCKET&   �׽��־��
* @return     
* @note
*/
extern void CloseSocket(SOCKET& sock);

#endif // !READ_IP_H
