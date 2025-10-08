
// zhangkeruiimageView.h: CzhangkeruiimageView 类的接口
//

#pragma once
#include"CHistogramDisplay.h"
#include"CSmoothDlg.h"
#include"CGeoTransDlg.h"
#include"MainFrm.h"

class CzhangkeruiimageView : public CScrollView
{
protected: // 仅从序列化创建
	CzhangkeruiimageView() noexcept;
	DECLARE_DYNCREATE(CzhangkeruiimageView)

	// 特性
public:
	CzhangkeruiimageDoc* GetDocument() const;
	double m_dZoom;

	// 操作
public:

	
	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 实现
public:
	virtual ~CzhangkeruiimageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
public://存放直方图数据的结构
	double m_dMaxR, m_dMaxG, m_dMaxB;
	long m_lValueR[256], m_lValueG[256], m_lValueB[2566];
	double m_dValueR[256], m_dValueG[256], m_dValueB[256];
	long m_lValue[256];
	double m_dValue[256];
	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnOpencv1(CCmdUI* pCmdUI);
//	virtual HRESULT accDoDefaultAction(VARIANT varChild);
//	virtual HRESULT accDoDefaultAction(VARIANT varChild);
	//afx_msg void OnProcessLinetran();
	//afx_msg void On32779();
	afx_msg void OnProcessLinetran1();
	
	
	afx_msg void OnProcessHistogram();
	afx_msg void OnImageSmooth();
	afx_msg void OnGeoTrans();
	afx_msg void OnFreqFour();
	afx_msg void OnButterworthLow();
	afx_msg void OnButterworthHi();
	afx_msg void OnQyzz();
	afx_msg void OnOstu();
	afx_msg void OnOpencv1();
	afx_msg void Onbianyuan();
};

#ifndef _DEBUG  // zhangkeruiimageView.cpp 中的调试版本
inline CzhangkeruiimageDoc* CzhangkeruiimageView::GetDocument() const
   { return reinterpret_cast<CzhangkeruiimageDoc*>(m_pDocument); }
#endif

