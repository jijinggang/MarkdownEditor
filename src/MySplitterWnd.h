#pragma once
#include "afxext.h"
class CMySplitterWnd :
	public CSplitterWnd
{
public:
	CMySplitterWnd(void);
	~CMySplitterWnd(void);

private :
	int _cx;
	int _cy;
	double _leftPercent;
	void ShowSplitter(bool bShow);
public:
	void ShowLeft(bool bShow);
};

