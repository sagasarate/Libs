// MainControlFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "MainControlFrame.h"



// CMainControlFrame

IMPLEMENT_DYNCREATE(CMainControlFrame, CMDIChildWnd)

CMainControlFrame::CMainControlFrame()
{
	m_pView=NULL;
}

CMainControlFrame::~CMainControlFrame()
{
}


BEGIN_MESSAGE_MAP(CMainControlFrame, CMDIChildWnd)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CMainControlFrame 消息处理程序

void CMainControlFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowWindow(SW_MINIMIZE);
	//CMDIChildWnd::OnClose();
}

int CMainControlFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	CCreateContext CreateContext;

	CreateContext.m_pCurrentDoc=NULL;
	CreateContext.m_pCurrentFrame=this;
	CreateContext.m_pLastView=NULL;
	CreateContext.m_pNewDocTemplate=NULL;
	CreateContext.m_pNewViewClass=RUNTIME_CLASS(CMainControlView);

	m_pView=(CMainControlView *)CreateView(&CreateContext);

	if(m_pView==NULL)
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}

	return 0;
}

void CMainControlFrame::OnSetFocus(CWnd* pOldWnd)
{
	CMDIChildWnd::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码
	if(m_pView)
		m_pView->SetFocus();	
}

BOOL CMainControlFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// 让视图第一次尝试该命令
	//if (m_View.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	// 否则，执行默认处理
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

