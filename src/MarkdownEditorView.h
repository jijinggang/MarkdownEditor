
// MarkdownEditorView.h : CMarkdownEditorView 类的接口
//

#pragma once
#include <string>
using namespace std;

class CMarkdownEditorView : public CHtmlView
{
private:
	bool _bFirstNavigate;
	string _strCSS;
	CComPtr<IDispatch> _spClickEvents; // cached onclick sink (ref-counted)
	void initCSS();
	void setClickEvents(IHTMLDocument2* htmlDocument2);
	string GetMdHtml(const string& str);
	void ResolveLocalImages(IHTMLDocument2* pHtmlDoc);
public:
	void UpdateMd(const string& strMd);

private:
	void NavigateHTML(const string& szHtml);
protected: // 仅从序列化创建
	CMarkdownEditorView();
	DECLARE_DYNCREATE(CMarkdownEditorView)

// 特性
public:
	CMarkdownEditorDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CMarkdownEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
//	virtual void OnNavigateComplete2(LPCTSTR strURL);
};

#ifndef _DEBUG  // MarkdownEditorView.cpp 中的调试版本
inline CMarkdownEditorDoc* CMarkdownEditorView::GetDocument() const
   { return reinterpret_cast<CMarkdownEditorDoc*>(m_pDocument); }
#endif

