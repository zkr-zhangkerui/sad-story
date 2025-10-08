
// zhangkeruiimageView.cpp: CzhangkeruiimageView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "zhangkeruiimage.h"
#endif

#include "zhangkeruiimageDoc.h"
#include "zhangkeruiimageView.h"
#include"MainFrm.h"
#include "afxdialogex.h"
#include "resource.h"
#include"cmath"
#include"QYZZ.h"
#include"vector"
using namespace cv;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace cv;

// CzhangkeruiimageView

IMPLEMENT_DYNCREATE(CzhangkeruiimageView, CScrollView)

BEGIN_MESSAGE_MAP(CzhangkeruiimageView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_VIEW_ZOOM_IN, &CzhangkeruiimageView::OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, &CzhangkeruiimageView::OnViewZoomOut)
	ON_WM_MOUSEMOVE()
//	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN, &CzhangkeruiimageView::OnOpencv1)

ON_COMMAND(32778, &CzhangkeruiimageView::OnProcessLinetran1)//线性变换

ON_COMMAND(ID_32779, &CzhangkeruiimageView::OnProcessHistogram)
ON_COMMAND(ID_IMAGE_SMOOTH, &CzhangkeruiimageView::OnImageSmooth)
ON_COMMAND(ID_GEO_TRANS, &CzhangkeruiimageView::OnGeoTrans)
ON_COMMAND(ID_FREQ_FOUR, &CzhangkeruiimageView::OnFreqFour)
ON_COMMAND(ID_BUTTERWORTH_LOW, &CzhangkeruiimageView::OnButterworthLow)
ON_COMMAND(ID_BUTTERWORTH_HI, &CzhangkeruiimageView::OnButterworthHi)
ON_COMMAND(ID_QYZZ, &CzhangkeruiimageView::OnQyzz)
ON_COMMAND(ID_Ostu, &CzhangkeruiimageView::OnOstu)
ON_COMMAND(ID_OPENCV_1, &CzhangkeruiimageView::OnOpencv1)
ON_COMMAND(ID_bianyuan, &CzhangkeruiimageView::Onbianyuan)
END_MESSAGE_MAP()

// CzhangkeruiimageView 构造/析构

CzhangkeruiimageView::CzhangkeruiimageView() noexcept
{
	// TODO: 在此处添加构造代码
	m_dZoom = 1.0;

	//显示直方图 初始化所添加的数据结构
	m_dMaxR = m_dMaxG = m_dMaxB = 0;
	for (int i = 0; i < 256; i++)
	{
		m_lValueB[i] = m_lValueG[i] = m_lValueR[i] = 0;
		m_dValueB[i] = m_dValueG[i] = m_dValueR[i] = 0;
	}
	
	
}

CzhangkeruiimageView::~CzhangkeruiimageView()
{
}

BOOL CzhangkeruiimageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}

// CzhangkeruiimageView 绘图

void CzhangkeruiimageView::OnDraw(CDC* pDC)
{
	CzhangkeruiimageDoc* pDoc = GetDocument();//获取图像
	ASSERT_VALID(pDoc);//检查pDoc是否有效
	if (!pDoc)
		return;

	//设置滚动条，以便视图可以滚动查看整个图片
	CSize sizeTotal;
	sizeTotal.cx = int(pDoc->imageWidth + 0.5);
	sizeTotal.cy = int(pDoc->imageHeight + 0.5);
	SetScrollSizes(MM_TEXT, sizeTotal);

	if (pDoc->m_nOpenMode == 1)//BMP
	{
		StretchDIBits(pDC->m_hDC, 0, 0,
			int(m_dZoom*pDoc->imageWidth+0.5), int(m_dZoom * pDoc->imageHeight + 0.5),0, 0, pDoc->imageWidth, pDoc->imageHeight,
			pDoc->m_pBits, pDoc->lpbmi, DIB_RGB_COLORS, SRCCOPY);
		//将图像数据绘制到视图中
	}

	// TODO: 在此处为本机数据添加绘制代码
	if (pDoc->m_nOpenMode == 2)//JPG
	{
		StretchDIBits(pDC->m_hDC, 0, 0, int(m_dZoom * pDoc->imageWidth + 0.5), int(m_dZoom * pDoc->imageHeight + 0.5),
			0, pDoc->image->rows, pDoc->image->cols, -pDoc->image->rows, pDoc->image->data, pDoc->lpbmi,
			DIB_RGB_COLORS, SRCCOPY);
		//StretchDIBits(pDC->m_hDC, 0, 0,
			//pDoc->image->cols, pDoc->image->rows, 0, pDoc->image->rows,
			//pDoc->image->cols, -pDoc->image->rows,
			//pDoc->image->data, pDoc->lpbmi, DIB_RGB_COLORS, SRCCOPY);
	}
}

void CzhangkeruiimageView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CzhangkeruiimageView 打印

BOOL CzhangkeruiimageView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CzhangkeruiimageView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CzhangkeruiimageView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CzhangkeruiimageView 诊断

#ifdef _DEBUG
void CzhangkeruiimageView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CzhangkeruiimageView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CzhangkeruiimageDoc* CzhangkeruiimageView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CzhangkeruiimageDoc)));
	return (CzhangkeruiimageDoc*)m_pDocument;
}
#endif //_DEBUG


// CzhangkeruiimageView 消息处理程序


void CzhangkeruiimageView::OnViewZoomIn()
{
	// TODO: 在此添加命令处理程序代码
	m_dZoom = m_dZoom / 2;//图像缩小比例
	Invalidate();//重新绘图
}


void CzhangkeruiimageView::OnViewZoomOut()
{
	// TODO: 在此添加命令处理程序代码
	m_dZoom = m_dZoom * 2;
	Invalidate();
}


void CzhangkeruiimageView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CzhangkeruiimageDoc* pDoc = GetDocument();
	if (pDoc->m_pBits != NULL || pDoc->image != NULL)//打开图像才进行以下内容
	{
		CPoint scrPoint = GetScrollPosition();//计算滚动量
		int nX = int(0.5 + (scrPoint.x + point.x) / m_dZoom);//计算到图像坐标x
		int nY = int(0.5 + (scrPoint.y + point.y) / m_dZoom);//计算到图像坐标y
		int nG, nR, nB;

		if (nX >= 0 && nX < pDoc->imageWidth && nY >= 0 && nY < pDoc->imageHeight)
		//坐标在图像内
		{
			CString strPrompt;
			if (pDoc->m_nOpenMode == 1)//BMP
			{
				if (pDoc->m_nColorBits == 8)//8位图像
				{
					nB = *(pDoc->m_pBits + (pDoc->imageHeight - nY - 1) * pDoc->imageWidth + nX);
					//获取像素点的灰度值
					strPrompt.Format(_T("(X,Y):(%4d,%4d);Gray:%3d"), nX, nY, nB);//显示该坐标处的灰度值
				}
				if (pDoc->m_nColorBits == 24)//真彩色
				{
					nB = *(pDoc->m_pBits + (pDoc->imageHeight - nY - 1) * pDoc->imageWidth * 3 + nX * 3 + 0);
					nG = *(pDoc->m_pBits + (pDoc->imageHeight - nY - 1) * pDoc->imageWidth * 3 + nX * 3 + 1);
					nR = *(pDoc->m_pBits + (pDoc->imageHeight - nY - 1) * pDoc->imageWidth * 3 + nX * 3 + 2);
					//获取像素点的红，绿6，蓝值

					strPrompt.Format(_T("(X,Y):(%4d,%4d); (R,G,B): (%3d,%3d,%3d)", nX, nY, nR, nG, nB));
				}
			
			}
			if (pDoc->m_nOpenMode == 2)//jpg
			{
				pDoc->m_pBits = (unsigned char*)pDoc->image->data;//获取图像数据

				if (pDoc->m_nColorBits == 8)//8位图像
				{
					nB = *(pDoc->m_pBits + nY*pDoc->imageWidth + nX);
					strPrompt.Format(_T("(X,Y):(%4d,%4d);Gray:%3d"), nX, nY, nB);
				}
				if (pDoc->m_nColorBits == 24)
				{
					nB = *(pDoc->m_pBits +nY * pDoc->imageWidth * 3 + nX * 3 + 0);
					nG = *(pDoc->m_pBits + nY * pDoc->imageWidth * 3 + nX * 3 + 1);
					nR = *(pDoc->m_pBits + nY * pDoc->imageWidth * 3 + nX * 3 + 2);

					strPrompt.Format(_T("(X,Y):(%4d,%4d); (R,G,B): (%3d,%3d,%3d)", nX, nY, nR, nG, nB));
				}
			}
			((CMainFrame*)AfxGetMainWnd())->SetStatusBarText(1, strPrompt);
		}
	}

	CScrollView::OnMouseMove(nFlags, point);
}










void CzhangkeruiimageView::OnProcessLinetran1()
{
	// TODO: 在此添加命令处理程序代码
	CzhangkeruiimageDoc* pDoc = GetDocument();//获取图像

	if (pDoc->m_nOpenMode == 1)
	{
		uchar* pBits = pDoc->m_pBits;//图像数据存储到pBits中
		if (pBits == NULL)return;
		if (pDoc->m_nColorBits == 24)
		{
			//设置颜色变换的上下限
			//int R_lower(150), R_upper = 255, G_lower(100), G_upper(200), B_lower(25), B_upper(125), L_lower(0), U_upper(255);
			int R_lower(150), R_upper = 255, G_lower(100), G_upper(200), B_lower(25), B_upper(125), L_lower(100), U_upper(200);
			for (int i = 0; i < pDoc->imageHeight; i++)
			{
				for (int j = 0; j < pDoc->imageWidth; j++)
				{
					//指向图像数据中的一个特定像素点
					uchar* ptr = pBits + pDoc->imageWidth * 3 * (i)+(j * 3);
					//进行颜色变换,24位图每个通道有3个通道，红绿蓝
					ptr[2] = int(R_lower + 1.0 * ((R_upper - R_lower) / (U_upper - L_lower)) * (ptr[2] - L_lower));
					ptr[1] = int(G_lower + 1.0 * ((G_upper - G_lower) / (U_upper - L_lower)) * (ptr[1] - L_lower));
					ptr[0] = int(B_lower + 1.0 * ((B_upper - B_lower) / (U_upper - L_lower)) * (ptr[2] - L_lower));
				}
			}
		}
	}
	if (pDoc->m_nOpenMode == 2)
	{
		uchar* pBits =pDoc->image->data;
		if (pBits == NULL)return;
		if (pDoc->image->channels() == 3)
		{
			int R_lower(150), R_upper(255), G_lower(100), G_upper(200), B_lower(25), B_upper(125), L_lower(100), U_upper(200);

			for (int i = 0; i < pDoc->image->rows; i++)
			{
				for (int j = 0; j < pDoc->image->cols; j++)
				{
					uchar* ptr = pBits + pDoc->image->step*(i)+(j * 3);
					//进行颜色变换
					ptr[2] = int(R_lower + 1.0 * ((R_upper - R_lower) / (U_upper - L_lower)) * (ptr[2] - L_lower));
					ptr[1] = int(G_lower + 1.0 * ((G_upper - G_lower) / (U_upper - L_lower)) * (ptr[1] - L_lower));
					ptr[0] = int(B_lower + 1.0 * ((B_upper - B_lower) / (U_upper - L_lower)) * (ptr[0] - L_lower));
				}
			}
		}
	}
	Invalidate();//重新绘图
}


void CzhangkeruiimageView::OnProcessHistogram()
{
	// TODO: 在此添加命令处理程序代码
	CzhangkeruiimageDoc* pDoc = GetDocument();


	if (pDoc->m_nOpenMode == 1)
	{
		uchar* pBits = pDoc->m_pBits;//获取图像数据指针
		if (pBits == NULL) return;
		long lTotal(0);
		if (pDoc->m_nColorBits == 24)
		{
			//计算每个通道的直方图
			for (int i = 0; i < pDoc->imageHeight; i++)
			{
				for (int j = 0; j < pDoc->imageWidth; j++)
				{
					//指向图像数据中的一个特定像素点
					uchar* ptr = pBits + pDoc->imageWidth * 3 * (i)+(j * 3);		///< 如果这里没有设置正确，可能会在图像部分区域显示颜色变化

					m_lValueR[int(ptr[2])] += 1;
					m_lValueG[int(ptr[1])] += 1;
					m_lValueB[int(ptr[0])] += 1;
					lTotal += 1;
				}
			}

			for (int i = 0; i < 256; i++)
			{
				//计算每个通道的最大值
				m_dValueR[i] = double(m_lValueR[i]) / double(lTotal);
				m_dMaxR = std::max(m_dMaxR, m_dValueR[i]);
				m_dValueG[i] = double(m_lValueG[i]) / double(lTotal);
				m_dMaxG = std::max(m_dMaxG, m_dValueG[i]);
				m_dValueB[i] = double(m_lValueB[i]) / double(lTotal);
				m_dMaxB = std::max(m_dMaxB, m_dValueB[i]);
			}
			CHistogramDisplay hisDlg;//创建对话框
			//将直方图数据传递给对话框
			hisDlg.SetData(m_dValueR, m_dValueG, m_dValueB, m_dMaxR, m_dMaxG, m_dMaxB, pDoc->m_nColorBits);
			hisDlg.DoModal();
		}
	}

	if (pDoc->m_nOpenMode == 2)
	{
		uchar* pBits = pDoc->image->data;
		if (pBits == NULL) return;
		long lTotal(0);
		if (pDoc->image->channels() == 3)
		{
			for (int i = 0; i < pDoc->image->cols; i++)
			{
				for (int j = 0; j < pDoc->image->rows; j++)
				{
					uchar* ptr = pBits + pDoc->image->step * (i)+(j * 3);

					m_lValueR[int(ptr[2])] += 1;
					m_lValueG[int(ptr[1])] += 1;
					m_lValueB[int(ptr[0])] += 1;
					lTotal += 1;
				}
			}

			for (int i = 0; i < 256; i++)
			{
				m_dValueR[i] = double(m_lValueR[i]) / double(lTotal);
				m_dMaxR = std::max(m_dMaxR, m_dValueR[i]);;
				m_dValueG[i] = double(m_lValueG[i]) / double(lTotal);
				m_dMaxG = std::max(m_dMaxG, m_dValueG[i]);
				m_dValueB[i] = double(m_lValueB[i]) / double(lTotal);
				m_dMaxB = std::max(m_dMaxB, m_dValueB[i]);
			}
			CHistogramDisplay hisDlg;
			hisDlg.SetData(m_dValueR, m_dValueG, m_dValueB, m_dMaxR, m_dMaxG, m_dMaxB, pDoc->m_nColorBits);
			hisDlg.DoModal();
		}
	}

}



void CzhangkeruiimageView::OnImageSmooth()
{
	// TODO: 在此添加命令处理程序代码
	CzhangkeruiimageDoc* pDoc = GetDocument();
	//创建对话框，并用DoModal函数显示它
	CSmoothDlg sdlg;
	if (sdlg.DoModal())
	{
		//H是存储图像平滑的权重矩阵
		double H[3][3];
		double k = sdlg.m_nScale;//缩放因子
		H[0][0] = sdlg.m_nSmooth1;
		H[0][1] = sdlg.m_nSmooth2;
		H[0][2] = sdlg.m_nSmooth3;
		H[1][0] = sdlg.m_nSmooth4;
		H[1][1] = sdlg.m_nSmooth5;
		H[1][2] = sdlg.m_nSmooth6;
		H[2][0] = sdlg.m_nSmooth7;
		H[2][1] = sdlg.m_nSmooth8;
		H[2][2] = sdlg.m_nSmooth9;

		uchar* pBits = NULL;
		uchar* pOldBits = NULL;
		int nWidth(0);//图像高度
		int nHeight(0);//图像宽度
		long lTotalR(0), lTotal(0);//图像总列数，总行数
		double dValue;//总像素数
		double pValue[3][3];//存储局部像素值的数组
		if (pDoc->m_nOpenMode == 1)
		{
			pBits = pDoc->m_pBits;//获取图像数据
			if (!pBits)return;

			nWidth = pDoc->imageWidth;//获取图像宽度
			nHeight = pDoc->imageHeight;//获取图像高度

			if (pDoc->m_nColorBits == 24)//真彩色
			{
				pOldBits = new uchar[3 * nWidth * nHeight];//图像原始数据
				CopyMemory(pOldBits, pBits, 3 * nWidth * nHeight);

				for (int i = 0; i < nHeight; i++)
				{
					lTotalR = i * nWidth;//行索引
					for (int j = 0; j < nWidth; j++)
					{
						lTotal = (lTotalR + j) * 3;//列索引
						//lTotal = lTotalR + j * 3;

						if (i == 0 || i == nHeight - 1 || j == 0 || j == nWidth - 1)//边缘点不处理
						{
							pBits[lTotal + 0] = pOldBits[lTotal + 0];
							pBits[lTotal + 1] = pOldBits[lTotal + 1];
							pBits[lTotal + 2] = pOldBits[lTotal + 2];
						}
						//边缘数据不做处理
						else
						{
							//对每个像素点的每个通道应用相同的平滑算法
							for (int k = 0; k < 3; k++)
							{
								//计算一个3×3的局部区域中每个像素的值
								for (int m = 0; m < 3; m++)
								{
									for (int n = 0; n < 3; n++)
									{
										//局部像素值的加权平均值
										pValue[m][n] = pOldBits[lTotal - (m - 1) * -3 * nWidth -(n - 1) * -3 + k];
									}
								}
								//计算局部像素值加权平均值的总和
								dValue = 0.0;
								for (int m = 0; m < 3; m++)
								{
									for (int n = 0; n < 3; n++)
									{
										dValue += H[m][n] * pValue[m][n];
									}
								}
								//dValue *= k;
								//pBits[lTotal + k] = int(dValue * k + 0.5);
								//确保平滑后的值在0到255之间，转化成图像数据额标准类型
								pBits[lTotal + k] = static_cast<uchar>(std::min(std::max(dValue, 0.0), (255.0,255.0,255.0)));
							}
							
						}
					}
				}
				Invalidate();
				delete pOldBits;
			}
		}
		if (pDoc->m_nOpenMode == 2)
		{
			pBits = pDoc->image->data;
			if (!pBits)return;

			nWidth = pDoc->image->cols;
			nHeight = pDoc->image->rows;

			if (pDoc->image->channels() == 3)
			{
				pOldBits = new uchar[3 * nWidth * nHeight];
				CopyMemory(pOldBits, pBits , 3 * nWidth * nHeight);

				for (int i = 0; i < nHeight; i++)
				{
					lTotalR = i * nWidth;
					for (int j = 0; j < nWidth; j++)
					{
						lTotal = (lTotalR + j) * 3;

						if(i == 0 || i == nHeight - 1 || j == 0 || j == nWidth - 1)
						{
							pBits[lTotal + 0] = pOldBits[lTotal + 0];
							pBits[lTotal + 1] = pOldBits[lTotal + 1];
							pBits[lTotal + 2] = pOldBits[lTotal + 2];
						}
						else
						{
							for (int k = 0; k < 3; k++)
							{
								pValue[0][0] = pOldBits[lTotal - 3 * nWidth - 3 * 1 + k];
								pValue[0][1] = pOldBits[lTotal - 3 * nWidth - 3 * 0 + k];
								pValue[0][2] = pOldBits[lTotal - 3 * nWidth + 3 * 1 + k];
								pValue[1][0] = pOldBits[lTotal - 3 * 0 - 3 * 1 + k];
								pValue[1][1] = pOldBits[lTotal - 3 * 0 - 3 * 0 + k];
								pValue[1][2] = pOldBits[lTotal - 3 * 0 + 3 * 1 + k];
								pValue[2][0] = pOldBits[lTotal + 3 * nWidth - 3 * 1 + k];
								pValue[2][1] = pOldBits[lTotal + 3 * nWidth - 3 * 0 + k];
								pValue[2][2] = pOldBits[lTotal + 3 * nWidth + 3 * 1 + k];

								dValue = 0.0;
								for (int m = 0; m < 3; m++)
								{
									for (int n = 0; n < 3; n++)
									{
										dValue += H[m][n] * pValue[m][n];
									}
								}
								//dValue *= k;
								pBits[lTotal + k] = static_cast<uchar>(std::min(std::max(dValue, 0.0), 255.0));
								//pBits[lTotal + k] = int(dValue * k + 0.5);
							}
						}
					}
				}
				Invalidate();
				delete pOldBits;
			}

		}
	}
	
}




void CzhangkeruiimageView::OnGeoTrans()
{
	// TODO: 在此添加命令处理程序代码
	CzhangkeruiimageDoc* pDoc = GetDocument();

	if (pDoc->m_nOpenMode != 2)return;
	CGeoTransDlg gdlg;
	//显示对话框
	if (gdlg.DoModal())
	{
		cv::Point2f srcTri[3];
		cv::Point2f dstTri[3];
		cv::Mat rot_mat(2, 3, CV_32FC1);
		cv::Mat warp_mat(2, 3, CV_32FC1);
		cv::Mat warp_dst, warp_rotate_dst;
		//存储放射变换和旋转矩阵的参数

		cv::Mat src(*pDoc->image);
		//获取图像数据指针

		warp_dst = cv::Mat::zeros(src.rows, src.cols, src.type());
		//创建一个与原始图像同尺寸同类型的矩阵，并初始化为0

		//放射变换
		srcTri[0] = cv::Point2f(0, 0);
		srcTri[1] = cv::Point2f(src.cols - 1, 0);
		srcTri[2] = cv::Point2f(0,src.rows-1);
		//源三角形
		dstTri[0] = cv::Point2f(src.cols * 0.00, src.rows * 0.33);
		dstTri[1] = cv::Point2f(src.cols * 0.85, src.rows * 0.25);
		dstTri[2] = cv::Point2f(src.cols * 0.15, src.rows * 0.70);
		//目标三角形
		warp_mat = cv::getAffineTransform(srcTri, dstTri);
		//计算放射变换矩阵
		cv::warpAffine(src, warp_dst, warp_mat, warp_dst.size());
		//变换原始图像

		//旋转矩阵
		cv::Point center = cv::Point(int(warp_mat.cols / 2), int(warp_mat.rows / 2));
		//计算旋转矩阵的中心点
		double angle = gdlg.m_dRotation;//从对话框中获取旋转角度
		double scale = gdlg.m_dScale;//从对话框中获取缩放因子
		rot_mat = cv::getRotationMatrix2D(center, angle, scale);//计算旋转矩阵
		cv::warpAffine(warp_dst, warp_rotate_dst, rot_mat, warp_dst.size());
		//进行旋转和缩放

		cv::namedWindow("原文件",1);
		cv::imshow("原文件", src);
		cv::namedWindow("放射变换", 1);
		cv::imshow("放射变换", warp_dst);
		cv::namedWindow("放射变换&缩放", 1);
		cv::imshow("放射变换&缩放", warp_rotate_dst);
		cv::waitKey();
		//显示图像

	}
}


void CzhangkeruiimageView::OnFreqFour()
{
	// TODO: 在此添加命令处理程序代码
	//Mat I = imread("E:\\新建文件夹\\大二上\\计算机视觉基础\\实习\\pic\\屏幕截图 2024-11-18 183236.jpg", IMREAD_GRAYSCALE);//读入图像灰度图
	Mat I = imread("E:\\新建文件夹\\大二上\\计算机视觉基础\\实习\\pic\\无标题.jpg", IMREAD_GRAYSCALE);
	Mat padded;//以0填充输入图像矩阵
	int m = getOptimalDFTSize(I.rows);
	int n = getOptimalDFTSize(I.cols);
	//计算傅里叶变换的最优尺寸

	//填充输入图像，输入矩阵为padded，上方和左方不做处理
	copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = { Mat_<float>(padded),Mat::zeros(padded.size(),CV_32F) };
	//为傅里叶变换准备数据，创建了一个包含实部和虚部的矩阵数组
	Mat complexI;
	merge(planes, 2, complexI);//将planes融合合并成一个多通道数组complexI

	dft(complexI, complexI);//进行傅里叶变换

	//计算振幅，转换到对数尺度
	split(complexI, planes);

	magnitude(planes[0], planes[1], planes[0]);//计算两个复数数组的模
	Mat magI = planes[0];//将planes【0】中的数据复制到magI中

	magI += Scalar::all(1);
	log(magI, magI);//转换为对数形式

	//如果有奇数行或列，则对频谱进行裁剪
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

	//重新排列傅里叶图像中的象限，使原点位于图像中心
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;
	Mat q0(magI, Rect(0, 0, cx, cy));//左上角划定ROI区域
	Mat q1(magI, Rect(cx, 0, cx, cy));//右上角图像
	Mat q2(magI, Rect(0, cy, cx, cy));//左下
	Mat q3(magI, Rect(cx, cy, cx, cy));//右下

	//变换左上和右下象限
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	//右上和左下
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	//归一化，用0，1之间的浮点数将矩阵变换为可视的图像格式
	normalize(magI, magI, 0, 1, CV_MINMAX);

	imshow("输入图像", I);
	imshow("频谱图", magI);
}


void CzhangkeruiimageView::OnButterworthLow()
{
	// TODO: 在此添加命令处理程序代码
	Mat img = imread("E:\\新建文件夹\\大二上\\计算机视觉基础\\实习\\pic\\屏幕截图 2024-11-18 183236.jpg", 0);
	//以灰度模式加载图像
	if (img.empty())
	{
		std::cout << "无法加载图像" << std::endl;
	}
	imshow("初始图像",img);
	Mat dftInput1, dftImage1, inverseDFT, inverseDFTconverted;
	img.convertTo(dftInput1, CV_32F);//将原始图像转化32位浮点数类型
	dft(dftInput1, dftImage1);//执行傅里叶变换
	imshow("傅里叶变换", dftImage1);
	int cx = dftImage1.cols / 2;//计算图像的宽和高
	int cy = dftImage1.rows / 2;
	Mat q0(dftImage1, Rect(0, 0, cx, cy));//top-left//创建图像的四个子区域
	Mat q1(dftImage1, Rect(cx, 0, cx, cy));//top-right
	Mat q2(dftImage1, Rect(0, cy, cx, cy));//bottom-left
	Mat q3(dftImage1, Rect(cx, cy, cx, cy));//bottom-right
	Mat tmp;
	q0.copyTo(tmp);//将子区域q0复制到tmp中
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	imshow("象限变换后", dftImage1);
	Scalar s0 = sum(dftImage1);
	int m = dftImage1.rows;//获取DFT图像的宽和高
	int n = dftImage1.cols;
	//创建一个与DFT图像有相同尺寸和类型的矩阵，并将其初始化为全零。
	Mat ButterLow(m, n, CV_32F);
	float DO = 100;//滤波器参数
	double p = 5;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			double d = sqrt(pow((i - m / 2), 2)) + pow((j - n / 2), 2);
			//计算一个与距离中心点的距离相关的值，作为一个权重，创建一个带通滤波器
			float mm = d / 100;
			double nn = 1 + pow(mm, 2 * p);
			ButterLow.at<float>(i, j) = 1 / nn;
		}
	}

	multiply(dftImage1, ButterLow, ButterLow);//在频域中应用滤波器
	int cx1 = ButterLow.cols / 2;
	int cy1 = ButterLow.rows / 2;

	Mat r0(ButterLow, Rect(0, 0, cx1, cy1));
	Mat r1(ButterLow, Rect(cx1, 0, cx1, cy1));
	Mat r2(ButterLow, Rect(0, cy1, cx1, cy1));
	Mat r3(ButterLow, Rect(cx1, cy1, cx1, cy1));
	//创建滤波器的四个子区域

	Mat tmp1;
	r0.copyTo(tmp1);
	r3.copyTo(r0);
	tmp1.copyTo(r3);
	r1.copyTo(tmp1);
	r2.copyTo(r1);
	tmp1.copyTo(r2);

	imshow("巴特沃斯低通滤波", ButterLow);

	//傅里叶变换
	idft(ButterLow, inverseDFT, DFT_SCALE | DFT_REAL_OUTPUT);//对滤波后的图像进行IDFT
	inverseDFT.convertTo(inverseDFTconverted, CV_8U);//将 数据转换为8位无符号整数类型
	Scalar s1 = sum(ButterLow);
	imshow("Output", inverseDFTconverted);
}


void CzhangkeruiimageView::OnButterworthHi()
{
	// TODO: 在此添加命令处理程序代码
	Mat img = imread("E:\\新建文件夹\\大二上\\计算机视觉基础\\实习\\pic\\屏幕截图 2024-11-18 183236.jpg", 0);
	if (img.empty())
	{
		std::cout << "无法加载图像" << std::endl;
	}
	imshow("初始图像", img);
	Mat dftInput1, dftImage1, inverseDFT, inverseDFTconverted;
	img.convertTo(dftInput1, CV_32F);
	dft(dftInput1, dftImage1);
	imshow("傅里叶变换", dftImage1);
	int cx = dftImage1.cols / 2;
	int cy = dftImage1.rows / 2;
	Mat q0(dftImage1, Rect(0, 0, cx, cy));//top-left
	Mat q1(dftImage1, Rect(cx, 0, cx, cy));//top-right
	Mat q2(dftImage1, Rect(0, cy, cx, cy));//bottom-left
	Mat q3(dftImage1, Rect(cx, cy, cx, cy));//bottom-right
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	imshow("象限变换后", dftImage1);
	Scalar s0 = sum(dftImage1);
	int m = dftImage1.rows;
	int n = dftImage1.cols;
	Mat ButterHigh(m, n, CV_32F);
	float DO = 100;
	double p = 5;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			double d = sqrt(pow((i - m / 2), 2)) + pow((j - n / 2), 2);
			float mm = DO / d;//主要区别
			double nn = 1 + pow(mm, 2 * p);
			ButterHigh.at<float>(i, j) = 1 / nn;
		}
	}

	multiply(dftImage1, ButterHigh, ButterHigh);
	int cx1 = ButterHigh.cols / 2;
	int cy1 = ButterHigh.rows / 2;

	Mat r0(ButterHigh, Rect(0, 0, cx1, cy1));
	Mat r1(ButterHigh, Rect(cx1, 0, cx1, cy1));
	Mat r2(ButterHigh, Rect(0, cy1, cx1, cy1));
	Mat r3(ButterHigh, Rect(cx1, cy1, cx1, cy1));

	Mat tmp1;
	r0.copyTo(tmp1);
	r3.copyTo(r0);
	tmp1.copyTo(r3);

	r1.copyTo(tmp1);
	r2.copyTo(r1);
	tmp1.copyTo(r2);

	imshow("巴特沃斯低通滤波", ButterHigh);

	//傅里叶变换
	idft(ButterHigh, inverseDFT, DFT_SCALE | DFT_REAL_OUTPUT);
	inverseDFT.convertTo(inverseDFTconverted, CV_8U);
	Scalar s1 = sum(ButterHigh);
	imshow("Output", inverseDFTconverted);
}


void CzhangkeruiimageView::OnQyzz()
{
	// TODO: 在此添加命令处理程序代码
	CzhangkeruiimageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!(pDoc->image))return;
	
	pDoc->m_pBits = pDoc->image->data;
	unsigned char* pBits = pDoc->m_pBits;
	if (!pBits)return;
		
	//pDoc->m_nColorBits = pDoc->image->channels() * pDoc->image->depth();
	//图像的总颜色位数。计算方法是将图像的通道数乘以每个通道的位深度

	std::vector<int*>pData;//用于储存指向图像数据的指针
	int nWidth = pDoc->imageWidth;
	int nHeight = pDoc->imageHeight;
	int nColorBits = pDoc->m_nColorBits;
	if (nColorBits == 8)
	{
		//弹出消息框
		MessageBox(TEXT("目前只支持真彩色图像的区域增长"), TEXT("系统提示"), MB_ICONINFORMATION | MB_OK);
		return;
	}
	else if (nColorBits == 24)
	{
		int linebyte = pDoc->image->step;

		pData.resize(3);//把pData的大小设置为3
		for (int k = 0; k < 3; k++)
		{
			pData[k] = new int[nWidth * nHeight];//为pData中每个通道分配内存
		}
		for (int i = 0; i < nHeight; i++)
		{
			int lTotalL = i * linebyte;
			for (int j = 0; j < nWidth; j++)
			{
				int lTotalD = lTotalL + j * 3;
				int idx = i * nWidth + j;
				pData[0][idx] = pBits[lTotalD];
				pData[1][idx] = pBits[lTotalD + 1];
				pData[2][idx] = pBits[lTotalD + 2];
				//存储将图像数据的每个通道
			}
		}
	}
	ImageRegions regions;//用于存储分割出的各各区域
	if (RegionGrowingSegmentation(pData, nWidth, nHeight, 200, regions))
	{
		CString str;
		str.Format(_T("分割出%d个区域"), regions.size());
		MessageBox(str, TEXT("系统提示"), MB_ICONINFORMATION | MB_OK);
		pDoc->SetModifiedFlag(TRUE);//设置脏标记
		pDoc->UpdateAllViews(NULL);//更新视图

	}
	else
	{
		MessageBox(TEXT("分割失败"), TEXT("系统提示"), MB_ICONINFORMATION | MB_OK);
		return;
	}
	//随机赋值颜色
	srand(time(0));//用time函数的返回值作为种子来初始化随机数生成器
	int b, g, r;//存储随机选择的颜色值
	int linebyte = pDoc->image->step;//获取图像的行字节数
	for (ImageRegions::iterator it = regions.begin(); it != regions.end(); ++it)
	{
		b = rand() % 256;
		g = rand() % 256;
		r = rand() % 256;
		for (int i = 0; i < it->m_points.size(); ++i)
		{
			int x = (*it).m_points.at(i).m_x;
			int y = (*it).m_points.at(i).m_y;
			int idx = y * linebyte + x * 3;

			pDoc->m_pBits[idx] = b;
			pDoc->m_pBits[idx + 1] = g;
			pDoc->m_pBits[idx + 2] = r;
			//遍历分割出的每个区域，并为区域内的像素点随机选择颜色
		}
	}
	EndWaitCursor();
}


void CzhangkeruiimageView::OnOstu()
{
	// TODO: 在此添加命令处理程序代码
	CzhangkeruiimageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	//pDoc->m_pBits = pDoc->image->data;

	unsigned char* pBits = pDoc->m_pBits;//数据指针赋值给成员变量m-pBits
	//pDoc->m_nColorBits = pDoc->image->channels() * pDoc->image->depth();
	if (!pBits)return;
	int nWidth = pDoc->imageWidth;
	int nHeight = pDoc->imageHeight;
	int nColorBits = pDoc->m_nColorBits;
	//获取图像的宽和高和颜色位数
	if (nColorBits !=24)
	{
		MessageBox(TEXT("目前只支持256位图像的Fourier变换"), TEXT("系统提示"), MB_ICONINFORMATION | MB_OK);
		return;
	}
	else 
	{
		unsigned char nValue = 0;//存储灰度值
		long lTotal = 0;//总像素数
		long lTotalD = 0;//当前像素在图像中的位置
		long lTotalL = 0;//当前行的起始位置
		int dMax = 0;//频率最大的灰度级的频率

		for (int k = 0; k < 256; k++)
		{
			m_lValue[k] = 0;//灰度为k像素的频数
			m_dValue[k] = 0.0;//灰度为k像素的概率
		}
		//灰度频数
		for (int i = 0; i < nHeight; i++)
		{
			lTotalL = nWidth * i*3;
			for (int j = 0; j < nWidth; j++)
			{
				lTotalD = lTotalL + j*3;
				//nValue = *(pBits + lTotalD);
				
				unsigned char blue = pBits[lTotalD];//获取当前像素的蓝色分量
				unsigned char green = pBits[lTotalD + 1];
				unsigned char red = pBits[lTotalD +2];
				nValue= static_cast<unsigned char>(0.299 * red + 0.587 * green + 0.114 * blue);
				//将RGB值转换成灰度值
				m_lValue[nValue] = m_lValue[nValue] + 1;//统计灰度为nValue的像素频数
				
			}
		}
		//灰度直方图
		lTotal = nWidth * nHeight;//计算总像素数
		for (int k = 0; k < 256; k++)
		{
			m_dValue[k] = 1.0 * m_lValue[k] / lTotal;//计算每个灰度级的概率
			if (dMax < m_dValue[k])dMax = m_dValue[k];//找到最大灰度级概率
		}
		int sp = 0;
		int mp = 255;
		//概率不为0的最小灰度等级
		for (int i = 0; i < 255; ++i)
		{
			if (m_lValue[i] != 0)
			{
				sp = i;
				break;
			}
		}
		//概率不为0的最大灰度等级
		for (int i = 255; i > 0; --i)
		{
			if (m_lValue[i] != 0)
			{
				mp = i;
				break;
			}
		}
		
		double g[256] = { 0.0 };//存储类间方差
		double gmax = 0;//最大类间方差
		unsigned char th = 0;//二值化阈值
		//Ostu算法
		for (int i = sp; i < mp + 1; i++)
		{
			int ip1 = 0, ip2 = 0, is1 = 0, is2 = 0; //存储前景（1）和背景（2）的灰度总和(ip)及像素个数(is)
			double w1 = 0, w2 = 0;//前景和背景像素的比例
			double mean1 = 0, mean2 = 0;//前景和背景的平均灰度
			for (int j = sp; j < mp + 1; j++)
			{
				if (j <= i)//背景
				{
					is2 += m_lValue[j];//像素个数
					ip2 = ip2 + j*m_lValue[j];//灰度总和
				}
				else
				{
					is1 += m_lValue[j];
					ip1 = ip1 + j*m_lValue[j];
				}
			}
			w1 = (double)is1 / (is1 + is2);//前景像素比例
			w2 = (double)is2 / (is1 + is2);
			mean1 = (double)ip1 / is1;//前景像素点的平均灰度
			mean2 = (double)ip2 / is2;
			g[i] = (w2 * w1) * (pow(mean1 - mean2, 2));//类间方差
			if (g[i] > gmax)
			{
				gmax = g[i];
				th = i;//阈值
			}
		}
		CString str;
		str.Format(_T("阈值为 %d"), th);
		MessageBox(str, TEXT("系统提示"), MB_ICONINFORMATION | MB_OK);
		//二值化
		for (int i = 0; i < nHeight; i++)
		{
			lTotalL = nWidth * i*3;
			for (int j = 0; j < nWidth; j++)
			{
				lTotalD = lTotalL + j*3;
				unsigned char blue = pBits[lTotalD];
				unsigned char green = pBits[lTotalD + 1];
				unsigned char red = pBits[lTotalD + 2];
				//RGB转化为灰度值
				unsigned char nValue = static_cast<unsigned char>(0.299 * red + 0.587 * green + 0.114 * blue);
				//二值化，大于阈值的为白色
				pBits[lTotalD] = pBits[lTotalD + 1] = pBits[lTotalD + 2] = nValue > th ? 255 : 0;
				//*(pBits + lTotalD) = *(pBits + lTotalD) > th ? 255 : 0;
			}
		}
		Invalidate();
		//设置脏标记
		pDoc->SetModifiedFlag(TRUE);
		//更新视图
		pDoc->UpdateAllViews(NULL);
		//恢复光标
		EndWaitCursor();
	}
}


void CzhangkeruiimageView::OnOpencv1()
{
	// TODO: 在此添加命令处理程序代码
	int nRetCode = 0;
	CvPoint center;
	double scale = -3;
	CzhangkeruiimageDoc* pDoc = GetDocument();
	if (pDoc->m_nOpenMode != 2)return;
	Mat* image = pDoc->image;
	if (!image->data) { return; }
	//Mat img = cvarrToMat(image);
	cv::namedWindow("test0", 1);
	cv::imshow("test0", *image);
	center = cvPoint(image->cols / 2, image->rows / 2);
	for (int i = 0; i < image->rows; i++)
	{
		for (int j = 0; j < image->cols; j++)
		{
			double dx = (double)(j - center.x) / center.x;
			double dy = (double)(i - center.y) / center.y;
			double weight = exp((dx * dx + dy * dy) * scale);
			//uchar* ptr = &CV_IMAGE_ELEM(image, uchar, i, j * 3);
			uchar* ptr = &(((uchar*)((image)->data + (image)->step * (i)))[(j * 3)]);
			//uchar* ptr = (*image).ptr<uchar>(i) + j * image->channels();
			ptr[0] = cvRound(ptr[0] * weight);
			ptr[1] = cvRound(ptr[1] * weight);
			ptr[2] = cvRound(ptr[2] * weight);
		}
	}
	//cv::Mat newimg = cv::cvarrToMat(image, true);
	//imwrite("E:\\新建文件夹\\大二上\\计算机视觉基础\\zhangkeruiimage\\outimage.jpg", newimg);
	namedWindow("test", 1);
	cv::imshow("test", *image);
	cv::waitKey();
}

//利用Opencv中的边缘提取算法以及hough变换来进行图像中的圆以及直线的提取

void CzhangkeruiimageView::Onbianyuan()
{
	// TODO: 在此添加命令处理程序代码
	Mat image = imread("E:\\新建文件夹\\大二上\\计算机视觉基础\\实习\\pic\\pic\\image002.jpg");
	namedWindow("input",1);
	imshow("input", image);
	cv::Mat gimage;//存储灰度图像
	cvtColor(image, gimage,COLOR_BGR2GRAY);//将真彩色图像转化为灰度图
	blur(gimage, gimage, Size(3, 3));//减少图像噪声
	namedWindow("gimage", 1);
	imshow("gimage", gimage);
	std::vector<Vec3f>circles;//circles向量用于存储圆检测的结果
	int hough_value = 30;
	HoughCircles(gimage, circles,HOUGH_GRADIENT, 1, 10, 110, hough_value, 10, 100);//在灰度图像中检测圆
	int num = 0;//圆的数量
	Mat houghcircle = image.clone();
	for (int i = 0; i < circles.size(); i++)
	{
		if (circles[i][2] > 1.0f && circles[i][2] < 100.0f)//圆的半径是否在范围内
		{
			num++;
		}
		circle(houghcircle, Point(circles[i][0], circles[i][1]), circles[i][2], Scalar(0, 0, 255), 2);//绘制出检测到的圆

	}
	imshow("houghcircle", houghcircle);

	std::vector<Vec4i> hierarchy;//存储轮廓点
	Mat result;//canny边缘检测后的结果
	Canny(gimage, result, 150, 70);
	namedWindow("canny边缘检测后的图像");
	imshow("canny边缘检测后的图像", result);
	//findContours(result, contour_vec, hierarchy, RETR_CCOMP, CHAIN_APPROX_TC89_L1);
	HoughLinesP(result, hierarchy, 1, CV_PI / 180, 50, 0, 0);//检测图像中的直线
	Mat houghlines = image.clone();
	double length = 0;//直线长度
	for (size_t i = 0; i < hierarchy.size(); i++)
	{
		//drawContours(houghlines, contour_vec, i, Scalar(0,0,255), 2);
		//double arcLength = cv::arcLength(hierarchy[i], true);
		Vec4i l = hierarchy[i];//第i条直线的参数
		line(houghlines, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3);//绘制检测到的直线
		length += cv::norm(cv::Point(l[0], l[1]) - cv::Point(l[2], l[3]));//计算长度
	}
	imshow("houghlines", houghlines);
	cv::waitKey(0);
	CString str;
	str.Format(_T("有 %d 个硬币,边缘长度为%f"), num,length);
	AfxMessageBox(str, MB_ICONINFORMATION | MB_OK);
	
}
