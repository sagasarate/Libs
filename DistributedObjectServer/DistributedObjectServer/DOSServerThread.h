#pragma once

#define SERVER_ENDING_TIME	3*60*1000

class CDOSServerThread : 
	public CDOSServer,
	public CBaseServer
{
protected:		
	enum SST_SERVER_STATUS
	{
		SST_SS_OBJECT_COUNT=50,
		SST_SS_ROUTE_IN_MSG_COUNT,
		SST_SS_ROUTE_IN_MSG_FLOW,
		SST_SS_ROUTE_OUT_MSG_COUNT,
		SST_SS_ROUTE_OUT_MSG_FLOW,
		SST_SS_ROUTE_CYCLE_TIME,
		SST_SS_ROUTE_MSG_QUEUE_LEN,
		SST_SS_ROUTE_CPU_USED_RATE,
		SST_SS_MONO_GC_USED_SIZE,
		SST_SS_MONO_GC_HEAP_SIZE,
		SST_SS_OBJECT_PROXY_CPU_USED_RATE=1000,
		SST_SS_GROUP_CYCLE_TIME=2000,
		SST_SS_GROUP_MAX_OBJECT_MSG_QUEUE_LEN=3000,
		SST_SS_GROUP_CPU_USED_RATE=4000,
	};
	CESVariableList				m_ESVariableList;
	CESFunctionList				m_ESFactionList;
	CESBolanStack				m_Script;
	CESThread					m_ESThread;
	CEasyScriptExecutor			m_ScriptExecutor;

	CSystemNetLinkManager		*m_pSysNetLinkManager;
	CSystemControlPort			*m_pUDPSystemControlPort;

	

	CEasyTimer					m_CountTimer;

	CThreadPerformanceCounter	m_ThreadPerformanceCounter;

	DECLARE_CLASS_INFO_STATIC(CDOSServerThread);
public:
	CDOSServerThread();
	virtual ~CDOSServerThread(void);

public:	
	

	virtual CNetServer * GetServer()
	{
		return this;
	}

	virtual bool PrintConsoleLog(int Level, LPCTSTR szLogMsg);

	virtual void ExecCommand(LPCTSTR szCommand);

	

	virtual void QueryShowDown();

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
	virtual bool IsServerTerminated();


	CSystemNetLinkManager * GetSysNetLinkManager()
	{
		return m_pSysNetLinkManager;
	}

	int StartLog(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int StopLog(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int TestLog(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);	
	int RebuildUDPControlPort(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFSetConsoleLogLevel(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFVerfyMemPool(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	
};
