/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSObjectProxyService.cpp                                */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


CDOSObjectProxyServiceDefault::CDOSObjectProxyServiceDefault(void)
{

}

CDOSObjectProxyServiceDefault::~CDOSObjectProxyServiceDefault(void)
{
}

void CDOSObjectProxyServiceDefault::Release()
{
	CNetService::Release();
}
void CDOSObjectProxyServiceDefault::Destory()
{
	CNetService::Destory();
}
BYTE CDOSObjectProxyServiceDefault::GetProxyType()
{
	return m_Config.ProxyType;
}
void CDOSObjectProxyServiceDefault::SetID(UINT ID)
{
	CNetService::SetID(ID);
}
UINT CDOSObjectProxyServiceDefault::GetID()
{
	return CNetService::GetID();
}
bool CDOSObjectProxyServiceDefault::StartService()
{
	return Start() != FALSE;
}
void CDOSObjectProxyServiceDefault::StopService()
{
	SafeTerminate();
}
bool CDOSObjectProxyServiceDefault::PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket)
{
	if (ObjectID.ObjectIndex == 0)
	{
		//发送到Service的消息
		((CDOSServer *)GetServer())->AddRefMessagePacket(pPacket);
#ifdef _DEBUG
		pPacket->SetAllocTime(0x13);
#endif
		if (m_MsgQueue.PushBack(&pPacket))
		{
			return true;
		}
		else
		{
			((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket);
			return false;
		}
	}
	else if (ObjectID.ObjectIndex == BROAD_CAST_OBJECT_INDEX)
	{
		//群发消息
		LPVOID Pos = m_ConnectionPool.GetFirstObjectPos();
		while (Pos)
		{
			CDOSObjectProxyConnectionDefault * pConnection = m_ConnectionPool.GetNextObject(Pos);
			if (pConnection)
			{
				pConnection->PushMessage(pPacket);
			}
		}
		return true;
	}
	else
	{
		//发送到Connection的消息
		CDOSObjectProxyConnectionDefault * pProxyConnection = GetConnection(ObjectID.ObjectIndex);
		if (pProxyConnection)
		{
			return pProxyConnection->PushMessage(pPacket);
		}
		else
		{
			PrintDOSDebugLog(_T("将[0x%llX]发出的消息[%X]递送到代理对象[%llX]时代理对象不存在"),
				pPacket->GetMessage().GetSenderID(),
				pPacket->GetMessage().GetMsgID(),
				ObjectID);
			return false;
		}
	}
}

UINT CDOSObjectProxyServiceDefault::GetConnectionCount()
{
	return m_ConnectionPool.GetObjectCount();
}
float CDOSObjectProxyServiceDefault::GetCPUUsedRate()
{
	return m_ThreadPerformanceCounter.GetCPUUsedRate();
}
float CDOSObjectProxyServiceDefault::GetCycleTime()
{
	return m_ThreadPerformanceCounter.GetCycleTime();
}
UINT CDOSObjectProxyServiceDefault::GetGroupCount()
{ 
	return (UINT)m_ConnectionGroups.GetCount();
}
float CDOSObjectProxyServiceDefault::GetGroupCPUUsedRate(UINT Index)
{ 
	if (Index < m_ConnectionGroups.GetCount())
	{
		return m_ConnectionGroups[Index].GetCPUUsedRate();
	}
	else
	{
		return 0;
	}
	
}
float CDOSObjectProxyServiceDefault::GetGroupCycleTime(UINT Index)
{ 
	if (Index < m_ConnectionGroups.GetCount())
	{
		return m_ConnectionGroups[Index].GetCycleTime();
	}
	else
	{
		return 0;
	}
}

bool CDOSObjectProxyServiceDefault::Init(CDOSServer * pServer, CLIENT_PROXY_CONFIG& Config)
{
	SetServer(pServer);
	m_Config = Config;	
	return true;
}



BOOL CDOSObjectProxyServiceDefault::OnStart()
{	

	m_ThreadPerformanceCounter.Init(GetThreadHandle(), THREAD_CPU_COUNT_TIME);

	if (!m_MsgQueue.Create(m_Config.ConnectionMsgQueueSize))
	{
		PrintDOSLog( _T("代理服务[%u]创建%u大小的消息队列失败！"),
			GetID(),
			m_Config.ConnectionMsgQueueSize);
		return FALSE;
	}

	if (!m_MessageMap.Create(m_Config.GlobalMsgMapSize, m_Config.GlobalMsgMapSize, 32))
	{
		PrintDOSLog( _T("代理服务[%u]创建%u大小的消息映射表失败！"),
			GetID(),
			m_Config.GlobalMsgMapSize);
		return FALSE;
	}

	if (m_Config.MinMsgCompressSize)
	{
		switch (m_Config.MsgCompressType)
		{
		case MSG_COMPRESS_LZO:
		{
			if (lzo_init() != LZO_E_OK)
			{
				PrintDOSLog( _T("代理服务开启消息压缩失败"));
				m_Config.MsgCompressType = MSG_COMPRESS_NONE;
				m_Config.MinMsgCompressSize = 0;
			}
			PrintDOSLog( _T("代理服务开启消息压缩"));
		}
		break;
		default:
			PrintDOSLog( _T("代理服务[%u]不支持消息压缩模式%u！"),
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

	if (!Create(IPPROTO_TCP,
		m_Config.AcceptQueueSize,
		m_Config.RecvBufferSize,
		m_Config.SendBufferSize,
		m_Config.ParallelAcceptCount,
		DEFAULT_PARALLEL_RECV,
		false))
	{
		PrintDOSLog( _T("代理服务[%u]创建失败！"), GetID());
		return FALSE;
	}

	if (!StartListen(m_Config.ListenAddress))
	{
		PrintDOSLog( _T("代理服务[%u]侦听于(%s:%u)失败！"),
			GetID(),
			m_Config.ListenAddress.GetIPString(),
			m_Config.ListenAddress.GetPort());
		return FALSE;
	}
	PrintDOSLog( _T("代理服务[%u]侦听于(%s:%u)！"),
		GetID(),
		m_Config.ListenAddress.GetIPString(),
		m_Config.ListenAddress.GetPort());

	PrintDOSLog( _T("对象代理[%u]线程[%u]已启动"), GetID(), GetThreadID());

	if (m_Config.MsgCompressType == MSG_COMPRESS_LZO)
	{
		if (m_CompressBuffer.GetBufferSize() < m_Config.MaxMsgSize)
		{
			if (!m_CompressBuffer.Create(m_Config.MaxMsgSize))
			{
				PrintDOSLog( _T("创建%u大小的压缩缓冲失败！"),
					m_Config.MaxMsgSize);
				return FALSE;
			}
		}
	}

	if (m_ConnectionPool.Create(m_Config.ConnectionPoolSetting))
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
		return FALSE;
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
			return FALSE;
		}


		m_ConnectionGroups.Resize(m_Config.ConnectionGroupCount);
		PrintDOSDebugLog(_T("代理服务[%u]创建了%u个连接组线程"), GetID(), (UINT)m_ConnectionGroups.GetCount());
		for (UINT i = 0; i < m_ConnectionGroups.GetCount(); i++)
		{
			if (!m_ConnectionGroups[i].Init(this, m_Config, i))
				return FALSE;
			m_ConnectionGroups[i].Start();
			PrintDOSDebugLog(_T("连接组线程[%u]已启动"), m_ConnectionGroups[i].GetThreadID());
		}
	}
	
	return TRUE;
}

BOOL CDOSObjectProxyServiceDefault::OnRun()
{
	m_ThreadPerformanceCounter.DoPerformanceCount();
	int ProcessCount = Update();
	if (ProcessCount == 0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}
	return TRUE;
}

void CDOSObjectProxyServiceDefault::OnTerminate()
{
	CNetService::Close();
	PrintDOSLog( _T("代理服务[%u]关闭"), GetID());
}

void CDOSObjectProxyServiceDefault::OnClose()
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(&pPacket))
	{
		if (!((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog( _T("释放消息内存块失败！"));
		}
	}	
	for (UINT i = 0; i < m_ConnectionGroups.GetCount(); i++)
	{
		m_ConnectionGroups[i].SafeTerminate();
	}
	LPVOID Pos=m_ConnectionPool.GetFirstObjectPos();
	while(Pos)
	{
		CDOSObjectProxyConnectionDefault * pConnection = m_ConnectionPool.GetNextObject(Pos);

		pConnection->Destory();
		 
	}
	m_ConnectionPool.Clear();
	m_MessageMap.Clear();
}

int CDOSObjectProxyServiceDefault::Update(int ProcessPacketLimit)
{
	int ProcessCount = CNetService::Update(ProcessPacketLimit);

	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(&pPacket))
	{
		//PrintDOSDebugLog(_T("发送了消息[%u]"),pPacket->GetMessage().GetMsgID());
		if (pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
			OnSystemMsg(&(pPacket->GetMessage()));
		else
			OnMsg(&(pPacket->GetMessage()));

		if (!((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog( _T("释放消息内存块失败！"));
		}

		ProcessCount++;
		if (ProcessCount >= ProcessPacketLimit)
			break;
	}

	if (m_Config.ConnectionGroupCount == 0)
	{
		//单线程模式，需要更新连接池的连接
		LPVOID Pos = m_ConnectionPool.GetFirstObjectPos();
		while (Pos)
		{
			CDOSObjectProxyConnectionDefault * pConnection = m_ConnectionPool.GetNextObject(Pos);
			if (pConnection->IsConnected())
				ProcessCount += pConnection->Update();
			else
				DeleteConnection(pConnection);
		}
	}
	else
	{
		//多线程模式，需要处理连接的销毁
		CDOSObjectProxyConnectionDefault * pConnection = NULL;
		int Count = 0;
		while (m_DestoryConnectionList.PopFront(pConnection))
		{
			DeleteConnection(pConnection);
			Count++;
			ProcessCount++;
			if (Count >= ProcessPacketLimit)
				break;
		}
	}
	return ProcessCount;
}


CBaseNetConnection * CDOSObjectProxyServiceDefault::CreateConnection(CIPAddress& RemoteAddress)
{
	CDOSObjectProxyConnectionDefault * pConnection = NULL;
	
	UINT ID = m_ConnectionPool.NewObject(&pConnection);
	if (pConnection)
	{
		if (!pConnection->Init(this, m_Config, ID))
		{
			m_ConnectionPool.DeleteObject(ID);
			pConnection = NULL;
		}
	}
	return pConnection;
}

bool CDOSObjectProxyServiceDefault::DeleteConnection(CBaseNetConnection * pConnection)
{
	pConnection->Destory();
	
	if (m_ConnectionPool.DeleteObject(pConnection->GetID()))
	{
		return true;
	}
	return false;
}



void CDOSObjectProxyServiceDefault::AcceptConnection(CDOSObjectProxyConnectionDefault * pConnection)
{
	if (m_Config.ConnectionGroupCount)
	{
		CDOSObjectProxyConnectionGroup * pGroup = NULL;
		for (UINT i = 0; i < m_ConnectionGroups.GetCount(); i++)
		{
			if (pGroup == NULL)
			{
				pGroup = m_ConnectionGroups.GetObject(i);
			}
			else if (pGroup->GetConnectionCount() > m_ConnectionGroups[i].GetConnectionCount())
			{
				pGroup = m_ConnectionGroups.GetObject(i);
			}
		}
		if (pGroup)
		{
			pGroup->AddConnection(pConnection);
		}
		else
		{
			PrintDOSLog(_T("未能找到合适的连接组"));
		}
	}
	else
	{
		pConnection->SetCompressBuffer(&m_CompressBuffer);
	}
}
void CDOSObjectProxyServiceDefault::QueryDestoryConnection(CDOSObjectProxyConnectionDefault * pConnection)
{
	if (m_Config.ConnectionGroupCount > 0)
	{
		if (!m_DestoryConnectionList.PushBack(pConnection))
		{
			PrintDOSLog(_T("连接销毁队列已满%u/%u"), m_DestoryConnectionList.GetObjectCount(), m_DestoryConnectionList.GetBufferSize());
		}
	}
}

OBJECT_ID CDOSObjectProxyServiceDefault::GetGlobalMsgMapObjectID(MSG_ID_TYPE MsgID)
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	OBJECT_ID * pObjectID = m_MessageMap.Find(MsgID);
	if (pObjectID)
	{
		return *pObjectID;
	}	
	return 0;
}

OBJECT_ID CDOSObjectProxyServiceDefault::GetUnhandleMsgReceiverID()
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	return m_UnhandleMsgReceiverID;
}


void CDOSObjectProxyServiceDefault::OnMsg(CDOSMessage * pMessage)
{
	PrintDOSLog( _T("收到非系统消息0x%llX"), pMessage->GetMsgID());
}

void CDOSObjectProxyServiceDefault::OnSystemMsg(CDOSMessage * pMessage)
{
	switch (pMessage->GetMsgID())
	{
	case DSM_PROXY_REGISTER_GLOBAL_MSG_MAP:
		if (pMessage->GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			int Count = (pMessage->GetDataLength()) / sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pMessage->GetDataBuffer());
			for (int i = 0; i < Count; i++)
			{
				DoRegisterGlobalMsgMap(pMsgIDs[i], pMessage->GetSenderID());
			}
		}
		break;
	case DSM_PROXY_UNREGISTER_GLOBAL_MSG_MAP:
		if (pMessage->GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			int Count = (pMessage->GetDataLength()) / sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pMessage->GetDataBuffer());
			for (int i = 0; i < Count; i++)
			{
				DoUnregisterGlobalMsgMap(pMsgIDs[i], pMessage->GetSenderID());
			}
		}
		break;
	case DSM_PROXY_SET_UNHANDLE_MSG_RECEIVER:
		{
			CAutoLockEx Lock;
			if (m_Config.ConnectionGroupCount)
			{
				//多线连接组模式需要加锁
				Lock.Lock(m_EasyCriticalSection);
			}
			m_UnhandleMsgReceiverID = pMessage->GetSenderID();
		}		
		break;
	case DSM_ROUTE_LINK_LOST:
		ClearMsgMapByRouterID(pMessage->GetSenderID().RouterID);
		break;
	default:
		PrintDOSLog( _T("收到未知系统消息0x%llX"), pMessage->GetMsgID());
	}
}

bool CDOSObjectProxyServiceDefault::DoRegisterGlobalMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	PrintDOSLog( _T("0x%llX注册了全局代理消息映射[0x%X]！"), ObjectID.ID, MsgID);
	return m_MessageMap.Insert(MsgID, ObjectID) != 0;
}
bool CDOSObjectProxyServiceDefault::DoUnregisterGlobalMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	PrintDOSLog( _T("0x%llX注销了全局代理消息映射[0x%X]！"), ObjectID.ID, MsgID);
	return m_MessageMap.Delete(MsgID) != FALSE;
}

void CDOSObjectProxyServiceDefault::ClearMsgMapByRouterID(UINT RouterID)
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	void * Pos = m_MessageMap.GetFirstObjectPos();
	while (Pos)
	{
		MSG_ID_TYPE MsgID;
		OBJECT_ID * pObjectID = m_MessageMap.GetNextObject(Pos, MsgID);
		if (pObjectID->RouterID == RouterID)
		{
			m_MessageMap.Delete(MsgID);
		}
	}
}


