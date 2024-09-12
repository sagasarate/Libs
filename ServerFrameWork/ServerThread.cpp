/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerThread.cpp                                         */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"
#include <malloc.h>


IMPLEMENT_CLASS_INFO_STATIC(CServerThread,CNetServer);

CServerThread::CServerThread()
{
	FUNCTION_BEGIN
	m_pSysNetLinkManager=NULL;
	m_pUDPSystemControlPort=NULL;
	m_pSystemControlPipe = NULL;
	FUNCTION_END;
}

CServerThread::~CServerThread(void)
{
	FUNCTION_BEGIN

	FUNCTION_END;
}

void CServerThread::Execute()
{
	FUNCTION_BEGIN;



	bool EnableGuardThread = CSystemConfig::GetInstance()->GetEnableGuardThread();
	if (EnableGuardThread)
	{
		m_GuardThread.SetTargetThread(this);
		m_GuardThread.SetKeepAliveTime(
			CSystemConfig::GetInstance()->GetGuardThreadKeepAliveTime(),
			CSystemConfig::GetInstance()->GetGuardThreadKeepAliveCount());
		m_GuardThread.Start();
	}

	while((!m_WantTerminate)&&(OnRun()))
	{
		if (EnableGuardThread)
			m_GuardThread.MakeKeepAlive();
	}

	if (EnableGuardThread)
		m_GuardThread.SafeTerminate();

	OnBeginTerminate();
	DWORD Time=CEasyTimer::GetTime();
	while (CEasyTimer::GetTimeToTime(Time, CEasyTimer::GetTime()) < SERVER_ENDING_TIME && OnTerminating())
	{
	}

	FUNCTION_END;
}


BOOL CServerThread::OnStart()
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
	CSystemConfig::GetInstance()->LoadConfig(CFileTools::MakeModuleFullPath(NULL,GetConfigFileName()));

#ifndef WIN32
	if (CSystemConfig::GetInstance()->GetMallocConfig().bSetTrimThreshold)
	{
		mallopt(M_TRIM_THRESHOLD, CSystemConfig::GetInstance()->GetMallocConfig().TrimThreshold);
		PrintImportantLog("Set M_TRIM_THRESHOLD=%d", CSystemConfig::GetInstance()->GetMallocConfig().TrimThreshold);
	}
	if (CSystemConfig::GetInstance()->GetMallocConfig().bSetMMapThreshold)
	{
		mallopt(M_MMAP_THRESHOLD, CSystemConfig::GetInstance()->GetMallocConfig().MMapThreshold);
		PrintImportantLog("Set M_MMAP_THRESHOLD=%d", CSystemConfig::GetInstance()->GetMallocConfig().MMapThreshold);
	}
	if (CSystemConfig::GetInstance()->GetMallocConfig().bSetMMapMax)
	{
		mallopt(M_MMAP_MAX, CSystemConfig::GetInstance()->GetMallocConfig().MMapMax);
		PrintImportantLog("Set M_MMAP_MAX=%d", CSystemConfig::GetInstance()->GetMallocConfig().MMapMax);
	}
	m_MallocTimer.SetTimeOut(CSystemConfig::GetInstance()->GetMallocConfig().TrimInterval);
#endif

	CEasyString LogFileName;
	CEasyString ModulePath = CFileTools::GetModulePath(NULL);

	CServerLogPrinter * pLog;



	LogFileName.Format("%s/Log/%s",(LPCTSTR)ModulePath,g_ProgramName);
	pLog = MONITORED_NEW(_T("CServerThread"), CServerLogPrinter, this, CServerLogPrinter::LOM_CONSOLE | CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(), LogFileName, CSystemConfig::GetInstance()->GetLogCacheSize());
	pLog->SetBackup(CSystemConfig::GetInstance()->GetLogBackupDir(), CSystemConfig::GetInstance()->GetLogBackupDelay());
	CLogManager::GetInstance()->AddChannel(SERVER_LOG_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	SetConsoleLogLevel(CSystemConfig::GetInstance()->GetConsoleLogLevel());

	LogFileName.Format("%s/Log/%s.Status",(LPCTSTR)ModulePath,g_ProgramName);
	CCSVFileLogPrinter * pCSVLog = MONITORED_NEW(_T("CServerThread"), CCSVFileLogPrinter, CSystemConfig::GetInstance()->GetLogLevel(), LogFileName,
		"CycleTime,CPUUsed,TCPRecvFlow,TCPSendFlow,UDPRecvFlow,UDPSendFlow,"
		"TCPRecvCount,TCPSendCount,UDPRecvCount=,UDPSendCount,ClientCount", CSystemConfig::GetInstance()->GetLogCacheSize());
	pCSVLog->SetBackup(CSystemConfig::GetInstance()->GetLogBackupDir(), CSystemConfig::GetInstance()->GetLogBackupDelay());
	CLogManager::GetInstance()->AddChannel(SERVER_STATUS_LOG_CHANNEL,pCSVLog);
	SAFE_RELEASE(pCSVLog);



	LogFileName.Format("%s/Log/%s.NetLib",(LPCTSTR)ModulePath,g_ProgramName);
	pLog = MONITORED_NEW(_T("CServerThread"), CServerLogPrinter, this, CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(), LogFileName, CSystemConfig::GetInstance()->GetLogCacheSize());
	pLog->SetBackup(CSystemConfig::GetInstance()->GetLogBackupDir(), CSystemConfig::GetInstance()->GetLogBackupDelay());
	CLogManager::GetInstance()->AddChannel(LOG_NET_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	LogFileName.Format("%s/Log/%s.DBLib",(LPCTSTR)ModulePath,g_ProgramName);
	pLog = MONITORED_NEW(_T("CServerThread"), CServerLogPrinter, this, CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(), LogFileName, CSystemConfig::GetInstance()->GetLogCacheSize());
	pLog->SetBackup(CSystemConfig::GetInstance()->GetLogBackupDir(), CSystemConfig::GetInstance()->GetLogBackupDelay());
	CLogManager::GetInstance()->AddChannel(LOG_DB_ERROR_CHANNEL,pLog);
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


	m_ESFactionList.AddCFunction("RebuildUDPControlPort",0,this,&CServerThread::RebuildUDPControlPort);
	m_ESFactionList.AddCFunction("SetConsoleLogLevel",1,this,&CServerThread::SFSetConsoleLogLevel);

	if(!CNetServer::OnStart())
		return FALSE;

	//初始化系统连接
	m_pSysNetLinkManager = MONITORED_NEW(_T("CServerThread"), CSystemNetLinkManager);
	m_pSysNetLinkManager->SetServerThread(this);

	xml_parser Parser;

	if (Parser.parse_file(CFileTools::MakeModuleFullPath(NULL, GetConfigFileName()), pug::parse_trim_attribute))
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

	if (CSystemConfig::GetInstance()->GetUDPControlAddress().GetPort())
	{
		m_pUDPSystemControlPort = MONITORED_NEW(_T("CServerThread"), CSystemControlPort);
		if (!m_pUDPSystemControlPort->Init(this))
		{
			Log("初始化UDP系统控制端口失败");
		}
	}

	if (CSystemConfig::GetInstance()->IsControlPipeEnable())
	{
		m_pSystemControlPipe = MONITORED_NEW(_T("CServerThread"), CSystemControlPipe);
		if (!m_pSystemControlPipe->Init(this))
		{
			Log("初始化系统控制管道失败");
		}
	}

	

	UINT64_CONVERTER Version;
	memcpy(Version.Words, g_ProgramVersion, sizeof(UINT64_CONVERTER));

	SetServerStatus(SC_SST_SS_PROGRAM_VERSION,CSmartValue(Version.QuadPart));
	SetServerStatusFormat(SC_SST_SS_PROGRAM_VERSION,"服务器版本",SSFT_VERSION);
	SetServerStatusFormat(SC_SST_SS_WORK_STATUS, "工作状态");
	SetServerStatus(SC_SST_SS_WORK_STATUS, CSmartValue((BYTE)SERVER_WORK_STATUS_STARTUP));
	SetServerStatusFormat(SC_SST_SS_CLIENT_COUNT,"客户端数量");
	SetServerStatusFormat(SC_SST_SS_CYCLE_TIME,"循环时间(毫秒)");
	SetServerStatusFormat(SC_SST_SS_CPU_COST,"CPU占用率",SSFT_PERCENT);
	SetServerStatusFormat(SC_SST_SS_TCP_RECV_FLOW,"TCP接收流量(Byte/S)",SSFT_FLOW);
	SetServerStatusFormat(SC_SST_SS_TCP_SEND_FLOW,"TCP发送流量(Byte/S)",SSFT_FLOW);
	SetServerStatusFormat(SC_SST_SS_UDP_RECV_FLOW,"UDP接收流量(Byte/S)",SSFT_FLOW);
	SetServerStatusFormat(SC_SST_SS_UDP_SEND_FLOW,"UDP发送流量(Byte/S)",SSFT_FLOW);
	SetServerStatusFormat(SC_SST_SS_TCP_RECV_COUNT,"TCP接收次数(次/S)");
	SetServerStatusFormat(SC_SST_SS_TCP_SEND_COUNT,"TCP发送次数(次/S)");
	SetServerStatusFormat(SC_SST_SS_UDP_RECV_COUNT,"UDP接收次数(次/S)");
	SetServerStatusFormat(SC_SST_SS_UDP_SEND_COUNT,"UDP发送次数(次/S)");

	m_ThreadPerformanceCounter.Init(GetThreadHandle(),SERVER_INFO_COUNT_TIME);



	Log("服务器成功启动");

	FUNCTION_END;
	return TRUE;
}

void CServerThread::OnBeginTerminate()
{

}

void CServerThread::OnTerminate()
{
	FUNCTION_BEGIN;

	SAFE_RELEASE(m_pSysNetLinkManager);
	SAFE_RELEASE(m_pUDPSystemControlPort);
	SAFE_RELEASE(m_pSystemControlPipe);
	CNetServer::OnTerminate();
	Log("服务器关闭");
	FUNCTION_END;
}

BOOL CServerThread::OnRun()
{
	FUNCTION_BEGIN;

	if(!CNetServer::OnRun())
		return FALSE;

	if(Update(CSystemConfig::GetInstance()->GetMainThreadProcessLimit())==0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
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

int CServerThread::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	int Process=0;
	Process+=CNetServer::Update(ProcessPacketLimit);
	Process+=m_pSysNetLinkManager->Update(ProcessPacketLimit);
	if (m_pUDPSystemControlPort)
		Process += m_pUDPSystemControlPort->Update(ProcessPacketLimit);
	if (m_pSystemControlPipe)
		Process += m_pSystemControlPipe->Update(ProcessPacketLimit);


#ifndef WIN32
	if (CSystemConfig::GetInstance()->GetMallocConfig().TrimMemory)
	{
		if (m_MallocTimer.IsTimeOut())
		{
			m_MallocTimer.SetTimeOut(CSystemConfig::GetInstance()->GetMallocConfig().TrimInterval);
			malloc_trim(0);
			Log("已执行内存整理");
		}
	}
#endif
	return Process;
	FUNCTION_END;
	return 0;
}

BOOL CServerThread::OnTerminating()
{
	return FALSE;
}


int CServerThread::GetClientCount()
{
	return 0;
}

LPCTSTR CServerThread::GetConfigFileName()
{
	return SYSTEM_NET_LINK_CONFIG_FILE_NAME;
}

bool CServerThread::PrintConsoleLog(int Level, LPCTSTR szLogMsg)
{
	CBaseServer::PrintConsoleLog(Level, szLogMsg);
	if (m_ConsoleLogLevel&Level)
	{
		if (m_pSysNetLinkManager)
		{
			m_pSysNetLinkManager->SendLogMsg(szLogMsg);
		}
		if (m_pSystemControlPipe)
		{
			m_pSystemControlPipe->SendLogMsg(szLogMsg);
		}
	}
	return true;
}

bool CServerThread::ExecCommand(LPCTSTR szCommand)
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
		return false;
	}
	RetCode=m_ScriptExecutor.ExecScript(m_ESThread);
	if(RetCode)
	{
		Log("解析命令出错:Line=%d,%s",
			m_ESThread.GetLastLine(),
			ESGetErrorMsg(RetCode));
		return false;
	}
	else
	{
		Log("执行命令结果:%s",
			(LPCTSTR)BolanToString(m_ESThread.GetResult()));
		return true;
	}
	FUNCTION_END;
	return false;
}



void CServerThread::QueryShowDown()
{
	FUNCTION_BEGIN;
	Log("服务器准备关闭");
	Terminate();
	FUNCTION_END;
}

bool CServerThread::IsServerTerminated()
{
	FUNCTION_BEGIN;
	return IsTerminated()!=FALSE;
	FUNCTION_END;
	return true;
}

int CServerThread::RebuildUDPControlPort(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;

	if (m_pUDPSystemControlPort)
	{
		if (!m_pUDPSystemControlPort->Init(this))
		{
			Log("重建UDP系统控制端口失败");
		}
		else
		{
			Log("重建UDP系统控制端口成功");
		}
	}
	else
	{
		Log("未配置UDP系统控制端口");
	}

	FUNCTION_END;
	return 0;
}

int CServerThread::SFSetConsoleLogLevel(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;

	SetConsoleLogLevel(pParams[0]);
	Log("控制台Log输出等级设置为%d",GetConsoleLogLevel());

	FUNCTION_END;
	return 0;
}

void CServerThread::DoServerStat()
{
	FUNCTION_BEGIN;
	int ClientCount=GetClientCount();
	float CycleTime=m_ThreadPerformanceCounter.GetCycleTime();
	float CPUCost=m_ThreadPerformanceCounter.GetCPUUsedRate();
	float TCPRecvFlow=(float)m_TCPRecvBytes*1000/SERVER_INFO_COUNT_TIME;
	float TCPSendFlow=(float)m_TCPSendBytes*1000/SERVER_INFO_COUNT_TIME;
	float UDPRecvFlow=(float)m_UDPRecvBytes*1000/SERVER_INFO_COUNT_TIME;
	float UDPSendFlow=(float)m_UDPSendBytes*1000/SERVER_INFO_COUNT_TIME;

	float TCPRecvCount=(float)m_TCPRecvCount*1000/SERVER_INFO_COUNT_TIME;
	float TCPSendCount=(float)m_TCPSendCount*1000/SERVER_INFO_COUNT_TIME;
	float UDPRecvCount=(float)m_UDPRecvCount*1000/SERVER_INFO_COUNT_TIME;
	float UDPSendCount=(float)m_UDPSendCount*1000/SERVER_INFO_COUNT_TIME;



	CLogManager::GetInstance()->PrintLog(SERVER_STATUS_LOG_CHANNEL,ILogPrinter::LOG_LEVEL_NORMAL,0,
		"%g,%g,%s,%s,%s,%s,%g,%g,%g,%g,%u",
		CycleTime,
		CPUCost,
		(LPCTSTR)FormatNumberWordsFloat(TCPRecvFlow,true),
		(LPCTSTR)FormatNumberWordsFloat(TCPSendFlow,true),
		(LPCTSTR)FormatNumberWordsFloat(UDPRecvFlow,true),
		(LPCTSTR)FormatNumberWordsFloat(UDPSendFlow,true),
		TCPRecvCount,
		TCPSendCount,
		UDPRecvCount,
		UDPSendCount,
		ClientCount);

	SetServerStatus(SC_SST_SS_CLIENT_COUNT,CSmartValue(ClientCount));
	SetServerStatus(SC_SST_SS_CYCLE_TIME,CSmartValue(CycleTime));
	SetServerStatus(SC_SST_SS_CPU_COST,CSmartValue(CPUCost));
	SetServerStatus(SC_SST_SS_TCP_RECV_FLOW,CSmartValue(TCPRecvFlow));
	SetServerStatus(SC_SST_SS_TCP_SEND_FLOW,CSmartValue(TCPSendFlow));
	SetServerStatus(SC_SST_SS_UDP_RECV_FLOW,CSmartValue(UDPRecvFlow));
	SetServerStatus(SC_SST_SS_UDP_SEND_FLOW,CSmartValue(UDPSendFlow));
	SetServerStatus(SC_SST_SS_TCP_RECV_COUNT,CSmartValue(TCPRecvCount));
	SetServerStatus(SC_SST_SS_TCP_SEND_COUNT,CSmartValue(TCPSendCount));
	SetServerStatus(SC_SST_SS_UDP_RECV_COUNT,CSmartValue(UDPRecvCount));
	SetServerStatus(SC_SST_SS_UDP_SEND_COUNT,CSmartValue(UDPSendCount));

	ResetFluxStat();

	if(CSystemConfig::GetInstance()->IsLogServerObjectUse())
	{
		PrintObjectStatus();
	}
	FUNCTION_END;
}
