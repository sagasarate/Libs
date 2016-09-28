#include "stdafx.h"



CDOSRouterLinkManager::CDOSRouterLinkManager()
{
	m_pServer = NULL;
	m_pRouter = NULL;
}


CDOSRouterLinkManager::~CDOSRouterLinkManager()
{
}


bool CDOSRouterLinkManager::Init(CDOSServer * pServer)
{
	m_pServer = pServer;
	if (m_pServer == NULL)
		return false;

	m_pRouter = m_pServer->GetRouter();

	if (m_pRouter == NULL)
		return false;
	
	m_RouterLinkConfig = m_pServer->GetConfig().RouterLinkConfig;
	//将连接ID都设置为RouterID
	m_RouterLinkConfig.ServerID = m_pServer->GetConfig().RouterID;
	for (UINT i = 0; i < m_RouterLinkConfig.ServiceConfig.ServiceList.GetCount(); i++)
	{
		m_RouterLinkConfig.ServiceConfig.ServiceList[i].ReportID = m_RouterLinkConfig.ServerID;
	}
	for (UINT i = 0; i < m_RouterLinkConfig.ConnectionConfig.ConnectionList.GetCount(); i++)
	{
		m_RouterLinkConfig.ConnectionConfig.ConnectionList[i].ReportID = m_RouterLinkConfig.ServerID;
	}

	return Start() != FALSE;
}

BOOL CDOSRouterLinkManager::OnStart()
{
	m_ThreadPerformanceCounter.Init(GetThreadHandle(), THREAD_CPU_COUNT_TIME);

	

	if (!CEasyNetLinkManager::Init(m_pServer, m_RouterLinkConfig))
	{
		PrintDOSLog(_T("连接管理器初始化失败！"));
		return FALSE;
	}

	PrintDOSLog(_T("路由连接管理器[%u]已启动"), GetThreadID());
	return TRUE;
}


BOOL CDOSRouterLinkManager::OnRun()
{
	int ProcessCount = Update();
	if (ProcessCount == 0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}
	return TRUE;
}

void CDOSRouterLinkManager::OnTerminate()
{
	CEasyNetLinkManager::Destory();
}

int CDOSRouterLinkManager::Update(int ProcessPacketLimit)
{
	m_ThreadPerformanceCounter.DoPerformanceCount();

	CAutoLock Lock(m_EasyCriticalSection);
	int ProcessCount = CEasyNetLinkManager::Update();	
	return ProcessCount;
}

CEasyNetLink * CDOSRouterLinkManager::OnCreateLink(UINT ID)
{
	return new CDOSRouterLink();
}

void CDOSRouterLinkManager::OnLinkStart(CEasyNetLink * pConnection)
{

}

void CDOSRouterLinkManager::OnLinkEnd(CEasyNetLink * pConnection)
{

}

bool CDOSRouterLinkManager::SendMessage(UINT RouterID, CDOSMessagePacket * pPacket)
{
	CAutoLock Lock(m_EasyCriticalSection);

	CDOSRouterLink * pRouterLink = dynamic_cast<CDOSRouterLink *>(GetLink(RouterID));
	if (pRouterLink)
	{
		pRouterLink->SendData(pPacket->GetPacketBuffer(), pPacket->GetPacketLength());
		return true;
	}
	else
	{
		PrintDOSLog(_T("无法找到路由%u！"), RouterID);
		return false;
	}
}

UINT CDOSRouterLinkManager::BroadcastMessage(CDOSMessagePacket * pPacket)
{
	CAutoLock Lock(m_EasyCriticalSection);

	UINT Count = 0;
	for (UINT i = 0; i < GetLinkCount(); i++)
	{
		CDOSRouterLink * pRouterLink = dynamic_cast<CDOSRouterLink *>(GetLinkByIndex(i));
		if (pRouterLink)
		{
			pRouterLink->SendData(pPacket->GetPacketBuffer(), pPacket->GetPacketLength());
			Count++;
		}
	}
	return Count;
}

