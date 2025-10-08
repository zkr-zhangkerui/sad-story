#include<vector>
#include<list>

//Ӱ������
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

//Ӱ�����꼯��
typedef std::vector<ImagePoint>ImagePoints;
//�ָ�����
struct ImageRegion
{
public:
	int m_id;
	ImagePoints m_points;//�����ڵĵ�
	ImagePoint m_ul;//zuoshang
	ImagePoint m_lr;//youxia
};
//�ָ����򼯺�
typedef std::list<ImageRegion>ImageRegions;
//��������
int RegionGrowingSegmentation(std::vector<int*>pData, int imageWidth, int imageHeight, int threshold, ImageRegions& regions);
