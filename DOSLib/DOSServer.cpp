/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSServer.cpp                                            */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CDOSServer,CNetServer);

CDOSServer::CDOSServer(void):CNetServer()
{
	FUNCTION_BEGIN;
	m_pProxyManager = NULL;
	m_pDOSRouter=NULL;
	m_pObjectManager=NULL;
	m_pRouterLinkManager = NULL;
	FUNCTION_END;
}

CDOSServer::~CDOSServer(void)
{
	FUNCTION_BEGIN;
	ShutDown();
	FUNCTION_END;
}

void CDOSServer::SetConfig(const DOS_CONFIG& Config)
{
	m_ServerConfig = Config;

	
	if (m_ServerConfig.ObjectConfig.ObjectPoolSetting.MaxSize() >= 0xFFFF)
	{
		PrintDOSLog(_T("DOS对象池的最大大小必须小于65535！"));
		m_ServerConfig.ObjectConfig.ObjectPoolSetting.LimitSize(0xFFFF - 1);
	}
	for (UINT i = 0; i < m_ServerConfig.ClientProxyConfigs.GetCount(); i++)
	{
		CLIENT_PROXY_CONFIG& ClientProxyConfig = m_ServerConfig.ClientProxyConfigs[i];
		if (ClientProxyConfig.ConnectionPoolSetting.MaxSize() >= 0xFFFF)
		{
			PrintDOSLog(_T("Proxy(%d)连接池池的最大大小必须小于65535！"), ClientProxyConfig.ProxyType);
			ClientProxyConfig.ConnectionPoolSetting.LimitSize(0xFFFF - 1);
		}
	}
}

bool CDOSServer::OnStartUp()
{
	FUNCTION_BEGIN;
	if(m_ServerConfig.MemoryPoolConfig.Enable)
	{
		if (m_MemoryPool.Create(m_ServerConfig.MemoryPoolConfig.MemoryPoolBlockSize,
			m_ServerConfig.MemoryPoolConfig.MemoryPoolLeveSize, m_ServerConfig.MemoryPoolConfig.MemoryPoolLevelCount, true))
		{
			PrintDOSDebugLog(_T("内存池初始化完毕"));
		}
		else
		{
			PrintDOSLog(_T("初始化内存池失败！"));
			return false;
		}
	}
	else
	{
		PrintDOSDebugLog(_T("内存池未启用"));
	}

	m_pDOSRouter = MONITORED_NEW(_T("CDOSServer"), CDOSRouter);
	if(!m_pDOSRouter->Init(this))
	{
		PrintDOSLog(_T("路由服务启动失败！"));
		return false;
	}
	//m_pDOSRouterService->WaitForWorking(DEFAULT_THREAD_STARTUP_TIME);
	PrintDOSLog(_T("路由服务启动！"));

	m_pObjectManager = MONITORED_NEW(_T("CDOSServer"), CDOSObjectManager);

	m_pObjectManager->SetServer(this);
	if(!m_pObjectManager->Initialize())
	{
		PrintDOSLog( _T("对象管理器初始化失败！"));
		return false;
	}

	m_pProxyManager = MONITORED_NEW(_T("CDOSServer"), CDOSProxyManager);
	if (!m_pProxyManager->Initialize(this))
	{
		PrintDOSLog(_T("代理管理器初始化失败！"));
		return false;
	}

	m_pRouterLinkManager = MONITORED_NEW(_T("CDOSServer"), CDOSRouterLinkManager);
	if (!m_pRouterLinkManager->Init(this))
	{
		PrintDOSLog(_T("路由连接管理器初始化失败！"));
		return false;
	}

	PrintDOSLog(_T("对象管理器启动！"));

	PrintDOSLog(_T("服务器(%d)启动！"),m_ServerConfig.RouterConfig.RouterID);

	return true;
	FUNCTION_END;
	return false;
}

void CDOSServer::OnShutDown()
{
	FUNCTION_BEGIN;

	if (m_pDOSRouter)
	{
		m_pDOSRouter->SafeTerminate();
	}

	SAFE_DELETE(m_pRouterLinkManager);
	SAFE_DELETE(m_pProxyManager);
	SAFE_DELETE(m_pObjectManager);	
	SAFE_DELETE(m_pDOSRouter);
	if(m_ServerConfig.MemoryPoolConfig.Enable)
	{
		m_MemoryPool.Verfy(LOG_DOS_CHANNEL);
		m_MemoryPool.Destory();
	}

	FUNCTION_END;
}
