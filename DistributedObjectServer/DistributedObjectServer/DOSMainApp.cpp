// DBProxy.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
//#include "DOSMainApp.h"



LPCTSTR g_ProgramName="DistributedObjectServer";
LPCTSTR g_ServiceName="DOSServer";
LPCTSTR g_ServiceDesc="Distributed Object Server";

// CMainApp

// CMainApp 构造

CDOSMainApp::CDOSMainApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_IsConfigLoaded=false;

	//_CrtSetBreakAlloc(12656);
}


// 唯一的一个 CMainApp 对象

CDOSMainApp theApp;


// CMainApp 初始化

//BOOL CMainApp::InitInstance()
//{
//	// 如果一个运行在 Windows XP 上的应用程序清单指定要
//	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
//	//则需要 InitCommonControls()。否则，将无法创建窗口。
//	InitCommonControls();
//
//	CWinApp::InitInstance();
//
//	AfxEnableControlContainer();
//
//	// 标准初始化
//	// 如果未使用这些功能并希望减小
//	// 最终可执行文件的大小，则应移除下列
//	// 不需要的特定初始化例程
//	// 更改用于存储设置的注册表项
//	// TODO: 应适当修改该字符串，
//	// 例如修改为公司或组织名
//	SetRegistryKey(_T("CGAServer"));
//
//	m_pConsoleDlg=new CServerConsoleDlg();
//	m_pConsoleDlg->Create(CServerConsoleDlg::IDD);
//	m_pConsoleDlg->RegisterTrayIcon(g_ProgramName);
//	m_pMainWnd=m_pConsoleDlg;
//
//	if(!CMainThread::GetInstance()->StartUp())
//		return FALSE;
//
//
//	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
//	// 而不是启动应用程序的消息泵。
//	return TRUE;
//}
//
//int CMainApp::ExitInstance()
//{
//
//	CMainThread::GetInstance()->ShutDown(MAX_SERVER_TERMINATE_WAIT_TIME);
//	SAFE_DELETE(m_pConsoleDlg);
//	return 0;
//}

BOOL CDOSMainApp::OnStartUp()
{
	CServerApp::OnStartUp();

	PrintImportantLog("DistributedObjectServer Start");
#ifdef USE_MONITORED_NEW
	CMemoryAllocatee::GetInstance()->SetLogDir(CFileTools::GetModulePath(NULL) + _T("/Log/"));
#endif


	CPerformanceStatistician::GetInstance()->ResetPerformanceStat();


	if(!m_IsConfigLoaded)
	{
		CSystemConfig::GetInstance()->LoadConfig(CFileTools::MakeModuleFullPath(NULL,CDOSMainThread::GetInstance()->GetConfigFileName()));
		m_IsConfigLoaded=CDOSConfig::GetInstance()->LoadConfig(CFileTools::MakeModuleFullPath(NULL,CONFIG_FILE_NAME));
	}
#ifdef WIN32
	UINT Flag=EXCEPTION_SET_DEFAULT_HANDLER|EXCEPTION_USE_API_HOOK|EXCEPTION_MAKE_DUMP;
	if(CSystemConfig::GetInstance()->IsPreLoadModuleSym())
		Flag|=EXCEPTION_PRE_INIT_SYM;
	if(CSystemConfig::GetInstance()->IsMakeFullDump())
		Flag|=EXCEPTION_MAKE_FULL_DUMP;
	if (CSystemConfig::GetInstance()->IsLogModuleSymStatus())
		Flag |= EXCEPTION_LOG_MODULE_SYM_STATUS;

	CExceptionParser::GetInstance()->Init(Flag);
#endif

	if(!m_IsConfigLoaded)
		return FALSE;

	if (CDOSMainThread::GetInstance()->StartUp(
		CDOSConfig::GetInstance()->GetNetConfig().EventObjectPool.StartSize,
		CDOSConfig::GetInstance()->GetNetConfig().NetWorkThreadCount,
		CDOSConfig::GetInstance()->GetNetConfig().EventRouterPool.StartSize,
		CDOSConfig::GetInstance()->GetNetConfig().EventObjectPool.GrowSize,
		CDOSConfig::GetInstance()->GetNetConfig().EventObjectPool.GrowLimit,
		CDOSConfig::GetInstance()->GetNetConfig().EventRouterPool.GrowSize,
		CDOSConfig::GetInstance()->GetNetConfig().EventRouterPool.GrowLimit))
	{
		PrintImportantLog("网络服务已启动:NetWorkThreadCount=%u,EventObjectPool=(%u,%u,%u),EventRouterPool=(%u,%u,%u)",
			CDOSConfig::GetInstance()->GetNetConfig().NetWorkThreadCount, 
			CDOSConfig::GetInstance()->GetNetConfig().EventObjectPool.StartSize,
			CDOSConfig::GetInstance()->GetNetConfig().EventObjectPool.GrowSize,
			CDOSConfig::GetInstance()->GetNetConfig().EventObjectPool.GrowLimit,
			CDOSConfig::GetInstance()->GetNetConfig().EventRouterPool.StartSize,
			CDOSConfig::GetInstance()->GetNetConfig().EventRouterPool.GrowSize,
			CDOSConfig::GetInstance()->GetNetConfig().EventRouterPool.GrowLimit
			);
	}
	else
	{
		PrintImportantLog("网络服务启动失败");
		return FALSE;
	}
		
	m_pServer=CDOSMainThread::GetInstance();
	return TRUE;
}
void CDOSMainApp::OnShutDown()
{	
	CDOSMainThread::GetInstance()->ShutDown(MAX_SERVER_TERMINATE_WAIT_TIME);
	PrintImportantLog("服务完成关闭");

	CDOSConfig::ReleaseInstance();
	CDOSMainThread::ReleaseInstance();	
}

void CDOSMainApp::OnSetServiceName()
{
	PrintImportantLog("OnSetServiceName");
	if(!m_IsConfigLoaded)
	{
		CSystemConfig::GetInstance()->LoadConfig(CFileTools::MakeModuleFullPath(NULL,CDOSMainThread::GetInstance()->GetConfigFileName()));
		m_IsConfigLoaded=CDOSConfig::GetInstance()->LoadConfig(CFileTools::MakeModuleFullPath(NULL,CONFIG_FILE_NAME));
	}
	if(m_IsConfigLoaded&&CDOSConfig::GetInstance()->HaveServiceName())
	{
#ifdef WIN32
		m_lpServiceName=CDOSConfig::GetInstance()->GetServiceName();
		m_lpDisplayName=CDOSConfig::GetInstance()->GetServiceDesc();
		PrintImportantLog("服务名设置为:%s/%s",m_lpServiceName,m_lpDisplayName);
#endif
	}
}
