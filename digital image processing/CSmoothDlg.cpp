// CSmoothDlg.cpp: 实现文件
//

#include "pch.h"
#include "afxdialogex.h"
#include "CSmoothDlg.h"
#include "resource.h"


// CSmoothDlg 对话框

IMPLEMENT_DYNAMIC(CSmoothDlg, CDialog)

CSmoothDlg::CSmoothDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_SMOOTH, pParent)
	, m_nSmooth1(1)
	, m_nScale(0.125)
	, m_nSmooth2(1)
	, m_nSmooth3(1)
	, m_nSmooth4(1)
	, m_nSmooth5(0)
	, m_nSmooth6(1)
	, m_nSmooth7(1)
	, m_nSmooth8(1)
	, m_nSmooth9(1)
{

}

CSmoothDlg::~CSmoothDlg()
{
}

void CSmoothDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nSmooth1);
	DDX_Text(pDX, IDC_EDIT10, m_nScale);
	DDX_Text(pDX, IDC_EDIT2, m_nSmooth2);
	DDX_Text(pDX, IDC_EDIT3, m_nSmooth3);
	DDX_Text(pDX, IDC_EDIT4, m_nSmooth4);
	DDX_Text(pDX, IDC_EDIT5, m_nSmooth5);
	DDX_Text(pDX, IDC_EDIT6, m_nSmooth6);
	DDX_Text(pDX, IDC_EDIT7, m_nSmooth7);
	DDX_Text(pDX, IDC_EDIT8, m_nSmooth8);
	DDX_Text(pDX, IDC_EDIT9, m_nSmooth9);
}


BEGIN_MESSAGE_MAP(CSmoothDlg, CDialog)
END_MESSAGE_MAP()


// CSmoothDlg 消息处理程序
