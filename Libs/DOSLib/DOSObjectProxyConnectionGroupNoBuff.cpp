#include "stdafx.h"



CDOSObjectProxyConnectionGroupNoBuff::CDOSObjectProxyConnectionGroupNoBuff()
{
	m_pService = NULL;
	m_Index = 0;
	m_ConnectionPool.SetTag(_T("CDOSObjectProxyConnectionGroupNoBuff"));
	m_CompressBuffer.SetTag(_T("CDOSObjectProxyConnectionGroupNoBuff"));
	m_EncyptBuffer.SetTag(_T("CDOSObjectProxyConnectionGroupNoBuff"));
}


CDOSObjectProxyConnectionGroupNoBuff::~CDOSObjectProxyConnectionGroupNoBuff()
{
}

bool CDOSObjectProxyConnectionGroupNoBuff::Init(CDOSObjectProxyServiceNoBuff * pService, CLIENT_PROXY_CONFIG& Config, UINT Index)
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

	if (!m_MsgQueue.Create(m_Config.ProxyMsgQueueSize))
	{
		PrintDOSLog(_T("创建%u大小的消息队列失败！"),
			m_Config.ProxyMsgQueueSize);
		return FALSE;
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

bool CDOSObjectProxyConnectionGroupNoBuff::AddConnection(CDOSObjectProxyConnectionNoBuff * pConnection)
{
	CAutoLock Lock(m_EasyCriticalSection);
		
	if (m_ConnectionPool.Insert(pConnection->GetID(), pConnection))
	{
		pConnection->SetCompressBuffer(&m_CompressBuffer, m_LZOCompressWorkMemory);
		pConnection->SetEncyptBuffer(&m_EncyptBuffer);
		pConnection->SetGroup(this);		
		PrintDOSDebugLog(_T("组%u已添加连接，现有连接%u"), m_Index, m_ConnectionPool.GetObjectCount());
		return true;
	}
	else
	{
		PrintDOSLog(_T("连接池已满(%u/%u)"), m_ConnectionPool.GetObjectCount(), m_ConnectionPool.GetBufferSize());
	}
	return false;
}

BOOL CDOSObjectProxyConnectionGroupNoBuff::OnStart()
{
	m_ThreadPerformanceCounter.Init(GetThreadHandle(), THREAD_CPU_COUNT_TIME);
	return TRUE;
}

BOOL CDOSObjectProxyConnectionGroupNoBuff::OnRun()
{

	m_ThreadPerformanceCounter.DoPerformanceCount();

	int ProcessCount = 0;

	LPVOID Pos = m_ConnectionPool.GetFirstObjectPos();
	while (Pos)
	{
		CDOSObjectProxyConnectionNoBuff* pConnection = NULL;
		{
			CAutoLock Lock(m_EasyCriticalSection);

			pConnection = *m_ConnectionPool.GetNextObject(Pos);
		}
		ProcessCount += pConnection->Update();
		if (!pConnection->IsConnected())
		{
			CAutoLock Lock(m_EasyCriticalSection);
			m_ConnectionPool.Delete(pConnection->GetID());
			PrintDOSDebugLog(_T("组%u已移除断线连接，现有连接%u"), m_Index, m_ConnectionPool.GetObjectCount());
			m_pService->QueryDestoryConnection(pConnection);
		}
	}

	DISPATCHED_MSG Msg;
	while (m_MsgQueue.PopFront(&Msg))
	{
		ProcessMsg(Msg);

		if (!m_pService->GetServer()->ReleaseMessagePacket(Msg.pMsgPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}

		ProcessCount++;
		if (ProcessCount >= 32)
			break;
	}

	if (ProcessCount == 0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}
	return TRUE;
}

void CDOSObjectProxyConnectionGroupNoBuff::OnTerminate()
{
	DISPATCHED_MSG Msg;
	while (m_MsgQueue.PopFront(&Msg))
	{
		if (!m_pService->GetServer()->ReleaseMessagePacket(Msg.pMsgPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
	}
}
bool CDOSObjectProxyConnectionGroupNoBuff::PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket* pPacket)
{
	m_pService->GetServer()->AddRefMessagePacket(pPacket);
#ifdef _DEBUG
	pPacket->SetAllocTime(0x16);
#endif
	DISPATCHED_MSG Msg;
	Msg.TargetObjectID = ObjectID;
	Msg.pMsgPacket = pPacket;
	if (m_MsgQueue.PushBack(&Msg))
	{
		return true;
	}
	else
	{
		if (!m_pService->GetServer()->ReleaseMessagePacket(Msg.pMsgPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
		return false;
	}
}
void CDOSObjectProxyConnectionGroupNoBuff::ProcessMsg(DISPATCHED_MSG& Msg)
{
	if (Msg.TargetObjectID.ObjectIndex == 0)
	{
		//发送到Group的消息
		if (Msg.pMsgPacket->GetMessage().GetMsgFlag() & DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
			OnSystemMsg(Msg.pMsgPacket);
		else
			OnMsg(&(Msg.pMsgPacket->GetMessage()));
	}
	else if (Msg.TargetObjectID.ObjectIndex == BROAD_CAST_OBJECT_INDEX)
	{
		//群发消息
		LPVOID Pos = m_ConnectionPool.GetFirstObjectPos();
		while (Pos)
		{
			CDOSObjectProxyConnectionNoBuff** ppConnection = m_ConnectionPool.GetNextObject(Pos);
			if (ppConnection)
			{
				(*ppConnection)->PushMessage(Msg.pMsgPacket);
			}
		}

		//Group也处理一下
		if (Msg.pMsgPacket->GetMessage().GetMsgFlag() & DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
			OnSystemMsg(Msg.pMsgPacket);
		else
			OnMsg(&(Msg.pMsgPacket->GetMessage()));
	}
	else
	{
		//发送到Connection的消息
		CDOSObjectProxyConnectionNoBuff** ppProxyConnection = m_ConnectionPool.Find(Msg.TargetObjectID.ObjectIndex);
		if (ppProxyConnection)
		{
			(*ppProxyConnection)->PushMessage(Msg.pMsgPacket);
		}
		else
		{
			//PrintDOSDebugLog(_T("将[0x%llX]发出的消息[%X]递送到代理对象[%llX]时代理对象不存在"),
			//	pPacket->GetMessage().GetSenderID(),
			//	pPacket->GetMessage().GetMsgID(),
			//	ObjectID);
		}
	}
}
void CDOSObjectProxyConnectionGroupNoBuff::OnMsg(CDOSMessage* pMessage)
{

}
void CDOSObjectProxyConnectionGroupNoBuff::OnSystemMsg(CDOSMessagePacket* pPacket)
{
	CDOSMessage* pMessage = &pPacket->GetMessage();
	switch (pMessage->GetMsgID())
	{
	case DSM_PROXY_GROUP_BROADCAST:
		if (pMessage->GetDataLength() >= sizeof(MASK_BROADCAST_INFO))
		{
			const MASK_BROADCAST_INFO* pInfo = (const MASK_BROADCAST_INFO*)(pMessage->GetMsgData());
			for (CDOSObjectProxyConnectionNoBuff* pConnection : m_ConnectionPool)
			{
				if (pConnection->IsConnected() && (pConnection->GetBroadcastMask() & pInfo->Mask))
				{
					pConnection->PushMessage(pPacket);
				}
			}
		}
		break;
	}
}

