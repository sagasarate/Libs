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

static char s_LZOCompressWorkMemory[LZO1X_1_MEM_COMPRESS];


CDOSObjectProxyConnectionDefault::CDOSObjectProxyConnectionDefault(void)
{
	m_RecentPingDelay = 0;
}

CDOSObjectProxyConnectionDefault::~CDOSObjectProxyConnectionDefault(void)
{
}

void CDOSObjectProxyConnectionDefault::Destory()
{
	CNetConnection::Destory();
	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(pPacket))
	{
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"), _T("CDOSProxyConnectionBase::Destory:释放消息内存块失败！"));
		}
	}
	m_MessageMap.Clear();
}
UINT CDOSObjectProxyConnectionDefault::AddUseRef()
{
	return CNetConnection::AddUseRef();
}
void CDOSObjectProxyConnectionDefault::Release()
{
	CNetConnection::Release();
}

bool CDOSObjectProxyConnectionDefault::PushMessage(CDOSMessagePacket * pPacket)
{
	if (!IsConnected())
		return false;

	((CDOSServer *)GetServer())->AddRefMessagePacket(pPacket);
	pPacket->SetAllocTime(3);
	if (m_MsgQueue.PushBack(pPacket))
	{
		pPacket->SetAllocTime(31);
		return true;
	}
	else
	{
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"), _T("CDOSProxyConnectionBase::PushMessage:释放消息内存块失败！"));
		}
		return false;
	}	
}



bool CDOSObjectProxyConnectionDefault::Init(CDOSObjectProxyServiceDefault * pService, UINT ID)
{
	SetID(ID);
	m_pService = pService;
	SetServer(pService->GetServer());

	m_ObjectID.ObjectIndex = GetID();
	m_ObjectID.GroupIndex = MAKE_PROXY_GROUP_INDEX(m_pService->GetConfig().ProxyType, pService->GetID());
	m_ObjectID.ObjectTypeID = DOT_PROXY_OBJECT;
	m_ObjectID.RouterID = GetServer()->GetRouter()->GetRouterID();

	m_MaxKeepAliveCount = m_pService->GetConfig().KeepAliveCount;
	m_KeepAliveTime = m_pService->GetConfig().KeepAliveTime;
	m_UseServerInitiativeKeepAlive = m_pService->GetConfig().UseServerInitiativeKeepAlive;


	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(pPacket))
	{
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"), _T("CDOSProxyConnectionBase::OnDisconnection:释放消息内存块失败！"));
		}
	}

	if (m_MsgQueue.GetBufferSize() < m_pService->GetConfig().ConnectionMsgQueueSize)
	{
		if (!m_MsgQueue.Create(m_pService->GetConfig().ConnectionMsgQueueSize))
		{
			PrintDOSLog(_T("DOSLib"), _T("创建%u大小的消息队列失败！"),
				m_pService->GetConfig().ConnectionMsgQueueSize);
			return false;
		}
	}

	if (m_MessageMap.GetBufferSize() < m_pService->GetConfig().MsgMapSize)
	{
		if (!m_MessageMap.Create(m_pService->GetConfig().MsgMapSize))
		{
			PrintDOSLog(_T("DOSLib"), _T("创建%u大小的消息映射表失败！"),
				m_pService->GetConfig().MsgMapSize);
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
	m_MaxKeepAliveCount = m_pService->GetConfig().KeepAliveCount;
	m_UnacceptConnectionKeepTimer.SaveTime();
	m_NeedDelayClose = false;

	if(m_AssembleBuffer.GetBufferSize()<m_pService->GetConfig().MaxMsgSize*2)
	{
		if(!m_AssembleBuffer.Create(m_pService->GetConfig().MaxMsgSize*2))
		{
			PrintDOSLog(_T("DOSLib"),_T("创建%u大小的拼包缓冲失败！"),
				m_pService->GetConfig().MaxMsgSize*2);
			return false;
		}
	}

	return true;
}



void CDOSObjectProxyConnectionDefault::OnRecvData(const BYTE * pData, UINT DataSize)
{
	MSG_LEN_TYPE PacketSize = 0;
	UINT PeekPos = 0;
	if (!m_AssembleBuffer.PushBack(pData, DataSize))
	{
		Disconnect();
		PrintDOSLog(_T("DOSLib"), _T("对象代理(%d)拼包缓冲溢出，连接断开！"), GetID());
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
					PrintDOSLog(_T("DOSLib"), _T("对象代理(%d)收到非法包，连接断开！"), GetID());
				}
				CDOSSimpleMessage * pMsg = (CDOSSimpleMessage *)m_AssembleBuffer.GetBuffer();
				if (pMsg->GetMsgFlag() && DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
					OnClientSystemMsg(pMsg);
				else
					OnClientMsg(pMsg);
				m_KeepAliveCount = 0;
				m_AssembleBuffer.PopFront(NULL, PacketSize);
				PeekPos = 0;
				PacketSize = 0;
				if (!m_AssembleBuffer.Peek(PeekPos, &PacketSize, sizeof(MSG_LEN_TYPE)))
					break;
			}
		}
	}
}

void CDOSObjectProxyConnectionDefault::OnConnection(BOOL IsSucceed)
{
	PrintDOSLog(_T("DOSLib"), _T("收到代理对象的连接！IP=%s"), GetRemoteAddress().GetIPString());
	SetSendDelay(m_pService->GetConfig().SendDelay);
	SetSendQueryLimit(m_pService->GetConfig().SendQueryLimit);
	PrintDOSLog(_T("DOSLib"), _T("发送延时设置为%u,并发发送限制为%u"),
		m_pService->GetConfig().SendDelay,
		m_pService->GetConfig().SendQueryLimit);
	m_NeedDelayClose = false;
	m_KeepAliveTimer.SaveTime();
	m_KeepAliveCount = 0;
	m_RecentPingDelay = 0;
}
void CDOSObjectProxyConnectionDefault::OnDisconnection()
{
	PrintDOSLog(_T("DOSLib"), _T("对象代理(%d)的连接断开！IP=%s"), GetID(), GetRemoteAddress().GetIPString());
	SendDisconnectNotify();

	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(pPacket))
	{
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"), _T("CDOSProxyConnectionBase::OnDisconnection:释放消息内存块失败！"));
		}
	}
	m_MessageMap.Clear();
	m_UnacceptConnectionKeepTimer.SaveTime();
}

int CDOSObjectProxyConnectionDefault::Update(int ProcessPacketLimit)
{
	int PacketLimit = ProcessPacketLimit / 2;
	int PacketCount = 0;

	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(pPacket))
	{
		pPacket->SetAllocTime(32);
		if (pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
			OnSystemMessage(pPacket);
		else
			OnMessage(pPacket);
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"), _T("CDOSProxyConnectionBase::Update:释放消息内存块失败！"));
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
			PrintDOSLog(_T("DOSLib"), _T("CDOSProxyConnectionBase::Update:KeepAlive超时！"));
			m_KeepAliveCount = 0;
			Disconnect();
		}
	}

	if (m_MessageMap.GetObjectCount() == 0)
	{
		if (m_UnacceptConnectionKeepTimer.IsTimeOut(m_pService->GetConfig().UnacceptConnectionKeepTime))
		{
			PrintDOSLog(_T("DOSLib"), _T("CDOSProxyConnectionBase::Update:未确认连接存在时间过长！"));
			Disconnect();
		}
	}

	if (m_NeedDelayClose)
	{
		if (m_DelayCloseTimer.IsTimeOut())
		{
			PrintDOSLog(_T("DOSLib"), _T("CDOSProxyConnectionBase::Update:连接延时关闭！"));
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


		OBJECT_ID TargetObjectID = GetMsgMapObjectID(pMessage->GetMsgID());
		if (TargetObjectID.ID)
		{
			GetServer()->GetRouter()->RouterMessage(m_ObjectID, TargetObjectID, 
				pMessage->GetMsgID(), pMessage->GetMsgFlag(), pMessage->GetDataBuffer(), pMessage->GetDataLength());
		}
		else
		{
			PrintDOSLog(_T("DOSLib"), _T("无法找到消息0x%X的接收者！"), pMessage->GetMsgID());
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
		PrintDOSLog(_T("DOSLib"), _T("对象代理连接(%d)收到未知系统消息0x%X"), GetID(), pMessage->GetMsgID());
	}
}

inline bool CDOSObjectProxyConnectionDefault::OnMessage(CDOSMessagePacket * pPacket)
{
	bool NeedSaveHead = pPacket->GetRefCount() > 1;

	CDOSMessage::DOS_MESSAGE_HEAD SaveHeader;

	if (NeedSaveHead)
	{
		SaveHeader = pPacket->GetMessage().GetMsgHeader();
	}
	CDOSSimpleMessage * pSimpleMessage = pPacket->GetMessage().MakeSimpleMessage();

	pSimpleMessage = CompressMsg(pSimpleMessage);
	if (pSimpleMessage == NULL)
	{
		if (NeedSaveHead)
		{
			pPacket->GetMessage().GetMsgHeader() = SaveHeader;
		}
		Disconnect();
		return false;
	}

	bool Ret = false;

	
	Ret = Send(pSimpleMessage, pSimpleMessage->GetMsgLength()) != FALSE;
	

	if (NeedSaveHead)
	{
		pPacket->GetMessage().GetMsgHeader() = SaveHeader;
	}

	return Ret;
}

bool CDOSObjectProxyConnectionDefault::OnSystemMessage(CDOSMessagePacket * pPacket)
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
			PrintDOSLog(_T("DOSLib"), _T("0x%llX请求在%uMS后断开连接！"), pPacket->GetMessage().GetSenderID().ID, Delay);
		}
		return true;
	case DSM_PROXY_GET_IP:
		{
			char Buff[256];
			char * pIPStr = Buff + sizeof(WORD);
			pIPStr[0] = 0;
			GetRemoteAddress().GetIPString(pIPStr, 250);
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
		PrintDOSLog(_T("DOSLib"), _T("ProxyConnection收到未知系统消息0x%llX"), pPacket->GetMessage().GetMsgID());
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
			PrintDOSLog(_T("DOSLib"), _T("分配消息内存块失败！"));
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
			PrintDOSLog(_T("DOSLib"), _T("向[0x%llX]发送代理对象断线通知"), pTargetObjectIDs[i]);
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

OBJECT_ID CDOSObjectProxyConnectionDefault::GetMsgMapObjectID(MSG_ID_TYPE CmdID)
{
	OBJECT_ID * pObjectID = m_MessageMap.Find(CmdID);
	if (pObjectID)
	{
		return *pObjectID;
	}
	else
	{
		OBJECT_ID ObjectID = m_pService->GetGlobalMsgMapObjectID(CmdID);
		if (ObjectID.ID == 0)
		{
			ObjectID = m_pService->GetUnhandleMsgReceiverID();
		}
		return ObjectID;
	}
}


bool CDOSObjectProxyConnectionDefault::DoRegisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
{
	OBJECT_ID * pObjectID = m_MessageMap.Find(MsgID);
	//if(pObjectID)
	//{
	//	PrintDOSLog(_T("DOSLib"),_T("0x%llX注册的代理[0x%X]消息映射[0x%X]被[0x%llX]取代！"),*pObjectID,GetID(),MsgID,ObjectID);
	//}
	//else
	//{
	//	PrintDOSLog(_T("DOSLib"),_T("0x%llX注册了代理[0x%X]消息映射[0x%X]！"),ObjectID.ID,GetID(),MsgID);
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
			//PrintDOSLog(_T("DOSLib"),_T("0x%llX注销了代理[0x%X]消息映射[0x%X]！"),ObjectID.ID,GetID(),MsgID);
			m_UnacceptConnectionKeepTimer.SaveTime();
			return m_MessageMap.Delete(MsgID) != FALSE;
		}
		else
		{
			//PrintDOSLog(_T("DOSLib"),_T("0x%llX注销代理[0x%X]消息映射[0x%X],现注册对象是[0x%llX],无法注销！"),ObjectID.ID,GetID(),MsgID,*pObjectID);
			return false;
		}
	}
	else
	{
		PrintDOSLog(_T("DOSLib"), _T("0x%llX注销代理[0x%X]消息映射[0x%X],未找到映射记录！"), ObjectID.ID, GetID(), MsgID);
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
		PrintDOSLog(_T("DOSLib"), _T("代理[0x%X]已经没有任何消息映射，连接断开！"), GetID());
		Disconnect();
	}
}



CDOSSimpleMessage * CDOSObjectProxyConnectionDefault::CompressMsg(CDOSSimpleMessage * pMsg)
{
	if (m_pService->GetConfig().MsgCompressType != MSG_COMPRESS_NONE&&
		m_pService->GetConfig().MinMsgCompressSize&&
		pMsg->GetDataLength() >= m_pService->GetConfig().MinMsgCompressSize)
	{

		switch(m_pService->GetConfig().MsgCompressType)
		{
		case MSG_COMPRESS_LZO:
			{
				CEasyBuffer& CompressBuffer = ((CDOSObjectProxyServiceDefault *)m_pService)->GetCompressBuffer();
				if (CompressBuffer.GetBufferSize()<pMsg->GetMsgLength())
				{
					if (!CompressBuffer.Create(pMsg->GetMsgLength()))
					{
						PrintDOSLog(_T("DOSLib"),_T("创建%u大小的压缩缓冲失败，关闭连接！"),
							pMsg->GetMsgLength());

						return NULL;
					}
				}
				CDOSSimpleMessage * pNewMsg = (CDOSSimpleMessage *)CompressBuffer.GetBuffer();
				pNewMsg->GetMsgHeader()=pMsg->GetMsgHeader();
				pNewMsg->GetMsgHeader().MsgFlag|=DOS_MESSAGE_FLAG_COMPRESSED;

				lzo_uint OutLen = CompressBuffer.GetBufferSize() - sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD);
				int Result=lzo1x_1_compress((BYTE *)pMsg->GetDataBuffer(),pMsg->GetDataLength(),
					(BYTE *)pNewMsg->GetDataBuffer(),&OutLen,
					s_LZOCompressWorkMemory);

				if(Result==LZO_E_OK)
				{
					pNewMsg->SetDataLength((MSG_LEN_TYPE)OutLen);
					pMsg=pNewMsg;
				}
				else
				{
					PrintDOSLog(_T("DOSLib"),_T("lzo压缩消息失败(%d)，将直接发送"),
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
			//			PrintDOSLog(_T("DOSLib"),_T("创建%u大小的压缩缓冲失败，关闭连接！"),
			//				pMsg->GetMsgLength());

			//			return NULL;
			//		}
			//	}
			//	CDOSSimpleMessage * pNewMsg=(CDOSSimpleMessage *)m_CompressBuffer.GetBuffer();
			//	pNewMsg->GetMsgHeader()=pMsg->GetMsgHeader();
			//	pNewMsg->GetMsgHeader().MsgFlag|=DOS_MESSAGE_FLAG_COMPRESSED;

			//	int ZipLevel=Z_DEFAULT_COMPRESSION;
			//	switch((m_pService->GetConfig().ProxyMsgCompressType)
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
			//		PrintDOSLog(_T("DOSLib"),_T("zip压缩消息失败(%d)，将直接发送"),
			//			Result);
			//	}
			//}
			break;
		}
	}

	return pMsg;
}

