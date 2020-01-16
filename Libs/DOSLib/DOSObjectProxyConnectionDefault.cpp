/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSProxyConnection.cpp                                   */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"




CDOSObjectProxyConnectionDefault::CDOSObjectProxyConnectionDefault(void)
{
	m_RecentPingDelay = 0;
	m_pService = NULL;
	m_pGroup = NULL;
	m_Status = STATUS_NONE;
	m_pCompressBuffer = NULL;
	m_pLZOCompressWorkBuffer = NULL;
	m_RecvCount = 0;
	m_RecvFlow = 0;
	m_ReleaseTime = 0;
	m_MsgQueue.SetTag(_T("CDOSObjectProxyConnectionDefault"));
	m_MessageMap.SetTag(_T("CDOSObjectProxyConnectionDefault"));
	m_AssembleBuffer.SetTag(_T("CDOSObjectProxyConnectionDefault"));
}

CDOSObjectProxyConnectionDefault::~CDOSObjectProxyConnectionDefault(void)
{
}

void CDOSObjectProxyConnectionDefault::Destory()
{
	m_pCompressBuffer = NULL;
	m_pLZOCompressWorkBuffer = NULL;
	CNetConnection::Close();
	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(&pPacket))
	{
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
	}
	m_Status = STATUS_DESTORYED;
	m_ReleaseTime = time(NULL);
}


bool CDOSObjectProxyConnectionDefault::PushMessage(CDOSMessagePacket * pPacket)
{
	if (m_Status != STATUS_CONNECTED)
		return false;

	((CDOSServer *)GetServer())->AddRefMessagePacket(pPacket);
#ifdef _DEBUG
	pPacket->SetAllocTime(0x14);
#endif
	if (m_MsgQueue.PushBack(&pPacket))
	{
		return true;
	}
	else
	{
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
		return false;
	}	
}



bool CDOSObjectProxyConnectionDefault::Init(CDOSObjectProxyServiceDefault * pService, const CLIENT_PROXY_CONFIG& Config, UINT ID)
{
	SetID(ID);
	m_pService = pService;
	SetServer(pService->GetServer());
	m_Config = Config;

	m_ObjectID.ObjectIndex = GetID();
	m_ObjectID.GroupIndex = MAKE_PROXY_GROUP_INDEX(m_Config.ProxyType);
	m_ObjectID.ObjectTypeID = DOT_PROXY_OBJECT;
	m_ObjectID.RouterID = GetServer()->GetRouter()->GetRouterID();

	m_MaxKeepAliveCount = m_Config.KeepAliveCount;
	m_KeepAliveTime = m_Config.KeepAliveTime;
	m_UseServerInitiativeKeepAlive = m_Config.UseServerInitiativeKeepAlive;

	//消息队列可能会有消息残留，释放掉
	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(&pPacket))
	{
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
	}

	if (m_MsgQueue.GetBufferSize() < m_Config.ConnectionMsgQueueSize)
	{
		if (!m_MsgQueue.Create(m_Config.ConnectionMsgQueueSize))
		{
			PrintDOSLog( _T("创建%u大小的消息队列失败！"),
				m_Config.ConnectionMsgQueueSize);
			return false;
		}
	}

	if (m_MessageMap.GetBufferSize() < m_Config.MsgMapSize)
	{
		if (!m_MessageMap.Create(m_Config.MsgMapSize, m_Config.MsgMapSize, 32))
		{
			PrintDOSLog( _T("创建%u大小的消息映射表失败！"),
				m_Config.MsgMapSize);
			return false;
		}
	}
	else
	{
		m_MessageMap.Clear();
	}

	m_KeepAliveTimer.SaveTime();
	m_KeepAliveCount = 0;
	m_RecentPingDelay = 0;
	m_MaxKeepAliveCount = m_Config.KeepAliveCount;
	m_UnacceptConnectionKeepTimer.SaveTime();
	m_NeedDelayClose = false;

	m_RecvProtectCheckTimer.SaveTime();

	if (m_AssembleBuffer.GetBufferSize() < m_Config.MaxMsgSize * 2)
	{
		if (!m_AssembleBuffer.Create(m_Config.MaxMsgSize * 2))
		{
			PrintDOSLog(_T("创建%u大小的拼包缓冲失败！"),
				m_Config.MaxMsgSize * 2);
			return false;
		}
	}
	m_Status = STATUS_INITED;
	return true;
}



void CDOSObjectProxyConnectionDefault::OnRecvData(const BYTE * pData, UINT DataSize)
{
	MSG_LEN_TYPE PacketSize = 0;
	UINT PeekPos = 0;
	if (!m_AssembleBuffer.PushBack(pData, DataSize))
	{
		PrintDOSLog(_T("对象代理(%d)拼包缓冲溢出(%u/%u)，连接断开！"), GetID(), m_AssembleBuffer.GetUsedSize(), m_AssembleBuffer.GetBufferSize());
		Disconnect();		
	}
	else
	{
		if (m_AssembleBuffer.Peek(PeekPos, &PacketSize, sizeof(MSG_LEN_TYPE)))
		{
			while (m_AssembleBuffer.GetUsedSize() >= PacketSize&&PacketSize)
			{
				if (PacketSize < CDOSSimpleMessage::GetMsgHeaderLength())
				{
					Disconnect();
					PrintDOSLog( _T("对象代理(%d)收到非法包，连接断开！"), GetID());
				}
				CDOSSimpleMessage * pMsg = (CDOSSimpleMessage *)m_AssembleBuffer.GetBuffer();
				if (pMsg->GetMsgFlag() & DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
					OnClientSystemMsg(pMsg);
				else
					OnClientMsg(pMsg);
				m_KeepAliveCount = 0;
				m_RecvCount++;
				m_RecvFlow += PacketSize;
				m_AssembleBuffer.PopFront(NULL, PacketSize);
				PeekPos = 0;
				PacketSize = 0;
				if (!m_AssembleBuffer.Peek(PeekPos, &PacketSize, sizeof(MSG_LEN_TYPE)))
					break;
			}
		}
	}
}

void CDOSObjectProxyConnectionDefault::OnConnection(bool IsSucceed)
{
	m_NeedDelayClose = false;
	m_KeepAliveTimer.SaveTime();
	m_KeepAliveCount = 0;
	m_RecentPingDelay = 0;
	m_RecvCount = 0;
	m_RecvFlow = 0;

	if (IsSucceed)
	{
		PrintDOSDebugLog(_T("收到代理对象的连接！IP=%s"), GetRemoteAddress().GetIPString());		
		m_Status = STATUS_CONNECTED;
		m_pService->AcceptConnection(this);
	}
	else
	{
		PrintDOSLog(_T("连接初始化失败！IP=%s"), GetRemoteAddress().GetIPString());
		m_Status = STATUS_DISCONNECTED;
		m_pService->QueryDestoryConnection(this);
	}
}
void CDOSObjectProxyConnectionDefault::OnDisconnection()
{
	PrintDOSDebugLog( _T("对象代理(%d)的连接断开！IP=%s"), GetID(), GetRemoteAddress().GetIPString());
	SendDisconnectNotify();

	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(&pPacket))
	{
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog( _T("释放消息内存块失败！"));
		}
	}
	m_MessageMap.Clear();
	m_UnacceptConnectionKeepTimer.SaveTime();
	m_Status = STATUS_DISCONNECTED;
}

int CDOSObjectProxyConnectionDefault::Update(int ProcessPacketLimit)
{
	int PacketLimit = ProcessPacketLimit / 2;
	int PacketCount = 0;

	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(&pPacket))
	{
#ifdef _DEBUG
		pPacket->SetAllocTime(0x15);
#endif
		if (pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
		{
			OnSystemMessage(pPacket);
		}			
		else
		{
			if (!OnMessage(pPacket))
			{
				PrintDOSLog(_T("处理消息失败！"));
				Disconnect();
			}
		}
			
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog( _T("释放消息内存块失败！"));
		}
		PacketCount++;
		if (PacketCount >= PacketLimit && (!m_WantClose))
			break;
	}

	PacketCount += CNetConnection::Update(PacketLimit);

	if (m_KeepAliveTimer.IsTimeOut(m_KeepAliveTime))
	{
		m_KeepAliveTimer.SaveTime();
		m_KeepAliveCount++;
		if (m_UseServerInitiativeKeepAlive)
		{
			SendKeepAliveMsg();
		}
		if (m_KeepAliveCount >= m_MaxKeepAliveCount)
		{
			PrintDOSLog( _T("KeepAlive超时！"));
			m_KeepAliveCount = 0;
			Disconnect();
		}
	}

	if (m_Config.RecvProtectCheckInterval)
	{
		if (m_RecvProtectCheckTimer.IsTimeOut(m_Config.RecvProtectCheckInterval * 1000))
		{
			m_RecvProtectCheckTimer.SaveTime();

			if (m_Config.RecvFreqProtect)
			{
				if ((m_RecvCount / m_Config.RecvProtectCheckInterval) > m_Config.RecvFreqProtect)
				{
					PrintDOSLog(_T("接受频率超出限制，断开连接！"));
					m_pService->OnRecvProtected(GetRemoteAddress());
					Disconnect();
				}
			}
			if (m_Config.RecvFlowProtect)
			{
				if ((m_RecvFlow / m_Config.RecvProtectCheckInterval) > m_Config.RecvFlowProtect)
				{
					PrintDOSLog(_T("接受流量超出限制，断开连接！"));
					m_pService->OnRecvProtected(GetRemoteAddress());
					Disconnect();
				}
			}
			m_RecvCount = 0;
			m_RecvFlow = 0;
		}
	}

	if (m_MessageMap.GetObjectCount() == 0)
	{
		if (m_UnacceptConnectionKeepTimer.IsTimeOut(m_Config.UnacceptConnectionKeepTime))
		{
			PrintDOSDebugLog( _T("未确认连接存在时间过长！"));
			Disconnect();
		}
	}

	if (m_NeedDelayClose)
	{
		if (m_DelayCloseTimer.IsTimeOut())
		{
			PrintDOSDebugLog( _T("连接延时关闭！"));
			Disconnect();
		}
	}

	return PacketCount;
}

void CDOSObjectProxyConnectionDefault::QueryDisconnect(UINT Delay)
{
	if (Delay == 0)
	{
		Disconnect();
	}
	else
	{
		m_NeedDelayClose = true;
		m_DelayCloseTimer.SetTimeOut(Delay);
	}
}

inline CDOSServer * CDOSObjectProxyConnectionDefault::GetServer()
{
	return (CDOSServer *)CNetConnection::GetServer();
}

void CDOSObjectProxyConnectionDefault::OnClientMsg(CDOSSimpleMessage * pMessage)
{
	if (!m_NeedDelayClose)
	{
		int PacketSize = CDOSMessagePacket::CaculatePacketLength(pMessage->GetDataLength(), 1);

		OBJECT_ID * pObjectID = m_MessageMap.Find(pMessage->GetMsgID());
		if (pObjectID)
		{
			WORD MsgFlag = pMessage->GetMsgFlag();
			MSG_LEN_TYPE DataLen = pMessage->GetDataLength();
			LPVOID pData = pMessage->GetDataBuffer();
			if (MsgFlag & DOS_MESSAGE_FLAG_ENCRYPT)
			{
				MsgFlag &= ~DOS_MESSAGE_FLAG_ENCRYPT;
				if (!DecyptMsg(pData, DataLen))
				{
					Disconnect();
					return;
				}
			}
			GetServer()->GetRouter()->RouterMessage(m_ObjectID, *pObjectID,
				pMessage->GetMsgID(), MsgFlag, pData, DataLen);
		}
		else
		{
			if (m_pService->HaveGlobalMsgMap(pMessage->GetMsgID()))
			{
				WORD MsgFlag = pMessage->GetMsgFlag();
				MSG_LEN_TYPE DataLen = pMessage->GetDataLength();
				LPVOID pData = pMessage->GetDataBuffer();
				if (MsgFlag & DOS_MESSAGE_FLAG_ENCRYPT)
				{
					MsgFlag &= ~DOS_MESSAGE_FLAG_ENCRYPT;
					if (!DecyptMsg(pData, DataLen))
					{
						Disconnect();
						return;
					}
				}
				m_pService->SendGlobalMapMessage(m_ObjectID, pMessage->GetMsgID(), MsgFlag, pData, DataLen);
			}
			else
			{
				PrintDOSDebugLog(_T("无法找到消息0x%X的接收者！"), pMessage->GetMsgID());
			}
		}
	}
}

void CDOSObjectProxyConnectionDefault::OnClientSystemMsg(CDOSSimpleMessage * pMessage)
{
	switch (pMessage->GetMsgID())
	{
	case DSM_PROXY_KEEP_ALIVE_PING:
		if (!m_UseServerInitiativeKeepAlive)
		{
			pMessage->SetMsgID(DSM_PROXY_KEEP_ALIVE_PONG);
			Send(pMessage, pMessage->GetMsgLength());
			if (pMessage->GetDataLength() >= sizeof(PING_DATA))
			{
				PING_DATA * pPingData = (PING_DATA *)pMessage->GetDataBuffer();
				m_RecentPingDelay = pPingData->RecentDelay;
			}
		}
		break;
	case DSM_PROXY_KEEP_ALIVE_PONG:
		if (m_UseServerInitiativeKeepAlive)
		{
			if (pMessage->GetDataLength() >= sizeof(PING_DATA))
			{
				PING_DATA * pPingData = (PING_DATA *)pMessage->GetDataBuffer();
				m_RecentPingDelay = GetTimeToTime(pPingData->Time, CEasyTimer::GetTime());
			}
		}		
		break;
	default:
		PrintDOSDebugLog( _T("对象代理连接(%d)收到未知系统消息0x%X"), GetID(), pMessage->GetMsgID());
	}
}

inline bool CDOSObjectProxyConnectionDefault::OnMessage(const CDOSMessagePacket * pPacket)
{
	bool NeedSaveHead = pPacket->GetRefCount() > 1;

	CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD MsgHeader;

	pPacket->GetMessage().MakeSimpleMessageHeader(MsgHeader);
	LPCVOID pMsgData = pPacket->GetMessage().GetDataBuffer();
	MSG_LEN_TYPE MsgDataLen = pPacket->GetMessage().GetDataLength();

	if (MsgDataLen > m_Config.MaxSendMsgSize)
	{
		PrintDOSLog(_T("(%d)消息0x%X过大(%u)无法发送"), GetID(), pPacket->GetMessage().GetMsgID(), MsgDataLen);
		return false;
	}
	
	//压缩
	LPCVOID pNewData = CompressMsg(pMsgData, MsgDataLen);
	if (pNewData == NULL)
	{
		PrintDOSLog(_T("(%d)消息0x%X压缩失败"), GetID(), pPacket->GetMessage().GetMsgID());
		return false;
	}
	if (pNewData != pMsgData)
	{
		MsgHeader.MsgFlag |= DOS_MESSAGE_FLAG_COMPRESSED;
		MsgHeader.MsgLen = sizeof(MsgHeader) + MsgDataLen;
		pMsgData = pNewData;
	}
	//加密
	if (EncyptMsg(pMsgData, MsgDataLen, pNewData))
	{
		MsgHeader.MsgFlag |= DOS_MESSAGE_FLAG_ENCRYPT;
		MsgHeader.MsgLen = sizeof(MsgHeader) + MsgDataLen;
		pMsgData = pNewData;
	}

	LPCVOID DataBuffers[2];
	UINT DataSizes[2];

	DataBuffers[0] = &MsgHeader;
	DataSizes[0] = sizeof(MsgHeader);
	DataBuffers[1] = pMsgData;
	DataSizes[1] = MsgDataLen;

	return SendMulti(DataBuffers, DataSizes, 2);
}

bool CDOSObjectProxyConnectionDefault::OnSystemMessage(const CDOSMessagePacket * pPacket)
{
	switch (pPacket->GetMessage().GetMsgID())
	{
	case DSM_PROXY_REGISTER_MSG_MAP:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			int Count = (pPacket->GetMessage().GetDataLength()) / sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pPacket->GetMessage().GetDataBuffer());
			for (int i = 0; i < Count; i++)
			{
				DoRegisterMsgMap(pMsgIDs[i], pPacket->GetMessage().GetSenderID());
			}
		}
		return true;
	case DSM_PROXY_UNREGISTER_MSG_MAP:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			int Count = (pPacket->GetMessage().GetDataLength()) / sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pPacket->GetMessage().GetDataBuffer());
			for (int i = 0; i < Count; i++)
			{
				DoUnregisterMsgMap(pMsgIDs[i], pPacket->GetMessage().GetSenderID());
			}
		}
		return true;
	case DSM_PROXY_DISCONNECT:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(UINT))
		{
			UINT Delay = *((UINT *)(pPacket->GetMessage().GetDataBuffer()));
			QueryDisconnect(Delay);
			PrintDOSDebugLog( _T("0x%llX请求在%uMS后断开连接！"), pPacket->GetMessage().GetSenderID().ID, Delay);
		}
		return true;
	case DSM_PROXY_GET_IP:
		{
			char Buff[256];
			char * pIPStr = Buff + sizeof(WORD);
			pIPStr[0] = 0;
			GetRemoteAddress().GetIPStringA(pIPStr, 250);
			*((WORD *)Buff) = GetRemoteAddress().GetPort();
			UINT Len = (UINT)strlen(pIPStr) + sizeof(WORD) + 1;
			

			GetServer()->GetRouter()->RouterMessage(m_ObjectID, pPacket->GetMessage().GetSenderID(),
				DSM_PROXY_IP_REPORT, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, Buff, Len);
		}
		return true;
	case DSM_ROUTE_LINK_LOST:
		ClearMsgMapByRouterID(pPacket->GetMessage().GetSenderID().RouterID);
		return true;
	case DSM_OBJECT_ALIVE_TEST:
		{
			BYTE IsEcho = 1;
			GetServer()->GetRouter()->RouterMessage(m_ObjectID, pPacket->GetMessage().GetSenderID(),
				DSM_OBJECT_ALIVE_TEST, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &IsEcho, sizeof(IsEcho));
		}
		return true;
	default:
		PrintDOSDebugLog( _T("收到未知系统消息0x%llX"), pPacket->GetMessage().GetMsgID());
	}
	return false;
}

bool CDOSObjectProxyConnectionDefault::SendDisconnectNotify()
{
	int MsgMapCount = m_MessageMap.GetObjectCount();
	if (MsgMapCount)
	{
		int PacketSize = CDOSMessagePacket::CaculatePacketLength(0, MsgMapCount);

		CDOSMessagePacket * pNewPacket = GetServer()->NewMessagePacket(PacketSize);
		if (pNewPacket == NULL)
		{
			PrintDOSLog( _T("分配消息内存块失败！"));
			return false;
		}

		pNewPacket->SetTargetIDs(0, NULL);
		pNewPacket->GetMessage().SetMsgID(DSM_PROXY_DISCONNECT);
		pNewPacket->GetMessage().SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
		pNewPacket->GetMessage().SetSenderID(m_ObjectID);
		OBJECT_ID * pTargetObjectIDs = pNewPacket->GetTargetIDs();

		void * Pos = m_MessageMap.GetFirstObjectPos();
		while (Pos)
		{
			MSG_ID_TYPE MsgID;
			OBJECT_ID * pTargetObjectID = m_MessageMap.GetNextObject(Pos, MsgID);
			pNewPacket->AddTargetID(*pTargetObjectID);
		}

		UINT RealTargetIDCount = DistinctObjectID(pTargetObjectIDs, MsgMapCount);
		for (UINT i = 0; i < RealTargetIDCount; i++)
		{
			PrintDOSDebugLog(_T("向[0x%llX]发送代理对象断线通知"), pTargetObjectIDs[i]);
		}
		pNewPacket->SetTargetIDs(RealTargetIDCount, NULL);
		pNewPacket->MakePacketLength();

		bool Ret = GetServer()->GetRouter()->RouterMessage(pNewPacket) != FALSE;
		GetServer()->ReleaseMessagePacket(pNewPacket);
		return Ret;
	}
	else
	{
		return true;
	}
}

void CDOSObjectProxyConnectionDefault::SendKeepAliveMsg()
{
	char * Buffer[sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD) + sizeof(PING_DATA)];
	CDOSSimpleMessage * pKeepAliveMsg = (CDOSSimpleMessage *)Buffer;
	pKeepAliveMsg->Init();
	pKeepAliveMsg->SetMsgID(DSM_PROXY_KEEP_ALIVE_PING);
	pKeepAliveMsg->SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	pKeepAliveMsg->SetDataLength(sizeof(PING_DATA));
	PING_DATA * pPingData = (PING_DATA *)pKeepAliveMsg->GetDataBuffer();
	pPingData->Time = CEasyTimer::GetTime();
	pPingData->RecentDelay = m_RecentPingDelay;
	
	Send(pKeepAliveMsg, pKeepAliveMsg->GetMsgLength());
}


bool CDOSObjectProxyConnectionDefault::DoRegisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
{
	OBJECT_ID * pObjectID = m_MessageMap.Find(MsgID);
	//if(pObjectID)
	//{
	//	PrintDOSLog(_T("0x%llX注册的代理[0x%X]消息映射[0x%X]被[0x%llX]取代！"),*pObjectID,GetID(),MsgID,ObjectID);
	//}
	//else
	//{
	//	PrintDOSLog(_T("0x%llX注册了代理[0x%X]消息映射[0x%X]！"),ObjectID.ID,GetID(),MsgID);
	//}

	m_UnacceptConnectionKeepTimer.SaveTime();
	return m_MessageMap.Insert(MsgID, ObjectID) != 0;
}
bool CDOSObjectProxyConnectionDefault::DoUnregisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
{
	OBJECT_ID * pObjectID = m_MessageMap.Find(MsgID);
	if (pObjectID)
	{
		if (*pObjectID == ObjectID)
		{
			//PrintDOSLog(_T("0x%llX注销了代理[0x%X]消息映射[0x%X]！"),ObjectID.ID,GetID(),MsgID);
			m_UnacceptConnectionKeepTimer.SaveTime();
			return m_MessageMap.Delete(MsgID) != FALSE;
		}
		else
		{
			//PrintDOSLog(_T("0x%llX注销代理[0x%X]消息映射[0x%X],现注册对象是[0x%llX],无法注销！"),ObjectID.ID,GetID(),MsgID,*pObjectID);
			return false;
		}
	}
	else
	{
		PrintDOSDebugLog( _T("0x%llX注销代理[0x%X]消息映射[0x%X],未找到映射记录！"), ObjectID.ID, GetID(), MsgID);
		return false;
	}
}




void CDOSObjectProxyConnectionDefault::ClearMsgMapByRouterID(UINT RouterID)
{
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
	if (m_MessageMap.GetObjectCount() <= 0)
	{
		PrintDOSDebugLog( _T("代理[0x%X]已经没有任何消息映射，连接断开！"), GetID());
		Disconnect();
	}
}



const void * CDOSObjectProxyConnectionDefault::CompressMsg(const void * pData, MSG_LEN_TYPE& DataLen)
{
	if (m_Config.MsgCompressType != MSG_COMPRESS_NONE&&
		m_Config.MinMsgCompressSize&&
		m_pCompressBuffer&&m_pLZOCompressWorkBuffer&&
		DataLen >= m_Config.MinMsgCompressSize)
	{

		switch(m_Config.MsgCompressType)
		{
		case MSG_COMPRESS_LZO:
			{
				if (m_pCompressBuffer->GetBufferSize() < DataLen)
				{
					PrintDOSLog(_T("数据过大(%u)，超过压缩缓冲大小(%u)！"),
						DataLen, m_pCompressBuffer->GetBufferSize());
					return NULL;
				}

				lzo_uint OutLen = m_pCompressBuffer->GetBufferSize();
				int Result = lzo1x_1_compress((const BYTE *)pData, DataLen,
					(BYTE *)m_pCompressBuffer->GetBuffer(), &OutLen,
					m_pLZOCompressWorkBuffer);

				if(Result==LZO_E_OK)
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					pData = m_pCompressBuffer->GetBuffer();
				}
				else
				{
					PrintDOSLog(_T("lzo压缩消息失败(%d)，将直接发送"),
						Result);
				}
			}
			break;
		case MSG_COMPRESS_ZIP_FAST:
		case MSG_COMPRESS_ZIP_NORMAL:
		case MSG_COMPRESS_ZIP_SLOW:
			//{
			//	UINT CompressBuffSize=compressBound(pMsg->GetMsgLength());
			//	if(m_CompressBuffer.GetBufferSize()<CompressBuffSize)
			//	{
			//		if(!m_CompressBuffer.Create(CompressBuffSize))
			//		{
			//			PrintDOSLog(_T("创建%u大小的压缩缓冲失败，关闭连接！"),
			//				pMsg->GetMsgLength());

			//			return NULL;
			//		}
			//	}
			//	CDOSSimpleMessage * pNewMsg=(CDOSSimpleMessage *)m_CompressBuffer.GetBuffer();
			//	pNewMsg->GetMsgHeader()=pMsg->GetMsgHeader();
			//	pNewMsg->GetMsgHeader().MsgFlag|=DOS_MESSAGE_FLAG_COMPRESSED;

			//	int ZipLevel=Z_DEFAULT_COMPRESSION;
			//	switch((m_Config.ProxyMsgCompressType)
			//	{
			//	case MSG_COMPRESS_ZIP_FAST:
			//		ZipLevel=Z_BEST_SPEED;
			//		break;
			//	case MSG_COMPRESS_ZIP_NORMAL:
			//		ZipLevel=Z_DEFAULT_COMPRESSION;
			//		break;
			//	case MSG_COMPRESS_ZIP_SLOW:
			//		ZipLevel=Z_BEST_COMPRESSION;
			//		break;
			//	}
			//	uLongf OutLen=m_CompressBuffer.GetBufferSize()-sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD);
			//	int Result=compress2((Bytef *)pNewMsg->GetDataBuffer(),&OutLen,
			//		(Bytef *)pMsg->GetDataBuffer(),pMsg->GetDataLength(),
			//		ZipLevel);

			//	if(Result==Z_OK)
			//	{
			//		pNewMsg->SetDataLength(OutLen);
			//		pMsg=pNewMsg;
			//	}
			//	else
			//	{
			//		PrintDOSLog(_T("zip压缩消息失败(%d)，将直接发送"),
			//			Result);
			//	}
			//}
			break;
		}
	}

	return pData;
}

bool CDOSObjectProxyConnectionDefault::EncyptMsg(const void * pData, MSG_LEN_TYPE& DataLen, LPCVOID& pOut)
{
	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE && m_Config.SecretKey.GetLength() && m_pCompressBuffer)
	{

		switch (m_Config.MsgEnCryptType)
		{
		case MSG_ENCRYPT_DES:
			{
				if (m_pCompressBuffer->GetBufferSize() < DataLen)
				{
					PrintDOSLog(_T("数据过大(%u)，超过压缩缓冲大小(%u)！"),
						DataLen, m_pCompressBuffer->GetBufferSize());
					return false;
				}
				
				size_t OutLen = m_pCompressBuffer->GetBufferSize();
				if (CCryptTools::DESEncryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(),
					(const BYTE *)pData, DataLen, (BYTE *)m_pCompressBuffer->GetBuffer(), OutLen))
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					pOut = m_pCompressBuffer->GetBuffer();
					return true;
				}
				else
				{
					PrintDOSLog(_T("DES加密消息失败，将直接发送"));
				}
			}			
			break;
		case MSG_ENCRYPT_AES:
			{
				if (m_pCompressBuffer->GetBufferSize() < DataLen)
				{
					PrintDOSLog(_T("数据过大(%u)，超过压缩缓冲大小(%u)！"),
						DataLen, m_pCompressBuffer->GetBufferSize());
					return false;
				}
				size_t OutLen = m_pCompressBuffer->GetBufferSize();
				if (CCryptTools::AESEncryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(),
					(const BYTE *)pData, DataLen, (BYTE *)m_pCompressBuffer->GetBuffer(), OutLen))
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					pOut = m_pCompressBuffer->GetBuffer();
					return true;
				}
				else
				{
					PrintDOSLog(_T("AES加密消息失败，将直接发送"));
				}
			}
			break;
		case MSG_ENCRYPT_TEA:
			{
				if (m_pCompressBuffer->GetBufferSize() < DataLen)
				{
					PrintDOSLog(_T("数据过大(%u)，超过压缩缓冲大小(%u)！"),
						DataLen, m_pCompressBuffer->GetBufferSize());
					return false;
				}
				size_t OutLen = m_pCompressBuffer->GetBufferSize();
				if (CCryptTools::TEAEncryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(), DEFAULT_TEA_CYCLE,
					(const BYTE *)pData, DataLen, (BYTE *)m_pCompressBuffer->GetBuffer(), OutLen))
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					pOut = m_pCompressBuffer->GetBuffer();
					return true;
				}
				else
				{
					PrintDOSLog(_T("TEA加密消息失败，将直接发送"));
				}
			}
			break;
		}
	}

	return false;
}

MSG_LEN_TYPE CDOSObjectProxyConnectionDefault::DecyptMsg(void * pData, MSG_LEN_TYPE DataLen)
{
	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE && m_Config.SecretKey.GetLength())
	{
		switch (m_Config.MsgEnCryptType)
		{
		case MSG_ENCRYPT_DES:
			{			
				size_t OutLen = DataLen;
				if (CCryptTools::DESDecryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(),
					(BYTE *)pData, DataLen, (BYTE *)pData, OutLen))
				{
					return (MSG_LEN_TYPE)OutLen;
				}
				else
				{
					PrintDOSLog(_T("DES解密消息失败"));
				}
			}
			break;
		case MSG_ENCRYPT_AES:
			{
				size_t OutLen = DataLen;
				if (CCryptTools::AESDecryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(),
					(BYTE *)pData, DataLen, (BYTE *)pData, OutLen))
				{
					return (MSG_LEN_TYPE)OutLen;
				}
				else
				{
					PrintDOSLog(_T("AES解密消息失败"));
				}
			}
			break;
		case MSG_ENCRYPT_TEA:
			{
				size_t OutLen = DataLen;
				if (CCryptTools::TEADecryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(), DEFAULT_TEA_CYCLE,
					(BYTE *)pData, DataLen, (BYTE *)pData, OutLen))
				{
					return (MSG_LEN_TYPE)OutLen;
				}
				else
				{
					PrintDOSLog(_T("TEA解密消息失败"));
				}
			}
			break;
		}
	}

	return 0;
}