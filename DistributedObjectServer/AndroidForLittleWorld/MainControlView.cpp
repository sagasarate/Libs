// MainControlView.cpp : 实现文件
//

#include "stdafx.h"
#include "MainControlView.h"

#define LOG_FRESH_TIMER			500
#define STAT_FRESH_TIMER		1000

// CMainControlView

IMPLEMENT_DYNCREATE(CMainControlView, CFormView)

CMainControlView::CMainControlView()
	: CFormView(CMainControlView::IDD)
{
	m_SendCount=0;
	m_SendBytes=0;
	m_RecvCount=0;
	m_RecvBytes=0;
	
}

CMainControlView::~CMainControlView()
{
	CMainApp::GetInstance()->OnMainControlViewClose();
}

void CMainControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENTS, m_lvClients);
	DDX_Control(pDX, IDC_LIST_INFOS, m_lvInfos);
	DDX_Control(pDX, IDC_SYSTEM_LOG, m_reSystemLog);
}

BEGIN_MESSAGE_MAP(CMainControlView, CFormView)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CLIENTS, &CMainControlView::OnNMDblclkListClients)
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_CUT, &CMainControlView::OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CMainControlView::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CMainControlView::OnEditPaste)
END_MESSAGE_MAP()


// CMainControlView 诊断

#ifdef _DEBUG
void CMainControlView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMainControlView 消息处理程序

void CMainControlView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	m_ItemSorter.SetParent(this);

	m_ItemSorter.SetSortType(0,CDialogItemSorter::SORT_VERTICAL|
		CDialogItemSorter::PARAM_ALIGN_SPACE|CDialogItemSorter::ENABLE_RESIZE_BAR,3,0);

	m_ItemSorter.AddDialogItem(0,1,
		CDialogItemSorter::SORT_HORIZONTAL|
		CDialogItemSorter::RESIZE_FIT_PARENT_WIDTH|
		CDialogItemSorter::RESIZE_FIT_PARENT_HEIGHT|
		CDialogItemSorter::PARAM_ALIGN_SPACE|
		CDialogItemSorter::ENABLE_RESIZE_BAR,
		0,CRect(0,0,0,0),3,0);

	m_ItemSorter.AddDialogItem(0,2,
		CDialogItemSorter::SORT_HORIZONTAL|
		CDialogItemSorter::RESIZE_FIT_PARENT_WIDTH|
		CDialogItemSorter::PARAM_ALIGN_SPACE,
		IDC_SYSTEM_LOG,CRect(0,0,0,0),0,0);

	m_ItemSorter.AddDialogItem(1,11,
		CDialogItemSorter::SORT_HORIZONTAL|CDialogItemSorter::RESIZE_FIT_PARENT_WIDTH|
		CDialogItemSorter::RESIZE_FIT_PARENT_HEIGHT,
		IDC_LIST_CLIENTS,CRect(0,0,0,0),0,0);

	m_ItemSorter.AddDialogItem(1,22,
		CDialogItemSorter::SORT_HORIZONTAL|
		CDialogItemSorter::RESIZE_FIT_PARENT_HEIGHT,
		IDC_LIST_INFOS,CRect(0,0,200,0),0,0);

	m_ItemSorter.DoSort();

	m_lvClients.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lvInfos.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_lvClients.InsertColumn(0,"账号",LVCFMT_LEFT,100);
	m_lvClients.InsertColumn(1,"状态",LVCFMT_CENTER,100);

	m_lvInfos.InsertColumn(0,"项",LVCFMT_LEFT,80);
	m_lvInfos.InsertColumn(1,"值",LVCFMT_LEFT,100);


	m_reSystemLog.SetBackgroundColor(false,RGB(0,0,0));
	CHARFORMAT CharFormat;

	CharFormat.dwMask=CFM_COLOR|CFM_SIZE|CFM_FACE;
	CharFormat.dwEffects=0;
	CharFormat.crTextColor=RGB(255,255,255);
	CharFormat.yHeight=250;
	strcpy_s(CharFormat.szFaceName,32,"宋体");
	m_reSystemLog.SetDefaultCharFormat(CharFormat);
	m_reSystemLog.SetSelectionCharFormat(CharFormat);
	

	m_LogBuffer.Create(LOG_BUFFER_SIZE);
	m_EventQueue.Create(EVENT_QUEUE_SIZE);
	SetTimer(LOG_FRESH_TIMER,LOG_FRESH_TIMER,NULL);
	SetTimer(STAT_FRESH_TIMER,STAT_FRESH_TIMER,NULL);
}

void CMainControlView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	m_ItemSorter.DoSort();
}


void CMainControlView::AddClient(CClient * pClient)
{
	int Item=m_lvClients.InsertItem(m_lvClients.GetItemCount(),pClient->GetClientName());
	UINT ID=MAKELONG(pClient->GetID(),pClient->GetClientGroup()->GetIndex());
	m_lvClients.SetItemData(Item,ID);
	pClient->SetMainControlViewIndex(Item);
	

}

void CMainControlView::PushEvent(int ItemIndex,int Status)
{
	CLIENT_EVENT Event;
	Event.ItemIndex=ItemIndex;
	Event.Status=Status;
	m_EventQueue.PushBack(Event);
}
void CMainControlView::OnNMDblclkListClients(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	POSITION Pos=m_lvClients.GetFirstSelectedItemPosition();
	if(Pos)
	{
		int Item=m_lvClients.GetNextSelectedItem(Pos);
		UINT ID=m_lvClients.GetItemData(Item);
		CClientGroup * pGroup=CMainApp::GetInstance()->GetClientGroup(HIWORD(ID));
		CClient * pClient=pGroup->GetClient(LOWORD(ID));
		if(pClient)
		{
			CClientFrame * pClientFrameFrame=(CClientFrame *)CMainApp::GetInstance()->GetMainFrame()->CreateNewChild(
				RUNTIME_CLASS(CClientFrame), IDR_AndroidForLittlTYPE, 
				CMainApp::GetInstance()->GetMenu(), CMainApp::GetInstance()->GetAccel());
			if(pClientFrameFrame)
			{
				pClientFrameFrame->SetWindowText(pClient->GetClientName());
				pClient->SetClientInfoView(pClientFrameFrame->GetView());
				pClientFrameFrame->GetView()->SetClient(pClient);
			}
		}
	}
}


void CMainControlView::PushLog(LPCTSTR szMsg)
{
	m_LogBuffer.PushBack(szMsg,strlen(szMsg));
}
void CMainControlView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CFormView::OnTimer(nIDEvent);
	if(nIDEvent==LOG_FRESH_TIMER)
	{
		int Limit=16;
		char Buffer[1024];
		while(m_LogBuffer.GetUsedSize()&&Limit)
		{
			if(m_reSystemLog.GetLineCount()>MAX_LOG_LINE)
				m_reSystemLog.Clear();

			int FetchSize=m_LogBuffer.GetUsedSize();
			if(FetchSize>1000)
				FetchSize=1000;
			m_LogBuffer.PopFront(Buffer,FetchSize);
			Buffer[FetchSize]=0;
			m_reSystemLog.SetSel(-1,-1);
			m_reSystemLog.ReplaceSel(Buffer);
			Limit--;
		}

		Limit=64;
		CLIENT_EVENT Event;
		while(m_EventQueue.PopFront(Event))
		{
			OnEvent(Event);
			Limit--;
			if(Limit<=0)
				break;
		}
	}
	else if(nIDEvent==STAT_FRESH_TIMER)
	{
		RefreshStat();
	}
}

void CMainControlView::OnEditCut()
{
	// TODO: 在此添加命令处理程序代码
	m_reSystemLog.Cut();
}

void CMainControlView::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
	m_reSystemLog.Copy();
}

void CMainControlView::OnEditPaste()
{
	// TODO: 在此添加命令处理程序代码
	m_reSystemLog.Paste();
}

void CMainControlView::OnEvent(CLIENT_EVENT& Event)
{
	CEasyString Temp;
	switch(Event.Status)
	{
	case CClient::STATUS_NONE:
		Temp="未启用";
		break;
	case CClient::STATUS_DISCONNECT:
		Temp="未连接";
		break;
	case CClient::STATUS_CONNECTING:
		Temp="连接中";
		break;
	case CClient::STATUS_LOGIN:
		Temp="登陆中";
		break;
	case CClient::STATUS_GAMING:
		Temp="游戏中";
		break;
	}

	m_lvClients.SetItemText(Event.ItemIndex,1,Temp);
}

void CMainControlView::RefreshStat()
{
	m_lvInfos.SetRedraw(false);
	m_lvInfos.DeleteAllItems();
	CEasyString Temp;

	int Item=m_lvInfos.InsertItem(0,"接收流量");
	Temp.Format("%s/S",
		(LPCTSTR)FormatNumberWordsFloat((float)m_RecvBytes*1000.0f/STAT_FRESH_TIMER));
	m_lvInfos.SetItemText(Item,1,Temp);
	Item=m_lvInfos.InsertItem(1,"接收次数");
	Temp.Format("%s/S",
		(LPCTSTR)FormatNumberWordsFloat((float)m_RecvCount*1000.0f/STAT_FRESH_TIMER));
	m_lvInfos.SetItemText(Item,1,Temp);
	Item=m_lvInfos.InsertItem(2,"发送流量");
	Temp.Format("%s/S",
		(LPCTSTR)FormatNumberWordsFloat((float)m_SendBytes*1000.0f/STAT_FRESH_TIMER));
	m_lvInfos.SetItemText(Item,1,Temp);
	Item=m_lvInfos.InsertItem(3,"发送次数");
	Temp.Format("%s/S",
		(LPCTSTR)FormatNumberWordsFloat((float)m_SendCount*1000.0f/STAT_FRESH_TIMER));
	m_lvInfos.SetItemText(Item,1,Temp);
	m_SendCount=0;
	m_SendBytes=0;
	m_RecvCount=0;
	m_RecvBytes=0;
	m_lvInfos.SetRedraw(true);
}