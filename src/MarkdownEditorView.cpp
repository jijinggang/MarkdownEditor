
// MarkdownEditorView.cpp : CMarkdownEditorView ���ʵ��
//

#include "stdafx.h"
#include "Util.h"
#include <string>
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "MarkdownEditor.h"
#endif

#include "MarkdownEditorDoc.h"
#include "MarkdownEditorView.h"
#include "MyClickEvents.h"
#include "LeftView.h"
#include "MdAnchors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMarkdownEditorView

IMPLEMENT_DYNCREATE(CMarkdownEditorView, CHtmlView)

BEGIN_MESSAGE_MAP(CMarkdownEditorView, CHtmlView)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CMarkdownEditorView ����/����

CMarkdownEditorView::CMarkdownEditorView()
{
	// TODO: �ڴ˴����ӹ������
	_bFirstNavigate = true;
	_lastTop = 0;
	_guardTick = 0;
	initCSS();
}

CMarkdownEditorView::~CMarkdownEditorView()
{
}

void CMarkdownEditorView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();
	SetTimer(IDT_SCROLLSYNC, 120, NULL);
}

void CMarkdownEditorView::OnDestroy()
{
	KillTimer(IDT_SCROLLSYNC);
	CHtmlView::OnDestroy();
}

BOOL CMarkdownEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CHtmlView::PreCreateWindow(cs);
}


// CMarkdownEditorView ���

#ifdef _DEBUG
void CMarkdownEditorView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CMarkdownEditorView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CMarkdownEditorDoc* CMarkdownEditorView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMarkdownEditorDoc)));
	return (CMarkdownEditorDoc*)m_pDocument;
}
#endif //_DEBUG

// The sink is heap-allocated once, cached for the view lifetime and
// ref-counted: put_onclick AddRefs its own copy, and SetContext swaps the
// (AddRef'd) document each time document.write creates a new one.
void CMarkdownEditorView::setClickEvents(IHTMLDocument2* htmlDocument2)
{
	if (!_spClickEvents) {
		// CComPtr::operator=(T*) takes ownership (no extra AddRef): exactly
		// one reference for the view. Do NOT route it through a temporary
		// CComPtr - the temporary's destructor would release the same
		// reference and destroy the sink while _spClickEvents still points
		// at it.
		_spClickEvents = new CMyClickEvents();
	}
	static_cast<CMyClickEvents*>((IDispatch*)_spClickEvents)
		->SetContext(htmlDocument2, GetDocument()->getFilePath().c_str());
	CComVariant var((IDispatch*)_spClickEvents);
	htmlDocument2->put_onclick(var);
}

// CMarkdownEditorView ��Ϣ��������

void CMarkdownEditorView::NavigateHTML(const string& strHtml)
{
	CComPtr<IDispatch> pDoc = GetHtmlDocument(); // AddRef'd; must not leak
	if(NULL == pDoc)
		return;
	// ȡ���ĵ��е�IPersistStreamInit����
    CComPtr<IHTMLDocument2> pHtmlDoc;
	HRESULT hr = pDoc ->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDoc);
    if (FAILED(hr) || !pHtmlDoc)
        return;

	const wstring wstrHtml = Util::Utf8ToUtf16(strHtml.c_str(), (int)strHtml.size());
	CComBSTR bstr((int)wstrHtml.size(), wstrHtml.c_str());
	// Creates a new one-dimensional array
	SAFEARRAY *psaStrings = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	if (psaStrings == NULL) {
		pHtmlDoc->close();
		return;
	}
	VARIANT *param = NULL;
	hr = SafeArrayAccessData(psaStrings, (LPVOID*)&param);
	if (SUCCEEDED(hr) && param != NULL) {
		param->vt = VT_BSTR;
		param->bstrVal = bstr.Detach(); // SafeArrayDestroy frees it
	}
	hr = SafeArrayUnaccessData(psaStrings);
	if (SUCCEEDED(hr))
		hr = pHtmlDoc->write(psaStrings);
	if (FAILED(hr))
		TRACE("CMarkdownEditorView::NavigateHTML: IHTMLDocument2::write failed\n");

	setClickEvents(pHtmlDoc);
	// SafeArrayDestroy calls SysFreeString for each BSTR
	if (psaStrings != NULL) {
		SafeArrayDestroy(psaStrings);
		pHtmlDoc->close();
	}
	ResolveLocalImages(pHtmlDoc);
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
    long scrollTop = 0;
	CComPtr<IHTMLTextContainer> pTextContainer = getContainer(pDisp);
    if (pTextContainer &&  S_OK == pTextContainer->get_scrollTop(&scrollTop) )
    {
		long height = 0;
		if (S_OK == pTextContainer->get_scrollHeight(&height) && height > 0)
			return ((float)scrollTop)/height ;
    }
	return 0.0;
}
void setScrollTop(IDispatch* pDisp, float scrollPercent)
{
	CComPtr<IHTMLTextContainer> pTextContainer = getContainer(pDisp);
    if (pTextContainer)
    {
		long height = 0;
		pTextContainer->get_scrollHeight(&height);
		if (height > 0) {
			long pos = (long)(scrollPercent * height);
			if (pos < 0)
				pos = 0;
			if (pos > height)
				pos = height;
			pTextContainer->put_scrollTop(pos);
		}
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
	CComPtr<IDispatch> pDisp = GetHtmlDocument(); // AddRef'd; must not leak
	
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



	// TODO: �ڴ�����ר�ô����/����û���
}


void CMarkdownEditorView::initCSS(){
	const CStringW strUserCss = Util::Utf8ToUtf16((Util::GetExePath() + "user.css").c_str()).c_str();
	if(PathFileExists(strUserCss)){
		_strCSS = Util::ReadStringFile(strUserCss);
	}else{
		Util::LoadStringRes(IDR_CSS,_T("CSS"),_strCSS); 
	}
}

const string HTML_TMPL = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/><style type=\"text/css\">{{0}}</style></head><body>{{1}}</body></html>";

// Raw HTML blocks pass through sundown, so a malicious file could carry a
// <script> element and MSHTML would execute it inside the preview. Strip
// script elements before handing the HTML to the browser control. (Links are
// separately gated by the scheme allowlist in CMyClickEvents::openUrl.)
static void StripScriptTags(string& html)
{
	string::size_type pos = 0;
	for (;;)
	{
		const string::size_type n = html.size();
		// find the next "<script" (case-insensitive, not a prefix like "<scriptx")
		string::size_type start = string::npos;
		for (string::size_type i = html.find('<', pos); i != string::npos && i + 7 <= n;
			i = html.find('<', i + 1))
		{
			if (strnicmp(html.c_str() + i, "<script", 7) == 0
				&& (i + 7 == n || !isalnum((unsigned char)html[i + 7])))
			{
				start = i;
				break;
			}
		}
		if (start == string::npos)
			return;
		// find the closing "</script" and its '>'; strip to the end if absent
		string::size_type end = n;
		for (string::size_type i = html.find('<', start + 7); i != string::npos && i + 8 <= n;
			i = html.find('<', i + 1))
		{
			if (strnicmp(html.c_str() + i, "</script", 8) == 0)
			{
				string::size_type gt = html.find('>', i + 8);
				end = (gt == string::npos) ? n : gt + 1;
				break;
			}
		}
		html.erase(start, end - start);
		pos = start;
	}
}

string CMarkdownEditorView::GetMdHtml(const string& str){
	string strHtml = HTML_TMPL;
	Util::ReplaceAllStr(strHtml,"{{0}}", _strCSS);
	string md = Util::Text2Md(str);
	StripScriptTags(md);
	Util::ReplaceAllStr(strHtml, "{{1}}", md);
	return strHtml;
}

// The preview document lives at about:blank, so relative <img src> values
// cannot resolve on their own. Rewriting the generated HTML instead (the old
// replaceImgSrc) would bake absolute paths into the document source, which is
// what right-click "View Source" offers for saving. Keep the written stream
// portable (relative paths only) and fix up the live DOM here: assigning the
// absolute URL to IHTMLImgElement::put_src triggers the actual image load.
void CMarkdownEditorView::ResolveLocalImages(IHTMLDocument2* pHtmlDoc)
{
	const string& dir = GetDocument()->getFilePath(); // UTF-8, forward slashes, trailing '/'
	if (dir.empty())
		return;
	CComPtr<IHTMLElementCollection> spImages;
	if (FAILED(pHtmlDoc->get_images(&spImages)) || !spImages)
		return;
	long nCount = 0;
	if (FAILED(spImages->get_length(&nCount)))
		return;
	for (long i = 0; i < nCount; i++) {
		CComVariant vIndex(i), vZero(0);
		CComPtr<IDispatch> spDisp;
		if (FAILED(spImages->item(vIndex, vZero, &spDisp)) || !spDisp)
			continue;
		CComPtr<IHTMLImgElement> spImg;
		if (FAILED(spDisp->QueryInterface(IID_IHTMLImgElement, (void**)&spImg)) || !spImg)
			continue;
		// NB: MSHTML resolves URL attributes on read, so an unresolvable
		// relative src comes back as "about:<relpath>" here (the document
		// itself lives at about:blank)
		CComBSTR bSrc;
		if (FAILED(spImg->get_src(&bSrc)) || !bSrc)
			continue;
		string raw = Util::Utf16ToUtf8(bSrc, SysStringLen(bSrc));
		if (raw.compare(0, 6, "about:") == 0)
			raw = raw.substr(6);
		if (raw.empty() || raw == "blank")
			continue;
		// leave anything that already carries a scheme (http:, file:, data:,
		// "c:/..." drive paths) untouched
		if (raw.find(':') != string::npos)
			continue;
		const string url = "file:///" + dir + raw;
		spImg->put_src(CComBSTR(Util::Utf8ToUtf16(url.c_str(), (int)url.size()).c_str()));
	}
}

void CMarkdownEditorView::UpdateMd(const string& strMd)
{
	// scroll-sync anchors are inserted on the wide (UTF-16) string so the
	// offsets share units with the RichEdit control's character indices
	const wstring wsrc = Util::Utf8ToUtf16(strMd.c_str(), (int)strMd.size());
	wstring wAnchored;
	MdAnchors::Insert(wsrc, wAnchored, _anchorChars);
	_anchorRichChars = MdAnchors::ToRichEditOffsets(wsrc, _anchorChars);
	const string anchored = Util::Utf16ToUtf8(wAnchored.c_str(), (int)wAnchored.size());
	string strHtml = GetMdHtml(anchored);
	NavigateHTML(strHtml);
	CacheAnchorElements();
}

// collect the live DOM elements for the anchors inserted by MdAnchors::Insert
void CMarkdownEditorView::CacheAnchorElements()
{
	_anchorElems.clear();
	CComPtr<IDispatch> pDisp = GetHtmlDocument();
	if (!pDisp)
		return;
	CComPtr<IHTMLDocument3> pDoc3;
	if (FAILED(pDisp->QueryInterface(IID_IHTMLDocument3, (void**)&pDoc3)) || !pDoc3)
		return;
	CComPtr<IHTMLElementCollection> pColl;
	if (FAILED(pDoc3->getElementsByTagName(CComBSTR(L"a"), &pColl)) || !pColl)
		return;
	long nCount = 0;
	if (FAILED(pColl->get_length(&nCount)))
		return;
	_anchorElems.resize(_anchorRichChars.size());
	for (long i = 0; i < nCount; i++) {
		CComVariant vIndex(i), vZero(0);
		CComPtr<IDispatch> spDisp;
		if (FAILED(pColl->item(vIndex, vZero, &spDisp)) || !spDisp)
			continue;
		CComPtr<IHTMLElement> pElem;
		if (FAILED(spDisp->QueryInterface(IID_IHTMLElement, (void**)&pElem)) || !pElem)
			continue;
		CComBSTR bId;
		if (FAILED(pElem->get_id(&bId)) || !bId)
			continue;
		const wstring id((LPCWSTR)bId, SysStringLen(bId));
		if (id.compare(0, 2, L"md") != 0)
			continue;
		const int idx = _wtoi(id.c_str() + 2);
		if (idx >= 0 && idx < (int)_anchorElems.size())
			_anchorElems[idx] = pElem;
	}
}

// absolute Y position (document coordinates) of anchor element idx, -1 if unknown
long CMarkdownEditorView::AnchorAbsY(int idx)
{
	if (idx < 0 || idx >= (int)_anchorElems.size() || !_anchorElems[idx])
		return -1;
	CComPtr<IHTMLElement2> pElem2;
	if (FAILED(_anchorElems[idx]->QueryInterface(IID_IHTMLElement2, (void**)&pElem2)) || !pElem2)
		return -1;
	CComPtr<IHTMLRect> pRect;
	if (FAILED(pElem2->getBoundingClientRect(&pRect)) || !pRect)
		return -1;
	long top = 0;
	if (FAILED(pRect->get_top(&top)))
		return -1;
	long scroll = 0;
	CComPtr<IDispatch> pDisp = GetHtmlDocument();
	if (pDisp) {
		CComPtr<IHTMLTextContainer> pText = getContainer(pDisp);
		if (pText)
			pText->get_scrollTop(&scroll);
	}
	return top + scroll;
}

CLeftView* CMarkdownEditorView::GetEditorPane()
{
	CWnd* pSplitter = GetParent();
	if (!pSplitter)
		return NULL;
	return DYNAMIC_DOWNCAST(CLeftView, ((CSplitterWnd*)pSplitter)->GetPane(0, 0));
}

// editor scrolled: bring the matching anchor to the top of the preview
void CMarkdownEditorView::ScrollPreviewToChar(long richChar)
{
	if (_anchorRichChars.empty())
		return;
	// last anchor at or before richChar
	int lo = 0, hi = (int)_anchorRichChars.size() - 1;
	int idx = -1;
	while (lo <= hi) {
		const int mid = (lo + hi) / 2;
		if (_anchorRichChars[mid] <= richChar) {
			idx = mid;
			lo = mid + 1;
		} else {
			hi = mid - 1;
		}
	}
	if (idx < 0)
		idx = 0;
	const long y = AnchorAbsY(idx);
	if (y < 0)
		return;
	CComPtr<IDispatch> pDisp = GetHtmlDocument();
	if (!pDisp)
		return;
	CComPtr<IHTMLTextContainer> pText = getContainer(pDisp);
	if (!pText)
		return;
	_guardTick = GetTickCount();
	_lastTop = (y > 4) ? (y - 4) : 0;
	pText->put_scrollTop(_lastTop);
}

// preview scrolled (timer): scroll the editor to the matching anchor
void CMarkdownEditorView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDT_SCROLLSYNC)
		SyncFromPreview();
	CView::OnTimer(nIDEvent);
}

void CMarkdownEditorView::SyncFromPreview()
{
	CComPtr<IDispatch> pDisp = GetHtmlDocument();
	if (!pDisp)
		return;
	CComPtr<IHTMLTextContainer> pText = getContainer(pDisp);
	if (!pText)
		return;
	long top = 0;
	if (FAILED(pText->get_scrollTop(&top)))
		return;
	if (top == _lastTop)
		return;
	_lastTop = top;
	const UINT now = GetTickCount();
	if (now - _guardTick < 250)
		return; // this change was caused by our own editor-driven sync
	const int n = (int)_anchorRichChars.size();
	if (n == 0)
		return;
	// last anchor at or above the viewport top (layout shifts when images
	// finish loading, so positions are computed fresh each time)
	int best = 0;
	long bestY = -1;
	for (int i = 0; i < n; i++) {
		const long y = AnchorAbsY(i);
		if (y < 0)
			continue;
		if (y <= top + 1 && y >= bestY) {
			bestY = y;
			best = i;
		}
	}
	CLeftView* pEdit = GetEditorPane();
	if (!pEdit)
		return;
	pEdit->ScrollEditorToChar(_anchorRichChars[best]);
}


