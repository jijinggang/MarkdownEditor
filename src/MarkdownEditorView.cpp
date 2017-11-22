
// MarkdownEditorView.cpp : CMarkdownEditorView 类的实现
//

#include "stdafx.h"
#include "Util.h"
#include <string>
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MarkdownEditor.h"
#endif

#include "MarkdownEditorDoc.h"
#include "MarkdownEditorView.h"
#include "MyClickEvents.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMarkdownEditorView

IMPLEMENT_DYNCREATE(CMarkdownEditorView, CHtmlView)

BEGIN_MESSAGE_MAP(CMarkdownEditorView, CHtmlView)
END_MESSAGE_MAP()

// CMarkdownEditorView 构造/析构

CMarkdownEditorView::CMarkdownEditorView()
{
	// TODO: 在此处添加构造代码
	_bFirstNavigate = true;
	initCSS();
}

CMarkdownEditorView::~CMarkdownEditorView()
{
}

BOOL CMarkdownEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CHtmlView::PreCreateWindow(cs);
}

void CMarkdownEditorView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

}


// CMarkdownEditorView 诊断

#ifdef _DEBUG
void CMarkdownEditorView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CMarkdownEditorView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CMarkdownEditorDoc* CMarkdownEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMarkdownEditorDoc)));
	return (CMarkdownEditorDoc*)m_pDocument;
}
#endif //_DEBUG

void setClickEvents(IHTMLDocument2* htmlDocument2, const char* dir) {

	static CMyClickEvents clickEvents;
	clickEvents.SetContext(htmlDocument2, dir);
	_variant_t clickDispatch;
	clickDispatch.vt = VT_DISPATCH;
	clickDispatch.pdispVal = &clickEvents;

	htmlDocument2->put_onclick(clickDispatch);
}

// CMarkdownEditorView 消息处理程序

void CMarkdownEditorView::NavigateHTML(const string& strHtml)
{
	IDispatch* pDoc = GetHtmlDocument();
	if(NULL == pDoc)
		return;
	// 取得文档中的IPersistStreamInit对象
    CComPtr<IHTMLDocument2> pHtmlDoc;
	HRESULT hr = pDoc ->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDoc);
    if (FAILED(hr))
        return;

	BSTR bstr = _com_util::ConvertStringToBSTR(strHtml.c_str());
	// Creates a new one-dimensional array
	SAFEARRAY *psaStrings = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	if (psaStrings == NULL) {
		pHtmlDoc->close();
		return;
	}
	VARIANT *param;
	hr = SafeArrayAccessData(psaStrings, (LPVOID*)&param);
	param->vt = VT_BSTR;
	param->bstrVal = bstr;
	hr = SafeArrayUnaccessData(psaStrings);
	hr = pHtmlDoc->write(psaStrings);

	setClickEvents(pHtmlDoc, GetDocument()->getFilePath().c_str());
	// SafeArrayDestroy calls SysFreeString for each BSTR
	if (psaStrings != NULL) {
		SafeArrayDestroy(psaStrings);
		pHtmlDoc->close();
	}
}

CComPtr<IHTMLTextContainer> getContainer(IDispatch* pDisp){
	if(NULL == pDisp)
		return NULL;
		CComPtr<IHTMLDocument2> pDocument2 = NULL; 
            if (S_OK == pDisp->QueryInterface(IID_IHTMLDocument2, (LPVOID*)&pDocument2)) 
            { 
                CComPtr<IHTMLElement> pElement = NULL; 
                if (S_OK == pDocument2->get_body(&pElement)) 
                { 
                    CComPtr<IHTMLTextContainer> pTextContainer = NULL; 
                    if (S_OK == pElement->QueryInterface(IID_IHTMLTextContainer, (LPVOID*)&pTextContainer)) 
                    { 
						return pTextContainer;
                    } 
                }                 
           } 
		return NULL;
}
float getScrollTop(IDispatch* pDisp)
{
    long scrollTop;
	CComPtr<IHTMLTextContainer> pTextContainer = getContainer(pDisp);
    if (pTextContainer &&  S_OK == pTextContainer->get_scrollTop(&scrollTop) ) 
    {
		long height;
		pTextContainer->get_scrollHeight(&height);
		return ((float)scrollTop)/height ;
    } 
	return 0.0;
}
void setScrollTop(IDispatch* pDisp, float scrollPercent)
{
	CComPtr<IHTMLTextContainer> pTextContainer = getContainer(pDisp);
    if (pTextContainer)
    {
		long top,height;
		pTextContainer->get_scrollTop(&top);
		pTextContainer->get_scrollHeight(&height);
		pTextContainer->put_scrollTop((long)(scrollPercent * height));
    } 
}
void CMarkdownEditorView::OnUpdate(CView* pSender, LPARAM /*lHint*/lParam, CObject* /*pHint*/)
{
	if(_bFirstNavigate){
		_bFirstNavigate = false;
		Navigate2(_T("about:blank"),NULL,NULL);
		//return;
	}
	if(!(lParam & LPARAM_Update))
		return;
	float scrollTop = 0;
	IDispatch* pDisp =GetHtmlDocument();
	
	if(pSender != NULL){
		scrollTop = getScrollTop(pDisp);
	}
	const string& str = GetDocument()->getText();	

	UpdateMd(str);
	if(lParam & LPARAM_MoveEnd){
		scrollTop = 1.0;
	}
	if(pSender != NULL){
		setScrollTop(pDisp,scrollTop);
	}



	// TODO: 在此添加专用代码和/或调用基类
}


void CMarkdownEditorView::initCSS(){
	string strUserCss = Util::GetExePath() + "user.css";
	if(PathFileExists(strUserCss.c_str())){
		_strCSS = Util::ReadStringFile(strUserCss.c_str());
	}else{
		Util::LoadStringRes(IDR_CSS,"CSS",_strCSS); 
	}
}

string&  replaceImgSrc(string& str, string path)
{
	if (path.size() == 0)
		return str;
	string old_value = "<img src=\"";
	string new_value = "<img src=\"" + path;
	for (string::size_type pos(0); pos != string::npos; pos += old_value.length())   {
		if ((pos = str.find(old_value, pos)) != string::npos){
			const char* start = str.c_str() + pos + old_value.length();
			if (strnicmp(start, "http://", 7) != 0 && strnicmp(start, "https://", 8) != 0)
				str.replace(pos, old_value.length(), new_value);
		}
		else   
			break;
	}
	return   str;
}
const string HTML_TMPL = "<html><head><style type=\"text/css\">{{0}}</style></head><body>{{1}}</body></html>";

string CMarkdownEditorView::GetMdHtml(const string& str){
	string strHtml = HTML_TMPL;
	Util::ReplaceAllStr(strHtml,"{{0}}", _strCSS);
	string md = Util::Text2Md(str);
	md = replaceImgSrc(md, GetDocument()->getFilePath());
	Util::ReplaceAllStr(strHtml, "{{1}}", md);
	return strHtml;
}

void CMarkdownEditorView::UpdateMd(const string& strMd)
{
	string strHtml = GetMdHtml(strMd);
	NavigateHTML(strHtml);
}


