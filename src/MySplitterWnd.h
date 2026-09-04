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
	bool _bLeftVisible;
	void ShowSplitter(bool bShow);
public:
	void ShowLeft(bool bShow);
	bool IsLeftVisible() const { return _bLeftVisible; }
	void SetLeftPercent(double percent) { _leftPercent = percent; }
	// remember the current left-pane proportion (client coordinates; safe on
	// zero-width/hidden splitters)
	void SaveLeftRatio();
	// resize column 0 back to the remembered proportion
	void ApplyLeftRatio();
};
