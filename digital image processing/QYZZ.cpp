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
	long lTotal = imageWidth * imageHeight;//总像素数

	int* pR = pData[0];
	int* pG = pData[1];
	int* pB = pData[2];

	std::vector<int>label(lTotal);//标记参数是否增长
	for (int i = 0; i < lTotal; ++i)//初始化，将所有像素点设为1（未增长）
	{
		label[i] = 1;
	}

	int direction[4][2] = { {0,-1},{-1,0},{0,1},{1,0} };//存储四个方向，用于图像中相邻像素点的搜索
	int regionnum = 0;
	//遍历每个像素点
	for (int j = 0; j < imageHeight; j++)
	{
		for (int i = 0; i < imageWidth; i++)
		{
			int idx = i + j * imageWidth;
			if (label[idx] == 1)
			{
				ImageRegion newobj;
				newobj.m_id = regionnum;//初始化区域编号

				ImagePoint pt(i, j);
				newobj.m_points.push_back(pt);//将pt添加到m_points中
				newobj.m_ul = pt;//更新属性
				newobj.m_lr = pt;

				label[idx] = 0;//表示已经增长过
				std::queue<ImagePoint>seed;//存储种子点
				seed.push(pt);//pt添加到种子点
				while (seed.size())
				{
					ImagePoint tmpseed = seed.front();//从种子的头部去除一个对象
					seed.pop();//从seed中移出
					for (int k = 0; k < 4; k++)
					{
						//遍历四个方向
						int x = tmpseed.m_x + direction[k][0];//计算相邻像素点的坐标
						int y = tmpseed.m_y + direction[k][1];
						if (x < 0 || x >= imageWidth || y < 0 || y >= imageHeight)
						{
							//检查坐标是否在图像范围内
							continue;
						}
						int xyidx = x + y * imageWidth;//计算索引
						//计算相邻像素点的颜色差
						int diff = std::abs(pR[idx] - pR[xyidx]) + std::abs(pG[idx] - pG[xyidx]) + std::abs(pB[idx] - pB[xyidx]);

						if ((label[xyidx] == 1) && (diff <= threshold))//设置阈值
						{
							ImagePoint tmppt(x, y);
							newobj.m_points.push_back(tmppt);
							label[xyidx] = 0;
							//更新左上角和右下角属性，确保包含当前增长区域的最大和最小
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
