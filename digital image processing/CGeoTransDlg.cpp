// CGeoTransDlg.cpp: 实现文件
//

#include "pch.h"
#include "afxdialogex.h"
#include "CGeoTransDlg.h"
#include"resource.h"


// CGeoTransDlg 对话框

IMPLEMENT_DYNAMIC(CGeoTransDlg, CDialog)

CGeoTransDlg::CGeoTransDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_GEOTRANS, pParent)
	, m_dRotation(60)
	, m_dScale(0.25)
{

}

CGeoTransDlg::~CGeoTransDlg()
{
}

void CGeoTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROTATION, m_dRotation);
	DDX_Text(pDX, IDC_EDIT_SCAIE, m_dScale);
}


BEGIN_MESSAGE_MAP(CGeoTransDlg, CDialog)
END_MESSAGE_MAP()


// CGeoTransDlg 消息处理程序
