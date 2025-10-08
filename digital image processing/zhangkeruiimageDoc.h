
// zhangkeruiimageDoc.h: CzhangkeruiimageDoc 类的接口
//


#pragma once
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc/imgproc_c.h>
//#include <opencv2/core/base.hpp>

class CzhangkeruiimageDoc : public CDocument
{
protected: // 仅从序列化创建
	CzhangkeruiimageDoc() noexcept;
	DECLARE_DYNCREATE(CzhangkeruiimageDoc)

// 特性
public:

	//IplImage* image;//OpenVC
	cv::Mat* image;
	BITMAPINFO*lpbmi;
	unsigned char*m_pBits;
	BITMAPFILEHEADER bmpFH;
	int imageWidth;
	int imageHeight;
	int m_nColorBits;//8 24
	int m_nOpenMode;//raw=0 bmp=1 open cv=2 
	//double m_dZoom;

// 操作
public:
	void ReadRAW(LPCTSTR lpszPathName, int nWidth, int nHeight);
	BOOL ReadBMP(LPCTSTR lpszPathName);
	BOOL SaveBMP(LPCTSTR lpszPathName)const;
// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CzhangkeruiimageDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
