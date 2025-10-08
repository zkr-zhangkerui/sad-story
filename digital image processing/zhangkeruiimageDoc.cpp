
// zhangkeruiimageDoc.cpp: CzhangkeruiimageDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "zhangkeruiimage.h"
#endif

#include "zhangkeruiimageDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CzhangkeruiimageDoc

IMPLEMENT_DYNCREATE(CzhangkeruiimageDoc, CDocument)

BEGIN_MESSAGE_MAP(CzhangkeruiimageDoc, CDocument)
END_MESSAGE_MAP()


// CzhangkeruiimageDoc 构造/析构

CzhangkeruiimageDoc::CzhangkeruiimageDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
	image = new cv::Mat();
	m_pBits = NULL;
	lpbmi = NULL;
	imageWidth = 768;
	imageHeight = 576;
	m_nOpenMode = 0;
	//m_nColorBits = 8;
	m_nColorBits = 24;
	//image = NULL;

}

CzhangkeruiimageDoc::~CzhangkeruiimageDoc()
{
}

BOOL CzhangkeruiimageDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CzhangkeruiimageDoc 序列化

void CzhangkeruiimageDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CzhangkeruiimageDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CzhangkeruiimageDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CzhangkeruiimageDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CzhangkeruiimageDoc 诊断

#ifdef _DEBUG
void CzhangkeruiimageDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CzhangkeruiimageDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CzhangkeruiimageDoc 命令


BOOL CzhangkeruiimageDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// TODO:  在此添加您专用的创建代码
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CString str = lpszPathName;
	str.MakeLower();//将文件路径中的所有字符转换成小写
	if (str.Find(_TEXT(".bmp")) != -1)//检查是否包含bmp
	{
		m_nOpenMode = 1;
		if (!ReadBMP(lpszPathName))return FALSE;
	}

	if (str.Find(_TEXT(".jpg")) != -1)//检查是否是jpg图像
	{
		m_nOpenMode = 2;

		USES_CONVERSION;
		*image = cv::imread(W2A(str));
		//W2A函数将str转换为ANSI字符，用imread函数读取图像数据
		if (!image->data)
		{
			MessageBox(NULL, lpszPathName, _T("图像打开失败"), MB_OK);
			return FALSE;
		}
		lpbmi = (LPBITMAPINFO)new char[sizeof(BITMAPINFO) + 4 * (1 << 8)];

		int ndept = image->depth();
		int nchan = image->channels();
		if (ndept == CV_8U) ndept = 8;
		//提取位深度和通道数
		lpbmi->bmiHeader.biBitCount = ndept*nchan;
		lpbmi->bmiHeader.biClrUsed = 0;
		lpbmi->bmiHeader.biHeight = image->rows;
		lpbmi->bmiHeader.biWidth = image->cols;
		lpbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		lpbmi->bmiHeader.biSizeImage = image->cols * image->rows * nchan;
		lpbmi->bmiHeader.biClrImportant = 0;
		lpbmi->bmiHeader.biCompression = 0;
		lpbmi->bmiHeader.biPlanes = 1;
		//设置结构体BITMAPINFOHEADER中的高度，宽度等数据
		imageWidth = lpbmi->bmiHeader.biWidth;
		imageHeight = lpbmi->bmiHeader.biHeight;
		for (int i = 0; i < (1 << 8); i++)
		{
			RGBQUAD& o = lpbmi->bmiColors[i];
			o.rgbRed = i;
			o.rgbBlue = i;
			o.rgbGreen = i;
			o.rgbReserved = 0;
			//初始化颜色表
		}

	}
	return TRUE;
}

BOOL CzhangkeruiimageDoc::ReadBMP(LPCTSTR lpszPathName)//函数的声明
{
	long lTotal = 0;//图像的总字节数
	CFile file;
	file.Open(lpszPathName, CFile::modeRead);//以只读模式打开指定的BMP文件
	file.Read(&bmpFH, sizeof(BITMAPFILEHEADER));//从文件中读取BITMAPFILEHEADER结构体的大小，并存储到bmFH中
	lpbmi = (LPBITMAPINFO)new char[sizeof(BITMAPINFO) + 4 * (1 << 8)];
	//分配内存以存储 BITMAPINFO 结构体和颜色表，如果图像的位深度为8位，则颜色表的大小为 4 * (1 << 8) 字节。
	file.Read(lpbmi, sizeof(BITMAPINFOHEADER));
	//从文件中读取 BITMAPINFOHEADER 结构体的大小，并存储在 lpbmi 变量中。
	m_nColorBits = lpbmi->bmiHeader.biBitCount;//位深度
	imageHeight = lpbmi->bmiHeader.biHeight;//位高度
	imageWidth = lpbmi->bmiHeader.biWidth;//位宽度

	if (m_nColorBits == 8)
	{
		lTotal = imageWidth * imageHeight;//计算总字节数
		file.Read(&(lpbmi->bmiColors[0]), 256 * 4);
	}
	else if (m_nColorBits == 24)
	{
		lTotal = imageWidth * imageHeight*3;

	}
	else
	{
		file.Close();
		return FALSE;
	}
	m_pBits = new unsigned char[lTotal];
	file.Read(m_pBits, lTotal);
	//分配内存以存储图像数据，并将读取的图像数据存储在 m_pBits中
	file.Close();
	return TRUE;
}
BOOL CzhangkeruiimageDoc::SaveBMP(LPCTSTR lpszPathName)const
{

	long lTotal = 0;
	CFile file;
	file.Open(lpszPathName, CFile::modeCreate | CFile::modeReadWrite);
	//Open函数以创建和读写模式打开指定的BMP文件路径
	file.Write(&bmpFH, sizeof(BITMAPFILEHEADER));
	file.Write(lpbmi, sizeof(BITMAPINFOHEADER));
	//写入文件
	if (m_nColorBits == 8)
	{
		lTotal = imageWidth * imageHeight;//计算总字节数
		file.Write(&(lpbmi->bmiColors[0]), 256 * 4);
		//写入颜色表
	}
	else if (m_nColorBits == 24)
	{
		lTotal = imageWidth * imageHeight * 3;
	}
	else
	{
		file.Close();
		return FALSE;
	}
	file.Write(m_pBits, lTotal);
	file.Close();
	return TRUE;
}

//图像保存
BOOL CzhangkeruiimageDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_nOpenMode == 1 && m_pBits != NULL)//BMP图像的保存
	{
		SaveBMP(lpszPathName);
		return TRUE;
	}
	CString str = lpszPathName;
	if (m_nOpenMode == 2 && image->data != NULL)//jpg图像用imwrite函数保存
	{
		//cvSaveimage(lpszPathName,image);
		//cv::Mat newimg = cv::cvarrToMat(image, true);
		USES_CONVERSION;
		cv:imwrite(W2A(str), *image);
		return TRUE;
	}
	
	return CDocument::OnSaveDocument(lpszPathName);
}
