#include "stdafx.h"



CDOSObjectProxyServiceCustom::CDOSObjectProxyServiceCustom()
{
	m_pServer = NULL;
	m_pProxyService = NULL;
}


CDOSObjectProxyServiceCustom::~CDOSObjectProxyServiceCustom()
{
}

void CDOSObjectProxyServiceCustom::Release()
{
	CNameObject::Release();
}
void CDOSObjectProxyServiceCustom::Destory()
{

	SAFE_RELEASE(m_pProxyService);


#ifdef WIN32
	if (FreeLibrary(m_PluginInfo.hModule))
#else
	if (dlclose(m_PluginInfo.hModule) == 0)
#endif
	{
		Log("代理插件释放成功%s", (LPCTSTR)m_PluginInfo.ModuleFileName);
	}
	else
	{
		Log("代理插件释放失败%s", (LPCTSTR)m_PluginInfo.ModuleFileName);
#ifndef WIN32
		Log("错误信息:%s", dlerror());
#endif
	}

	m_PluginInfo.hModule = NULL;
}
BYTE CDOSObjectProxyServiceCustom::GetProxyType()
{
	return m_Config.ProxyType;
}
void CDOSObjectProxyServiceCustom::SetID(UINT ID)
{
	CNameObject::SetID(ID);
}
UINT CDOSObjectProxyServiceCustom::GetID()
{
	return CNameObject::GetID();
}
bool CDOSObjectProxyServiceCustom::StartService()
{
	if (m_pProxyService)
		m_pProxyService->StartService(this);
	return false;
}
void CDOSObjectProxyServiceCustom::StopService()
{
	if (m_pProxyService)
		m_pProxyService->StopService();
}
bool CDOSObjectProxyServiceCustom::PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket)
{
	if (m_pProxyService)
		return m_pProxyService->PushMessage(ObjectID, pPacket);
	return false;
}

UINT  CDOSObjectProxyServiceCustom::GetConnectionCount()
{
	if (m_pProxyService)
		return m_pProxyService->GetConnectionCount();
	return 0;
}

float CDOSObjectProxyServiceCustom::GetCPUUsedRate()
{
	if (m_pProxyService)
		return m_pProxyService->GetCPUUsedRate();
	return 0;
}
float CDOSObjectProxyServiceCustom::GetCycleTime()
{
	if (m_pProxyService)
		return m_pProxyService->GetCycleTime();
	return 0;
}

UINT CDOSObjectProxyServiceCustom::GetGroupCount()
{
	if (m_pProxyService)
		return m_pProxyService->GetGroupCount();
	return 0;
}
float CDOSObjectProxyServiceCustom::GetGroupCPUUsedRate(UINT Index)
{
	if (m_pProxyService)
		return m_pProxyService->GetGroupCPUUsedRate(Index);
	return 0;
}
float CDOSObjectProxyServiceCustom::GetGroupCycleTime(UINT Index)
{
	if (m_pProxyService)
		return m_pProxyService->GetGroupCycleTime(Index);
	return 0;
}


OBJECT_ID CDOSObjectProxyServiceCustom::GetObjectID()
{
	return m_ObjectID;
}
const CLIENT_PROXY_CONFIG& CDOSObjectProxyServiceCustom::GetConfig()
{
	return m_Config;
}

CNetServer * CDOSObjectProxyServiceCustom::GetNetServer()
{
	return m_pServer;
}
bool CDOSObjectProxyServiceCustom::SendMessage(OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize)
{
	return m_pServer->GetRouter()->RouterMessage(m_ObjectID, ReceiverID, MsgID, MsgFlag, pData, DataSize) != FALSE;
}

bool CDOSObjectProxyServiceCustom::SendMessageMulti(OBJECT_ID * pReceiverIDList, UINT ReceiverCount, bool IsSorted, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize)
{
	if (pReceiverIDList == NULL || ReceiverCount == 0)
	{
		return false;
	}
	int PacketSize = CDOSMessagePacket::CaculatePacketLength(DataSize, ReceiverCount);

	CDOSMessagePacket * pNewPacket = m_pServer->NewMessagePacket(PacketSize);
	if (pNewPacket == NULL)
	{
		PrintDOSLog(_T("分配消息内存块失败！"));
		return false;
	}
	pNewPacket->GetMessage().SetMsgID(MsgID);
	pNewPacket->GetMessage().SetSenderID(GetObjectID());
	pNewPacket->GetMessage().SetDataLength(DataSize);
	pNewPacket->GetMessage().SetMsgFlag(MsgFlag);
	if (pData)
		memcpy(pNewPacket->GetMessage().GetDataBuffer(), pData, DataSize);
	pNewPacket->SetTargetIDs(ReceiverCount, pReceiverIDList);
	if (!IsSorted)
	{
		qsort(pNewPacket->GetTargetIDs(), ReceiverCount, sizeof(OBJECT_ID), OBJECT_ID::Compare);
	}

	pNewPacket->MakePacketLength();

	BOOL Ret = m_pServer->GetRouter()->RouterMessage(pNewPacket);
	m_pServer->ReleaseMessagePacket(pNewPacket);
	return Ret != FALSE;
}
CDOSMessagePacket * CDOSObjectProxyServiceCustom::NewMessagePacket(UINT DataSize, UINT ReceiverCount)
{
	if (ReceiverCount == 0)
	{
		return NULL;
	}
	int PacketSize = CDOSMessagePacket::CaculatePacketLength(DataSize, ReceiverCount);

	return m_pServer->NewMessagePacket(PacketSize);
}
bool CDOSObjectProxyServiceCustom::ReleaseMessagePacket(CDOSMessagePacket * pPacket)
{
	return m_pServer->ReleaseMessagePacket(pPacket) != FALSE;;
}
bool CDOSObjectProxyServiceCustom::SendMessagePacket(CDOSMessagePacket * pPacket)
{
	return m_pServer->GetRouter()->RouterMessage(pPacket) != FALSE;
}

BOOL CDOSObjectProxyServiceCustom::Init(CDOSServer * pServer, CLIENT_PROXY_PLUGIN_INFO& PluginInfo)
{
	m_pServer = pServer;
	m_Config = PluginInfo;
	m_PluginInfo = PluginInfo;

	m_ObjectID.ObjectIndex = 0;
	m_ObjectID.GroupIndex = MAKE_PROXY_GROUP_INDEX(m_Config.ProxyType);
	m_ObjectID.ObjectTypeID = DOT_PROXY_OBJECT;
	m_ObjectID.RouterID = GetRouterID();

	if (m_PluginInfo.ModuleFileName.IsEmpty())
#ifdef _DEBUG
		m_PluginInfo.ModuleFileName = m_PluginInfo.PluginName + "D";
#else
		m_PluginInfo.ModuleFileName = m_PluginInfo.PluginName;
#endif

#ifndef WIN32
	m_PluginInfo.ModuleFileName.Replace('\\', '/');
#endif
	m_PluginInfo.ModuleFileName = CFileTools::MakeModuleFullPath(NULL, m_PluginInfo.ModuleFileName);
	//扩展名补足
	CEasyString FileExt = CFileTools::GetPathFileExtName(m_PluginInfo.ModuleFileName);
	if (FileExt.GetLength() <= 1)
	{
#ifdef WIN32
		m_PluginInfo.ModuleFileName = m_PluginInfo.ModuleFileName + ".dll";
#else
		m_PluginInfo.ModuleFileName = m_PluginInfo.ModuleFileName + ".so";
#endif
	}

	if (PluginInfo.PluginName.IsEmpty())
		PluginInfo.PluginName = CFileTools::GetPathFileName(PluginInfo.ModuleFileName);

	//检查配置目录和日志目录
	if (m_PluginInfo.ConfigDir.IsEmpty())
	{
		m_PluginInfo.ConfigDir = CFileTools::GetModulePath(NULL);
	}
	else
	{
		if (CFileTools::IsAbsolutePath(m_PluginInfo.ConfigDir))
			m_PluginInfo.ConfigDir = CFileTools::MakeFullPath(m_PluginInfo.ConfigDir);
		else
			m_PluginInfo.ConfigDir = CFileTools::MakeModuleFullPath(NULL, m_PluginInfo.ConfigDir);
	}

	if (m_PluginInfo.LogDir.IsEmpty())
	{
		m_PluginInfo.LogDir = CFileTools::MakeModuleFullPath(NULL, "Log");
	}
	else
	{
		if (CFileTools::IsAbsolutePath(m_PluginInfo.LogDir))
			m_PluginInfo.LogDir = CFileTools::MakeFullPath(m_PluginInfo.LogDir);
		else
			m_PluginInfo.LogDir = CFileTools::MakeModuleFullPath(NULL, m_PluginInfo.LogDir);
	}

	if (!CFileTools::IsDirExist(m_PluginInfo.LogDir))
	{
		if (!CFileTools::CreateDirEx(m_PluginInfo.LogDir))
		{
			Log("无法创建日志目录:%s", (LPCTSTR)m_PluginInfo.LogDir);
			return false;
		}
	}

	Log("开始装载代理插件%s", (LPCTSTR)m_PluginInfo.ModuleFileName);
#ifdef WIN32
	m_PluginInfo.hModule = LoadLibrary(m_PluginInfo.ModuleFileName);
#else
	m_PluginInfo.hModule = dlopen(m_PluginInfo.ModuleFileName, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
#endif
	if (m_PluginInfo.hModule)
	{
#ifdef WIN32
		if (CSystemConfig::GetInstance()->IsPreLoadModuleSym())
		{
			CExceptionParser::GetInstance()->SymLoadFromModule(m_PluginInfo.ModuleFileName);
		}
		m_PluginInfo.pInitFN = (CLIENT_PROXY_INIT_FN)GetProcAddress(m_PluginInfo.hModule, CLIENT_PROXY_INIT_FN_NAME);
		m_PluginInfo.pGetServiceFN = (CLIENT_PROXY_GET_SERVICE_FN)GetProcAddress(m_PluginInfo.hModule, CLIENT_PROXY_GET_SERVICE_FN_NAME);
		if (m_PluginInfo.pInitFN && m_PluginInfo.pGetServiceFN)
#else
		m_PluginInfo.pInitFN = (CLIENT_PROXY_INIT_FN)dlsym(m_PluginInfo.hModule, CLIENT_PROXY_INIT_FN_NAME);
		LPCTSTR InitFNError = dlerror();
		m_PluginInfo.pGetServiceFN = (CLIENT_PROXY_GET_SERVICE_FN)dlsym(m_PluginInfo.hModule, CLIENT_PROXY_GET_SERVICE_FN_NAME);
		LPCTSTR GetServiceFNError = dlerror();
		if (InitFNError == NULL && GetServiceFNError == NULL && m_PluginInfo.pInitFN && m_PluginInfo.pGetServiceFN)
#endif
		{
			if (m_PluginInfo.pInitFN(m_PluginInfo.ID, m_PluginInfo.LogChannel, m_PluginInfo.ConfigDir, m_PluginInfo.LogDir))
			{
				m_pProxyService = m_PluginInfo.pGetServiceFN();
				if (m_pProxyService)
				{
					CEasyString LogFileName;

					CServerLogPrinter * pLog;
					LogFileName.Format("%s/Log/Plugin.%s", (LPCTSTR)m_PluginInfo.LogDir, (LPCTSTR)CFileTools::GetPathFileName(m_PluginInfo.PluginName));
					pLog = new CServerLogPrinter(CDOSMainThread::GetInstance(), CServerLogPrinter::LOM_CONSOLE | CServerLogPrinter::LOM_FILE,
						CSystemConfig::GetInstance()->GetLogLevel(), LogFileName);
					CLogManager::GetInstance()->AddChannel(m_PluginInfo.LogChannel, pLog);
					SAFE_RELEASE(pLog);


					Log("代理插件装载成功%s", (LPCTSTR)m_PluginInfo.ModuleFileName);
					m_PluginInfo.PluginStatus = PLUGIN_STATUS_RUNNING;
					return true;
				}
				else
				{
					Log("代理服务创建失败%s", (LPCTSTR)m_PluginInfo.ModuleFileName);

					return false;
				}
			}
			else
			{
				Log("代理插件初始化失败%s", (LPCTSTR)m_PluginInfo.ModuleFileName);

				return false;
			}

		}
		else
		{
			Log("不合法的代理插件%s", (LPCTSTR)m_PluginInfo.ModuleFileName);
#ifndef WIN32
			Log("InitFN错误信息:%s", InitFNError);
			Log("GetServiceFN错误信息:%s", GetServiceFNError);
#endif
		}
	}
	else
	{
		Log("无法装载代理插件(%d)%s", GetLastError(), (LPCTSTR)m_PluginInfo.ModuleFileName);
#ifndef WIN32
		Log("错误信息:%s", dlerror());
#endif
	}

	m_PluginInfo.PluginStatus = PLUGIN_STATUS_ERROR;

	return false;
}




