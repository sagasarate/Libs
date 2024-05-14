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


CDOSObjectProxyServiceNoBuff::CDOSObjectProxyServiceNoBuff(void)
{
	m_FreeObjectCheckPtr = 1;
	m_MsgQueue.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
	m_MessageMap.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
	m_ConnectionPool.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
	m_DestoryConnectionList.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
	m_ConnectionGroups.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
	m_CompressBuffer.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
	m_EncyptBuffer.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
	m_IPBlackList.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
	m_RecvProtectedIPList.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
	m_PrepareIPBlackList.SetTag(_T("CDOSObjectProxyServiceNoBuff"));
}

CDOSObjectProxyServiceNoBuff::~CDOSObjectProxyServiceNoBuff(void)
{
}

void CDOSObjectProxyServiceNoBuff::Release()
{
	CNetService::Release();
}
void CDOSObjectProxyServiceNoBuff::Destory()
{
	CNetService::Destory();
}
UINT CDOSObjectProxyServiceNoBuff::AddUseRef()
{
	return CNetService::AddUseRef();
}
UINT CDOSObjectProxyServiceNoBuff::GetUseRef()
{
	return CNetService::GetUseRef();
}
BYTE CDOSObjectProxyServiceNoBuff::GetProxyType()
{
	return m_Config.ProxyType;
}
void CDOSObjectProxyServiceNoBuff::SetID(UINT ID)
{
	CNetService::SetID(ID);
}
UINT CDOSObjectProxyServiceNoBuff::GetID()
{
	return CNetService::GetID();
}
bool CDOSObjectProxyServiceNoBuff::StartService()
{
	return Start() != FALSE;
}
bool CDOSObjectProxyServiceNoBuff::StartService(IDOSObjectProxyServiceOperator* pOperator)
{
	return StartService();
}
void CDOSObjectProxyServiceNoBuff::StopService()
{
	SafeTerminate();
}
bool CDOSObjectProxyServiceNoBuff::PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket)
{
	if (ObjectID.ObjectIndex == BROAD_CAST_OBJECT_INDEX)
	{
		//群发消息
		for (CDOSObjectProxyConnectionGroupNoBuff& Group : m_ConnectionGroups)
		{
			Group.PushMessage(ObjectID, pPacket);
	}

		((CDOSServer*)GetServer())->AddRefMessagePacket(pPacket);
#ifdef _DEBUG
		pPacket->SetAllocTime(0x13);
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
			((CDOSServer*)GetServer())->ReleaseMessagePacket(pPacket);
			return false;
		}
}
	else
	{
		if ((m_ConnectionGroups.GetCount() == 0) || (ObjectID.ObjectIndex == 0))
		{
			//单线模式，发给Service的消息，塞进Service的消息队列
			((CDOSServer*)GetServer())->AddRefMessagePacket(pPacket);
#ifdef _DEBUG
			pPacket->SetAllocTime(0x13);
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
				((CDOSServer*)GetServer())->ReleaseMessagePacket(pPacket);
				return false;
			}
		}
		else
		{
			//多线模式，塞进对应的消息组
			UINT Index = ObjectID.ObjectIndex % m_ConnectionGroups.GetCount();
			return m_ConnectionGroups[Index].PushMessage(ObjectID, pPacket);
		}
	}
}

UINT CDOSObjectProxyServiceNoBuff::GetConnectionCount()
{
	return m_ConnectionPool.GetObjectCount();
}

float CDOSObjectProxyServiceNoBuff::GetCPUUsedRate()
{
	return m_ThreadPerformanceCounter.GetCPUUsedRate();
}
float CDOSObjectProxyServiceNoBuff::GetCycleTime()
{
	return m_ThreadPerformanceCounter.GetCycleTime();
}
UINT CDOSObjectProxyServiceNoBuff::GetMsgQueueLen()
{
	return m_MsgQueue.GetUsedSize();
}
UINT CDOSObjectProxyServiceNoBuff::GetGroupCount()
{
	return (UINT)m_ConnectionGroups.GetCount();
}
float CDOSObjectProxyServiceNoBuff::GetGroupCPUUsedRate(UINT Index)
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
float CDOSObjectProxyServiceNoBuff::GetGroupCycleTime(UINT Index)
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

UINT CDOSObjectProxyServiceNoBuff::GetGroupMsgQueueLen(UINT Index)
{
	if (Index < m_ConnectionGroups.GetCount())
	{
		return m_ConnectionGroups[Index].GetMsgQueueLen();
	}
	else
	{
		return 0;
	}
}

bool CDOSObjectProxyServiceNoBuff::Init(CDOSServer * pServer, CLIENT_PROXY_CONFIG& Config)
{
	SetServer(pServer);
	m_Config = Config;
	return true;
}



BOOL CDOSObjectProxyServiceNoBuff::OnStart()
{

	m_ThreadPerformanceCounter.Init(GetThreadHandle(), THREAD_CPU_COUNT_TIME);

	if (!m_MsgQueue.Create(m_Config.ProxyMsgQueueSize))
	{
		PrintDOSLog( _T("代理服务[%u]创建%u大小的消息队列失败！"),
			GetID(),
			m_Config.ProxyMsgQueueSize);
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

	if (!CheckEncryptConfig())
	{
		m_Config.MsgEnCryptType = MSG_ENCRYPT_NONE;
		PrintDOSLog(_T("加密设置不合法，消息加密被取消"));
	}

	if (m_Config.EnableBlackList)
	{
		CAutoLock Lock(m_BlackListCriticalSection);

		m_IPBlackList.Create(m_Config.BlackListPoolSetting);
		for (const CIPAddress& IP: m_Config.InitBlackList)
		{
			IP_INFO * pInfo = NULL;
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
				PrintDOSLog( _T("创建%u大小的压缩缓冲失败！"),
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

	if (m_Config.EnableGuardThread)
	{
		m_GuardThread.SetTargetThread(this);
		m_GuardThread.SetKeepAliveTime(m_Config.GuardThreadKeepAliveTime, m_Config.GuardThreadKeepAliveCount);
		m_GuardThread.Start();
	}



	return TRUE;
}

BOOL CDOSObjectProxyServiceNoBuff::OnRun()
{
	m_ThreadPerformanceCounter.DoPerformanceCount();

	if (m_Config.EnableGuardThread)
	{
		m_GuardThread.MakeKeepAlive();
	}

	int ProcessCount = Update();
	if (ProcessCount == 0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}
	return TRUE;
}

void CDOSObjectProxyServiceNoBuff::OnTerminate()
{
	if (m_Config.EnableGuardThread)
		m_GuardThread.SafeTerminate();
	CNetService::Close();
	PrintDOSLog( _T("代理服务[%u]关闭"), GetID());
}

void CDOSObjectProxyServiceNoBuff::OnClose()
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
	LPVOID Pos=m_ConnectionPool.GetFirstObjectPos();
	while(Pos)
	{
		CDOSObjectProxyConnectionNoBuff * pConnection = m_ConnectionPool.GetNextObject(Pos);

		pConnection->Destory();

	}
	m_ConnectionPool.Clear();
	m_MessageMap.Clear();
}

int CDOSObjectProxyServiceNoBuff::Update(int ProcessPacketLimit)
{
	int ProcessCount = CNetService::Update(ProcessPacketLimit);

	DISPATCHED_MSG Msg;
	while (m_MsgQueue.PopFront(&Msg))
	{
		ProcessMsg(Msg);
		if (!((CDOSServer*)GetServer())->ReleaseMessagePacket(Msg.pMsgPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
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
			CDOSObjectProxyConnectionNoBuff * pConnection = m_ConnectionPool.GetNextObject(Pos);
			if (pConnection->IsConnected())
				ProcessCount += pConnection->Update();
			else
				DeleteConnection(pConnection);
		}
	}
	else
	{
		//多线程模式，需要处理连接的销毁
		CDOSObjectProxyConnectionNoBuff * pConnection = NULL;
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
	if (m_Config.EnableBlackList)
	{
		//处理新增的IP黑名单
		if (m_PrepareIPBlackList.GetCount())
		{
			CAutoLock Lock(m_BlackListCriticalSection);

			for (IP_INFO& IPInfo : m_PrepareIPBlackList)
			{
				IP_INFO * pBlackInfo = NULL;
				m_IPBlackList.New(IPInfo.IP, &pBlackInfo);
				if (pBlackInfo)
				{
					*pBlackInfo = IPInfo;
				}
				else
				{
					PrintDOSLog(_T("无法新增连接黑名单%u/%u"), m_IPBlackList.GetObjectCount() / m_IPBlackList.GetBufferSize());
				}
			}
			m_PrepareIPBlackList.Empty();
		}

		if (m_BlackListUpdateTimer.IsTimeOut(BLACK_LIST_UPDATE_TIME))
		{
			m_BlackListUpdateTimer.SaveTime();

			UINT CurTime = (UINT)time(NULL);
			void * Pos = m_IPBlackList.GetFirstObjectPos();
			while (Pos)
			{
				IP_INFO * pInfo = m_IPBlackList.GetNextObject(Pos);
				if (pInfo->ExpireTime)
				{
					if (pInfo->ExpireTime <= CurTime)
					{
						m_IPBlackList.Delete(pInfo->IP);
					}
				}
			}
		}
	}

	//已释放连接的对象彻底释放
	if (m_Config.MaxIdleTimeToFree)
		ProcessCount += CheckFreeObject();

	return ProcessCount;
}


CBaseNetConnection * CDOSObjectProxyServiceNoBuff::CreateConnection(CIPAddress& RemoteAddress)
{
	CDOSObjectProxyConnectionNoBuff * pConnection = NULL;

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

bool CDOSObjectProxyServiceNoBuff::DeleteConnection(CBaseNetConnection * pConnection)
{
	pConnection->Destory();

	if (m_ConnectionPool.DeleteObject(pConnection->GetID()))
	{
		return true;
	}
	return false;
}



void CDOSObjectProxyServiceNoBuff::AcceptConnection(CDOSObjectProxyConnectionNoBuff * pConnection)
{
	if (m_Config.EnableBlackList)
	{
		CIPAddress IP = pConnection->GetRemoteAddress();
		IP.SetPort(0);
		if (m_IPBlackList.Find(IP) != NULL)
		{
			PrintDOSLog(_T("%s在连接黑名单中，拒绝连接"), pConnection->GetRemoteAddress().GetAddressString());
			pConnection->Disconnect();
			return;
		}
	}
	if (m_Config.ConnectionGroupCount)
	{
		if (m_ConnectionGroups.GetCount())
		{
			UINT Index = pConnection->GetID() % m_ConnectionGroups.GetCount();
			m_ConnectionGroups[Index].AddConnection(pConnection);
		}
		else
		{
			pConnection->Disconnect();
			PrintDOSLog(_T("未能找到合适的连接组"));
		}
	}
	else
	{
		pConnection->SetCompressBuffer(&m_CompressBuffer, m_LZOCompressWorkMemory);
		pConnection->SetEncyptBuffer(&m_EncyptBuffer);
	}
}
void CDOSObjectProxyServiceNoBuff::QueryDestoryConnection(CDOSObjectProxyConnectionNoBuff * pConnection)
{
	if (m_Config.ConnectionGroupCount > 0)
	{
		if (!m_DestoryConnectionList.PushBack(pConnection))
		{
			PrintDOSLog(_T("连接销毁队列已满%u/%u"), m_DestoryConnectionList.GetObjectCount(), m_DestoryConnectionList.GetBufferSize());
		}
	}
}
bool CDOSObjectProxyServiceNoBuff::HaveGlobalMsgMap(MSG_ID_TYPE MsgID)
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	MSG_MAP_INFO * pMapInfo = m_MessageMap.Find(MsgID);
	if (pMapInfo)
	{
		return true;
	}
	return m_UnhandleMsgReceiverID.ID != 0;
}

bool CDOSObjectProxyServiceNoBuff::SendGlobalMapMessage(CDOSMessagePacket * pPacket)
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	MSG_MAP_INFO * pMapInfo = m_MessageMap.Find(pPacket->GetMessage().GetMsgID());
	if (pMapInfo)
	{
		switch (pMapInfo->MsgMapType)
		{
		case GLOBAL_MSG_MAP_TYPE_OVERLAP:
			{
				pPacket->SetTargetIDs(1, pMapInfo->ObjectIDList.GetObject(0));
				return GetServer()->GetRouter()->RouterMessage(pPacket) != FALSE;
			}
			break;
		case GLOBAL_MSG_MAP_TYPE_RANDOM:
			{
				CEasyArray<OBJECT_ID>&	ObjectIDList = pMapInfo->ObjectIDList;
				UINT Index = 0;
				if (ObjectIDList.GetCount() > 1)
					Index = GetRand((UINT)0, (UINT)ObjectIDList.GetCount() - 1);
				pPacket->SetTargetIDs(1, ObjectIDList.GetObject(Index));
				return GetServer()->GetRouter()->RouterMessage(pPacket) != FALSE;
			}
			break;
		case GLOBAL_MSG_MAP_TYPE_BROADCAST:
			{
				CEasyArray<OBJECT_ID>&	ObjectIDList = pMapInfo->ObjectIDList;
				//需要复制消息包
				CDOSMessagePacket * pNewPacket = GetServer()->NewMessagePacket(
					CDOSMessagePacket::CaculatePacketLength(pPacket->GetMessage().GetDataLength(), ObjectIDList.GetCount()));
				pNewPacket->GetMessage().GetMsgHeader() = pPacket->GetMessage().GetMsgHeader();
				memcpy(&pNewPacket->GetMessage(), &pPacket->GetMessage(), pPacket->GetMessage().GetMsgLength());				
				bool Ret = GetServer()->GetRouter()->RouterMessage(pNewPacket) != FALSE;
				if (!GetServer()->ReleaseMessagePacket(pNewPacket))
				{
					PrintDOSLog(_T("释放消息内存块失败！"));
				}
				return Ret;
			}
			break;
		}
		return true;
	}
	else if (m_UnhandleMsgReceiverID.ID)
	{
		pPacket->SetTargetIDs(1, &m_UnhandleMsgReceiverID);
		return GetServer()->GetRouter()->RouterMessage(pPacket) != FALSE;
	}
	return false;
}

void CDOSObjectProxyServiceNoBuff::ProcessMsg(DISPATCHED_MSG& Msg)
{
	if (Msg.TargetObjectID.ObjectIndex == 0)
	{
		//发送到Service的消息
		if (Msg.pMsgPacket->GetMessage().GetMsgFlag() & DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
			OnSystemMsg(Msg.pMsgPacket);
		else
			OnMsg(&(Msg.pMsgPacket->GetMessage()));
	}
	else if (Msg.TargetObjectID.ObjectIndex == BROAD_CAST_OBJECT_INDEX)
	{
		//群发消息
		if (m_Config.ConnectionGroupCount == 0)
		{
			//单线模式
			LPVOID Pos = m_ConnectionPool.GetFirstObjectPos();
			while (Pos)
			{
				CDOSObjectProxyConnectionNoBuff* pConnection = m_ConnectionPool.GetNextObject(Pos);
				if (pConnection)
				{
					pConnection->PushMessage(Msg.pMsgPacket);
				}
			}
		}

		//Service也处理一下
		if (Msg.pMsgPacket->GetMessage().GetMsgFlag() & DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
			OnSystemMsg(Msg.pMsgPacket);
		else
			OnMsg(&(Msg.pMsgPacket->GetMessage()));
	}
	else
	{
		//发送到Connection的消息
		CDOSObjectProxyConnectionNoBuff* pProxyConnection = GetConnection(Msg.TargetObjectID.ObjectIndex);
		if (pProxyConnection)
		{
			pProxyConnection->PushMessage(Msg.pMsgPacket);
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
void CDOSObjectProxyServiceNoBuff::OnMsg(CDOSMessage * pMessage)
{
	PrintDOSLog( _T("收到非系统消息0x%llX"), pMessage->GetMsgID());
}

void CDOSObjectProxyServiceNoBuff::OnSystemMsg(CDOSMessagePacket * pPacket)
{
	CDOSMessage * pMessage = &pPacket->GetMessage();
	switch (pMessage->GetMsgID())
	{
	case DSM_PROXY_REGISTER_GLOBAL_MSG_MAP:
		if (pMessage->GetDataLength() >= sizeof(UINT)*2)
		{
			UINT * pData = (UINT *)(pMessage->GetMsgData());
			DoRegisterGlobalMsgMap((MSG_ID_TYPE)(pData[0]), (int)(pData[1]), pMessage->GetSenderID());
		}
		break;
	case DSM_PROXY_UNREGISTER_GLOBAL_MSG_MAP:
		if (pMessage->GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			MSG_ID_TYPE MsgID = *((MSG_ID_TYPE *)(pMessage->GetMsgData()));
			DoUnregisterGlobalMsgMap(MsgID, pMessage->GetSenderID());
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
	case DSM_PROXY_SET_BROADCAST_MASK:
		if (pMessage->GetDataLength() >= sizeof(BROADCAST_MASK_SET_INFO))
		{
			const BROADCAST_MASK_SET_INFO* pInfo = (const BROADCAST_MASK_SET_INFO*)(pMessage->GetMsgData());
			BYTE ProxyType = GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(pInfo->ProxyObjectID.GroupIndex);
			if (ProxyType == m_Config.ProxyType)
			{
				CDOSObjectProxyConnectionNoBuff* pProxyConnection = GetConnection(pInfo->ProxyObjectID.ObjectIndex);
				if (pProxyConnection)
				{
					pProxyConnection->SetBroadcastMask(pInfo->Mask);
				}
				else
				{
					PrintDOSLog(_T("对象0x%llX不存在"), pInfo->ProxyObjectID);
				}
			}
			else
			{
				PrintDOSLog(_T("对象0x%llX不属于本服务"), pInfo->ProxyObjectID);
			}
		}		
		break;	
	case DSM_PROXY_ADD_BROADCAST_MASK:
		if (pMessage->GetDataLength() >= sizeof(BROADCAST_MASK_SET_INFO))
		{
			const BROADCAST_MASK_SET_INFO* pInfo = (const BROADCAST_MASK_SET_INFO*)(pMessage->GetMsgData());
			BYTE ProxyType = GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(pInfo->ProxyObjectID.GroupIndex);
			if (ProxyType == m_Config.ProxyType)
			{
				CDOSObjectProxyConnectionNoBuff* pProxyConnection = GetConnection(pInfo->ProxyObjectID.ObjectIndex);
				if (pProxyConnection)
				{
					pProxyConnection->AddBroadcastMask(pInfo->Mask);
				}
				else
				{
					PrintDOSLog(_T("对象0x%llX不存在"), pInfo->ProxyObjectID);
				}
			}
			else
			{
				PrintDOSLog(_T("对象0x%llX不属于本服务"), pInfo->ProxyObjectID);
			}
		}
		break;
	case DSM_PROXY_REMOVE_BROADCAST_MASK:
		if (pMessage->GetDataLength() >= sizeof(BROADCAST_MASK_SET_INFO))
		{
			const BROADCAST_MASK_SET_INFO* pInfo = (const BROADCAST_MASK_SET_INFO*)(pMessage->GetMsgData());
			BYTE ProxyType = GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(pInfo->ProxyObjectID.GroupIndex);
			if (ProxyType == m_Config.ProxyType)
			{
				CDOSObjectProxyConnectionNoBuff* pProxyConnection = GetConnection(pInfo->ProxyObjectID.ObjectIndex);
				if (pProxyConnection)
				{
					pProxyConnection->RemoveBroadcastMask(pInfo->Mask);
				}
				else
				{
					PrintDOSLog(_T("对象0x%llX不存在"), pInfo->ProxyObjectID);
				}
			}
			else
			{
				PrintDOSLog(_T("对象0x%llX不属于本服务"), pInfo->ProxyObjectID);
			}
		}
		break;
	case DSM_PROXY_ADD_BROADCAST_GROUP:
		if (pMessage->GetDataLength() >= sizeof(BROADCAST_GROUP_SET_INFO))
		{
			const BROADCAST_GROUP_SET_INFO* pInfo = (const BROADCAST_GROUP_SET_INFO*)(pMessage->GetMsgData());
			BYTE ProxyType = GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(pInfo->ProxyObjectID.GroupIndex);
			if (ProxyType == m_Config.ProxyType)
			{
				CDOSObjectProxyConnectionNoBuff* pProxyConnection = GetConnection(pInfo->ProxyObjectID.ObjectIndex);
				if (pProxyConnection)
				{
					pProxyConnection->AddBroadcastGroup(pInfo->GroupID);
				}
				else
				{
					PrintDOSLog(_T("对象0x%llX不存在"), pInfo->ProxyObjectID);
				}
			}
			else
			{
				PrintDOSLog(_T("对象0x%llX不属于本服务"), pInfo->ProxyObjectID);
			}
		}
		break;
	case DSM_PROXY_REMOVE_BROADCAST_GROUP:
		if (pMessage->GetDataLength() >= sizeof(BROADCAST_GROUP_SET_INFO))
		{
			const BROADCAST_GROUP_SET_INFO* pInfo = (const BROADCAST_GROUP_SET_INFO*)(pMessage->GetMsgData());
			BYTE ProxyType = GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(pInfo->ProxyObjectID.GroupIndex);
			if (ProxyType == m_Config.ProxyType)
			{
				CDOSObjectProxyConnectionNoBuff* pProxyConnection = GetConnection(pInfo->ProxyObjectID.ObjectIndex);
				if (pProxyConnection)
				{
					pProxyConnection->RemoveBroadcastGroup(pInfo->GroupID);
				}
				else
				{
					PrintDOSLog(_T("对象0x%llX不存在"), pInfo->ProxyObjectID);
				}
			}
			else
			{
				PrintDOSLog(_T("对象0x%llX不属于本服务"), pInfo->ProxyObjectID);
			}
		}
		break;
	case DSM_PROXY_BROADCAST_BY_MASK:
		if (pMessage->GetDataLength() >= sizeof(MASK_BROADCAST_INFO))
		{
			if (m_Config.ConnectionGroupCount == 0)
			{
				//单线程模式
				const MASK_BROADCAST_INFO* pInfo = (const MASK_BROADCAST_INFO*)(pMessage->GetMsgData());
				for (CDOSObjectProxyConnectionNoBuff Connect : m_ConnectionPool)
				{
					if (Connect.IsConnected() && (Connect.GetBroadcastMask() & pInfo->Mask))
					{
						Connect.PushMessage(pPacket);
					}
				}
			}
			else
			{
				//多线程模式
				for (CDOSObjectProxyConnectionGroupNoBuff& Group : m_ConnectionGroups)
				{
					Group.PushMessage(0, pPacket);
				}
			}
		}
		break;
	case DSM_PROXY_BROADCAST_BY_GROUP:
		if (pMessage->GetDataLength() >= sizeof(GROUP_BROADCAST_INFO))
		{
			if (m_Config.ConnectionGroupCount == 0)
			{
				//单线程模式
				const GROUP_BROADCAST_INFO* pInfo = (const GROUP_BROADCAST_INFO*)(pMessage->GetMsgData());
				void* Pos = GetFirstConnectionPos();
				while (Pos)
				{
					CDOSObjectProxyConnectionNoBuff* pConnect = GetNextConnection(Pos);
					if (pConnect->IsConnected() && (pConnect->IsInBroadcastGroup(pInfo->GroupID)))
					{
						pConnect->PushMessage(pPacket);
					}
				}
			}
			else
			{
				//多线程模式
				for (CDOSObjectProxyConnectionGroupNoBuff& Group : m_ConnectionGroups)
				{
					Group.PushMessage(0, pPacket);
				}
			}
		}
		break;
	case DSM_ROUTE_LINK_LOST:
		ClearMsgMapByRouterID(pMessage->GetSenderID().RouterID);
		break;
	default:
		PrintDOSLog( _T("收到未知系统消息0x%llX"), pMessage->GetMsgID());
	}
}

bool CDOSObjectProxyServiceNoBuff::DoRegisterGlobalMsgMap(MSG_ID_TYPE MsgID, int MapType, OBJECT_ID ObjectID)
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}

	MSG_MAP_INFO * pMapInfo = NULL;
	m_MessageMap.New(MsgID, &pMapInfo);
	if (pMapInfo)
	{
		pMapInfo->MsgMapType = MapType;
		if (pMapInfo->MsgMapType == GLOBAL_MSG_MAP_TYPE_OVERLAP)
		{
			if (pMapInfo->ObjectIDList.GetCount() != 1)
			{
				pMapInfo->ObjectIDList.Resize(1);
			}
			pMapInfo->ObjectIDList[0] = ObjectID;
		}
		else
		{
			bool IsExist = false;
			for (UINT i = 0; i < pMapInfo->ObjectIDList.GetCount(); i++)
			{
				if (pMapInfo->ObjectIDList[i] == ObjectID)
				{
					IsExist = true;
					break;
				}
			}
			if (!IsExist)
			{
				pMapInfo->ObjectIDList.Add(ObjectID);
				if (pMapInfo->ObjectIDList.GetCount())
					qsort(pMapInfo->ObjectIDList.GetBuffer(), pMapInfo->ObjectIDList.GetCount(), sizeof(OBJECT_ID), OBJECT_ID::Compare);
			}
		}
		//PrintDOSLog(_T("0x%llX注册了全局代理消息映射[0x%X,%d]！"), ObjectID.ID, MsgID, MapType);
		return true;
	}
	else
	{
		PrintDOSLog(_T("全局代理映射已满(%u,%u)！"), m_MessageMap.GetObjectCount(), m_MessageMap.GetBufferSize());
		return false;
	}
}
bool CDOSObjectProxyServiceNoBuff::DoUnregisterGlobalMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
{
	CAutoLockEx Lock;
	if (m_Config.ConnectionGroupCount)
	{
		//多线连接组模式需要加锁
		Lock.Lock(m_EasyCriticalSection);
	}
	MSG_MAP_INFO * pMapInfo = m_MessageMap.Find(MsgID);
	if (pMapInfo)
	{
		if (pMapInfo->MsgMapType == GLOBAL_MSG_MAP_TYPE_OVERLAP)
		{
			pMapInfo->ObjectIDList.Empty();
			m_MessageMap.Delete(MsgID);
		}
		else
		{
			for (UINT i = 0; i < pMapInfo->ObjectIDList.GetCount(); i++)
			{
				if (pMapInfo->ObjectIDList[i] == ObjectID)
				{
					pMapInfo->ObjectIDList.Delete(i);
					break;
				}
			}
			if (pMapInfo->ObjectIDList.GetCount() == 0)
			{
				m_MessageMap.Delete(MsgID);
			}
		}
		//PrintDOSLog(_T("0x%llX注销了全局代理消息映射[0x%X]！"), ObjectID.ID, MsgID);
		return true;
	}
	return false;
}

void CDOSObjectProxyServiceNoBuff::ClearMsgMapByRouterID(UINT RouterID)
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
		MSG_MAP_INFO * pMapInfo = m_MessageMap.GetNextObject(Pos, MsgID);
		for (int i = (int)pMapInfo->ObjectIDList.GetCount() - 1; i >= 0; i--)
		{
			if (pMapInfo->ObjectIDList[i].RouterID == RouterID)
			{
				pMapInfo->ObjectIDList.Delete(i);
			}
		}
		if (pMapInfo->ObjectIDList.GetCount() == 0)
		{
			m_MessageMap.Delete(MsgID);
		}
	}
}


bool CDOSObjectProxyServiceNoBuff::CheckEncryptConfig()
{
	switch (m_Config.MsgEnCryptType)
	{
	case MSG_ENCRYPT_DES:
		{
			if (m_Config.SecretKey.GetLength() < DES_KEY_SIZE)
			{
				PrintDOSLog(_T("DES密钥过短,必须8字符以上"));
				return false;
			}
			PrintDOSLog(_T("启用DES消息加密"));
			return true;
		}
		break;
	case MSG_ENCRYPT_AES:
		{
			if (m_Config.SecretKey.GetLength() < AES_KEYSIZE_128)
			{
				PrintDOSLog(_T("AES密钥过短,必须16字符以上"));
				return false;
			}
			PrintDOSLog(_T("启用AES消息加密"));
			return true;
		}
		break;
	case MSG_ENCRYPT_TEA:
		{
			if (m_Config.SecretKey.GetLength() < TEA_KEY_SIZE)
			{
				PrintDOSLog(_T("TEA密钥过短,必须16字符以上"));
				return false;
			}
			PrintDOSLog(_T("启用TEA消息加密"));
			return true;
		}
		break;
	}
	return true;
}
inline CDOSServer * CDOSObjectProxyServiceNoBuff::GetServer()
{
	return (CDOSServer *)CNetService::GetServer();
}

bool CDOSObjectProxyServiceNoBuff::AddBlackList(CIPAddress IP, UINT Duration)
{
	if (!m_Config.EnableBlackList)
		return false;
	CAutoLock Lock(m_BlackListCriticalSection);

	IP.SetPort(0);
	bool IsExist = false;
	for (IP_INFO& IPInfo : m_PrepareIPBlackList)
	{
		if (IPInfo.IP == IP)
		{
			if (Duration)
				IPInfo.ExpireTime = (UINT)time(NULL) + Duration;
			else
				IPInfo.ExpireTime = 0;
			IsExist = true;
			break;
		}
	}
	if (!IsExist)
	{
		IP_INFO * pIPInfo = m_PrepareIPBlackList.AddEmpty();
		pIPInfo->IP = IP;
		if (Duration)
			pIPInfo->ExpireTime = (UINT)time(NULL) + Duration;
		else
			pIPInfo->ExpireTime = 0;
	}
	return true;
}

bool CDOSObjectProxyServiceNoBuff::OnRecvProtected(CIPAddress IP)
{
	if (!m_Config.EnableBlackList)
		return false;
	CAutoLock Lock(m_BlackListCriticalSection);

	IP.SetPort(0);
	IP_INFO * pIPInfo = m_RecvProtectedIPList.Find(IP);

	if (pIPInfo == NULL)
	{
		m_RecvProtectedIPList.New(IP, &pIPInfo);
		if (pIPInfo)
		{
			pIPInfo->IP = IP;
			pIPInfo->ExpireTime = 0;
		}
	}

	if (pIPInfo)
	{
		pIPInfo->ExpireTime++;
		if (pIPInfo->ExpireTime >= m_Config.ProtectCountToAddBlackList)
		{
			PrintDOSLog(_T("%s保护启动次数超过规定，加入黑名单%u秒"), IP.GetIPString(), m_Config.ProtectBlockTime);
			AddBlackList(IP, m_Config.ProtectBlockTime);
			m_RecvProtectedIPList.Delete(IP);
		}

	}
	return true;
}

int CDOSObjectProxyServiceNoBuff::CheckFreeObject()
{
	int ProcessCount = 0;
	UINT CurTime = (UINT)time(NULL);
	for (UINT i = 0; i < CONNECTION_FREE_CHECK_BATCH; i++)
	{
		LPVOID Pos = m_ConnectionPool.GetFreeObjectPosByID(m_FreeObjectCheckPtr);
		if (Pos)
		{
			CDOSObjectProxyConnectionNoBuff * pConnection = m_ConnectionPool.GetFreeObject(Pos);
			if (pConnection)
			{
				if (pConnection->GetReleaseTime() + m_Config.MaxIdleTimeToFree <= CurTime)
				{
					m_ConnectionPool.ReleaseFreeObject(Pos);
					ProcessCount++;
				}
			}
		}
		m_FreeObjectCheckPtr++;
		if (m_FreeObjectCheckPtr > m_ConnectionPool.GetBufferSize())
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
