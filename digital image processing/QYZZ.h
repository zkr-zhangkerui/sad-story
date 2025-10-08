#include<vector>
#include<list>

//影像坐标
struct ImagePoint
{
public:
	int m_x, m_y;

	ImagePoint()
	{
		m_x = 0; m_y = 0;
	}
	ImagePoint(int x, int y)
	{
		m_x = x; m_y = y;
	}
};

//影像坐标集合
typedef std::vector<ImagePoint>ImagePoints;
//分割区域
struct ImageRegion
{
public:
	int m_id;
	ImagePoints m_points;//区域内的点
	ImagePoint m_ul;//zuoshang
	ImagePoint m_lr;//youxia
};
//分割区域集合
typedef std::list<ImageRegion>ImageRegions;
//声明函数
int RegionGrowingSegmentation(std::vector<int*>pData, int imageWidth, int imageHeight, int threshold, ImageRegions& regions);
