
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "MarkdownEditor.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "MarkdownEditorView.h"
#include "MarkdownEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IS_VIEWER_KEY  "isViewer"
void saveViewer(bool enable) //把是否为阅读器模式保存到注册表,方便下次打开程序时自动使用之前的状态
{
	int value = enable ? 1 : 0;
	AfxGetApp()->WriteProfileInt("", IS_VIEWER_KEY, value);
}
bool isViewer() //根据
{
	int value = AfxGetApp()->GetProfileInt("", IS_VIEWER_KEY, 0);
	return value == 1;
}
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

	BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_COMMAND(IDM_SWITCH, &CMainFrame::OnSwitch)
		ON_COMMAND(IDM_ABOUT, &CMainFrame::OnAbout)
	END_MESSAGE_MAP()

	static UINT indicators[] =
	{
		ID_SEPARATOR,           // 状态行指示器
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
	};

	// CMainFrame 构造/析构

	CMainFrame::CMainFrame()
	{
		_bInited = false;
		_bShowLeft = !isViewer();
		// TODO: 在此添加成员初始化代码
	}

	CMainFrame::~CMainFrame()
	{
	}

	int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
			return -1;

		this->MoveWindow(0,0,800,600);
		this->CenterWindow();

		if (!m_wndStatusBar.Create(this))
		{
			TRACE0("未能创建状态栏\n");
			return -1;      // 未能创建
		}
		m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
		return 0;
	}

	BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
		CCreateContext* pContext)
	{
		// 创建拆分窗口
		if (!m_wndSplitter.CreateStatic(this, 1, 2))
			return FALSE;

		CRect rect;
		this->GetWindowRect(&rect);
		CSize size(rect.Width() /2 , rect.Height() /2);

		if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), size, pContext) ||
			!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CMarkdownEditorView), size, pContext))
		{
			m_wndSplitter.DestroyWindow();
			return FALSE;
		}

		_bInited = true;
		return TRUE;
	}

	BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
		if( !CFrameWnd::PreCreateWindow(cs) )
			return FALSE;
		// TODO: 在此处通过修改
		//  CREATESTRUCT cs 来修改窗口类或样式

		return TRUE;
	}

	// CMainFrame 诊断

#ifdef _DEBUG
	void CMainFrame::AssertValid() const
	{
		CFrameWnd::AssertValid();
	}

	void CMainFrame::Dump(CDumpContext& dc) const
	{
		CFrameWnd::Dump(dc);
	}
#endif //_DEBUG


	// CMainFrame 消息处理程序


	void CMainFrame::OnSize(UINT nType, int cx, int cy)
	{
		CFrameWnd::OnSize(nType, cx, cy);
		if(!_bInited)
			return;
		if(cx == 0 || cy == 0)
			return;
		int cxCur, cxMin;
		m_wndSplitter.GetColumnInfo(0, cxCur, cxMin); 
		if(cxCur <= 0)
			return;
		m_wndSplitter.SetColumnInfo(0,cx/2,10);
		m_wndSplitter.RecalcLayout();

		//打开时保留上次是否显示左侧编辑框的状态
		static bool sFirst = true;
		if (sFirst) {
			sFirst = false;
			bool show = isViewer();
			if (show)
				switchViewer(show);
		}
	}


	void CMainFrame::OnSwitch(){
		_bShowLeft = !_bShowLeft;
		switchViewer(!_bShowLeft);
		saveViewer(!_bShowLeft);
	}
	void CMainFrame::switchViewer(bool viewer) {
		m_wndSplitter.ShowLeft(!viewer);
	}


	const string STR_ABOUT = "# MarkdownEditor 1.2\nProject: <https://github.com/jijinggang/MarkdownEditor>\n## Author\njijinggang@gmail.com\n## Copyright\nFree For All";
	//注意，此相应函数必须放在MainFrame中，如果放在MarkdownEditorView中，如果MarkdownEditorView失去焦点，则菜单不能点
	void CMainFrame::OnAbout()
	{
		static bool s_bShowAbout = false;
		CMarkdownEditorView* pView = dynamic_cast<CMarkdownEditorView*>(m_wndSplitter.GetPane(0,1));
		if(pView == NULL)
			return;
		if(!s_bShowAbout)
			pView->UpdateMd(STR_ABOUT);
		else{
			CLeftView* pLeft = dynamic_cast<CLeftView*>(m_wndSplitter.GetPane(0,0));
			pView->GetDocument()->UpdateAllViews(pLeft, LPARAM_Update);
		}
		s_bShowAbout = !s_bShowAbout;
	}
