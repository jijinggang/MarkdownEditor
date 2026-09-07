
// LeftView.cpp : CLeftView 类的实现
//

#include "stdafx.h"
#include "MarkdownEditor.h"

#include "MarkdownEditorDoc.h"
#include "LeftView.h"
#include "./Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CRichEditView)

BEGIN_MESSAGE_MAP(CLeftView, CRichEditView)
	ON_CONTROL_REFLECT(EN_CHANGE, &CLeftView::OnEnChange)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CLeftView 构造/析构

CLeftView::CLeftView()
{
	// TODO: 在此处添加构造代码
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bCreate = CRichEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping
	return bCreate;
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	//return CRichEditView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();
}


// CLeftView 诊断

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CMarkdownEditorDoc* CLeftView::GetDocument() // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMarkdownEditorDoc)));
	return (CMarkdownEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView 消息处理程序

void CLeftView::OnEnChange()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CRichEditView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。 

	CString str;
	this->GetWindowText(str);
	string strText = Util::Utf16ToUtf8(str.GetString(), str.GetLength());
	// Ignore EN_CHANGE caused by programmatically setting the control text
	// (document load / new document). A real user edit always differs
	// from the document text, so it is never swallowed.
	if (strText == GetDocument()->getText())
		return;
	bool bMoveToEnd = false;
	long iStart, iEnd;
	this->GetRichEditCtrl().GetSel(iStart, iEnd);
	if(iEnd >= str.GetLength() - 1){
		bMoveToEnd = true;
	}
	GetDocument()->UpdateText(strText, this, bMoveToEnd);
	GetRichEditCtrl().SetFocus();
}


void CLeftView::OnUpdate(CView* pSender, LPARAM lHint, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pSender == this || !(lHint & LPARAM_Update))
		return;
	if (pSender == NULL) {
		CStringW strText = Util::Utf8ToUtf16(GetDocument()->getText().c_str()).c_str();
		GetRichEditCtrl().SetWindowText(strText);
	}
}

int CLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CRichEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	_font.CreatePointFont(110,_T("Consolas")); 
	SetFont(&_font); 
	//this->SetTabStops(16);
	//this->ShowScrollBar(SB_HORZ,FALSE);
	return 0;
}
