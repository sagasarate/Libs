#include "stdafx.h"



CWebSocketProxyService::CWebSocketProxyService()
{
	m_WSConnectionPool.SetTag(_T("CWebSocketProxyService"));
}
CWebSocketProxyService::~CWebSocketProxyService()
{
	Destory();
}


bool CWebSocketProxyService::OnStart()
{
	m_ThreadPerformanceCounter.Init(GetThreadHandle(), THREAD_CPU_COUNT_TIME);

	if (!m_MsgQueue.Create(m_Config.ProxyMsgQueueSize))
	{
		PrintDOSLog(_T("代理服务[%u]创建%u大小的消息队列失败！"),
			GetID(),
			m_Config.ProxyMsgQueueSize);
		return false;
	}

	if (!m_MessageMap.Create(m_Config.GlobalMsgMapSize, m_Config.GlobalMsgMapSize, 32))
	{
		PrintDOSLog(_T("代理服务[%u]创建%u大小的消息映射表失败！"),
			GetID(),
			m_Config.GlobalMsgMapSize);
		return false;
	}

	if (m_Config.MinMsgCompressSize)
	{
		switch (m_Config.MsgCompressType)
		{
		case MSG_COMPRESS_LZO:
		{
			if (lzo_init() != LZO_E_OK)
			{
				PrintDOSLog(_T("代理服务开启消息压缩失败"));
				m_Config.MsgCompressType = MSG_COMPRESS_NONE;
				m_Config.MinMsgCompressSize = 0;
			}
			PrintDOSLog(_T("代理服务开启消息压缩"));
		}
		break;
		default:
			PrintDOSLog(_T("代理服务[%u]不支持消息压缩模式%u！"),
				GetID(), m_Config.MsgCompressType);
			m_Config.MsgCompressType = MSG_COMPRESS_NONE;
			m_Config.MinMsgCompressSize = 0;
			break;
		}
	}
	else
	{
		m_Config.MsgCompressType = MSG_COMPRESS_NONE;
	}

	if (!CheckEncryptConfig())
	{
		m_Config.MsgEnCryptType = MSG_ENCRYPT_NONE;
		PrintDOSLog(_T("加密设置不合法，消息加密被取消"));
	}

	if (m_Config.EnableBlackList)
	{
		CAutoLock Lock(m_BlackListCriticalSection);

		m_IPBlackList.Create(m_Config.BlackListPoolSetting);
		for (const CIPAddress& IP : m_Config.InitBlackList)
		{
			IP_INFO* pInfo = NULL;
			m_IPBlackList.New(IP, &pInfo);
			if (pInfo)
			{
				pInfo->IP = IP;
				pInfo->ExpireTime = 0;
			}
		}
		PrintDOSLog(_T("已加载%u个黑名单"), m_IPBlackList.GetObjectCount());

		m_RecvProtectedIPList.Create(m_Config.BlackListPoolSetting);
	}

	m_BlackListUpdateTimer.SaveTime();

	if (!Create(IPPROTO_TCP,
		m_Config.AcceptQueueSize,
		m_Config.RecvBufferSize,
		m_Config.SendBufferSize,
		m_Config.ParallelAcceptCount,
		DEFAULT_PARALLEL_RECV,
		false))
	{
		PrintDOSLog(_T("代理服务[%u]创建失败！"), GetID());
		return false;
	}

	if (!StartListen(m_Config.ListenAddress))
	{
		PrintDOSLog(_T("代理服务[%u]侦听于(%s:%u)失败！"),
			GetID(),
			m_Config.ListenAddress.GetIPString(),
			m_Config.ListenAddress.GetPort());
		return false;
	}
	PrintDOSLog(_T("代理服务[%u]侦听于(%s:%u)！"),
		GetID(),
		m_Config.ListenAddress.GetIPString(),
		m_Config.ListenAddress.GetPort());

	PrintDOSLog(_T("对象代理[%u]线程[%u]已启动"), GetID(), GetThreadID());

	if (m_Config.MsgCompressType == MSG_COMPRESS_LZO)
	{
		UINT CompressBufferSize = m_Config.SendBufferSize * NET_DATA_BLOCK_SIZE;
		if (m_CompressBuffer.GetBufferSize() < CompressBufferSize)
		{
			if (m_CompressBuffer.Create(CompressBufferSize))
			{
				PrintDOSLog(_T("已创建%u大小的压缩缓冲"),
					CompressBufferSize);
			}
			else
			{
				PrintDOSLog(_T("创建%u大小的压缩缓冲失败！"),
					CompressBufferSize);
				return false;
			}
		}
	}

	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE)
	{
		UINT EncyptBufferSize = m_Config.RecvBufferSize * NET_DATA_BLOCK_SIZE;
		if (m_EncyptBuffer.GetBufferSize() < EncyptBufferSize)
		{
			if (m_EncyptBuffer.Create(EncyptBufferSize))
			{
				PrintDOSLog(_T("已创建%u大小的解密缓冲"),
					EncyptBufferSize);
			}
			else
			{
				PrintDOSLog(_T("创建%u大小的解密缓冲失败！"),
					EncyptBufferSize);
				return false;
			}
		}
	}

	if (m_WSConnectionPool.Create(m_Config.ConnectionPoolSetting))
	{
		PrintDOSDebugLog(_T("代理服务[%u]创建(%u,%u,%u)大小的连接池成功！"),
			GetID(),
			m_Config.ConnectionPoolSetting.StartSize, m_Config.ConnectionPoolSetting.GrowSize, m_Config.ConnectionPoolSetting.GrowLimit);
	}
	else
	{
		PrintDOSLog(_T("代理服务[%u]创建(%u,%u,%u)大小的连接池失败！"),
			GetID(),
			m_Config.ConnectionPoolSetting.StartSize, m_Config.ConnectionPoolSetting.GrowSize, m_Config.ConnectionPoolSetting.GrowLimit);
		return false;
	}



	if (m_Config.ConnectionGroupCount)
	{
		if (m_DestoryConnectionList.Create(m_Config.ConnectionPoolSetting))
		{
			PrintDOSDebugLog(_T("代理服务[%u]创建(%u,%u,%u)大小的连接销毁队列成功！"),
				GetID(),
				m_Config.ConnectionPoolSetting.StartSize, m_Config.ConnectionPoolSetting.GrowSize, m_Config.ConnectionPoolSetting.GrowLimit);
		}
		else
		{
			PrintDOSLog(_T("代理服务[%u]创建(%u,%u,%u)大小的连接销毁队列失败！"),
				GetID(),
				m_Config.ConnectionPoolSetting.StartSize, m_Config.ConnectionPoolSetting.GrowSize, m_Config.ConnectionPoolSetting.GrowLimit);
			return false;
		}


		m_ConnectionGroups.Resize(m_Config.ConnectionGroupCount);
		PrintDOSDebugLog(_T("代理服务[%u]创建了%u个连接组线程"), GetID(), (UINT)m_ConnectionGroups.GetCount());
		for (UINT i = 0; i < m_ConnectionGroups.GetCount(); i++)
		{
			if (!m_ConnectionGroups[i].Init(this, m_Config, i))
				return false;
			m_ConnectionGroups[i].Start();
			PrintDOSDebugLog(_T("连接组线程[%u]已启动"), m_ConnectionGroups[i].GetThreadID());
		}
	}

	if (m_Config.EnableGuardThread)
	{
		m_GuardThread.SetTargetThread(this);
		m_GuardThread.SetKeepAliveTime(m_Config.GuardThreadKeepAliveTime, m_Config.GuardThreadKeepAliveCount);
		m_GuardThread.Start();
	}



	return true;
}

UINT CWebSocketProxyService::GetConnectionCount()
{
	return m_WSConnectionPool.GetObjectCount();
}
void CWebSocketProxyService::OnClose()
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	DISPATCHED_MSG Msg;
	while (m_MsgQueue.PopFront(&Msg))
	{
		if (!((CDOSServer*)GetServer())->ReleaseMessagePacket(Msg.pMsgPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
	}
	for (UINT i = 0; i < m_ConnectionGroups.GetCount(); i++)
	{
		m_ConnectionGroups[i].SafeTerminate();
	}
	LPVOID Pos = m_WSConnectionPool.GetFirstObjectPos();
	while (Pos)
	{
		CWebSocketProxyConnection* pConnection = m_WSConnectionPool.GetNextObject(Pos);

		pConnection->Destory();

	}
	m_WSConnectionPool.Clear();
	m_MessageMap.Clear();
}
CBaseNetConnection* CWebSocketProxyService::CreateConnection(CIPAddress& RemoteAddress)
{
	CWebSocketProxyConnection* pConnection = NULL;

	UINT ID = m_WSConnectionPool.NewObject(&pConnection);
	if (pConnection)
	{
		if (!pConnection->Init(this, m_Config, ID))
		{
			m_WSConnectionPool.DeleteObject(ID);
			pConnection = NULL;
			PrintDOSDebugLog(_T("现有连接数%u"), m_WSConnectionPool.GetObjectCount());
		}
	}
	return pConnection;
}
bool CWebSocketProxyService::DeleteConnection(CBaseNetConnection* pConnection)
{
	pConnection->Destory();

	if (m_WSConnectionPool.DeleteObject(pConnection->GetID()))
	{
		PrintDOSDebugLog(_T("现有连接数%u"), m_WSConnectionPool.GetObjectCount());
		return true;
	}
	return false;
}
CDOSObjectProxyConnectionDefault* CWebSocketProxyService::GetConnection(UINT ID)
{
	return m_WSConnectionPool.GetObject(ID);
}
LPVOID CWebSocketProxyService::GetFirstConnectionPos()
{
	return m_WSConnectionPool.GetFirstObjectPos();
}
CDOSObjectProxyConnectionDefault* CWebSocketProxyService::GetNextConnection(LPVOID& Pos)
{
	return m_WSConnectionPool.GetNextObject(Pos);
}
int CWebSocketProxyService::CheckFreeObject()
{
	int ProcessCount = 0;
	UINT CurTime = (UINT)time(NULL);
	for (UINT i = 0; i < CONNECTION_FREE_CHECK_BATCH; i++)
	{
		LPVOID Pos = m_WSConnectionPool.GetFreeObjectPosByID(m_FreeObjectCheckPtr);
		if (Pos)
		{
			CWebSocketProxyConnection* pConnection = m_WSConnectionPool.GetFreeObject(Pos);
			if (pConnection)
			{
				if (pConnection->GetReleaseTime() + m_Config.MaxIdleTimeToFree <= CurTime)
				{
					m_WSConnectionPool.ReleaseFreeObject(Pos);
					ProcessCount++;
				}
			}
		}
		m_FreeObjectCheckPtr++;
		if (m_FreeObjectCheckPtr > m_WSConnectionPool.GetBufferSize())
		{
			m_FreeObjectCheckPtr = 1;
		}
	}
	if (ProcessCount)
	{
		PrintDOSDebugLog(_T("已彻底释放%d个连接"), ProcessCount);
	}
	return ProcessCount;
}