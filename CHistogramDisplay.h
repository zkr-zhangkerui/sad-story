#pragma once
#include "afxdialogex.h"


// CHistogramDisplay 对话框

class CHistogramDisplay : public CDialogEx
{
	DECLARE_DYNAMIC(CHistogramDisplay)

public:
	CHistogramDisplay(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CHistogramDisplay();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HISTOGRAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	double m_dValueR[256];
	double m_dValueG[256];
	double m_dValueB[256];
	double m_dMaxR, m_dMaxG, m_dMaxB;
	int m_nColorbits;

	void DrawGraph(CDC* pDC);
	void SetData(double* ValueR, double* ValueG, double* ValueB, double dMaxR, double dMaxG, double dMaxB, int Colorbits);
	afx_msg void OnPaint();
};
