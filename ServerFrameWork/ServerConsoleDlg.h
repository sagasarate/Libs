/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerConsoleDlg.h                                       */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
// SuperStarServerDlg.h : 头文件
//

#pragma once





#define WM_TRAY_ICON_NOTIFY	WM_USER+1122
// CServerConsoleDlg 对话框
class CServerConsoleDlg : public CDialog
{
// 构造
public:
	CServerConsoleDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CServerConsoleDlg();

	void RegisterTrayIcon(LPCTSTR szTip);
	void UnregisterTrayIcon();
	void SetServer(CBaseServer * pServer);

// 对话框数据
	enum { IDD = IDD_SERVER_CONSOLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	
	
// 实现
protected:
	HICON m_hIcon;
	CBaseServer *		m_pServer;

	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
	afx_msg LRESULT OnTryIconNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	void OnOK();
	void OnCancel();
	//取得版本
	void ShowVersion();
	int FetchConsoleMsg(int ProcessLimit);

protected:
	virtual void OnCommand(LPCTSTR szCommand);
	virtual void OnLogMsg(LPCTSTR szLogMsg);
	void OnTimer(UINT_PTR nIDEvent);
	void OnBnClickedExecCommand();
	afx_msg void OnBnClickedCloseServer();

	CEdit m_edMsgWnd;	
	CString m_CycleTime;
	CString m_TCPRecv;
	CString m_TCPSend;
	CString m_UDPRecv;
	CString m_UCPSend;
	CString	m_Command;
	CString m_WorkStatus;

	CEasyBuffer m_ServerStatus;
	CDlgServerStatus m_DlgServerStatus;
	
public:
	afx_msg void OnBnClickedShowServerStatus();
	
};

inline void CServerConsoleDlg::SetServer(CBaseServer * pServer)
{
	m_pServer=pServer;
}