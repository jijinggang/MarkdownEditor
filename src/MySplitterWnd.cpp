#include "MySplitterWnd.h"


CMySplitterWnd::CMySplitterWnd(void)
{
	_leftPercent = 0.5;
	_cx = _cy = 0;
	_bLeftVisible = true;
}


CMySplitterWnd::~CMySplitterWnd(void)
{
}


void CMySplitterWnd::ShowSplitter(bool bShow)
{
	if(!bShow){
		_cx = m_cxSplitterGap;//savepreviouscx
		_cy = m_cySplitterGap;//saveprevious cy
		m_cxSplitterGap=1;
		m_cySplitterGap=1;
	}else{
		m_cxSplitterGap=_cx;
		m_cySplitterGap=_cy;
	}
}

void CMySplitterWnd::SaveLeftRatio()
{
	CRect rect;
	GetClientRect(&rect);
	int cxCur, cxMin;
	GetColumnInfo(0, cxCur, cxMin);
	if (rect.Width() > 0 && cxCur > 0)
		_leftPercent = 1.0 * cxCur / rect.Width();
}

void CMySplitterWnd::ApplyLeftRatio()
{
	CRect rect;
	GetClientRect(&rect);
	if (rect.Width() > 0)
		SetColumnInfo(0, (int)(_leftPercent * rect.Width()), 10);
}

void CMySplitterWnd::ShowLeft(bool bShow){
	CWnd* pLeft = GetPane(0,0);
	if (pLeft == NULL)
		return;
	// save the gap only on the visible->hidden transition (never on the way back)
	if (_bLeftVisible && !bShow) {
		SaveLeftRatio();
		ShowSplitter(false);
		SetColumnInfo(0,0,10); // collapse the left pane; column 1 fills the width
	} else if (!_bLeftVisible && bShow) {
		ShowSplitter(true);
		ApplyLeftRatio();
	}
	_bLeftVisible = bShow;
	pLeft->ShowWindow(bShow);
	RecalcLayout();
}
