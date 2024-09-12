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


static void DumpBinData(const BYTE * pData, UINT DataLen, UINT MaxDumpSize)
{
	static UINT Index = 0;

	PrintDOSLog(_T("(%u)DumpBinData=%u"), Index, DataLen);
	AtomicInc(&Index);
	if (DataLen > MaxDumpSize)
		DataLen = MaxDumpSize;
	for (UINT i = 0; i < DataLen; i += 8)
	{
		PrintDOSLog(_T("%02X %02X %02X %02X %02X %02X %02X %02X"),
			((i < DataLen) ? pData[i] : 0),
			((i + 1 < DataLen) ? pData[i + 1] : 0),
			((i + 2 < DataLen) ? pData[i + 2] : 0),
			((i + 3 < DataLen) ? pData[i + 3] : 0),
			((i + 4 < DataLen) ? pData[i + 4] : 0),
			((i + 5 < DataLen) ? pData[i + 5] : 0),
			((i + 6 < DataLen) ? pData[i + 6] : 0),
			((i + 7 < DataLen) ? pData[i + 7] : 0)
		);
	}
}

#undef  PrintDOSLog
#undef  PrintDOSDebugLog

#ifdef WIN32
#define PrintDOSLog(_Format, ...)	LogConnection(_T(__FUNCTION__), _Format, ##__VA_ARGS__)
#define PrintDOSDebugLog(_Format, ...)	LogDebugConnection(_T(__FUNCTION__), _Format, ##__VA_ARGS__)
#else
#define PrintDOSLog(_Format, ...)	LogConnection(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)
#define PrintDOSDebugLog(_Format, ...)	LogDebugConnection(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)
#endif



CDOSObjectProxyConnectionDefault::CDOSObjectProxyConnectionDefault(void)
{
	m_RecentPingDelay = 0;
	m_pService = NULL;
	m_pGroup = NULL;
	m_Status = STATUS_NONE;
	m_pCompressBuffer = NULL;
	m_pEncyptBuffer = NULL;
	m_pLZOCompressWorkBuffer = NULL;
	m_RecvCount = 0;
	m_RecvFlow = 0;
	m_ReleaseTime = 0;
	m_TotalMsgSendCount = 0;
	m_TotalMsgRecvCount = 0;
	m_BroadcastMask = 0;
	m_MsgQueue.SetTag(_T("CDOSObjectProxyConnectionDefault"));
	m_MessageMap.SetTag(_T("CDOSObjectProxyConnectionDefault"));
	m_AssembleBuffer.SetTag(_T("CDOSObjectProxyConnectionDefault"));
	m_BroadcastGroupList.SetTag(_T("CDOSObjectProxyConnectionDefault"));
}

CDOSObjectProxyConnectionDefault::~CDOSObjectProxyConnectionDefault(void)
{
	PrintDOSDebugLog(_T("连接销毁%u"), GetID());
}

void CDOSObjectProxyConnectionDefault::Destory()
{
	m_pCompressBuffer = NULL;
	m_pEncyptBuffer = NULL;
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
	m_MessageMap.Clear();
	m_AssembleBuffer.SetUsedSize(0);
	m_BroadcastGroupList.Clear();
	m_Status = STATUS_DESTORYED;
	m_ReleaseTime = (UINT)time(NULL);
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

	PrintDOSDebugLog(_T("连接开始初始化%u"), GetID());

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
	m_TotalMsgSendCount = 0;
	m_TotalMsgRecvCount = 0;
	m_BroadcastMask = 0;

	if (m_AssembleBuffer.GetBufferSize() < m_Config.MaxMsgSize * 2)
	{
		if (!m_AssembleBuffer.Create(m_Config.MaxMsgSize * 2))
		{
			PrintDOSLog(_T("创建%u大小的拼包缓冲失败！"),
				m_Config.MaxMsgSize * 2);
			return false;
		}
	}
	else
	{
		m_AssembleBuffer.SetUsedSize(0);
	}
	if (m_BroadcastGroupList.GetBufferSize())
	{
		m_BroadcastGroupList.Clear();
	}
	else
	{
		if (!m_BroadcastGroupList.Create(m_Config.BroadcastGroupPoolSetting))
		{
			PrintDOSLog(_T("创建群发组列表失败！"));
			return false;
		}
	}
	m_Status = STATUS_INITED;
	return true;
}



void CDOSObjectProxyConnectionDefault::OnRecvData(const BYTE * pData, UINT DataSize)
{
	MSG_LEN_TYPE PacketSize = 0;
	size_t PeekPos = 0;
	if (!m_AssembleBuffer.PushBack(pData, DataSize))
	{
		PrintDOSLog(_T("对象代理拼包缓冲溢出(%u/%u)，连接断开！"), m_AssembleBuffer.GetUsedSize(), m_AssembleBuffer.GetBufferSize());
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
					PrintDOSLog( _T("对象代理收到非法包，连接断开！"));
				}
				CDOSSimpleMessage * pMsg = (CDOSSimpleMessage *)m_AssembleBuffer.GetBuffer();
				ProcessClientMsg(pMsg->GetMsgID(), pMsg->GetMsgFlag(), pMsg->GetCRC(), pMsg->GetMsgData(), pMsg->GetDataLength());				
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

		SendProtocolOption();
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
	PrintDOSDebugLog(_T("对象代理的连接断开！IP=%s,MsgRecv=%u,MsgSend=%u"),
		GetRemoteAddress().GetIPString(), m_TotalMsgRecvCount, m_TotalMsgSendCount);
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
	m_AssembleBuffer.SetUsedSize(0);
	m_UnacceptConnectionKeepTimer.SaveTime();
	m_TotalMsgSendCount = 0;
	m_TotalMsgRecvCount = 0;
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
			if (!OnMessage(pPacket->GetMessage().GetMsgID(), pPacket->GetMessage().GetMsgFlag(), pPacket->GetMessage().GetMsgData(), pPacket->GetMessage().GetDataLength()))
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
			PrintDOSLog(_T("KeepAlive超时！BuffLeft=%u"), m_AssembleBuffer.GetUsedSize());
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
void CDOSObjectProxyConnectionDefault::ProcessClientMsg(MSG_ID_TYPE MsgID, WORD MsgFlag, WORD CRC, const void* pData, MSG_LEN_TYPE DataLen)
{
	if (!m_NeedDelayClose)
	{
		if (MsgFlag & DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
		{
			OnClientSystemMsg(MsgID, MsgFlag, CRC, pData, DataLen);
		}			
		else
		{
			m_TotalMsgRecvCount++;

			if (DataLen < m_Config.MinMsgSize)
			{
				PrintDOSLog(_T("消息大小%u小于最小大小%u"), DataLen, m_Config.MinMsgSize);
				Disconnect();
				return;
			}
			else if (MsgFlag & DOS_MESSAGE_FLAG_ENCRYPT)
			{
				if (m_Config.DumpMsg)
				{
					PrintDOSLog(_T("Msg:0x%X,CRC=0x%X,MsgCount=%d"), MsgID, CRC, m_TotalMsgRecvCount);
					DumpBinData((const BYTE*)pData, DataLen, m_Config.MaxMsgDumpSize);
				}

				MsgFlag &= ~DOS_MESSAGE_FLAG_ENCRYPT;
				pData = DecyptMsg((LPVOID)pData, DataLen, CRC, m_TotalMsgRecvCount);
				if (pData == NULL)
				{
					Disconnect();
					return;
				}
				if (m_Config.DumpMsg)
					DumpBinData((const BYTE*)pData, DataLen, m_Config.MaxMsgDumpSize);

			}
			else
			{
				if (m_Config.MsgEnCryptMode == MSG_ENCRYPT_MODE_FORCE)
				{
					PrintDOSLog(_T("消息未加密"));
					Disconnect();
					return;
				}
			}
			OnClientMsg(MsgID, MsgFlag, CRC, pData, DataLen);
		}			
	}
}
void CDOSObjectProxyConnectionDefault::OnClientMsg(MSG_ID_TYPE MsgID, WORD MsgFlag, WORD CRC, const void* pData, MSG_LEN_TYPE DataLen)
{
	if (!m_NeedDelayClose)
	{
		
		int PacketSize = CDOSMessagePacket::CaculatePacketLength(DataLen, 1);

		OBJECT_ID * pObjectID = m_MessageMap.Find(MsgID);
		if (pObjectID)
		{
			
			GetServer()->GetRouter()->RouterMessage(m_ObjectID, *pObjectID,
				MsgID, MsgFlag, pData, DataLen);
		}
		else
		{
			if (m_pService->HaveGlobalMsgMap(MsgID))
			{				
				m_pService->SendGlobalMapMessage(m_ObjectID, MsgID, MsgFlag, pData, DataLen);
			}
			else
			{
				PrintDOSDebugLog(_T("无法找到消息0x%X的接收者！"), MsgID);
			}
		}
	}
}

void CDOSObjectProxyConnectionDefault::OnClientSystemMsg(MSG_ID_TYPE MsgID, WORD MsgFlag, WORD CRC, const void* pData, MSG_LEN_TYPE DataLen)
{
	switch (MsgID)
	{
	case DSM_PROXY_KEEP_ALIVE_PING:
		if (DataLen >= sizeof(PING_DATA))
		{
			if (!m_UseServerInitiativeKeepAlive)
			{
				const PING_DATA* pPingData = (const PING_DATA*)pData;
				OnKeepAliveMsg(pPingData->Time, pPingData->RecentDelay);
			}
		}
		break;
	case DSM_PROXY_KEEP_ALIVE_PONG:
		
		if (DataLen >= sizeof(PING_DATA))
		{
			if (m_UseServerInitiativeKeepAlive)
			{
				const PING_DATA* pPingData = (const PING_DATA*)pData;
				OnKeepAliveAckMsg(pPingData->Time);
			}
		}
		break;
	default:
		PrintDOSDebugLog( _T("对象代理连接收到未知系统消息0x%X"), MsgID);
	}
}

inline bool CDOSObjectProxyConnectionDefault::OnMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, const void * pData, MSG_LEN_TYPE DataLen)
{
	m_TotalMsgSendCount++;

	CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD MsgHeader;

	MsgHeader.MsgID = MsgID;
	MsgHeader.MsgFlag = MsgFlag;
	MsgHeader.CRC = 0;
	
	
	if ((m_Config.MsgCompressType != MSG_COMPRESS_NONE) && ((MsgFlag&DOS_MESSAGE_FLAG_NO_COMPRESS) == 0))
	{
		//压缩
		LPCVOID pNewData = CompressMsg(pData, DataLen);
		if (pNewData == NULL)
		{
			PrintDOSLog(_T("消息0x%X压缩失败"), MsgID);
			return false;
		}
		if (pNewData != pData)
		{
			MsgHeader.MsgFlag |= DOS_MESSAGE_FLAG_COMPRESSED;
			pData = pNewData;
		}
	}

	MsgHeader.MsgLen = sizeof(MsgHeader) + DataLen;

	LPCVOID DataBuffers[2];
	UINT DataSizes[2];

	DataBuffers[0] = &MsgHeader;
	DataSizes[0] = sizeof(MsgHeader);
	DataBuffers[1] = pData;
	DataSizes[1] = DataLen;

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
			const MSG_ID_TYPE * pMsgIDs = (const MSG_ID_TYPE *)(pPacket->GetMessage().GetMsgData());
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
			const MSG_ID_TYPE * pMsgIDs = (const MSG_ID_TYPE *)(pPacket->GetMessage().GetMsgData());
			for (int i = 0; i < Count; i++)
			{
				DoUnregisterMsgMap(pMsgIDs[i], pPacket->GetMessage().GetSenderID());
			}
		}
		return true;
	case DSM_PROXY_DISCONNECT:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(UINT))
		{
			UINT Delay = *((const UINT *)(pPacket->GetMessage().GetMsgData()));
			QueryDisconnect(Delay);
			PrintDOSDebugLog( _T("0x%llX请求在%uMS后断开连接%s！"), pPacket->GetMessage().GetSenderID().ID, Delay,GetRemoteAddress().GetIPString());
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
	case DSM_PROXY_BROADCAST_BY_MASK:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(MASK_BROADCAST_INFO))
		{
			const BYTE * pData = (const BYTE *)pPacket->GetMessage().GetMsgData();
			MSG_LEN_TYPE DataLen = pPacket->GetMessage().GetDataLength();
			MASK_BROADCAST_INFO * pInfo = (MASK_BROADCAST_INFO *)pData;
			pData += sizeof(MASK_BROADCAST_INFO);
			DataLen -= sizeof(MASK_BROADCAST_INFO);

			OnMessage(pInfo->MsgID, pInfo->MsgFlag, pData, DataLen);
		}
		return true;
	case DSM_PROXY_BROADCAST_BY_GROUP:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(GROUP_BROADCAST_INFO))
		{
			const BYTE* pData = (const BYTE*)pPacket->GetMessage().GetMsgData();
			MSG_LEN_TYPE DataLen = pPacket->GetMessage().GetDataLength();
			GROUP_BROADCAST_INFO* pInfo = (GROUP_BROADCAST_INFO*)pData;
			pData += sizeof(GROUP_BROADCAST_INFO);
			DataLen -= sizeof(GROUP_BROADCAST_INFO);

			OnMessage(pInfo->MsgID, pInfo->MsgFlag, pData, DataLen);
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
		pNewPacket->SetTargetIDCount(0);
		void * Pos = m_MessageMap.GetFirstObjectPos();
		while (Pos)
		{
			MSG_ID_TYPE MsgID;
			OBJECT_ID * pTargetObjectID = m_MessageMap.GetNextObject(Pos, MsgID);
			pNewPacket->AddTargetID(*pTargetObjectID);
		}		
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
void CDOSObjectProxyConnectionDefault::OnKeepAliveMsg(UINT Timestamp, UINT RecentDelay)
{
	m_RecentPingDelay = RecentDelay;

	BYTE Buffer[sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD) + sizeof(PING_DATA)];
	CDOSSimpleMessage* pKeepAliveMsg = (CDOSSimpleMessage*)Buffer;
	pKeepAliveMsg->Init();
	pKeepAliveMsg->SetMsgID(DSM_PROXY_KEEP_ALIVE_PONG);
	pKeepAliveMsg->SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	pKeepAliveMsg->SetDataLength(sizeof(PING_DATA));
	PING_DATA* pPingData = (PING_DATA*)pKeepAliveMsg->GetMsgData();
	pPingData->Time = Timestamp;
	pPingData->RecentDelay = m_RecentPingDelay;

	Send(pKeepAliveMsg, pKeepAliveMsg->GetMsgLength());
}
void CDOSObjectProxyConnectionDefault::OnKeepAliveAckMsg(UINT Timestamp)
{
	m_RecentPingDelay = CEasyTimer::GetTimeToTime(Timestamp, CEasyTimer::GetTime());
}
void CDOSObjectProxyConnectionDefault::SendKeepAliveMsg()
{
	BYTE Buffer[sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD) + sizeof(PING_DATA)];
	CDOSSimpleMessage * pKeepAliveMsg = (CDOSSimpleMessage *)Buffer;
	pKeepAliveMsg->Init();
	pKeepAliveMsg->SetMsgID(DSM_PROXY_KEEP_ALIVE_PING);
	pKeepAliveMsg->SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	pKeepAliveMsg->SetDataLength(sizeof(PING_DATA));
	PING_DATA * pPingData = (PING_DATA *)pKeepAliveMsg->GetMsgData();
	pPingData->Time = CEasyTimer::GetTime();
	pPingData->RecentDelay = m_RecentPingDelay;
	
	Send(pKeepAliveMsg, pKeepAliveMsg->GetMsgLength());
}


void CDOSObjectProxyConnectionDefault::SendProtocolOption()
{
	BYTE Buffer[sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD) + sizeof(PROTOCOL_OPTION)];
	CDOSSimpleMessage * pMsg = (CDOSSimpleMessage *)Buffer;
	pMsg->Init();
	pMsg->SetMsgID(DSM_PROTOCOL_OPTION);
	pMsg->SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	pMsg->SetDataLength(sizeof(PROTOCOL_OPTION));
	PROTOCOL_OPTION * pData = (PROTOCOL_OPTION *)pMsg->GetMsgData();
	pData->Flag = 0;
	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE)
		pData->Flag |= PROTOCOL_OPTION_FLAG_UP_MSG_USE_ENCRYPT;

	Send(pMsg, pMsg->GetMsgLength());

	PrintDOSDebugLog(_T("已发送协议配置0x%X"), pData->Flag);
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
		//PrintDOSDebugLog( _T("0x%llX注销代理[0x%llX]消息映射[0x%X],未找到映射记录！"), ObjectID.ID, GetID(), MsgID);
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
		PrintDOSDebugLog( _T("代理已经没有任何消息映射，连接断开！"));
		Disconnect();
	}
}



const void * CDOSObjectProxyConnectionDefault::CompressMsg(const void * pData, MSG_LEN_TYPE& DataLen)
{
	if (m_Config.MinMsgCompressSize&&
		m_pCompressBuffer&&m_pLZOCompressWorkBuffer&&
		DataLen >= m_Config.MinMsgCompressSize)
	{

		switch(m_Config.MsgCompressType)
		{
		case MSG_COMPRESS_LZO:
			{
				if (m_pCompressBuffer->GetBufferSize() < DataLen + sizeof(UINT))
				{
					PrintDOSLog(_T("数据过大(%u)，超过压缩缓冲大小(%u)！"),
						DataLen, m_pCompressBuffer->GetBufferSize());
					return NULL;
				}

				
				BYTE * pOutBuffer = (BYTE *)m_pCompressBuffer->GetBuffer();
				*((UINT *)pOutBuffer) = DataLen;
				pOutBuffer += sizeof(UINT);
				lzo_uint OutLen = m_pCompressBuffer->GetBufferSize() - sizeof(UINT);
				int Result = lzo1x_1_compress((const BYTE *)pData, DataLen,
					pOutBuffer, &OutLen,
					m_pLZOCompressWorkBuffer);

				if(Result==LZO_E_OK)
				{
					DataLen = (MSG_LEN_TYPE)OutLen + sizeof(UINT);
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

const void * CDOSObjectProxyConnectionDefault::EncyptMsg(const void * pData, MSG_LEN_TYPE& DataLen, WORD& CRC, UINT MsgSequence)
{
	CRC = 0;
	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE && m_Config.SecretKey.GetLength() && m_pEncyptBuffer)
	{
		CRC = MakeCRC(pData, DataLen, m_Config.SecretKey.GetBuffer(), (UINT)m_Config.SecretKey.GetLength(), MsgSequence);

		if (m_pEncyptBuffer->GetBufferSize() < DataLen)
		{
			PrintDOSLog(_T("数据过大(%u)，超过加密缓冲大小(%u)！"),
				DataLen, m_pEncyptBuffer->GetBufferSize());
			return NULL;
		}

		switch (m_Config.MsgEnCryptType)
		{
		case MSG_ENCRYPT_CHECK_SUM:
			return pData;
			break;
		case MSG_ENCRYPT_DES:
			{
				size_t OutLen = m_pEncyptBuffer->GetBufferSize();
				if (CCryptTools::DESEncryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(),
					(const BYTE *)pData, DataLen, (BYTE *)m_pEncyptBuffer->GetBuffer(), OutLen))
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					return m_pEncyptBuffer->GetBuffer();
				}
				else
				{
					PrintDOSLog(_T("DES加密消息失败，将直接发送"));
				}
			}			
			break;
		case MSG_ENCRYPT_AES:
			{				
				size_t OutLen = m_pEncyptBuffer->GetBufferSize();
				if (CCryptTools::AESEncryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(),
					(const BYTE *)pData, DataLen, (BYTE *)m_pEncyptBuffer->GetBuffer(), OutLen))
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					return m_pEncyptBuffer->GetBuffer();
				}
				else
				{
					PrintDOSLog(_T("AES加密消息失败，将直接发送"));
				}
			}
			break;
		case MSG_ENCRYPT_TEA:
			{				
				size_t OutLen = m_pEncyptBuffer->GetBufferSize();
				if (CCryptTools::TEAEncryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(), DEFAULT_TEA_CYCLE,
					(const BYTE *)pData, DataLen, (BYTE *)m_pEncyptBuffer->GetBuffer(), OutLen))
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					return m_pEncyptBuffer->GetBuffer();
				}
				else
				{
					PrintDOSLog(_T("TEA加密消息失败，将直接发送"));
				}
			}
			break;
		default:
			PrintDOSLog(_T("未知加密方式"));
		}
	}

	return NULL;
}

void * CDOSObjectProxyConnectionDefault::DecyptMsg(void * pData, MSG_LEN_TYPE& DataLen, WORD CRC, UINT MsgSequence)
{
	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE && m_Config.SecretKey.GetLength() && m_pEncyptBuffer)
	{
		if (m_pEncyptBuffer->GetBufferSize() < DataLen)
		{
			PrintDOSLog(_T("数据过大(%u)，超过加密缓冲大小(%u)！"),
				DataLen, m_pEncyptBuffer->GetBufferSize());
			return NULL;
		}

		switch (m_Config.MsgEnCryptType)
		{
		case MSG_ENCRYPT_CHECK_SUM:
			{
				WORD TargetCRC = MakeCRC(pData, (UINT)DataLen, m_Config.SecretKey.GetBuffer(), (UINT)m_Config.SecretKey.GetLength(), MsgSequence);
				if (TargetCRC == CRC)
				{
					return pData;
				}
				else
				{
					PrintDOSLog(_T("CHECK_SUM:CRC校验失败"));
				}
			}
			break;
		case MSG_ENCRYPT_DES:
			{
				size_t OutLen = DataLen;
				if (CCryptTools::DESDecryptECB((const BYTE *)m_Config.SecretKey.GetBuffer(), m_Config.SecretKey.GetLength(),
					(BYTE *)pData, DataLen, (BYTE *)m_pEncyptBuffer->GetBuffer(), OutLen))
				{					
					WORD TargetCRC = MakeCRC(m_pEncyptBuffer->GetBuffer(), (UINT)OutLen, m_Config.SecretKey.GetBuffer(), (UINT)m_Config.SecretKey.GetLength(), MsgSequence);
					if (TargetCRC == CRC)
					{
						DataLen = (UINT)OutLen;
						return m_pEncyptBuffer->GetBuffer();
					}
					else
					{
						PrintDOSLog(_T("DES:CRC校验失败"));
					}
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
					(BYTE *)pData, DataLen, (BYTE *)m_pEncyptBuffer->GetBuffer(), OutLen))
				{					
					WORD TargetCRC = MakeCRC(m_pEncyptBuffer->GetBuffer(), (UINT)OutLen, m_Config.SecretKey.GetBuffer(), (UINT)m_Config.SecretKey.GetLength(), MsgSequence);
					if (TargetCRC == CRC)
					{
						DataLen = (UINT)OutLen;
						return m_pEncyptBuffer->GetBuffer();
					}
					else
					{
						PrintDOSLog(_T("AES:CRC校验失败"));
					}
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
					(BYTE *)pData, DataLen, (BYTE *)m_pEncyptBuffer->GetBuffer(), OutLen))
				{
					WORD TargetCRC = MakeCRC(m_pEncyptBuffer->GetBuffer(), (UINT)OutLen, m_Config.SecretKey.GetBuffer(), (UINT)m_Config.SecretKey.GetLength(), MsgSequence);
					if (TargetCRC == CRC)
					{
						DataLen = (UINT)OutLen;
						return m_pEncyptBuffer->GetBuffer();
					}
					else
					{
						PrintDOSLog(_T("TEA:CRC校验失败"));
						if (m_Config.DumpMsg)
							DumpBinData((const BYTE *)m_pEncyptBuffer->GetBuffer(), OutLen, m_Config.MaxMsgDumpSize);
					}
				}
				else
				{
					PrintDOSLog(_T("TEA解密消息失败"));
				}
			}
			break;
		default:
			PrintDOSLog(_T("未知加密方式"));
		}
	}

	return NULL;
}

WORD CDOSObjectProxyConnectionDefault::MakeCRC(const void * pData, UINT DataLen, const void * pKey, UINT KeyLen, UINT MsgSequence)
{
	CCRCMaker CRCMaker;
	CRCMaker.CRC16_Fast_Init();
	if (pData)
		CRCMaker.CRC16_Fast_Append((const BYTE *)pData, DataLen);
	if(pKey)
		CRCMaker.CRC16_Fast_Append((const BYTE *)pKey, KeyLen);
	CRCMaker.CRC16_Fast_Append((const BYTE *)&MsgSequence, sizeof(UINT));
	return CRCMaker.CRC16_Fast_Final();
}