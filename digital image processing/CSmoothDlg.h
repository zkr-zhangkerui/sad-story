#pragma once
#include "afxdialogex.h"


// CSmoothDlg 对话框

class CSmoothDlg : public CDialog
{
	DECLARE_DYNAMIC(CSmoothDlg)

public:
	CSmoothDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSmoothDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SMOOTH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nSmooth1;
	double m_nScale;
	int m_nSmooth2;
	int m_nSmooth3;
	int m_nSmooth4;
	int m_nSmooth5;
	int m_nSmooth6;
	int m_nSmooth7;
	int m_nSmooth8;
	int m_nSmooth9;
};
