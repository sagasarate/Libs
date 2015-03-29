/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerThread.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define SERVER_ENDING_TIME	3*60*1000

class CServerThread : 
	public CNetServer,
	public IBaseServer
{
protected:	
	CESVariableList				m_ESVariableList;
	CESFunctionList				m_ESFactionList;
	CESBolanStack				m_Script;
	CESThread					m_ESThread;
	CEasyScriptExecutor			m_ScriptExecutor;


	CSystemNetLinkManager		*m_pSysNetLinkManager;
	CSystemControlPort			*m_pUDPSystemControlPort;
	CSmartStruct				m_ServerStatus;
	int							m_ConsoleLogLevel;


	CEasyTimer					m_CountTimer;

	CThreadPerformanceCounter	m_ThreadPerformanceCounter;

	DECLARE_CLASS_INFO_STATIC(CServerThread);
public:
	CServerThread();
	virtual ~CServerThread(void);
	
public:	
	

	virtual CNetServer * GetServer()
	{
		return this;
	}

	virtual BOOL PrintConsoleLog(int Level,LPCTSTR szLogMsg);

	virtual void ExecCommand(LPCTSTR szCommand);

	virtual BOOL SetServerStatus(WORD StatusID,const CSmartValue& Value);
	virtual void SetServerStatusFormat(WORD StatusID,LPCTSTR szStatusName,int FormatType=SSFT_DEFAULT);
	virtual CSmartValue GetServerStatus(WORD StatusID)
	{
		return m_ServerStatus.GetMember(StatusID);
	}
	virtual CSmartStruct& GetAllServerStatus()
	{
		return m_ServerStatus;
	}

	void SetConsoleLogLevel(int Level)
	{
		m_ConsoleLogLevel=Level;
	}
	int GetConsoleLogLevel()
	{
		return m_ConsoleLogLevel;
	}

	virtual void QueryShowDown();
	virtual bool IsServerTerminated();

protected:

	virtual void Execute();
	
	virtual BOOL OnRun();	


	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	virtual BOOL OnStart();	
	virtual void OnTerminate();

	virtual BOOL OnTerminating();
	virtual void OnBeginTerminate();

	virtual int GetClientCount();
	virtual LPCTSTR GetConfigFileName();
	
	virtual void DoServerStat();


	CSystemNetLinkManager * GetSysNetLinkManager()
	{
		return m_pSysNetLinkManager;
	}

	int StartLog(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int StopLog(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int TestLog(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);	
	int RebuildUDPControlPort(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFSetConsoleLogLevel(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
};
