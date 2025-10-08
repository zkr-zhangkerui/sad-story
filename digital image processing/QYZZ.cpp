#include"pch.h"
#include"stdafx.h"
#include"QYZZ.h"
#include<queue>
#include<cmath>

//using namespace std;

int RegionGrowingSegmentation(std::vector<int*>pData, int imageWidth, int imageHeight, int threshold, ImageRegions& regions)
{
	if (!pData.size())
	{
		return 0;
	}
	long lTotal = imageWidth * imageHeight;//��������

	int* pR = pData[0];
	int* pG = pData[1];
	int* pB = pData[2];

	std::vector<int>label(lTotal);//��ǲ����Ƿ�����
	for (int i = 0; i < lTotal; ++i)//��ʼ�������������ص���Ϊ1��δ������
	{
		label[i] = 1;
	}

	int direction[4][2] = { {0,-1},{-1,0},{0,1},{1,0} };//�洢�ĸ���������ͼ�����������ص������
	int regionnum = 0;
	//����ÿ�����ص�
	for (int j = 0; j < imageHeight; j++)
	{
		for (int i = 0; i < imageWidth; i++)
		{
			int idx = i + j * imageWidth;
			if (label[idx] == 1)
			{
				ImageRegion newobj;
				newobj.m_id = regionnum;//��ʼ��������

				ImagePoint pt(i, j);
				newobj.m_points.push_back(pt);//��pt��ӵ�m_points��
				newobj.m_ul = pt;//��������
				newobj.m_lr = pt;

				label[idx] = 0;//��ʾ�Ѿ�������
				std::queue<ImagePoint>seed;//�洢���ӵ�
				seed.push(pt);//pt��ӵ����ӵ�
				while (seed.size())
				{
					ImagePoint tmpseed = seed.front();//�����ӵ�ͷ��ȥ��һ������
					seed.pop();//��seed���Ƴ�
					for (int k = 0; k < 4; k++)
					{
						//�����ĸ�����
						int x = tmpseed.m_x + direction[k][0];//�����������ص������
						int y = tmpseed.m_y + direction[k][1];
						if (x < 0 || x >= imageWidth || y < 0 || y >= imageHeight)
						{
							//��������Ƿ���ͼ��Χ��
							continue;
						}
						int xyidx = x + y * imageWidth;//��������
						//�����������ص����ɫ��
						int diff = std::abs(pR[idx] - pR[xyidx]) + std::abs(pG[idx] - pG[xyidx]) + std::abs(pB[idx] - pB[xyidx]);

						if ((label[xyidx] == 1) && (diff <= threshold))//������ֵ
						{
							ImagePoint tmppt(x, y);
							newobj.m_points.push_back(tmppt);
							label[xyidx] = 0;
							//�������ϽǺ����½����ԣ�ȷ��������ǰ���������������С
							if (newobj.m_ul.m_x > tmppt.m_x)
							{
								newobj.m_ul.m_x = tmppt.m_x;
							}
							if (newobj.m_lr.m_x < tmppt.m_x)
							{
								newobj.m_lr.m_x = tmppt.m_x;
							}
							if (newobj.m_ul.m_y > tmppt.m_y)
							{
								newobj.m_ul.m_y = tmppt.m_y;
							}
							if (newobj.m_lr.m_y < tmppt.m_y)
							{
								newobj.m_lr.m_y = tmppt.m_y;
							}
							seed.push(tmppt);
						}
					}
				}
				regions.push_back(newobj);
				regionnum++;
			}
		}
	}
	return regionnum;
}
