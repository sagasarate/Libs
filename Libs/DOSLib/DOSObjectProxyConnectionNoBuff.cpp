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



CDOSObjectProxyConnectionNoBuff::CDOSObjectProxyConnectionNoBuff(void)
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
	m_MsgQueue.SetTag(_T("CDOSObjectProxyConnectionNoBuff"));
	m_MessageMap.SetTag(_T("CDOSObjectProxyConnectionNoBuff"));
	m_BroadcastGroupList.SetTag(_T("CDOSObjectProxyConnectionDefault"));
}

CDOSObjectProxyConnectionNoBuff::~CDOSObjectProxyConnectionNoBuff(void)
{
}

void CDOSObjectProxyConnectionNoBuff::Destory()
{
	m_pCompressBuffer = NULL;
	m_pEncyptBuffer = NULL;
	m_pLZOCompressWorkBuffer = NULL;
	CNetConnectionNoBuff::Close();
	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(&pPacket))
	{
		if (!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
	}
	m_MessageMap.Clear();
	m_BroadcastGroupList.Clear();
	m_Status = STATUS_DESTORYED;
	m_ReleaseTime = (UINT)time(NULL);
}


bool CDOSObjectProxyConnectionNoBuff::PushMessage(CDOSMessagePacket * pPacket)
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



bool CDOSObjectProxyConnectionNoBuff::Init(CDOSObjectProxyServiceNoBuff * pService, const CLIENT_PROXY_CONFIG& Config, UINT ID)
{
	SetID(ID);
	m_pService = pService;
	SetServer(pService->GetServer());
	m_Config = Config;

	PrintDOSDebugLog(_T("连接开始初始化%u"), GetID());
	SetPacketHeaderSize(sizeof(MSG_LEN_TYPE));

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

	m_Status = STATUS_INITED;
	return true;
}

bool CDOSObjectProxyConnectionNoBuff::AllocRecvBuffer(const BYTE * pPacketHeader, UINT HeaderSize, LPBYTE& pBuffer, UINT& BufferSize, LPCVOID& Param)
{
	UINT MsgSize = *((MSG_LEN_TYPE *)pPacketHeader);
	CDOSMessagePacket * pPacket = GetServer()->NewMessagePacket(CDOSMessagePacket::CaculatePacketLength(MsgSize, 1));
	if (pPacket)
	{
		pBuffer = (LPBYTE)pPacket->GetMessage().MakeSimpleMessage();
		BufferSize = MsgSize;
		Param = pPacket;
		return true;
	}
	return false;
}
void CDOSObjectProxyConnectionNoBuff::ReleaseRecvBuffer(BYTE * pBuffer, LPCVOID Param)
{
	if (!GetServer()->ReleaseMessagePacket((CDOSMessagePacket *)Param))
	{
		PrintDOSLog(_T("释放消息内存块失败！"));
	}
}
void CDOSObjectProxyConnectionNoBuff::ReleaseSendBuffer(LPCVOID pBuffer, LPCVOID Param)
{
	if(Param)
	{
		if (!GetServer()->ReleaseMessagePacket((CDOSMessagePacket *)Param))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
	}
}

void CDOSObjectProxyConnectionNoBuff::OnRecvData(const BYTE * pData, UINT DataSize, LPCVOID Param)
{
	if (DataSize < CDOSSimpleMessage::GetMsgHeaderLength())
	{
		Disconnect();
		PrintDOSLog(_T("对象代理收到非法包，连接断开！"));
		return;
	}
	CDOSMessagePacket * pPacket = (CDOSMessagePacket *)Param;
	const CDOSSimpleMessage * pMsg = (const CDOSSimpleMessage *)pData;
	WORD MsgFlag = pMsg->GetMsgID();
	MSG_ID_TYPE MsgID = pMsg->GetMsgFlag();
	MSG_LEN_TYPE DataLen = pMsg->GetDataLength();
	WORD CRC = pMsg->GetCRC();
	pPacket->GetMessage().SetMsgID(MsgID);
	pPacket->GetMessage().SetMsgFlag(MsgFlag);
	pPacket->GetMessage().SetDataLength(DataLen);

	if (MsgFlag & DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
		OnClientSystemMsg(pPacket);
	else
		OnClientMsg(pPacket, CRC);
	m_KeepAliveCount = 0;
	m_RecvCount++;
	m_RecvFlow += DataSize;
}

void CDOSObjectProxyConnectionNoBuff::OnConnection(bool IsSucceed)
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
void CDOSObjectProxyConnectionNoBuff::OnDisconnection()
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
	m_UnacceptConnectionKeepTimer.SaveTime();
	m_TotalMsgSendCount = 0;
	m_TotalMsgRecvCount = 0;
	m_Status = STATUS_DISCONNECTED;
}

int CDOSObjectProxyConnectionNoBuff::Update(int ProcessPacketLimit)
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
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
		PacketCount++;
		if (PacketCount >= PacketLimit && (!m_WantClose))
			break;
	}

	PacketCount += CNetConnectionNoBuff::Update(PacketLimit);

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
			PrintDOSLog(_T("KeepAlive超时！"));
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

void CDOSObjectProxyConnectionNoBuff::QueryDisconnect(UINT Delay)
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

inline CDOSServer * CDOSObjectProxyConnectionNoBuff::GetServer()
{
	return (CDOSServer *)CNetConnectionNoBuff::GetServer();
}

void CDOSObjectProxyConnectionNoBuff::OnClientMsg(CDOSMessagePacket * pPacket, WORD CRC)
{
	if (!m_NeedDelayClose)
	{
		m_TotalMsgRecvCount++;

		WORD MsgFlag = pPacket->GetMessage().GetMsgFlag();
		MSG_LEN_TYPE DataLen = pPacket->GetMessage().GetDataLength();
		LPCVOID pData = pPacket->GetMessage().GetMsgData();
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
				PrintDOSLog(_T("Msg:0x%X,CRC=0x%X,MsgCount=%d"), pPacket->GetMessage().GetMsgID(), CRC, m_TotalMsgRecvCount);
				DumpBinData((const BYTE *)pData, DataLen, m_Config.MaxMsgDumpSize);
			}

			MsgFlag &= ~DOS_MESSAGE_FLAG_ENCRYPT;
			if (!DecyptMsg((LPVOID)pData, DataLen, CRC, m_TotalMsgRecvCount))
			{
				Disconnect();
				return;
			}
			pPacket->GetMessage().SetDataLength(DataLen);
			if (m_Config.DumpMsg)
				DumpBinData((const BYTE *)pData, DataLen, m_Config.MaxMsgDumpSize);
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

		pPacket->GetMessage().SetSenderID(m_ObjectID);
		OBJECT_ID * pObjectID = m_MessageMap.Find(pPacket->GetMessage().GetMsgID());
		if (pObjectID)
		{
			pPacket->SetTargetIDs(1, pObjectID);
			GetServer()->GetRouter()->RouterMessage(pPacket);
		}
		else
		{
			if (m_pService->HaveGlobalMsgMap(pPacket->GetMessage().GetMsgID()))
			{
				m_pService->SendGlobalMapMessage(pPacket);
			}
			else
			{
				PrintDOSDebugLog(_T("无法找到消息0x%X的接收者！"), pPacket->GetMessage().GetMsgID());
			}
		}
	}
}

void CDOSObjectProxyConnectionNoBuff::OnClientSystemMsg(CDOSMessagePacket * pPacket)
{
	switch (pPacket->GetMessage().GetMsgID())
	{
	case DSM_PROXY_KEEP_ALIVE_PING:
		if (!m_UseServerInitiativeKeepAlive)
		{
			pPacket->IncRefCount();
			CDOSSimpleMessage * pSimpleMsg = pPacket->GetMessage().MakeSimpleMessage();
			if (pSimpleMsg->GetDataLength() >= sizeof(PING_DATA))
			{
				PING_DATA * pPingData = (PING_DATA *)pSimpleMsg->GetMsgData();
				m_RecentPingDelay = pPingData->RecentDelay;
			}
			pSimpleMsg->SetMsgID(DSM_PROXY_KEEP_ALIVE_PONG);
			Send(pSimpleMsg, pSimpleMsg->GetMsgLength(), pPacket);
		}
		break;
	case DSM_PROXY_KEEP_ALIVE_PONG:
		if (m_UseServerInitiativeKeepAlive)
		{
			if (pPacket->GetMessage().GetDataLength() >= sizeof(PING_DATA))
			{
				PING_DATA * pPingData = (PING_DATA *)pPacket->GetMessage().GetMsgData();
				m_RecentPingDelay = CEasyTimer::GetTimeToTime(pPingData->Time, CEasyTimer::GetTime());
			}
		}
		break;
	default:
		PrintDOSDebugLog( _T("对象代理连接收到未知系统消息0x%X"), pPacket->GetMessage().GetMsgID());
	}
}

inline bool CDOSObjectProxyConnectionNoBuff::OnMessage(CDOSMessagePacket * pPacket)
{
	m_TotalMsgSendCount++;

	//压缩
	CDOSMessagePacket * pNewPacket = CompressMsg(pPacket);
	if (pNewPacket == NULL)
	{
		PrintDOSLog(_T("消息0x%X压缩失败"), pPacket->GetMessage().GetMsgID());
		return false;
	}

	CDOSSimpleMessage * pSimpleMsg = pNewPacket->GetMessage().MakeSimpleMessage();
	return Send(pSimpleMsg, pSimpleMsg->GetMsgLength(), pNewPacket);
}

bool CDOSObjectProxyConnectionNoBuff::OnSystemMessage(const CDOSMessagePacket * pPacket)
{
	switch (pPacket->GetMessage().GetMsgID())
	{
	case DSM_PROXY_REGISTER_MSG_MAP:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			int Count = (pPacket->GetMessage().GetDataLength()) / sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pPacket->GetMessage().GetMsgData());
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
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pPacket->GetMessage().GetMsgData());
			for (int i = 0; i < Count; i++)
			{
				DoUnregisterMsgMap(pMsgIDs[i], pPacket->GetMessage().GetSenderID());
			}
		}
		return true;
	case DSM_PROXY_DISCONNECT:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(UINT))
		{
			UINT Delay = *((UINT *)(pPacket->GetMessage().GetMsgData()));
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
			BYTE * pData = (BYTE *)pPacket->GetMessage().GetMsgData();
			MSG_LEN_TYPE DataLen = pPacket->GetMessage().GetDataLength();
			MASK_BROADCAST_INFO * pInfo = (MASK_BROADCAST_INFO *)pData;
			DataLen -= sizeof(MASK_BROADCAST_INFO);

			OnBroadCastMessage(pInfo->MsgID, pInfo->MsgFlag, pData, DataLen);
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

bool CDOSObjectProxyConnectionNoBuff::OnBroadCastMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, const void * pData, MSG_LEN_TYPE DataLen)
{
	CDOSMessagePacket * pNewPacket = GetServer()->NewMessagePacket(CDOSMessagePacket::CaculatePacketLength(DataLen, 0));
	pNewPacket->GetMessage().SetMsgID(MsgID);
	pNewPacket->GetMessage().SetMsgFlag(MsgFlag);
	pNewPacket->GetMessage().SetData(pData, DataLen);

	bool Ret = OnMessage(pNewPacket);
	if (!GetServer()->ReleaseMessagePacket(pNewPacket))
	{
		PrintDOSLog(_T("释放消息内存块失败！"));
	}
	return Ret;
}

bool CDOSObjectProxyConnectionNoBuff::SendDisconnectNotify()
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

void CDOSObjectProxyConnectionNoBuff::SendKeepAliveMsg()
{
	static BYTE Buffer[sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD) + sizeof(PING_DATA)];
	CDOSSimpleMessage * pKeepAliveMsg = (CDOSSimpleMessage *)Buffer;
	pKeepAliveMsg->Init();
	pKeepAliveMsg->SetMsgID(DSM_PROXY_KEEP_ALIVE_PING);
	pKeepAliveMsg->SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	pKeepAliveMsg->SetDataLength(sizeof(PING_DATA));
	PING_DATA * pPingData = (PING_DATA *)pKeepAliveMsg->GetMsgData();
	pPingData->Time = CEasyTimer::GetTime();
	pPingData->RecentDelay = m_RecentPingDelay;

	Send(pKeepAliveMsg, pKeepAliveMsg->GetMsgLength(), NULL);
}

void CDOSObjectProxyConnectionNoBuff::SendProtocolOption()
{
	static BYTE Buffer[sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD) + sizeof(PROTOCOL_OPTION)];
	CDOSSimpleMessage * pMsg = (CDOSSimpleMessage *)Buffer;
	pMsg->Init();
	pMsg->SetMsgID(DSM_PROTOCOL_OPTION);
	pMsg->SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	pMsg->SetDataLength(sizeof(PROTOCOL_OPTION));
	PROTOCOL_OPTION * pData = (PROTOCOL_OPTION *)pMsg->GetMsgData();
	pData->Flag = 0;
	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE)
		pData->Flag |= PROTOCOL_OPTION_FLAG_UP_MSG_USE_ENCRYPT;

	Send(pMsg, pMsg->GetMsgLength(), NULL);

	PrintDOSDebugLog(_T("已发送协议配置0x%X"), pData->Flag);
}

bool CDOSObjectProxyConnectionNoBuff::DoRegisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
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
bool CDOSObjectProxyConnectionNoBuff::DoUnregisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
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




void CDOSObjectProxyConnectionNoBuff::ClearMsgMapByRouterID(UINT RouterID)
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



CDOSMessagePacket * CDOSObjectProxyConnectionNoBuff::CompressMsg(CDOSMessagePacket * pPacket)
{
	BYTE * pData = (BYTE *)pPacket->GetMessage().GetMsgData();
	UINT DataLen = pPacket->GetMessage().GetDataLength();
	if ((m_Config.MsgCompressType != MSG_COMPRESS_NONE) &&
		((pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_NO_COMPRESS) == 0)&&
		m_Config.MinMsgCompressSize&&
		m_pLZOCompressWorkBuffer&&
		DataLen >= m_Config.MinMsgCompressSize)
	{

		switch(m_Config.MsgCompressType)
		{
		case MSG_COMPRESS_LZO:
			{
				lzo_uint OutLen = DataLen + 32;
				CDOSMessagePacket * pNewPacket = GetServer()->NewMessagePacket(CDOSMessagePacket::CaculatePacketLength(OutLen, 0));
				pNewPacket->GetMessage().GetMsgHeader() = pPacket->GetMessage().GetMsgHeader();

				int Result = lzo1x_1_compress(pData, DataLen,
					(BYTE *)pNewPacket->GetMessage().GetMsgData(), &OutLen,
					m_pLZOCompressWorkBuffer);

				if(Result==LZO_E_OK)
				{
					pNewPacket->GetMessage().SetMsgFlag(pNewPacket->GetMessage().GetMsgFlag() | DOS_MESSAGE_FLAG_COMPRESSED);
					pNewPacket->GetMessage().SetDataLength(OutLen);
					return pNewPacket;
				}
				else
				{
					if (!GetServer()->ReleaseMessagePacket(pNewPacket))
					{
						PrintDOSLog(_T("释放消息内存块失败！"));
					}
					PrintDOSLog(_T("lzo压缩消息失败(%d)，将直接发送"),
						Result);
				}
			}
			break;
		case MSG_COMPRESS_ZIP_FAST:
		case MSG_COMPRESS_ZIP_NORMAL:
		case MSG_COMPRESS_ZIP_SLOW:
			break;
		}
	}
	if (pPacket->GetRefCount() > 1)
	{
		//需要复制消息包
		CDOSMessagePacket * pNewPacket = GetServer()->NewMessagePacket(pPacket->GetPacketLength());
		memcpy((LPVOID)pNewPacket->GetPacketData(), pPacket->GetPacketData(), pPacket->GetPacketLength());
		return pNewPacket;
	}
	else
	{
		pPacket->IncRefCount();
		return pPacket;
	}
}

const void * CDOSObjectProxyConnectionNoBuff::EncyptMsg(const void * pData, MSG_LEN_TYPE& DataLen, WORD& CRC, UINT MsgSequence)
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

bool CDOSObjectProxyConnectionNoBuff::DecyptMsg(void * pData, MSG_LEN_TYPE& DataLen, WORD CRC, UINT MsgSequence)
{
	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE && m_Config.SecretKey.GetLength())
	{
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
					(BYTE *)pData, DataLen, (BYTE *)pData, OutLen))
				{
					WORD TargetCRC = MakeCRC(pData, (UINT)OutLen, m_Config.SecretKey.GetBuffer(), (UINT)m_Config.SecretKey.GetLength(), MsgSequence);
					if (TargetCRC == CRC)
					{
						DataLen = (UINT)OutLen;
						return true;
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
					(BYTE *)pData, DataLen, (BYTE *)pData, OutLen))
				{
					WORD TargetCRC = MakeCRC(pData, (UINT)OutLen, m_Config.SecretKey.GetBuffer(), (UINT)m_Config.SecretKey.GetLength(), MsgSequence);
					if (TargetCRC == CRC)
					{
						DataLen = (UINT)OutLen;
						return true;
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
					(BYTE *)pData, DataLen, (BYTE *)pData, OutLen))
				{
					WORD TargetCRC = MakeCRC(pData, (UINT)OutLen, m_Config.SecretKey.GetBuffer(), (UINT)m_Config.SecretKey.GetLength(), MsgSequence);
					if (TargetCRC == CRC)
					{
						DataLen = (UINT)OutLen;
						return true;
					}
					else
					{
						PrintDOSLog(_T("TEA:CRC校验失败"));
						if (m_Config.DumpMsg)
							DumpBinData((const BYTE *)pData, OutLen, m_Config.MaxMsgDumpSize);
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

WORD CDOSObjectProxyConnectionNoBuff::MakeCRC(const void * pData, UINT DataLen, const void * pKey, UINT KeyLen, UINT MsgSequence)
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
