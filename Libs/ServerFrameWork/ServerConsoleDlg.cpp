/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerConsoleDlg.cpp                                     */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
// SuperStarServerDlg.cpp : 实现文件
//

#include "stdafx.h"



//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


#define PANEL_TIMER_ID		87832
#define LOG_MSG_FETCH_TIMER	84759

#define LOG_MSG_FETCH_TIME	200

#define LOG_MSG_FETCH_COUNT	50

// CServerConsoleDlg 对话框



CServerConsoleDlg::CServerConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerConsoleDlg::IDD, pParent)
	, m_CycleTime(_T("0"))
	, m_TCPRecv(_T(""))
	, m_TCPSend(_T(""))
	, m_UDPRecv(_T(""))
	, m_UCPSend(_T(""))
	, m_WorkStatus(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_TRAY);
	m_pServer=NULL;
}

CServerConsoleDlg::~CServerConsoleDlg()
{
	UnregisterTrayIcon();
}

void CServerConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edMsgWnd);
	DDX_Text(pDX, IDC_ST_CYCLE_TIME, m_CycleTime);
	DDX_Text(pDX, IDC_ST_TCP_RECV, m_TCPRecv);
	DDX_Text(pDX, IDC_ST_TCP_SEND, m_TCPSend);
	DDX_Text(pDX, IDC_ST_UDP_RECV, m_UDPRecv);
	DDX_Text(pDX, IDC_ST_UDP_SEND, m_UCPSend);
	DDX_Text(pDX, IDC_EDIT_COMMAND, m_Command);
	DDX_Text(pDX, IDC_ST_WORK_STATUS, m_WorkStatus);
}

BEGIN_MESSAGE_MAP(CServerConsoleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY, OnTryIconNotify)
	//}}AFX_MSG_MA
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_EXEC_COMMAND, OnBnClickedExecCommand)
	ON_BN_CLICKED(IDC_CLOSE_SERVER, OnBnClickedCloseServer)
	ON_BN_CLICKED(IDC_SHOW_SERVER_STATUS, &CServerConsoleDlg::OnBnClickedShowServerStatus)
END_MESSAGE_MAP()


// CServerConsoleDlg 消息处理程序

BOOL CServerConsoleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。


	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//初始化版本信息
	ShowVersion();
	
	m_ServerStatus.Create(SERVER_STATUS_BLOCK_SIZE);
	m_DlgServerStatus.Create(m_DlgServerStatus.IDD,this);
	

	SetTimer(PANEL_TIMER_ID,SERVER_INFO_COUNT_TIME,NULL);
	SetTimer(LOG_MSG_FETCH_TIMER,LOG_MSG_FETCH_TIME,NULL);
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CServerConsoleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerConsoleDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CServerConsoleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerConsoleDlg::OnOK()
{
}

void CServerConsoleDlg::OnCancel()
{
	//int nRetCode = MessageBox("程序将要关闭?", "警告", MB_YESNO);
	//if(nRetCode == IDYES)
	{		
		CDialog::OnCancel();
	}	
}


void CServerConsoleDlg::ShowVersion()
{
	CString Caption;
	Caption.Format("[%s]版本:%u.%u.%u.%u  启动时间:%s",
		(LPCTSTR)g_ProgramName,
		g_ProgramVersion[3],
		g_ProgramVersion[2],
		g_ProgramVersion[1],
		g_ProgramVersion[0],
		(LPCTSTR)(CTime::GetCurrentTime().Format("%m-%d %H:%M")));

	SetWindowText(Caption);	
}

int CServerConsoleDlg::FetchConsoleMsg(int ProcessLimit)
{
	int ProcessCount=0;
	if (m_pServer)
	{
		char Buffer[5001];
		UINT MsgLen = 0;
		LPCTSTR szMsg = m_pServer->GetConsoleLog(MsgLen);
		while (szMsg&&MsgLen)
		{
			if (IsWindowVisible())
			{
				if (MsgLen > 5000)
					MsgLen = 5000;
				memcpy(Buffer, szMsg, MsgLen);
				Buffer[MsgLen] = 0;
				OnLogMsg(Buffer);
			}

			m_pServer->PopupConsoleLog(MsgLen);

			ProcessCount++;
			if (ProcessCount > ProcessLimit)
				break;
			szMsg = m_pServer->GetConsoleLog(MsgLen);
		}
	}

	return ProcessCount;
}



void CServerConsoleDlg::OnLogMsg(LPCTSTR szLogMsg)
{

	int s1,s2;
	int sm1,sm2;
	int SelLine;
	int EndLine;

	m_edMsgWnd.SetRedraw(false);

	m_edMsgWnd.GetSel(sm1,sm2);
	m_edMsgWnd.SetSel(0,-1);
	m_edMsgWnd.GetSel(s1,s2);
	SelLine=m_edMsgWnd.LineFromChar(sm1);
	EndLine=m_edMsgWnd.GetLineCount()-1;
	if(s2>MAX_CONTROL_PANEL_MSG_LEN)
	{
		m_edMsgWnd.SetSel(0,-1);
		m_edMsgWnd.Clear();
		s2=0;
	}
	m_edMsgWnd.SetSel(s2,s2);
	//m_edMsgWnd.ReplaceSel(CTime::GetCurrentTime().Format("%H:%M:%S :"));


	//m_edMsgWnd.SetSel(0,-1);
	//m_edMsgWnd.GetSel(s1,s2);
	//m_edMsgWnd.SetSel(s2,s2);
	m_edMsgWnd.ReplaceSel(szLogMsg);


	//m_edMsgWnd.SetSel(0,-1);
	//m_edMsgWnd.GetSel(s1,s2);
	//m_edMsgWnd.SetSel(s2,s2);	

	//m_edMsgWnd.ReplaceSel("\r\n");



	m_edMsgWnd.SetRedraw(true);	


	if(SelLine==EndLine)
		m_edMsgWnd.LineScroll(m_edMsgWnd.GetLineCount());
	else
		m_edMsgWnd.SetSel(sm1,sm2);
}

void CServerConsoleDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == PANEL_TIMER_ID&&IsWindowVisible() && m_pServer)
	{
		UINT Size=m_pServer->GetAllServerStatus((BYTE *)m_ServerStatus.GetBuffer(),m_ServerStatus.GetBufferSize());
		CSmartStruct StatusPacket(m_ServerStatus.GetBuffer(),Size,false);

		CEasyArray<SERVER_STATUS_FORMAT_INFO> FormatInfoList;
		m_pServer->GetAllServerStatusFormat(FormatInfoList);

		UpdateData(true);		
		static char buff[128];
		sprintf_s(buff,128,"%.2f",
			(float)StatusPacket.GetMember(SC_SST_SS_CYCLE_TIME));
		m_CycleTime=buff;
		m_TCPRecv=FormatNumberWordsFloat(StatusPacket.GetMember(SC_SST_SS_TCP_RECV_FLOW),true);
		m_TCPSend=FormatNumberWordsFloat(StatusPacket.GetMember(SC_SST_SS_TCP_SEND_FLOW),true);
		m_UDPRecv=FormatNumberWordsFloat(StatusPacket.GetMember(SC_SST_SS_UDP_RECV_FLOW),true);
		m_UCPSend=FormatNumberWordsFloat(StatusPacket.GetMember(SC_SST_SS_UDP_SEND_FLOW),true);
		BYTE WorkStatus = StatusPacket.GetMember(SC_SST_SS_WORK_STATUS);
		m_WorkStatus = WorkStatus <= SERVER_WORK_STATUS_SHUTDOWN ? g_szSERVER_WORK_STATUS[WorkStatus] : _T("未知");

		UpdateData(false);

		m_DlgServerStatus.FlushStatus(StatusPacket, FormatInfoList);
	}
	if(nIDEvent==LOG_MSG_FETCH_TIMER)
	{
		FetchConsoleMsg(LOG_MSG_FETCH_COUNT);
	}

	CDialog::OnTimer(nIDEvent);
}

void CServerConsoleDlg::OnBnClickedExecCommand()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	m_Command.Trim();
	if(!m_Command.IsEmpty())
		OnCommand(m_Command);
	m_Command.Empty();
	UpdateData(false);
}

void CServerConsoleDlg::OnCommand(LPCTSTR szCommand)
{
	if (m_pServer)
		m_pServer->PushConsoleCmd(szCommand);
}

void CServerConsoleDlg::RegisterTrayIcon(LPCTSTR szTip)
{

	NOTIFYICONDATA Nid;

	Nid.cbSize = sizeof(NOTIFYICONDATA);
	Nid.hWnd = GetSafeHwnd();
	Nid.uID = 0;
	Nid.hIcon = m_hIcon;

	Nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	Nid.uCallbackMessage = WM_TRAY_ICON_NOTIFY;

	strcpy_s(Nid.szTip, sizeof(Nid.szTip),szTip);
	Shell_NotifyIcon(NIM_ADD, &Nid);

}

void CServerConsoleDlg::UnregisterTrayIcon()
{

	NOTIFYICONDATA Nid;

	Nid.cbSize = sizeof(NOTIFYICONDATA);
	Nid.hWnd = GetSafeHwnd();
	Nid.uID = 0;
	Nid.hIcon = m_hIcon;

	Nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	Nid.uCallbackMessage = WM_TRAY_ICON_NOTIFY;

	Shell_NotifyIcon(NIM_DELETE, &Nid);

}

LRESULT CServerConsoleDlg::OnTryIconNotify(WPARAM wParam, LPARAM lParam)
{
	

	switch(lParam)
	{
	case WM_LBUTTONDBLCLK:
		ShowWindow(SW_SHOW);
		break;
	}

	

	return 0;


}
void CServerConsoleDlg::OnBnClickedCloseServer()
{
	

	// TODO: 在此添加控件通知处理程序代码
	if(MessageBox("是否要关闭服务器","提示",MB_YESNO)==IDYES)
	{		
		if(m_pServer)
			m_pServer->QueryShowDown();
	}
	
}

void CServerConsoleDlg::OnBnClickedShowServerStatus()
{
	// TODO: 在此添加控件通知处理程序代码
	m_DlgServerStatus.ShowWindow(SW_SHOW);
}
