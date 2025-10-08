// CHistogramDisplay.cpp: 实现文件
//

#include "pch.h"
#include "afxdialogex.h"
#include "CHistogramDisplay.h"
#include "resource.h"


// CHistogramDisplay 对话框

IMPLEMENT_DYNAMIC(CHistogramDisplay, CDialogEx)

CHistogramDisplay::CHistogramDisplay(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HISTOGRAM, pParent)
{

}

CHistogramDisplay::~CHistogramDisplay()
{
}

void CHistogramDisplay::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHistogramDisplay, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CHistogramDisplay 消息处理程序
// 数据初始化
void CHistogramDisplay::SetData(double* ValueR, double* ValueG, double* ValueB, double dMaxR, double dMaxG, double dMaxB, int Colorbits)
{
	m_nColorbits = Colorbits;
	m_dMaxR = dMaxR;
	m_dMaxG = dMaxG;
	m_dMaxB = dMaxB;
	//将 ValueR 数组中的256个双精度浮点数复制到 m_dValueR 数组中。
	memcpy(m_dValueR, ValueR, 256 * sizeof(double));
	memcpy(m_dValueG, ValueG, 256 * sizeof(double));
	memcpy(m_dValueB, ValueB, 256 * sizeof(double));
}
//显示直方图
void CHistogramDisplay::DrawGraph(CDC* pDC)
{
	int X0(20), Y0(520);
	int WX(770), HY(500);
	int H(300), W(256);
	//定义直方图的坐标和尺寸

	CPen cyPen(PS_SOLID, 2, RGB(10, 10, 10));
	CPen* oldPen = pDC->SelectObject(&cyPen);
	//创建一个黑色的实线画笔，oldpen指向画笔对象

	pDC->MoveTo(X0, Y0);
	pDC->LineTo(X0 + WX, Y0);
	pDC->MoveTo(X0, Y0);
	pDC->LineTo(X0, Y0 - HY);

	pDC->MoveTo(X0, Y0 - HY);
	pDC->LineTo(X0 + WX, Y0 - HY);
	pDC->MoveTo(X0 + WX, Y0);
	pDC->LineTo(X0 + WX, Y0 - HY);
	pDC->SelectObject(&oldPen);
	//MoveTo,LineTo函数绘制直方图的外围矩形
	{
		CPen redPen(PS_SOLID, 1, RGB(255, 0, 0));
		oldPen = pDC->SelectObject(&redPen);
		//红色画笔

		for (int i = 0; i < 256; i++)
		{
			pDC->MoveTo(X0 + i * 3, Y0);
			pDC->LineTo(X0 + i * 3, int(Y0 - 1.0 * H * m_dValueR[i] / m_dMaxR + 0.5));
		}
		pDC->SelectObject(&oldPen);
		//绘制红色直方图，m_dValueR[i],m_dMaxR表示红色通道的值和最大值
		//绿，蓝相似
	}

	{
		CPen grnPen(PS_SOLID, 2, RGB(0, 255, 0));
		oldPen = pDC->SelectObject(&grnPen);

		for (int i = 0; i < 256; i++)
		{
			pDC->MoveTo(X0 + i * 3 + 1, Y0);
			pDC->LineTo(X0 + i * 3 + 1, int(Y0 - 1.0 * H * m_dValueG[i] / m_dMaxG + 0.5));
		}
		pDC->SelectObject(&oldPen);
	}

	{
		CPen bluPen(PS_SOLID, 2, RGB(0, 0, 255));
		oldPen = pDC->SelectObject(&bluPen);

		for (int i = 0; i < 256; i++)
		{
			pDC->MoveTo(X0 + i * 3 + 2, Y0);
			pDC->LineTo(X0 + i * 3 + 2, int(Y0 - 1.0 * H * m_dValueB[i] / m_dMaxB + 0.5));
		}
		pDC->SelectObject(&oldPen);
	}
}

//绘制直方图函数
void CHistogramDisplay::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	DrawGraph(&dc);
}
