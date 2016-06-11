#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CDOSServerThread,CDOSServer);

CDOSServerThread::CDOSServerThread()
{
	FUNCTION_BEGIN;
	m_pSysNetLinkManager=NULL;
	m_pUDPSystemControlPort=NULL;
	m_ConsoleLogLevel=0;
	FUNCTION_END;
}

CDOSServerThread::~CDOSServerThread(void)
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

void CDOSServerThread::Execute()
{
	FUNCTION_BEGIN;
#ifndef _DEBUG
	CMainGuardThread::GetInstance()->SetTargetThreadID(GetThreadID());
	CMainGuardThread::GetInstance()->Start();
#endif
	while((!m_WantTerminate)&&(OnRun()))
	{
#ifndef _DEBUG
		CMainGuardThread::GetInstance()->MakeKeepAlive();
#endif
	}
#ifndef _DEBUG
	CMainGuardThread::GetInstance()->SafeTerminate();
#endif
	OnBeginTerminate();
	DWORD Time=CEasyTimer::GetTime();
	while(GetTimeToTime(Time,CEasyTimer::GetTime())<SERVER_ENDING_TIME&&OnTerminating())
	{
	}

	FUNCTION_END;
}


BOOL CDOSServerThread::OnStart()
{
	FUNCTION_BEGIN;


	m_TCPRecvBytes=0;
	m_TCPSendBytes=0;
	m_UDPRecvBytes=0;
	m_UDPSendBytes=0;
	m_TCPRecvCount=0;
	m_TCPSendCount=0;
	m_UDPRecvCount=0;
	m_UDPSendCount=0;
	m_CountTimer.SaveTime();

	//装载系统配置
	CSystemConfig::GetInstance()->LoadConfig(MakeModuleFullPath(NULL,GetConfigFileName()));


	CEasyString LogFileName;
	CEasyString ModulePath=GetModulePath(NULL);

	CServerLogPrinter * pLog;



	LogFileName.Format("%s/Log/%s",(LPCTSTR)ModulePath,g_ProgramName);
	pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_CONSOLE|CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
	CLogManager::GetInstance()->AddChannel(SERVER_LOG_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	SetConsoleLogLevel(CSystemConfig::GetInstance()->GetConsoleLogLevel());




	LogFileName.Format("%s/Log/DOSLib",(LPCTSTR)ModulePath);
	pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
	CLogManager::GetInstance()->AddChannel(LOG_DOS_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	LogFileName.Format("%s/Log/DOSObjectStat",(LPCTSTR)ModulePath);
	pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
	CLogManager::GetInstance()->AddChannel(LOG_DOS_OBJECT_STATE_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	LogFileName.Format("%s/Log/NetLib",(LPCTSTR)ModulePath);
	pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
	CLogManager::GetInstance()->AddChannel(LOG_NET_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	LogFileName.Format("%s/Log/DBLib",(LPCTSTR)ModulePath);
	pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
	CLogManager::GetInstance()->AddChannel(LOG_DB_ERROR_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	LogFileName.Format("%s/Log/Mono", (LPCTSTR)ModulePath);
	pLog = new CServerLogPrinter(this, CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(), LogFileName);
	CLogManager::GetInstance()->AddChannel(LOG_MONO_CHANNEL, pLog);
	SAFE_RELEASE(pLog);



	Log("开始启动服务器,当前版本:%u.%u.%u.%u",
		g_ProgramVersion[3],
		g_ProgramVersion[2],
		g_ProgramVersion[1],
		g_ProgramVersion[0]);





	m_ESVariableList.Create(128);
	m_ESFactionList.Create(128);
	CESFunctionLib::GetInstance()->AddFunction(&m_ESFactionList);
	m_ESThread.SetVariableList(&m_ESVariableList);
	m_ESThread.SetFunctionList(&m_ESFactionList);
	m_ESThread.SetScript(&m_Script);


	m_ESFactionList.AddCFunction("StartLog",3,this,&CDOSServerThread::StartLog);
	m_ESFactionList.AddCFunction("StopLog",2,this,&CDOSServerThread::StopLog);
	m_ESFactionList.AddCFunction("TestLog",1,this,&CDOSServerThread::TestLog);
	m_ESFactionList.AddCFunction("RebuildUDPControlPort",0,this,&CDOSServerThread::RebuildUDPControlPort);
	m_ESFactionList.AddCFunction("SetConsoleLogLevel",1,this,&CDOSServerThread::SFSetConsoleLogLevel);
	m_ESFactionList.AddCFunction("VerfyMemPool",0,this,&CDOSServerThread::SFVerfyMemPool);

	if(!CDOSServer::OnStart())
		return FALSE;

	//初始化系统连接
	m_pSysNetLinkManager=new CSystemNetLinkManager();
	m_pSysNetLinkManager->SetServerThread(this);

	xml_parser Parser;

	if(Parser.parse_file(MakeModuleFullPath(NULL,GetConfigFileName()),pug::parse_trim_attribute))
	{
		xml_node Config=Parser.document();
		if (Config.moveto_child("Main"))
		{
			if (Config.moveto_child("SystemLink"))
			{
				if (m_pSysNetLinkManager->Init(this, Config))
				{
					Log("初始化系统连接管理器成功");
				}
				else
				{
					Log("初始化系统连接管理器失败");
				}
			}
			else
			{
				Log("不合法的系统连接配置文件%s", GetConfigFileName());
			}
		}
		else
		{
			Log("不合法的系统连接配置文件%s", GetConfigFileName());
		}
	}
	else
	{
		Log("未找到系统连接配置文件%s",GetConfigFileName());
	}

	m_pUDPSystemControlPort=new CSystemControlPort();
	if(!m_pUDPSystemControlPort->Init(this))
	{
		Log("初始化UDP系统控制端口失败");
	}


	m_ServerStatus.Create(SERVER_STATUS_BLOCK_SIZE);

	ULONG64_CONVERTER Version;
	memcpy(Version.Words,g_ProgramVersion,sizeof(ULONG64_CONVERTER));

	CEasyString Temp;

	SetServerStatus(SC_SST_SS_PROGRAM_VERSION,CSmartValue(Version.QuadPart));
	SetServerStatusFormat(SC_SST_SS_PROGRAM_VERSION,"服务器版本",SSFT_VERSION);
	SetServerStatusFormat(SC_SST_SS_CYCLE_TIME,"循环时间(毫秒)");
	SetServerStatusFormat(SC_SST_SS_CPU_COST,"CPU占用率",SSFT_PERCENT);
	SetServerStatusFormat(SC_SST_SS_TCP_RECV_FLOW,"TCP接收流量(Byte/S)",SSFT_FLOW);
	SetServerStatusFormat(SC_SST_SS_TCP_SEND_FLOW,"TCP发送流量(Byte/S)",SSFT_FLOW);
	SetServerStatusFormat(SC_SST_SS_TCP_RECV_COUNT,"TCP接收次数(次/S)");
	SetServerStatusFormat(SC_SST_SS_TCP_SEND_COUNT,"TCP发送次数(次/S)");
	SetServerStatusFormat(SST_SS_OBJECT_COUNT,"系统对象数");
	SetServerStatusFormat(SST_SS_ROUTE_IN_MSG_COUNT,"路由输入消息数");
	SetServerStatusFormat(SST_SS_ROUTE_IN_MSG_FLOW,"路由输入消息流量",SSFT_FLOW);
	SetServerStatusFormat(SST_SS_ROUTE_OUT_MSG_COUNT,"路由输出消息数");
	SetServerStatusFormat(SST_SS_ROUTE_OUT_MSG_FLOW,"路由输出消息流量",SSFT_FLOW);
	SetServerStatusFormat(SST_SS_ROUTE_CYCLE_TIME,"路由循环时间(MS)");
	SetServerStatusFormat(SST_SS_ROUTE_MSG_QUEUE_LEN,"路由消息队列长度");	
	Temp.Format("路由(%u)CPU占用率",GetRouter()->GetThreadID());
	SetServerStatusFormat(SST_SS_ROUTE_CPU_USED_RATE,Temp,SSFT_PERCENT);
	SetServerStatusFormat(SST_SS_MONO_GC_USED_SIZE, "MonoGC内存使用量", SSFT_FLOW);
	SetServerStatusFormat(SST_SS_MONO_GC_HEAP_SIZE, "MonoGC堆大小", SSFT_FLOW);


	CEasyString CSVLogHeader="CycleTime,CPUUsed,TCPRecvFlow,TCPSendFlow,TCPRecvCount,TCPSendCount,ObjectCount,"
		"RouteInMsgCount,RouteInMsgFlow,RouteOutMsgCount,RouteOutMsgFlow,RouteCycleTime,RouteMsgQueueLen,RouteCPUUsed,MonoGCUsedSize,MonoGCHeapSize";

	for (UINT i = 0; i < GetProxyManager()->GetProxyServiceCount(); i++)
	{
		IDOSObjectProxyServiceBase * pProxyService = GetProxyManager()->GetProxyService(i);
		if (pProxyService)
		{
			Temp.Format("对象代理(%u)CPU占用率", pProxyService->GetID());
			SetServerStatusFormat(SST_SS_OBJECT_PROXY_CPU_USED_RATE + i, Temp, SSFT_PERCENT);

			Temp.Format(",Proxy%dCPUUsed", pProxyService->GetID());
			CSVLogHeader += Temp;
		}
	}

	for(UINT i=0;i<GetObjectManager()->GetGroupCount();i++)
	{

		Temp.Format("对象组%d循环时间(MS)",i);
		SetServerStatusFormat(SST_SS_GROUP_CYCLE_TIME+i,Temp);
		Temp.Format("对象组%d最长对象消息队列",i);
		SetServerStatusFormat(SST_SS_GROUP_MAX_OBJECT_MSG_QUEUE_LEN+i,Temp);
		Temp.Format("对象组%d(%u)CPU占用率",i,GetObjectManager()->GetGroup(i)->GetThreadID());
		SetServerStatusFormat(SST_SS_GROUP_CPU_USED_RATE+i,Temp,SSFT_PERCENT);
		Temp.Format(",Group%dCycleTime,Group%dMaxMsgQueueLen,Group%dCPUUsed",i,i,i);
		CSVLogHeader+=Temp;
	}

	LogFileName.Format("%s/Log/%s.Status",(LPCTSTR)ModulePath,g_ProgramName);
	CCSVFileLogPrinter * pCSVLog=new CCSVFileLogPrinter();
	pCSVLog->Init(CSystemConfig::GetInstance()->GetLogLevel(),LogFileName,CSVLogHeader);
	CLogManager::GetInstance()->AddChannel(SERVER_STATUS_LOG_CHANNEL,pCSVLog);
	SAFE_RELEASE(pCSVLog);


	m_ThreadPerformanceCounter.Init(GetThreadHandle(),SERVER_INFO_COUNT_TIME);

	Log("服务器成功启动");

	FUNCTION_END;
	return TRUE;
}

void CDOSServerThread::OnBeginTerminate()
{

}

void CDOSServerThread::OnTerminate()
{
	FUNCTION_BEGIN;

	SAFE_RELEASE(m_pSysNetLinkManager);
	SAFE_RELEASE(m_pUDPSystemControlPort);
	CDOSServer::OnTerminate();
	Log("服务器关闭");
	FUNCTION_END;
}

BOOL CDOSServerThread::OnRun()
{
	FUNCTION_BEGIN;

	if(!CBaseNetServer::OnRun())
		return FALSE;

	if(Update(CSystemConfig::GetInstance()->GetMainThreadProcessLimit())==0)
	{
		DoSleep(1);
	}

	m_ThreadPerformanceCounter.DoPerformanceCount();
	//计算服务器循环时间
	if(m_CountTimer.IsTimeOut(SERVER_INFO_COUNT_TIME))
	{
		m_CountTimer.SaveTime();
		DoServerStat();
	}

	//执行控制台命令
	SERVER_COMMAND Command;
	while (m_CommandPool.PopBack(Command))
	{
		ExecCommand(Command.Command);
	}

	FUNCTION_END;
	return TRUE;
}
int CDOSServerThread::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	int Process=0;
	Process+=CDOSServer::Update(ProcessPacketLimit);
	Process+=m_pSysNetLinkManager->Update(ProcessPacketLimit);
	Process+=m_pUDPSystemControlPort->Update(ProcessPacketLimit);
	return Process;
	FUNCTION_END;
	return 0;
}

BOOL CDOSServerThread::OnTerminating()
{
	return FALSE;
}


int CDOSServerThread::GetClientCount()
{
	return 0;
}

LPCTSTR CDOSServerThread::GetConfigFileName()
{
	return SYSTEM_NET_LINK_CONFIG_FILE_NAME;
}

bool CDOSServerThread::PrintConsoleLog(int Level, LPCTSTR szLogMsg)
{
	FUNCTION_BEGIN;
	CBaseServer::PrintConsoleLog(Level, szLogMsg);
	if (m_pSysNetLinkManager && (m_ConsoleLogLevel&Level))
	{
		m_pSysNetLinkManager->SendLogMsg(szLogMsg);
	}
	return true;
	FUNCTION_END;
	return false;
}

void CDOSServerThread::ExecCommand(LPCTSTR szCommand)
{
	FUNCTION_BEGIN;
	int RetCode;
	ES_BOLAN Result;

	Log("执行命令:%s",szCommand);

	RetCode=m_ESThread.PushScript(szCommand);
	if(RetCode)
	{
		Log("解析命令出错:Line=%d,%s",
			m_ESThread.GetLastLine(),
			ESGetErrorMsg(RetCode));
	}
	RetCode=m_ScriptExecutor.ExecScript(m_ESThread);
	if(RetCode)
	{
		Log("解析命令出错:Line=%d,%s",
			m_ESThread.GetLastLine(),
			ESGetErrorMsg(RetCode));
	}
	else
	{
		Log("执行命令结果:%s",
			(LPCTSTR)BolanToString(m_ESThread.GetResult()));
	}
	FUNCTION_END;
}


void CDOSServerThread::QueryShowDown()
{
	FUNCTION_BEGIN;
	Log("服务器准备关闭");
	Terminate();
	FUNCTION_END;
}

bool CDOSServerThread::IsServerTerminated()
{
	FUNCTION_BEGIN;
	return IsTerminated()!=FALSE;
	FUNCTION_END;
	return true;
}

int CDOSServerThread::StartLog(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	CServerLogPrinter * pLog=NULL;
	if(_stricmp(pParams[0].StrValue,"Normal")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(SERVER_LOG_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"Status")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(SERVER_STATUS_LOG_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"DB")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(LOG_DB_ERROR_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"Net")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(LOG_NET_CHANNEL);
	}
	if(pLog)
	{
		int WitchMode=0;
		if(_stricmp(pParams[1].StrValue,"VS")==0)
		{
			WitchMode=CServerLogPrinter::LOM_VS;
		}
		else if(_stricmp(pParams[1].StrValue,"Console")==0)
		{
			WitchMode=CServerLogPrinter::LOM_CONSOLE;
		}
		else if(_stricmp(pParams[1].StrValue,"File")==0)
		{
			WitchMode=CServerLogPrinter::LOM_FILE;
		}
		if(WitchMode)
		{
			UINT Mode=pLog->GetLogMode();
			Mode|=WitchMode;

			int Level=pLog->GetLogLevel();

			CEasyString LogFileName;
			if(pParams[2].StrValue.IsEmpty())
			{
				LogFileName=pLog->GetLogFileName();
			}
			else
			{
				CEasyString ModulePath=GetModulePath(NULL);
				LogFileName.Format("%s/Log/%s",(LPCTSTR)ModulePath,(LPCTSTR)pParams[2].StrValue);
			}


			pLog->SetLogMode(Mode,Level,LogFileName);
			Log("Log模块[%s],模式[%s]的输出已被开启",
				(LPCTSTR)(pParams[0].StrValue),
				(LPCTSTR)(pParams[1].StrValue));
		}
	}
	FUNCTION_END;
	return 0;
}

int CDOSServerThread::StopLog(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	CServerLogPrinter * pLog=NULL;
	if(_stricmp(pParams[0].StrValue,"Normal")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(SERVER_LOG_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"Status")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(SERVER_STATUS_LOG_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"DB")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(LOG_DB_ERROR_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"Net")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(LOG_NET_CHANNEL);
	}
	if(pLog)
	{
		int WitchMode=0;
		if(_stricmp(pParams[1].StrValue,"VS")==0)
		{
			WitchMode=CServerLogPrinter::LOM_VS;
		}
		else if(_stricmp(pParams[1].StrValue,"Console")==0)
		{
			WitchMode=CServerLogPrinter::LOM_CONSOLE;
		}
		else if(_stricmp(pParams[1].StrValue,"File")==0)
		{
			WitchMode=CServerLogPrinter::LOM_FILE;
		}
		if(WitchMode)
		{
			UINT Mode=pLog->GetLogMode();
			Mode&=~WitchMode;
			int Level=pLog->GetLogLevel();
			pLog->SetLogMode(Mode,Level,NULL);
			Log("Log模块[%s],模式[%s]的输出已被关闭",
				(LPCTSTR)(pParams[0].StrValue),
				(LPCTSTR)(pParams[1].StrValue));
		}
	}
	FUNCTION_END;
	return 0;
}

int CDOSServerThread::TestLog(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	CServerLogPrinter * pLog=NULL;
	if(_stricmp(pParams[0].StrValue,"Normal")==0)
	{
		Log("Normal");
	}
	else if(_stricmp(pParams[0].StrValue,"Debug")==0)
	{
		LogDebug("Debug");
	}
	

	FUNCTION_END;
	return 0;
}


int CDOSServerThread::RebuildUDPControlPort(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	if(!m_pUDPSystemControlPort->Init(this))
	{
		Log("重建UDP系统控制端口失败");
	}
	else
	{
		Log("重建UDP系统控制端口成功");
	}
	FUNCTION_END;
	return 0;
}

int CDOSServerThread::SFSetConsoleLogLevel(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;

	SetConsoleLogLevel(pParams[0]);
	Log("控制台Log输出等级设置为%d",GetConsoleLogLevel());

	FUNCTION_END;
	return 0;
}

int CDOSServerThread::SFVerfyMemPool(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;

	((CDOSServer *)GetServer())->GetMemoryPool()->Verfy(SERVER_LOG_CHANNEL);

	FUNCTION_END;
	return 0;
}

void CDOSServerThread::DoServerStat()
{
	FUNCTION_BEGIN;
	int ClientCount=GetClientCount();
	float CycleTime=m_ThreadPerformanceCounter.GetCycleTime();
	float CPUCost=m_ThreadPerformanceCounter.GetCPUUsedRate();
	float TCPRecvFlow=(float)m_TCPRecvBytes*1000/SERVER_INFO_COUNT_TIME;
	float TCPSendFlow=(float)m_TCPSendBytes*1000/SERVER_INFO_COUNT_TIME;

	float TCPRecvCount=(float)m_TCPRecvCount*1000/SERVER_INFO_COUNT_TIME;
	float TCPSendCount=(float)m_TCPSendCount*1000/SERVER_INFO_COUNT_TIME;

	UINT ObjectCount=GetObjectManager()->GetObejctCount();

	float RouteInMsgCount=(float)GetRouter()->GetInMsgCount()*1000/SERVER_INFO_COUNT_TIME;
	float RouteInMsgFlow=(float)GetRouter()->GetInMsgFlow()*1000/SERVER_INFO_COUNT_TIME;
	float RouteOutMsgCount=(float)GetRouter()->GetOutMsgCount()*1000/SERVER_INFO_COUNT_TIME;
	float RouteOutMsgFlow=(float)GetRouter()->GetOutMsgFlow()*1000/SERVER_INFO_COUNT_TIME;
	GetRouter()->ResetStatData();

	UINT64 MonoGCUsedSize = 0;
	UINT64 MonoGCHeapSize = 0;

	if (CDOSConfig::GetInstance()->GetMonoConfig().EnableMono)
	{
		MonoGCUsedSize = mono_gc_get_used_size();
		MonoGCHeapSize = mono_gc_get_heap_size();
	}
	

	SetServerStatus(SC_SST_SS_CYCLE_TIME,CSmartValue(CycleTime));
	SetServerStatus(SC_SST_SS_CPU_COST,CSmartValue(CPUCost));
	SetServerStatus(SC_SST_SS_TCP_RECV_FLOW,CSmartValue(TCPRecvFlow));
	SetServerStatus(SC_SST_SS_TCP_SEND_FLOW,CSmartValue(TCPSendFlow));
	SetServerStatus(SC_SST_SS_TCP_RECV_COUNT,CSmartValue(TCPRecvCount));
	SetServerStatus(SC_SST_SS_TCP_SEND_COUNT,CSmartValue(TCPSendCount));
	SetServerStatus(SST_SS_OBJECT_COUNT,CSmartValue(ObjectCount));
	SetServerStatus(SST_SS_ROUTE_IN_MSG_COUNT,CSmartValue(RouteInMsgCount));
	SetServerStatus(SST_SS_ROUTE_IN_MSG_FLOW,CSmartValue(RouteInMsgFlow));
	SetServerStatus(SST_SS_ROUTE_OUT_MSG_COUNT,CSmartValue(RouteOutMsgCount));
	SetServerStatus(SST_SS_ROUTE_OUT_MSG_FLOW,CSmartValue(RouteOutMsgFlow));
	SetServerStatus(SST_SS_ROUTE_CYCLE_TIME,CSmartValue(GetRouter()->GetCycleTime()));
	SetServerStatus(SST_SS_ROUTE_MSG_QUEUE_LEN,CSmartValue(GetRouter()->GetMsgQueueLen()));
	SetServerStatus(SST_SS_ROUTE_CPU_USED_RATE,CSmartValue(GetRouter()->GetCPUUsedRate()));
	SetServerStatus(SST_SS_MONO_GC_USED_SIZE, CSmartValue(MonoGCUsedSize));
	SetServerStatus(SST_SS_MONO_GC_HEAP_SIZE, CSmartValue(MonoGCHeapSize));


	CEasyString ProxyStatData;

	for (UINT i = 0; i < GetProxyManager()->GetProxyServiceCount(); i++)
	{
		CEasyString Temp;
		IDOSObjectProxyServiceBase * pProxyService = GetProxyManager()->GetProxyService(i);
		if (pProxyService)
		{
			SetServerStatus(SST_SS_OBJECT_PROXY_CPU_USED_RATE + i, CSmartValue(pProxyService->GetCPUUsedRate()));
			Temp.Format(",%g", pProxyService->GetCPUUsedRate());
		}
	}


	CEasyString GroupStatData;

	for(UINT i=0;i<GetObjectManager()->GetGroupCount();i++)
	{
		CEasyString Temp;
		CDOSObjectGroup * pGroup=GetObjectManager()->GetGroup(i);
		if(pGroup)
		{
			SetServerStatus(SST_SS_GROUP_CYCLE_TIME+i,CSmartValue(pGroup->GetCycleTime()));
			SetServerStatus(SST_SS_GROUP_MAX_OBJECT_MSG_QUEUE_LEN+i,CSmartValue(pGroup->GetMaxObjectMsgQueueLen()));
			SetServerStatus(SST_SS_GROUP_CPU_USED_RATE+i,CSmartValue(pGroup->GetCPUUsedRate()));
			Temp.Format(",%g,%d,%g",pGroup->GetCycleTime(),pGroup->GetMaxObjectMsgQueueLen(),pGroup->GetCPUUsedRate());
		}
		else
		{
			Temp=",0,0,0";
		}
		GroupStatData+=Temp;
	}


	CLogManager::GetInstance()->PrintLog(SERVER_STATUS_LOG_CHANNEL,ILogPrinter::LOG_LEVEL_NORMAL,0,
		"%g,%g,%s,%s,%g,%g,%u,%s,%s,%s,%s,%g,%u,%g,%s,%s%s%s",
		CycleTime,
		CPUCost,
		(LPCTSTR)FormatNumberWordsFloat(TCPRecvFlow,true),
		(LPCTSTR)FormatNumberWordsFloat(TCPSendFlow,true),
		TCPRecvCount,
		TCPSendCount,
		ObjectCount,
		(LPCTSTR)FormatNumberWordsFloat(RouteInMsgCount,true),
		(LPCTSTR)FormatNumberWordsFloat(RouteInMsgFlow,true),
		(LPCTSTR)FormatNumberWordsFloat(RouteOutMsgCount,true),
		(LPCTSTR)FormatNumberWordsFloat(RouteOutMsgFlow,true),
		GetRouter()->GetCycleTime(),
		GetRouter()->GetMsgQueueLen(),
		GetRouter()->GetCPUUsedRate(),
		(LPCTSTR)FormatNumberWords(MonoGCUsedSize, true),
		(LPCTSTR)FormatNumberWords(MonoGCHeapSize, true),
		(LPCTSTR)ProxyStatData,
		(LPCTSTR)GroupStatData);

	ResetFluxStat();

	if(CSystemConfig::GetInstance()->IsLogServerObjectUse())
	{
		PrintObjectStatus();
	}
	PrintDOSObjectStatLog("================================================================");
	PrintDOSObjectStatLog("开始统计对象使用情况");
	PrintDOSObjectStatLog("================================================================");
	GetObjectManager()->PrintGroupInfo(LOG_DOS_OBJECT_STATE_CHANNEL);

	UINT AllocCount=((CDOSServer *)GetServer())->GetMemoryPool()->GetAllocCount();
	UINT FreeCount=((CDOSServer *)GetServer())->GetMemoryPool()->GetFreeCount();
	UINT SystemAllocCount=((CDOSServer *)GetServer())->GetMemoryPool()->GetSystemAllocCount();
	UINT SystemFreeCount=((CDOSServer *)GetServer())->GetMemoryPool()->GetSystemFreeCount();

	PrintDOSObjectStatLog("内存池分配数:%u",AllocCount);
	PrintDOSObjectStatLog("内存池释放数:%u,%u",FreeCount,AllocCount-FreeCount);
	PrintDOSObjectStatLog("系统内存分配数:%u",SystemAllocCount);
	PrintDOSObjectStatLog("系统内存释放数:%u,%u",SystemFreeCount,SystemAllocCount-SystemFreeCount);
	FUNCTION_END;
}
