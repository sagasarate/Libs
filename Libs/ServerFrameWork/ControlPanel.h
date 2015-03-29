/****************************************************************************/
/*                                                                          */
/*      文件名:    ControlPanel.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#define MAX_CONSOLE_MSG_LEN			1024
#define MAX_SERVER_STATUS_NAME_LEN	128


enum CONSOLE_MSG_TYPE
{
	CON_MSG_LOG,
};

struct PANEL_MSG
{
	UINT	ID;
	UINT	MsgType;
	char	Msg[MAX_CONSOLE_MSG_LEN];
};

enum SERVER_STATUS_FORMAT_TYPE
{
	SSFT_DEFAULT,
	SSFT_FLOW,
	SSFT_VERSION,
	SSFT_PERCENT,
};
struct SERVER_STATUS_FORMAT_INFO
{
	char	szName[MAX_SERVER_STATUS_NAME_LEN];
	int		FormatType;
};

class CControlPanel :
	public CStaticObject<CControlPanel>
{
public:
	
#ifdef WIN32
protected:
	HWND								m_hWnd;
public:
	void SetHwnd(HWND hWnd)
	{
		m_hWnd=hWnd;
	}
#endif

protected:
	

	CThreadSafeIDStorage<PANEL_MSG>				m_MsgPool;
	CThreadSafeIDStorage<PANEL_MSG>				m_CommandPool;
	CEasyBuffer									m_ServerStatus;
	CEasyCriticalSection						m_CriticalSection;
	CEasyMap<WORD,SERVER_STATUS_FORMAT_INFO>	m_ServerStatusFormats;
	
public:
	CControlPanel(void);
	~CControlPanel(void);

	

	BOOL PushMsg(LPCTSTR szMsg);
	BOOL PushMsg(UINT Type,LPCVOID pData,int Len);
	PANEL_MSG * GetMsg();
	BOOL ReleaseMsg(UINT ID);	
	

	BOOL PushCommand(LPCTSTR szCommand);
	PANEL_MSG * GetCommand();
	BOOL ReleaseCommand(UINT ID);

	void SetServerStatus(LPCVOID pData,UINT DataSize);
	UINT GetServerStatus(LPVOID pBuffer,UINT BufferSize);
	void SetServerStatusFormat(WORD StatusID,LPCTSTR szStatusName,int FormatType=SSFT_DEFAULT);
	SERVER_STATUS_FORMAT_INFO * GetServerStatusFormat(WORD StatusID);
	CEasyMap<WORD,SERVER_STATUS_FORMAT_INFO>& GetAllServerStatusFormat();
};

inline CEasyMap<WORD,SERVER_STATUS_FORMAT_INFO>& CControlPanel::GetAllServerStatusFormat()
{
	return m_ServerStatusFormats;
}