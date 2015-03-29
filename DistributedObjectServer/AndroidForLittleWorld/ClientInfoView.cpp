// ClientInfoView.cpp : 实现文件
//

#include "stdafx.h"
#include "ClientInfoView.h"




// CClientInfoView

IMPLEMENT_DYNCREATE(CClientInfoView, CFormView)

CClientInfoView::CClientInfoView()
	: CFormView(CClientInfoView::IDD)
{
	m_pClient=NULL;
	m_CurCommandHistoryPtr=0;
}

CClientInfoView::~CClientInfoView()
{
	if(m_pClient)
	{
		m_pClient->SetClientInfoView(NULL);
		m_pClient=NULL;
	}
}

void CClientInfoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG, m_reLogMsg);
	DDX_Control(pDX, IDC_EDIT_COMMAND, m_edCommand);
}

BEGIN_MESSAGE_MAP(CClientInfoView, CFormView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SHOW_MAP, &CClientInfoView::OnBnClickedShowMap)
	ON_BN_CLICKED(IDC_EXEC, &CClientInfoView::OnBnClickedExec)
	ON_COMMAND(ID_EDIT_COPY, &CClientInfoView::OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, &CClientInfoView::OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, &CClientInfoView::OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, &CClientInfoView::OnEditUndo)
END_MESSAGE_MAP()


// CClientInfoView 诊断

#ifdef _DEBUG
void CClientInfoView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CClientInfoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CClientInfoView 消息处理程序

void CClientInfoView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	m_reLogMsg.SetBackgroundColor(false,RGB(0,0,0));
	CHARFORMAT CharFormat;

	CharFormat.dwMask=CFM_COLOR|CFM_SIZE|CFM_FACE;
	CharFormat.dwEffects=0;
	CharFormat.crTextColor=RGB(255,255,255);
	CharFormat.yHeight=250;
	strcpy_s(CharFormat.szFaceName,32,"宋体");
	m_reLogMsg.SetDefaultCharFormat(CharFormat);


	m_ItemSorter.SetParent(this);

	m_ItemSorter.SetSortType(0,CDialogItemSorter::SORT_VERTICAL|
		CDialogItemSorter::PARAM_ALIGN_SPACE,3,0);

	m_ItemSorter.AddDialogItem(0,1,
		CDialogItemSorter::RESIZE_FIT_PARENT_WIDTH|
		CDialogItemSorter::RESIZE_FIT_PARENT_HEIGHT,
		IDC_LOG,CRect(0,0,0,0),0,0);
	m_ItemSorter.AddDialogItem(0,2,
		CDialogItemSorter::SORT_HORIZONTAL|
		CDialogItemSorter::RESIZE_FIT_PARENT_WIDTH|
		CDialogItemSorter::PARAM_ALIGN_SPACE,
		0,CRect(0,0,0,30),5,0);

	m_ItemSorter.AddDialogItem(2,21,
		CDialogItemSorter::RESIZE_FIT_PARENT_WIDTH,
		IDC_EDIT_COMMAND,CRect(0,0,0,0),0,0);

	m_ItemSorter.AddDialogItem(2,22,
		0,
		IDC_EXEC,CRect(0,0,0,0),0,0);

	m_ItemSorter.AddDialogItem(2,23,
		0,
		IDC_SHOW_MAP,CRect(0,0,0,0),0,0);

	m_ItemSorter.DoSort();

	m_LogBuffer.Create(LOG_BUFFER_SIZE);

	m_DlgMapView.Create(m_DlgMapView.IDD,this);

	SetTimer(1001,500,NULL);

}

void CClientInfoView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_ItemSorter.DoSort();
}

void CClientInfoView::PushLog(LPCTSTR szMsg)
{
	m_LogBuffer.PushBack(szMsg,strlen(szMsg));
}
void CClientInfoView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int Limit=16;
	char Buffer[1024];
	while(m_LogBuffer.GetUsedSize()&&Limit)
	{
		if(m_reLogMsg.GetLineCount()>MAX_LOG_LINE)
			m_reLogMsg.Clear();

		int FetchSize=m_LogBuffer.GetUsedSize();
		if(FetchSize>1000)
			FetchSize=1000;
		m_LogBuffer.PopFront(Buffer,FetchSize);
		Buffer[FetchSize]=0;
		m_reLogMsg.SetSel(-1,-1);
		m_reLogMsg.ReplaceSel(Buffer);
		Limit--;
	}


	//CFormView::OnTimer(nIDEvent);
}

void CClientInfoView::OnBnClickedShowMap()
{
	// TODO: 在此添加控件通知处理程序代码
	m_DlgMapView.ShowWindow(SW_SHOW);
}

void CClientInfoView::OnBnClickedExec()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pClient)
	{
		CString szCommand;
		m_edCommand.GetWindowText(szCommand);
		m_pClient->PushCommand(szCommand);
		m_CurCommandHistoryPtr=CMainApp::GetInstance()->AddCommandHistory(szCommand)+1;
		m_edCommand.SetWindowText("");
	}
}

void CClientInfoView::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
	if(GetFocus()==&m_reLogMsg)
	{
		m_reLogMsg.Copy();
	}
	else
	{
		m_edCommand.Copy();
	}
}

void CClientInfoView::OnEditCut()
{
	// TODO: 在此添加命令处理程序代码
	if(GetFocus()==&m_reLogMsg)
	{
		m_reLogMsg.Cut();
	}
	else
	{
		m_edCommand.Cut();
	}
}

void CClientInfoView::OnEditPaste()
{
	// TODO: 在此添加命令处理程序代码
	if(GetFocus()==&m_reLogMsg)
	{
		m_reLogMsg.Paste();
	}
	else
	{
		m_edCommand.Paste();
	}
}

void CClientInfoView::OnEditUndo()
{
	// TODO: 在此添加命令处理程序代码
	if(GetFocus()==&m_reLogMsg)
	{
		m_reLogMsg.Undo();
	}
	else
	{
		m_edCommand.Undo();
	}
}



BOOL CClientInfoView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(GetFocus()==&m_edCommand&&pMsg->message==WM_KEYUP)
	{
		if(pMsg->wParam==VK_UP)
		{
			m_CurCommandHistoryPtr--;
			LPCTSTR szCommand=CMainApp::GetInstance()->GetCommandHistory(m_CurCommandHistoryPtr);
			if(szCommand)
			{
				m_edCommand.SetWindowText(szCommand);
			}
		}
		else if(pMsg->wParam==VK_DOWN)
		{
			m_CurCommandHistoryPtr++;
			LPCTSTR szCommand=CMainApp::GetInstance()->GetCommandHistory(m_CurCommandHistoryPtr);
			if(szCommand)
			{
				m_edCommand.SetWindowText(szCommand);
			}
		}
	}
	return CFormView::PreTranslateMessage(pMsg);
}
