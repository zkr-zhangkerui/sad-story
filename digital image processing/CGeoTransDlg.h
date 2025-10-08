#pragma once
#include "afxdialogex.h"


// CGeoTransDlg 对话框

class CGeoTransDlg : public CDialog
{
	DECLARE_DYNAMIC(CGeoTransDlg)

public:
	CGeoTransDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGeoTransDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GEOTRANS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_dRotation;
	double m_dScale;
};
