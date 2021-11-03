#include "stdafx.h"



CDOSObjectProxyConnectionGroup::CDOSObjectProxyConnectionGroup()
{
	m_pService = NULL;
	m_Index = 0;
	m_ConnectionPool.SetTag(_T("CDOSObjectProxyConnectionGroup"));
	m_CompressBuffer.SetTag(_T("CDOSObjectProxyConnectionGroup"));
	m_EncyptBuffer.SetTag(_T("CDOSObjectProxyConnectionGroup"));
}


CDOSObjectProxyConnectionGroup::~CDOSObjectProxyConnectionGroup()
{
}

bool CDOSObjectProxyConnectionGroup::Init(CDOSObjectProxyServiceDefault * pService, CLIENT_PROXY_CONFIG& Config, UINT Index)
{
	m_pService = pService;
	m_Config = Config;
	m_Index = Index;
	if (m_Config.MsgCompressType == MSG_COMPRESS_LZO)
	{
		UINT CompressBufferSize = m_Config.SendBufferSize*NET_DATA_BLOCK_SIZE;
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
				return FALSE;
			}
		}
	}

	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE)
	{
		UINT EncyptBufferSize = m_Config.RecvBufferSize*NET_DATA_BLOCK_SIZE;
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
				return FALSE;
			}
		}
	}
	
	if (!m_ConnectionPool.Create(m_Config.ConnectionPoolSetting))
	{
		PrintDOSLog(_T("创建(%u,%u,%u)大小的连接池失败！"),
			GetID(),
			m_Config.ConnectionPoolSetting.StartSize, m_Config.ConnectionPoolSetting.GrowSize, m_Config.ConnectionPoolSetting.GrowLimit);
		return false;
	}
	return true;
}

bool CDOSObjectProxyConnectionGroup::AddConnection(CDOSObjectProxyConnectionDefault * pConnection)
{
	CAutoLock Lock(m_EasyCriticalSection);
		
	if (m_ConnectionPool.PushBack(pConnection))
	{
		pConnection->SetCompressBuffer(&m_CompressBuffer, m_LZOCompressWorkMemory);
		pConnection->SetEncyptBuffer(&m_EncyptBuffer);
		pConnection->SetGroup(this);		
		return true;
	}
	else
	{
		PrintDOSLog(_T("连接池已满(%u/%u)"), m_ConnectionPool.GetObjectCount(), m_ConnectionPool.GetBufferSize());
	}
	return false;
}

BOOL CDOSObjectProxyConnectionGroup::OnStart()
{
	m_ThreadPerformanceCounter.Init(GetThreadHandle(), THREAD_CPU_COUNT_TIME);
	return TRUE;
}

BOOL CDOSObjectProxyConnectionGroup::OnRun()
{

	m_ThreadPerformanceCounter.DoPerformanceCount();

	int ProcessCount = 0;

	LPVOID Pos = m_ConnectionPool.GetFirstObjectPos();
	while (Pos)
	{
		LPVOID OldPos = Pos;
		CDOSObjectProxyConnectionDefault * pConnection = NULL;
		{
			CAutoLock Lock(m_EasyCriticalSection);

			pConnection = *m_ConnectionPool.GetNextObject(Pos);
		}
		ProcessCount += pConnection->Update();
		if (!pConnection->IsConnected())
		{
			CAutoLock Lock(m_EasyCriticalSection);
			m_ConnectionPool.DeleteObjectByPos(OldPos);
			m_pService->QueryDestoryConnection(pConnection);
		}
	}

	if (ProcessCount == 0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}
	return TRUE;
}
