
// MarkdownEditorDoc.cpp : CMarkdownEditorDoc 类的实现
//

#include "stdafx.h"
#include "./Util.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MarkdownEditor.h"
#endif

#include "MarkdownEditorDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMarkdownEditorDoc

IMPLEMENT_DYNCREATE(CMarkdownEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMarkdownEditorDoc, CDocument)

END_MESSAGE_MAP()


// CMarkdownEditorDoc 构造/析构

CMarkdownEditorDoc::CMarkdownEditorDoc()
{
	// TODO: 在此添加一次性构造代码
	resetData();
}

CMarkdownEditorDoc::~CMarkdownEditorDoc()
{
}

BOOL CMarkdownEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	resetData();
	this->UpdateAllViews(NULL);
	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	return TRUE;
}




// CMarkdownEditorDoc 序列化

void CMarkdownEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar.WriteString(Util::ANSIToUTF8(_strText.c_str()).c_str());
		// TODO: 在此添加存储代码
	}
	else
	{
		CString str;
		const int BUF_SIZE = 64*1024;
		unsigned char buf[BUF_SIZE + 1];
		while(true){
			UINT uRead = ar.Read(buf, BUF_SIZE);
			buf[uRead] = '\0';
			str += (const char*)buf;
			if(uRead < BUF_SIZE)
				break;
		}

		_strText = Util::UTF8ToANSI(str);
		this->UpdateAllViews(NULL);
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMarkdownEditorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CMarkdownEditorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMarkdownEditorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMarkdownEditorDoc 诊断

#ifdef _DEBUG
void CMarkdownEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMarkdownEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMarkdownEditorDoc 命令
//更新文本内容
void CMarkdownEditorDoc::UpdateText(const string& text, CView* pSender){
	_strText = text;
	this->UpdateAllViews(pSender);
	this->SetModifiedFlag();
}

void CMarkdownEditorDoc::resetData(void)
{
	_strText = "";
}
