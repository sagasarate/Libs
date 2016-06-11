/****************************************************************************/
/*                                                                          */
/*      文件名:    IBaseServer.h                                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define SERVER_STATUS_BLOCK_SIZE			10240
#define MAX_SERVER_STATUS_NAME_LEN			128
#define SERVER_STATUS_FORMAT_INFO_SIZE		(MAX_SERVER_STATUS_NAME_LEN+64)
#define MAX_SERVER_CMD_LEN					1024
#define DEFAULT_CONSOLE_LOG_BUFF_LEN		(1024 * 1024)
#define DEFAULT_CONSOLE_CMD_COUNT			64


enum SERVER_STATUS_FORMAT_TYPE
{
	SSFT_DEFAULT,
	SSFT_FLOW,
	SSFT_VERSION,
	SSFT_PERCENT,
};
struct SERVER_STATUS_FORMAT_INFO
{
	WORD	StatusID;
	char	szName[MAX_SERVER_STATUS_NAME_LEN + 1];
	int		FormatType;
};

struct SERVER_COMMAND
{
	UINT	ID;
	char	Command[MAX_SERVER_CMD_LEN];
};

class CBaseServer
{
protected:
	CThreadSafeCycleBuffer						m_LogBuffer;
	CThreadSafeIDStorage<SERVER_COMMAND>		m_CommandPool;
	int											m_ConsoleLogLevel;
	CSmartStruct								m_ServerStatus;
	CEasyArray<SERVER_STATUS_FORMAT_INFO>		m_ServerStatusFormatInfoList;
	CEasyCriticalSection						m_ServerStatusCriticalSection;
	
public:	

	CBaseServer();
	virtual ~CBaseServer();

	virtual CNetServer * GetServer()=0;
	virtual void QueryShowDown() = 0;
	virtual bool IsServerTerminated() = 0;

	virtual bool PrintConsoleLog(int Level, LPCTSTR szLogMsg);
	LPCTSTR GetConsoleLog(UINT& MsgLen);
	void PopupConsoleLog(UINT MsgLen);
	void SetConsoleLogLevel(int Level)
	{
		m_ConsoleLogLevel = Level;
	}
	int GetConsoleLogLevel()
	{
		return m_ConsoleLogLevel;
	}

	bool PushConsoleCmd(LPCTSTR szCommand);

	virtual bool SetServerStatus(WORD StatusID, const CSmartValue& Value);
	virtual CSmartValue GetServerStatus(WORD StatusID);
	virtual CSmartStruct& GetAllServerStatus();
	virtual UINT GetAllServerStatus(BYTE * pBuff, UINT BuffLen);
	virtual void SetServerStatusFormat(WORD StatusID, LPCTSTR szStatusName, int FormatType = SSFT_DEFAULT);
	virtual bool GetServerStatusFormat(WORD StatusID, SERVER_STATUS_FORMAT_INFO& FormatInfo);
	virtual void GetAllServerStatusFormat(CEasyArray<SERVER_STATUS_FORMAT_INFO>& FormatInfoList);
	
};