
// MainFrm.cpp : CMainFrame ���ʵ��
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

#define IS_VIEWER_KEY  _T("isViewer")
void saveViewer(bool enable) //���Ƿ�Ϊ�Ķ���ģʽ���浽ע���,�����´δ򿪳���ʱ�Զ�ʹ��֮ǰ��״̬
{
	int value = enable ? 1 : 0;
	AfxGetApp()->WriteProfileInt(_T(""), IS_VIEWER_KEY, value);
}
bool isViewer() //����
{
	int value = AfxGetApp()->GetProfileInt(_T(""), IS_VIEWER_KEY, 0);
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
		ID_SEPARATOR,           // ״̬��ָʾ��
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
	};

	// CMainFrame ����/����

	CMainFrame::CMainFrame()
	{
		_bInited = false;
		_bShowLeft = !isViewer();
		// TODO: �ڴ����ӳ�Ա��ʼ������
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
			TRACE0("δ�ܴ���״̬��\n");
			return -1;      // δ�ܴ���
		}
		m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
		return 0;
	}

	BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
		CCreateContext* pContext)
	{
		// ������ִ���
		if (!m_wndSplitter.CreateStatic(this, 1, 2))
			return FALSE;
		m_wndSplitter.SetLeftPercent(0.5);

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
		// TODO: �ڴ˴�ͨ���޸�
		//  CREATESTRUCT cs ���޸Ĵ��������ʽ

		return TRUE;
	}

	// CMainFrame ���

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


	// CMainFrame ��Ϣ��������


	void CMainFrame::OnSize(UINT nType, int cx, int cy)
	{
		CFrameWnd::OnSize(nType, cx, cy);
		if(!_bInited)
			return;
		if(cx == 0 || cy == 0)
			return;
		if(!m_wndSplitter.IsLeftVisible())
			return;
		// always keep the split 50/50 when resizing
		m_wndSplitter.SetColumnInfo(0,cx/2,10);
		m_wndSplitter.RecalcLayout();

		//��ʱ�����ϴ��Ƿ���ʾ���༭���״̬
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
	//ע�⣬����Ӧ�����������MainFrame�У��������MarkdownEditorView�У����MarkdownEditorViewʧȥ���㣬��˵����ܵ�
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
