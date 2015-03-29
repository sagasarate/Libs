#include "StdAfx.h"

#define SERVER_STAT_TIME		(5*60*1000)

IMPLEMENT_CLASS_INFO_STATIC(CMainThread,CDOSServerThread);

CMainThread::CMainThread(void)
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

CMainThread::~CMainThread(void)
{
}

BOOL CMainThread::OnStart()
{
	FUNCTION_BEGIN;

	

	

	CDOSServerThread::SetConfig(CMainConfig::GetInstance()->GetDOSConfig());

	if(!CDOSServerThread::OnStart())
		return FALSE;

	

	if(!m_PluginObjectManager.Init(GetObjectManager(),CMainConfig::GetInstance()->GetMaxPluginObject()))
	{
		Log("初始化插件对象管理器失败");
		return FALSE;
	}

	if(!LoadPlugins())
		return FALSE;


	m_ESFactionList.AddCFunction("ShowObjectCount",0,this,&CMainThread::ShowObjectCount);
	m_ESFactionList.AddCFunction("ShowGroupInfo",0,this,&CMainThread::ShowGroupInfo);
	m_ESFactionList.AddCFunction("ListPlugin",0,this,&CMainThread::ListPlugin);
	m_ESFactionList.AddCFunction("ReleasePlugin",1,this,&CMainThread::ReleasePlugin);

	return TRUE;
	FUNCTION_END;
	return FALSE;
}

int CMainThread::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	int Process=0;

	Process+=CDOSServerThread::Update(SERVER_UPDATE_LIMIT);

	UINT PluginID;
	while(m_PluginUnloadQueue.PopFront(PluginID))
	{
		FreePlugin(PluginID);
	}

	if(m_PluginList.GetCount()==0)
	{
		Log("CMainThread::Update:已无活动插件存在，服务器关闭");
		QueryShowDown();
	}

	return Process;
	FUNCTION_END;
	return 1;
}

void CMainThread::OnBeginTerminate()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

BOOL CMainThread::OnTerminating()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
	return false;
}

void CMainThread::OnTerminate()
{
	FUNCTION_BEGIN;		

	CDOSServerThread::OnTerminate();
	
	m_PluginUnloadQueue.Destory();
	m_PluginObjectManager.Destory();	

	FreePlugins();

	FUNCTION_END;
}



int CMainThread::GetClientCount()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
	return 0;
}

LPCTSTR CMainThread::GetConfigFileName()
{
	FUNCTION_BEGIN;
	return CONFIG_FILE_NAME;
	FUNCTION_END;
	return "";
}

bool CMainThread::QueryFreePlugin(UINT PluginID)
{
	return m_PluginUnloadQueue.PushBack(PluginID);
}

BOOL CMainThread::LoadPlugins()
{
	FUNCTION_BEGIN;

	

	Log("一共有%u个插件",CMainConfig::GetInstance()->GetPluginCount());
	UINT LogChannel=PLUGIN_LOG_CHANNEL_START;
	for(UINT i=0;i<CMainConfig::GetInstance()->GetPluginCount();i++)
	{
		PLUGIN_INFO PluginInfo;

		PluginInfo.ID=i+1;
#ifdef WIN32
		PluginInfo.PluginFileName=MakeModuleFullPath(NULL,CMainConfig::GetInstance()->GetPluginFileName(i));
#else
		PluginInfo.PluginFileName=CMainConfig::GetInstance()->GetPluginFileName(i);
		PluginInfo.PluginFileName.Trim();		
		PluginInfo.PluginFileName.Replace('\\','/');		
		if(PluginInfo.PluginFileName[0]!='/')
		{
			PluginInfo.PluginFileName=MakeModuleFullPath(NULL,PluginInfo.PluginFileName);
		}
		int Pos=PluginInfo.PluginFileName.ReverseFind('.');
		if(Pos>=0)
		{
			PluginInfo.PluginFileName=PluginInfo.PluginFileName.Left(Pos)+".so";
		}
#endif
		
		Log("开始装载插件%s",(LPCTSTR)PluginInfo.PluginFileName);
#ifdef WIN32
		PluginInfo.hModule=LoadLibrary(PluginInfo.PluginFileName);
#else
		PluginInfo.hModule=dlopen(PluginInfo.PluginFileName,RTLD_NOW|RTLD_DEEPBIND);
#endif
		if(PluginInfo.hModule)
		{
#ifdef WIN32
			if(CSystemConfig::GetInstance()->IsPreLoadModuleSym())
			{
				CExceptionParser::GetInstance()->SymLoadFromModule(PluginInfo.PluginFileName);
			}
			PluginInfo.pInitFN=(PLUGIN_INIT_FN)GetProcAddress(PluginInfo.hModule,PLUGIN_INIT_FN_NAME);
			PluginInfo.pQueryReleaseFN=(PLUGIN_QUERY_RELEASE_FN)GetProcAddress(PluginInfo.hModule,PLUGIN_QUERY_RELEASE_FN_NAME);
			PluginInfo.pReleaseFN=(PLUGIN_RELEASE_FN)GetProcAddress(PluginInfo.hModule,PLUGIN_RELEASE_FN_NAME);
			if(PluginInfo.pInitFN&&PluginInfo.pReleaseFN)
#else
			dlerror();
			PluginInfo.pInitFN=(PLUGIN_INIT_FN)dlsym (PluginInfo.hModule,PLUGIN_INIT_FN_NAME);
			LPCTSTR InitFNError=dlerror();
			PluginInfo.pQueryReleaseFN=(PLUGIN_QUERY_RELEASE_FN)dlsym (PluginInfo.hModule,PLUGIN_QUERY_RELEASE_FN_NAME);
			LPCTSTR QueryReleaseFNError=dlerror();
			PluginInfo.pReleaseFN=(PLUGIN_RELEASE_FN)dlsym (PluginInfo.hModule,PLUGIN_RELEASE_FN_NAME);
			LPCTSTR ReleaseFNError=dlerror();
			if(InitFNError==NULL&&ReleaseFNError==NULL&&PluginInfo.pInitFN&&PluginInfo.pReleaseFN)
#endif			
			{
				CEasyString LogFileName;
				CEasyString ModulePath=GetModulePath(NULL);

				CServerLogPrinter * pLog;

				LogFileName.Format("%s/Log/Plugin.%s",(LPCTSTR)ModulePath,(LPCTSTR)GetPathFileName(PluginInfo.PluginFileName));
				pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_CONSOLE|CServerLogPrinter::LOM_FILE,
					CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
				CLogManager::GetInstance()->AddChannel(LogChannel,pLog);
				SAFE_RELEASE(pLog);

				if((*(PluginInfo.pInitFN))(&m_PluginObjectManager,PluginInfo.ID,LogChannel))
				{
					m_PluginList.Add(PluginInfo);
					Log("插件装载成功%s",(LPCTSTR)PluginInfo.PluginFileName);
				}
				else
				{
					Log("插件初始化失败%s",(LPCTSTR)PluginInfo.PluginFileName);
				}

				LogChannel++;
			}
			else
			{
				Log("不合法的插件%s",(LPCTSTR)PluginInfo.PluginFileName);
#ifndef WIN32
				Log("InitFN错误信息:%s",InitFNError);
				Log("ReleaseFN错误信息:%s",ReleaseFNError);
#endif
			}
		}
		else
		{
			Log("无法装载插件(%d)%s",GetLastError(),(LPCTSTR)PluginInfo.PluginFileName);
#ifndef WIN32
			Log("错误信息:%s",dlerror());
#endif
		}

	}

	

	m_PluginUnloadQueue.Create(m_PluginList.GetCount());

	Log("插件装载完毕");

#ifdef WIN32
	CExceptionParser::GetInstance()->OnFinishModuleLoad();
#endif

	return TRUE;
	FUNCTION_END;
	return FALSE;
}

void CMainThread::FreePlugins()
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<m_PluginList.GetCount();i++)
	{
		(*(m_PluginList[i].pReleaseFN))();
#ifdef WIN32
		if(FreeLibrary(m_PluginList[i].hModule))
#else
		if(dlclose(m_PluginList[i].hModule)==0)
#endif
		{
			Log("插件释放成功%s",(LPCTSTR)m_PluginList[i].PluginFileName);
		}
		else
		{
			Log("插件释放失败%s",(LPCTSTR)m_PluginList[i].PluginFileName);
#ifndef WIN32
			Log("错误信息:%s",dlerror());
#endif
		}
	}

	FUNCTION_END;
}

bool CMainThread::FreePlugin(UINT PluginID)
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<m_PluginList.GetCount();i++)
	{
		if(m_PluginList[i].ID==PluginID)
		{
			(*(m_PluginList[i].pReleaseFN))();
#ifdef WIN32
			if(FreeLibrary(m_PluginList[i].hModule))
#else
			if(dlclose(m_PluginList[i].hModule)==0)
#endif
			{
				Log("插件释放成功%s",(LPCTSTR)m_PluginList[i].PluginFileName);
				m_PluginList.Delete(i);
				return true;
			}
			else
			{
				Log("插件释放失败%s",(LPCTSTR)m_PluginList[i].PluginFileName);
#ifndef WIN32
				Log("错误信息:%s",dlerror());
#endif
				m_PluginList.Delete(i);
				return false;
			}			
		}
	}

	Log("插件%u未找到",PluginID);
	FUNCTION_END;
	return false;
}

int CMainThread::ShowObjectCount(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	GetDistributedObjectManager()->PrintObjectCount();
	FUNCTION_END;
	return 0;
}

int  CMainThread::ShowGroupInfo(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	GetObjectManager()->PrintGroupInfo(SERVER_LOG_CHANNEL);
	FUNCTION_END;
	return 0;
}

int CMainThread::ListPlugin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	for(size_t i=0;i<m_PluginList.GetCount();i++)
	{
		Log("ID=%u,File=%s",m_PluginList[i].ID,(LPCTSTR)m_PluginList[i].PluginFileName);
	}
	FUNCTION_END;
	return 0;
}
int CMainThread::ReleasePlugin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	UINT PluginID=pParams[0];
	for(size_t i=0;i<m_PluginList.GetCount();i++)
	{
		if(m_PluginList[i].ID==PluginID)
		{
			if(m_PluginList[i].pQueryReleaseFN)
			{
				(*(m_PluginList[i].pQueryReleaseFN))();
			}
			else
			{
				Log("插件%u没有释放请求函数",PluginID);
			}
			return 0;
		}
	}
	Log("插件%u未找到",PluginID);
	FUNCTION_END;
	return 0;
}