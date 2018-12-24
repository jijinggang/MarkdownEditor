
// LeftView.h : CLeftView 类的接口
//


#pragma once

class CMarkdownEditorDoc;

class CLeftView : public CRichEditView
{
private:
	CFont _font;
protected: // 仅从序列化创建
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// 特性
public:
	CMarkdownEditorDoc* GetDocument();

// 操作
public:

// 重写
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChange();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // LeftView.cpp 中的调试版本
inline CMarkdownEditorDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CMarkdownEditorDoc*>(m_pDocument); }
#endif

