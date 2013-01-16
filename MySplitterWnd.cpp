#include "MySplitterWnd.h"


CMySplitterWnd::CMySplitterWnd(void)
{
}


CMySplitterWnd::~CMySplitterWnd(void)
{
}


void CMySplitterWnd::ShowSplitter(bool bShow)
{
	if(!bShow){
		_cx = m_cxSplitterGap;//savepreviouscx
		_cy = m_cxSplitterGap;//saveprevious cy
		m_cxSplitterGap=1;
		m_cySplitterGap=1;
	}else{
		m_cxSplitterGap=_cx;
		m_cySplitterGap=_cy;
	}
}

void CMySplitterWnd::ShowLeft(bool bShow){
	CWnd* pLeft = this->GetPane(0,0);
	CRect rect;
	GetWindowRect(&rect);
	if(bShow){
		this->SetColumnInfo(0,_leftPercent*rect.Width(),10);
	}else{
		int cxCur, cxMin;
		this->GetColumnInfo(0, cxCur, cxMin); 
		_leftPercent = 1.0* cxCur /rect.Width();
		this->SetColumnInfo(0,0,10);
	}
	ShowSplitter(bShow);
	RecalcLayout();
	pLeft->ShowWindow(bShow);
}
