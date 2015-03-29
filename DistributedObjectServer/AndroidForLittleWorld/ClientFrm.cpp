// ChildFrm.cpp : CClientFrame 类的实现
//
#include "stdafx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientFrame

IMPLEMENT_DYNCREATE(CClientFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CClientFrame, CMDIChildWnd)
	ON_COMMAND(ID_FILE_CLOSE, &CClientFrame::OnFileClose)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CClientFrame 构造/析构

CClientFrame::CClientFrame()
{
	// TODO: 在此添加成员初始化代码
	m_pView=NULL;
}

CClientFrame::~CClientFrame()
{
}


BOOL CClientFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CClientFrame 诊断

#ifdef _DEBUG
void CClientFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CClientFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// CClientFrame 消息处理程序
void CClientFrame::OnFileClose() 
{
	// 若要关闭框架，只需发送 WM_CLOSE，
	// 这相当于从系统菜单中选择关闭。
	SendMessage(WM_CLOSE);
}

int CClientFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CCreateContext CreateContext;

	CreateContext.m_pCurrentDoc=NULL;
	CreateContext.m_pCurrentFrame=this;
	CreateContext.m_pLastView=NULL;
	CreateContext.m_pNewDocTemplate=NULL;
	CreateContext.m_pNewViewClass=RUNTIME_CLASS(CClientInfoView);

	m_pView=(CClientInfoView *)CreateView(&CreateContext);

	if(m_pView==NULL)
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}

	return 0;
}

void CClientFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);

	m_pView->SetFocus();
}

BOOL CClientFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// 让视图第一次尝试该命令
	//if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;
	
	// 否则，执行默认处理
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
