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

IMPLEMENT_CLASS_INFO_STATIC(CDOSProxyConnection, CNetConnection);


CEasyBuffer	CDOSProxyConnection::m_CompressBuffer;

char g_LZOCompressWorkMemory[LZO1X_1_MEM_COMPRESS];


CDOSProxyConnection::CDOSProxyConnection(void):CNetConnection()
{
	FUNCTION_BEGIN;
	m_pService=NULL;
	m_KeepAliveCount=0;
	m_MaxKeepAliveCount=10;
	m_KeepAliveTime=30000;
	m_NeedDelayClose=false;
	m_UseServerInitiativeKeepAlive=false;
	m_UseWebSocketProtocol = false;
	m_WebSocketStatus = WEB_SOCKET_STATUS_NONE;
	FUNCTION_END;
}

CDOSProxyConnection::~CDOSProxyConnection(void)
{
}

BOOL CDOSProxyConnection::Init(CDOSObjectProxyService * pService,UINT ID)
{
	FUNCTION_BEGIN;
	SetID(ID);
	m_pService=pService;
	SetServer(pService->GetServer());

	m_ObjectID.ObjectIndex=GetID();
	m_ObjectID.GroupIndex = MAKE_PROXY_GROUP_INDEX(m_pService->GetConfig().ProxyType, pService->GetID());
	m_ObjectID.ObjectTypeID=DOT_PROXY_OBJECT;
	m_ObjectID.RouterID=GetServer()->GetConfig().RouterID;

	m_MaxKeepAliveCount = m_pService->GetConfig().KeepAliveCount;
	m_KeepAliveTime=m_pService->GetConfig().KeepAliveTime;
	m_UseServerInitiativeKeepAlive=m_pService->GetConfig().UseServerInitiativeKeepAlive;

	m_UseWebSocketProtocol = m_pService->GetConfig().UseWebSocketProtocol;

	if(m_AssembleBuffer.GetBufferSize()<m_pService->GetConfig().MaxMsgSize*2)
	{
		if(!m_AssembleBuffer.Create(m_pService->GetConfig().MaxMsgSize*2))
		{
			PrintDOSLog(_T("DOSLib"),_T("创建%u大小的拼包缓冲失败！"),
				m_pService->GetConfig().MaxMsgSize*2);
			return FALSE;
		}
	}

	if (m_pService->GetConfig().MsgCompressType == MSG_COMPRESS_LZO)
	{
		if(m_CompressBuffer.GetBufferSize()<m_pService->GetConfig().MaxMsgSize)
		{
			if(!m_CompressBuffer.Create(m_pService->GetConfig().MaxMsgSize))
			{
				PrintDOSLog(_T("DOSLib"),_T("创建%u大小的压缩缓冲失败！"),
					m_pService->GetConfig().MaxMsgSize);
				return FALSE;
			}
		}
	}

	CDOSMessagePacket * pPacket;
	while(m_MsgQueue.PopFront(pPacket))
	{
		if(!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"),_T("CDOSProxyConnection::OnDisconnection:释放消息内存块失败！"));
		}
	}

	if(m_MsgQueue.GetBufferSize()<m_pService->GetConfig().ConnectionMsgQueueSize)
	{
		if (!m_MsgQueue.Create(m_pService->GetConfig().ConnectionMsgQueueSize))
		{
			PrintDOSLog(_T("DOSLib"),_T("创建%u大小的消息队列失败！"),
				m_pService->GetConfig().ConnectionMsgQueueSize);
			return FALSE;
		}
	}

	if(m_MessageMap.GetBufferSize()<m_pService->GetConfig().MsgMapSize)
	{
		if (!m_MessageMap.Create(m_pService->GetConfig().MsgMapSize))
		{
			PrintDOSLog(_T("DOSLib"),_T("创建%u大小的消息映射表失败！"),
				m_pService->GetConfig().MsgMapSize);
			return FALSE;
		}
	}
	else
	{
		m_MessageMap.Clear();
	}

	m_KeepAliveTimer.SaveTime();
	m_KeepAliveCount=0;
	m_MaxKeepAliveCount=m_pService->GetConfig().KeepAliveCount;
	m_UnacceptConnectionKeepTimer.SaveTime();
	m_NeedDelayClose=false;
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

void CDOSProxyConnection::Destory()
{
	FUNCTION_BEGIN;
	CNetConnection::Destory();

	//CDOSMessagePacket * pPacket;
	//while(m_MsgQueue.PopFront(pPacket))
	//{
	//	if(!GetServer()->ReleaseMessagePacket(pPacket))
	//	{
	//		PrintDOSLog(_T("DOSLib"),_T("CDOSProxyConnection::Destory:释放消息内存块失败！"));
	//	}
	//}
	//m_MessageMap.Clear();
	FUNCTION_END;
}

void CDOSProxyConnection::OnConnection(BOOL IsSucceed)
{
	FUNCTION_BEGIN;
	PrintDOSLog(_T("DOSLib"),_T("收到代理对象的连接！IP=%s"),GetRemoteAddress().GetIPString());
	SetSendDelay(m_pService->GetConfig().SendDelay);
	SetSendQueryLimit(m_pService->GetConfig().SendQueryLimit);
	PrintDOSLog(_T("DOSLib"),_T("发送延时设置为%u,并发发送限制为%u"),
		m_pService->GetConfig().SendDelay,
		m_pService->GetConfig().SendQueryLimit);
	m_NeedDelayClose=false;
	m_KeepAliveTimer.SaveTime();
	m_KeepAliveCount=0;
	FUNCTION_END;
}
void CDOSProxyConnection::OnDisconnection()
{
	FUNCTION_BEGIN;
	PrintDOSLog(_T("DOSLib"), _T("对象代理(%d)的连接断开！IP=%s"), GetID(), GetRemoteAddress().GetIPString());
	SendDisconnectNotify();

	CDOSMessagePacket * pPacket;
	while(m_MsgQueue.PopFront(pPacket))
	{
		if(!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"),_T("CDOSProxyConnection::OnDisconnection:释放消息内存块失败！"));
		}
	}
	m_MessageMap.Clear();
	m_UnacceptConnectionKeepTimer.SaveTime();
	FUNCTION_END;
}

void CDOSProxyConnection::OnRecvData(const BYTE * pData, UINT DataSize)
{
	FUNCTION_BEGIN;
	if (m_UseWebSocketProtocol)
	{
		switch (m_WebSocketStatus)
		{
		case WEB_SOCKET_STATUS_NONE:
			ProcessHTTPMsg(pData, DataSize);
			break;
		case WEB_SOCKET_STATUS_ACCEPTED:
			ProcessWebSocketData(pData, DataSize);
			break;
		}
	}
	else
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
					OnMsg((CDOSSimpleMessage *)m_AssembleBuffer.GetBuffer());
					m_AssembleBuffer.PopFront(NULL, PacketSize);
					PeekPos = 0;
					PacketSize = 0;
					if (!m_AssembleBuffer.Peek(PeekPos, &PacketSize, sizeof(MSG_LEN_TYPE)))
						break;
				}
			}
		}
	}
	FUNCTION_END;
}

void CDOSProxyConnection::OnMsg(CDOSSimpleMessage * pMessage)
{
	FUNCTION_BEGIN;
	switch(pMessage->GetMsgID())
	{
	case DSM_PROXY_KEEP_ALIVE:		
		{
			if(!m_UseServerInitiativeKeepAlive)
			{
				Send(pMessage,pMessage->GetMsgLength());
			}
			m_KeepAliveCount=0;
		}

		break;
	default:
		if (!m_NeedDelayClose)
			SendInSideMsg(pMessage->GetMsgID(),pMessage->GetDataBuffer(),pMessage->GetDataLength());
	}

	FUNCTION_END;
}

BOOL CDOSProxyConnection::PushMessage(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;


	if(!IsConnected())
		return FALSE;

	((CDOSServer *)GetServer())->AddRefMessagePacket(pPacket);
	pPacket->SetAllocTime(3);
	if(m_MsgQueue.PushBack(pPacket))
	{
		pPacket->SetAllocTime(31);
		return TRUE;
	}
	else
	{
		if(!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"),_T("CDOSProxyConnection::PushMessage:释放消息内存块失败！"));
		}
	}
	FUNCTION_END;
	return FALSE;
}

int CDOSProxyConnection::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	int PacketLimit=ProcessPacketLimit/2;
	int PacketCount=0;

	CDOSMessagePacket * pPacket;
	while(m_MsgQueue.PopFront(pPacket))
	{
		pPacket->SetAllocTime(32);
		if (pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
			OnSystemMessage(pPacket);
		else
			SendOutSideMsg(pPacket);
		if(!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"),_T("CDOSProxyConnection::Update:释放消息内存块失败！"));
		}
		PacketCount++;
		if(PacketCount>=PacketLimit&&(!m_WantClose))
			break;
	}

	PacketCount+=CNetConnection::Update(PacketLimit);

	if(m_KeepAliveTimer.IsTimeOut(m_KeepAliveTime))
	{
		m_KeepAliveTimer.SaveTime();
		m_KeepAliveCount++;
		if(m_UseServerInitiativeKeepAlive)
		{
			if (m_UseWebSocketProtocol)
			{
				SendWebSocketPingMsg();
			}
			else
			{
				CDOSSimpleMessage KeepAliveMsg;
				KeepAliveMsg.Init();
				KeepAliveMsg.SetMsgID(DSM_PROXY_KEEP_ALIVE);
				KeepAliveMsg.SetDataLength(0);
				KeepAliveMsg.SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
				Send(&KeepAliveMsg, KeepAliveMsg.GetMsgLength());
			}
		}
		if(m_KeepAliveCount>=m_MaxKeepAliveCount)
		{
			PrintDOSLog(_T("DOSLib"),_T("CDOSProxyConnection::Update:KeepAlive超时！"));
			m_KeepAliveCount=0;
			Disconnect();
		}
	}

	if(m_MessageMap.GetObjectCount()==0)
	{
		if(m_UnacceptConnectionKeepTimer.IsTimeOut(m_pService->GetConfig().UnacceptConnectionKeepTime))
		{
			PrintDOSLog(_T("DOSLib"),_T("CDOSProxyConnection::Update:未确认连接存在时间过长！"));
			Disconnect();
		}
	}

	if(m_NeedDelayClose)
	{
		if(m_DelayCloseTimer.IsTimeOut())
		{
			PrintDOSLog(_T("DOSLib"),_T("CDOSProxyConnection::Update:连接延时关闭！"));
			Disconnect();
		}
	}

	return PacketCount;
	FUNCTION_END;
	return 0;
}

inline CDOSServer * CDOSProxyConnection::GetServer()
{
	FUNCTION_BEGIN;
	return (CDOSServer *)CNetConnection::GetServer();
	FUNCTION_END;
	return NULL;
}

BOOL CDOSProxyConnection::OnSystemMessage(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	switch (pPacket->GetMessage().GetMsgID())
	{
	case DSM_PROXY_REGISTER_MSG_MAP:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			int Count = (pPacket->GetMessage().GetDataLength()) / sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pPacket->GetMessage().GetDataBuffer());
			for (int i = 0; i < Count; i++)
			{
				RegisterMsgMap(pMsgIDs[i], pPacket->GetMessage().GetSenderID());
			}
		}
		return TRUE;
	case DSM_PROXY_UNREGISTER_MSG_MAP:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			int Count = (pPacket->GetMessage().GetDataLength()) / sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pPacket->GetMessage().GetDataBuffer());
			for (int i = 0; i < Count; i++)
			{
				UnregisterMsgMap(pMsgIDs[i], pPacket->GetMessage().GetSenderID());
			}
		}
		return TRUE;
	case DSM_PROXY_DISCONNECT:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(UINT))
		{
			UINT Delay = *((UINT *)(pPacket->GetMessage().GetDataBuffer()));
			m_NeedDelayClose = true;
			m_DelayCloseTimer.SetTimeOut(Delay);
			if (m_UseWebSocketProtocol)
			{
				SendWebSocketCloseMsg();
			}
			PrintDOSLog(_T("DOSLib"), _T("0x%llX请求在%uMS后断开连接！"), pPacket->GetMessage().GetSenderID().ID, Delay);
		}
		return TRUE;
	case DSM_PROXY_GET_IP:
		{
			PROXY_CLIENT_IP_INFO IPInfo;
			IPInfo.IP = GetRemoteAddress().GetIP();
			IPInfo.Port = GetRemoteAddress().GetPort();

			GetServer()->GetRouter()->RouterMessage(m_ObjectID, pPacket->GetMessage().GetSenderID(),
				DSM_PROXY_IP_REPORT, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &IPInfo, sizeof(IPInfo));
		}
		return TRUE;
	case DSM_ROUTE_LINK_LOST:
		ClearMsgMapByRouterID(pPacket->GetMessage().GetSenderID().RouterID);
		return TRUE;
	case DSM_OBJECT_ALIVE_TEST:
		{
			BYTE IsEcho = 1;
			GetServer()->GetRouter()->RouterMessage(m_ObjectID, pPacket->GetMessage().GetSenderID(),
				DSM_OBJECT_ALIVE_TEST, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &IsEcho, sizeof(IsEcho));
		}
		return TRUE;	
	default:
		PrintDOSLog(_T("DOSLib"), _T("ProxyConnection收到未知系统消息0x%llX"), pPacket->GetMessage().GetMsgID());
	}
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSProxyConnection::SendInSideMsg(MSG_ID_TYPE MsgID,LPVOID pData,UINT DataSize)
{
	FUNCTION_BEGIN;
	int PacketSize=CDOSMessagePacket::CaculatePacketLength(DataSize,1);


	OBJECT_ID TargetObjectID=GetMsgMapObjectID(MsgID);
	if(TargetObjectID.ID)
	{
		return GetServer()->GetRouter()->RouterMessage(m_ObjectID,TargetObjectID,MsgID,0,pData,DataSize);
	}
	else
	{
		PrintDOSLog(_T("DOSLib"),_T("无法找到消息0X%x的接收者！"),MsgID);
		return FALSE;
	}
	FUNCTION_END;
	return FALSE;
}



inline BOOL CDOSProxyConnection::SendOutSideMsg(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
		
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
		return FALSE;
	}

	BOOL Ret = FALSE;

	if (m_UseWebSocketProtocol)
	{
		Ret = SendMsgByWebSocket(pSimpleMessage);
	}
	else
	{
		Ret = Send(pSimpleMessage, pSimpleMessage->GetMsgLength());
	}

	if (NeedSaveHead)
	{
		pPacket->GetMessage().GetMsgHeader() = SaveHeader;
	}

	return Ret;
	
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSProxyConnection::SendDisconnectNotify()
{
	FUNCTION_BEGIN;
	int MsgMapCount=m_MessageMap.GetObjectCount();
	if(MsgMapCount)
	{
		int PacketSize=CDOSMessagePacket::CaculatePacketLength(0,MsgMapCount);

		CDOSMessagePacket * pNewPacket=GetServer()->NewMessagePacket(PacketSize);
		if(pNewPacket==NULL)
		{
			PrintDOSLog(_T("DOSLib"),_T("分配消息内存块失败！"));
			return FALSE;
		}

		pNewPacket->SetTargetIDs(0,NULL);
		pNewPacket->GetMessage().SetMsgID(DSM_PROXY_DISCONNECT);
		pNewPacket->GetMessage().SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
		pNewPacket->GetMessage().SetSenderID(m_ObjectID);
		OBJECT_ID * pTargetObjectIDs=pNewPacket->GetTargetIDs();

		void * Pos=m_MessageMap.GetFirstObjectPos();
		while(Pos)
		{
			MSG_ID_TYPE MsgID;
			OBJECT_ID * pTargetObjectID=m_MessageMap.GetNextObject(Pos,MsgID);
			pNewPacket->AddTargetID(*pTargetObjectID);
		}

		UINT RealTargetIDCount=DistinctObjectID(pTargetObjectIDs,MsgMapCount);
		for(UINT i=0;i<RealTargetIDCount;i++)
		{
			PrintDOSLog(_T("DOSLib"),_T("向[0x%llX]发送代理对象断线通知"),pTargetObjectIDs[i]);
		}
		pNewPacket->SetTargetIDs(RealTargetIDCount,NULL);
		pNewPacket->MakePacketLength();

		BOOL Ret=GetServer()->GetRouter()->RouterMessage(pNewPacket);
		GetServer()->ReleaseMessagePacket(pNewPacket);
		return Ret;
	}
	else
	{
		return TRUE;
	}
	FUNCTION_END;
	return FALSE;
}


OBJECT_ID CDOSProxyConnection::GetMsgMapObjectID(MSG_ID_TYPE CmdID)
{
	FUNCTION_BEGIN;
	OBJECT_ID * pObjectID=m_MessageMap.Find(CmdID);
	if(pObjectID)
	{
		return *pObjectID;
	}
	else
	{
		return m_pService->GetGlobalMsgMapObjectID(CmdID);
	}
	FUNCTION_END;
	return 0;
}


BOOL CDOSProxyConnection::RegisterMsgMap(MSG_ID_TYPE MsgID,OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	OBJECT_ID * pObjectID=m_MessageMap.Find(MsgID);
	//if(pObjectID)
	//{
	//	PrintDOSLog(_T("DOSLib"),_T("0x%llX注册的代理[0x%X]消息映射[0x%X]被[0x%llX]取代！"),*pObjectID,GetID(),MsgID,ObjectID);
	//}
	//else
	//{
	//	PrintDOSLog(_T("DOSLib"),_T("0x%llX注册了代理[0x%X]消息映射[0x%X]！"),ObjectID.ID,GetID(),MsgID);
	//}

	m_UnacceptConnectionKeepTimer.SaveTime();
	return m_MessageMap.Insert(MsgID,ObjectID);
	FUNCTION_END;
	return FALSE;
}
BOOL CDOSProxyConnection::UnregisterMsgMap(MSG_ID_TYPE MsgID,OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	OBJECT_ID * pObjectID=m_MessageMap.Find(MsgID);
	if(pObjectID)
	{
		if(*pObjectID==ObjectID)
		{
			//PrintDOSLog(_T("DOSLib"),_T("0x%llX注销了代理[0x%X]消息映射[0x%X]！"),ObjectID.ID,GetID(),MsgID);
			m_UnacceptConnectionKeepTimer.SaveTime();
			return m_MessageMap.Delete(MsgID);
		}
		else
		{
			//PrintDOSLog(_T("DOSLib"),_T("0x%llX注销代理[0x%X]消息映射[0x%X],现注册对象是[0x%llX],无法注销！"),ObjectID.ID,GetID(),MsgID,*pObjectID);
			return FALSE;
		}
	}
	else
	{
		PrintDOSLog(_T("DOSLib"),_T("0x%llX注销代理[0x%X]消息映射[0x%X],未找到映射记录！"),ObjectID.ID,GetID(),MsgID);
		return FALSE;
	}
	FUNCTION_END;
	return FALSE;
}




void CDOSProxyConnection::ClearMsgMapByRouterID(UINT RouterID)
{
	FUNCTION_BEGIN;
	void * Pos=m_MessageMap.GetFirstObjectPos();
	while(Pos)
	{
		MSG_ID_TYPE MsgID;
		OBJECT_ID * pObjectID=m_MessageMap.GetNextObject(Pos,MsgID);
		if(pObjectID->RouterID==RouterID)
		{
			m_MessageMap.Delete(MsgID);
		}
	}
	if(m_MessageMap.GetObjectCount()<=0)
	{
		PrintDOSLog(_T("DOSLib"),_T("代理[0x%X]已经没有任何消息映射，连接断开！"),GetID());
		Disconnect();
	}
	FUNCTION_END;
}

CDOSSimpleMessage * CDOSProxyConnection::CompressMsg(CDOSSimpleMessage * pMsg)
{
	FUNCTION_BEGIN;
	if (m_pService->GetConfig().MsgCompressType != MSG_COMPRESS_NONE&&
		m_pService->GetConfig().MinMsgCompressSize&&
		pMsg->GetDataLength() >= m_pService->GetConfig().MinMsgCompressSize)
	{

		switch(m_pService->GetConfig().MsgCompressType)
		{
		case MSG_COMPRESS_LZO:
			{
				if(m_CompressBuffer.GetBufferSize()<pMsg->GetMsgLength())
				{
					if(!m_CompressBuffer.Create(pMsg->GetMsgLength()))
					{
						PrintDOSLog(_T("DOSLib"),_T("创建%u大小的压缩缓冲失败，关闭连接！"),
							pMsg->GetMsgLength());

						return NULL;
					}
				}
				CDOSSimpleMessage * pNewMsg=(CDOSSimpleMessage *)m_CompressBuffer.GetBuffer();
				pNewMsg->GetMsgHeader()=pMsg->GetMsgHeader();
				pNewMsg->GetMsgHeader().MsgFlag|=DOS_MESSAGE_FLAG_COMPRESSED;

				lzo_uint OutLen=m_CompressBuffer.GetBufferSize()-sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD);
				int Result=lzo1x_1_compress((BYTE *)pMsg->GetDataBuffer(),pMsg->GetDataLength(),
					(BYTE *)pNewMsg->GetDataBuffer(),&OutLen,
					g_LZOCompressWorkMemory);

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

	FUNCTION_END;
	return NULL;
}

void CDOSProxyConnection::ProcessHTTPMsg(const BYTE * pData, UINT DataSize)
{
	UINT Ptr = 0;
	HTTP_REQUEST_PARSE_STATUS ParseStatus = HRPS_NONE;
	while (Ptr < DataSize)
	{
		switch (ParseStatus)
		{
		case HRPS_NONE:
			if (pData[Ptr] == '\r')
				ParseStatus = HRPS_RETURN1;
			break;
		case HRPS_RETURN1:
			if (pData[Ptr] == '\n')
				ParseStatus = HRPS_NEW_LINE1;
			else
				ParseStatus = HRPS_NONE;
			break;
		case HRPS_NEW_LINE1:
			if (pData[Ptr] == '\r')
				ParseStatus = HRPS_RETURN2;
			else
				ParseStatus = HRPS_NONE;
			break;
		case HRPS_RETURN2:
			if (pData[Ptr] == '\n')
			{
				//请求命令已经完整
				((BYTE *)pData)[Ptr] = 0;
				((BYTE *)pData)[Ptr - 1] = 0;
				OnHTTPRequest((const char *)pData);
				break;
			}
			else
			{
				ParseStatus = HRPS_NONE;
			}
			break;
		}
		Ptr++;
	}
}

void CDOSProxyConnection::OnHTTPRequest(const char * szRequest)
{
	PrintDOSLog(_T("DOSLib"), _T("CHTTPSession::OnHTTPRequest:%s"), szRequest);

	CEasyArray<char *> Lines;
	ParseStringLines((char *)szRequest, Lines);
	CEasyString GetField;
	CEasyString UpgradeField;
	CEasyString ConnectionField;
	CEasyString SecWebSocketKeyField;
	CEasyString SecWebSocketVersionField;
	CEasyString SecWebSocketExtensionsField;
	CEasyString HostField;
	CEasyString OriginField;
	for (UINT i = 0; i < Lines.GetCount(); i++)
	{
		if (strncmp(Lines[i], "GET ", 4) == 0)
		{
			GetField = Lines[i] + 4;
		}
		else if (strncmp(Lines[i], "Upgrade:", 8) == 0)
		{
			UpgradeField = Lines[i] + 8;
			UpgradeField.Trim();
		}
		else if (strncmp(Lines[i], "Connection:", 11) == 0)
		{
			ConnectionField = Lines[i] + 11;
			ConnectionField.Trim();
		}
		else if (strncmp(Lines[i], "Sec-WebSocket-Key:", 18) == 0)
		{
			SecWebSocketKeyField = Lines[i] + 18;
			SecWebSocketKeyField.Trim();
		}
		else if (strncmp(Lines[i], "Sec-WebSocket-Version:", 18) == 0)
		{
			SecWebSocketVersionField = Lines[i] + 22;
			SecWebSocketVersionField.Trim();
		}
		else if (strncmp(Lines[i], "Sec-WebSocket-Extensions:", 25) == 0)
		{
			SecWebSocketExtensionsField = Lines[i] + 25;
			SecWebSocketExtensionsField.Trim();
		}
		else if (strncmp(Lines[i], "Host:", 5) == 0)
		{
			HostField = Lines[i] + 5;
			HostField.Trim();
		}
		else if (strncmp(Lines[i], "Origin:", 7) == 0)
		{
			OriginField = Lines[i] + 7;
			OriginField.Trim();
		}
	}
	if (!GetField.IsEmpty() && UpgradeField.CompareNoCase("websocket") == 0 && ConnectionField.CompareNoCase("Upgrade") == 0 &&
		SecWebSocketVersionField == "13" && !SecWebSocketKeyField.IsEmpty())
	{
		SecWebSocketKeyField = SecWebSocketKeyField + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		CSHA1 sha1;
		BYTE HashBin[20];

		sha1.Update((BYTE *)SecWebSocketKeyField.GetBuffer(), SecWebSocketKeyField.GetLength());
		sha1.Final();
		sha1.GetHash(HashBin);
		CEasyString HashStr = CBase64::Encode(HashBin, 20);
		SendHTTPRespond(101, HashStr);
		m_WebSocketStatus = WEB_SOCKET_STATUS_ACCEPTED;
	}
	else
	{
		SendHTTPRespond(403, "Not Support");		
	}
}

void CDOSProxyConnection::SendHTTPRespond(int Code, LPCSTR szContent)
{
	char Buff[4096];
	switch (Code)
	{
	case 101:
		sprintf_s(Buff, 4096, "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n",
			szContent);
		break;	
	case 403:
		sprintf_s(Buff, 4096, "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s",
			strlen(szContent), szContent);
		break;
	default:
		sprintf_s(Buff, 4096, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s",
			strlen(szContent), szContent);
		break;
	}
	Send(Buff, strlen(Buff));
	if (Code != 101)
	{
		m_NeedDelayClose = true;
		m_DelayCloseTimer.SetTimeOut(1000);
	}
}

void CDOSProxyConnection::ParseStringLines(char * szStr, CEasyArray<char *>& StrLines)
{
	char * szLine = szStr;
	while (*szStr)
	{
		if (*szStr == '\r' || *szStr == '\n')
		{
			*szStr = 0;
			if (szStr - szLine > 1)
				StrLines.Add(szLine);
			szLine = szStr + 1;
		}
		szStr++;
	}
	if (szStr - szLine > 1)
		StrLines.Add(szLine);
}

void CDOSProxyConnection::ProcessWebSocketData(const BYTE * pData, UINT DataSize)
{
	if (m_AssembleBuffer.PushBack(pData, DataSize))
	{
		BYTE Header1, Header2;
		UINT Pos = 0;
		m_AssembleBuffer.Peek(Pos, &Header1, sizeof(Header1));
		m_AssembleBuffer.Peek(Pos, &Header2, sizeof(Header2));
		BYTE OPCode = Header1 & 0x0F;
		BYTE ReserveCode = (Header1 >> 4) & 0x07;
		bool IsFinFrame = (Header1 & 0x80) != 0;
		bool HaveMask = (Header2 & 0x80) != 0;
		UINT64 DataLen = Header2 & 0x7F;
		if (DataLen == 126)
		{
			m_AssembleBuffer.Peek(Pos, &DataLen, sizeof(UINT));
		}
		else if (DataLen == 127)
		{
			m_AssembleBuffer.Peek(Pos, &DataLen, sizeof(UINT64));
		}
		BYTE MaskKey[4];
		if (HaveMask)
		{
			m_AssembleBuffer.Peek(Pos, MaskKey, sizeof(BYTE) * 4);
		}
		if (m_AssembleBuffer.GetUsedSize() >= Pos + DataLen)
		{
			OnWebSocketFrame(OPCode, IsFinFrame, HaveMask, MaskKey, (BYTE *)m_AssembleBuffer.GetBuffer(), Pos + DataLen, Pos);
			m_AssembleBuffer.PopBack(NULL, Pos + DataLen);
		}
	}
	else
	{
		Disconnect();
		PrintDOSLog(_T("DOSLib"), _T("对象代理(%d)拼包缓冲溢出，连接断开！"), GetID());
	}
}

void CDOSProxyConnection::OnWebSocketFrame(BYTE OPCode, bool IsFinFrame, bool HaveMask, BYTE * MaskKey, BYTE * pFrameData, UINT FrameLen, UINT DataPos)
{
	switch (OPCode)
	{
	case WEB_SOCKET_OP_CODE_CONTINUOUS_DATA:
	case WEB_SOCKET_OP_CODE_TEXT_DATA:
	case WEB_SOCKET_OP_CODE_BINARY_DATA:
		{
			BYTE * pData = pFrameData + DataPos;
			UINT DataLen = FrameLen - DataPos;
			if (HaveMask)
			{
				for (UINT i = 0; i < DataLen; i++)
				{
					pData[i] = pData[i] ^ MaskKey[i % 4];
				}
			}
			if (DataLen >= sizeof(MSG_ID_TYPE))
			{
				MSG_ID_TYPE MsgID = *((MSG_ID_TYPE *)pData);
				SendInSideMsg(MsgID, pData + sizeof(MSG_ID_TYPE), DataLen - sizeof(MSG_ID_TYPE));
			}
		}
		break;
	case WEB_SOCKET_OP_CODE_CLOSE:
		{
			SendWebSocketCloseMsg();
			m_NeedDelayClose = true;
			m_DelayCloseTimer.SetTimeOut(1000);
		}
	case WEB_SOCKET_OP_CODE_KEEP_ALIVE_PING:
		{
			m_KeepAliveCount = 0;
			*pFrameData = ((*pFrameData) & 0x0F) | WEB_SOCKET_OP_CODE_KEEP_ALIVE_PONG;
			Send(pFrameData, FrameLen);
		}
		break;
	case WEB_SOCKET_OP_CODE_KEEP_ALIVE_PONG:
		{
			m_KeepAliveCount = 0;
		}
		break;
	default:
		PrintDOSLog(_T("DOSLib"), _T("对象代理(%d)未知的WebSocket的OPCode(%d)，连接断开！"), GetID(), OPCode);
		Disconnect();
	}
}

void CDOSProxyConnection::SendWebSocketCloseMsg()
{
	static BYTE CloseFrame[] = { WEB_SOCKET_OP_CODE_CLOSE | 0x80, 0 };
	if (m_WebSocketStatus == WEB_SOCKET_STATUS_ACCEPTED)
		Send(CloseFrame, sizeof(CloseFrame));
}
void CDOSProxyConnection::SendWebSocketPingMsg()
{
	static BYTE PingFrame[] = { WEB_SOCKET_OP_CODE_KEEP_ALIVE_PING | 0x80, 0 };
	if (m_WebSocketStatus == WEB_SOCKET_STATUS_ACCEPTED)
		Send(PingFrame, sizeof(PingFrame));
}

BOOL CDOSProxyConnection::SendMsgByWebSocket(CDOSSimpleMessage * pSimpleMessage)
{
	return SendWebSocketFrame(WEB_SOCKET_OP_CODE_BINARY_DATA, (BYTE *)pSimpleMessage->GetDataBuffer(), pSimpleMessage->GetDataLength());
}

BOOL CDOSProxyConnection::SendWebSocketFrame(WEB_SOCKET_OP_CODE OPCode, BYTE * pData, UINT DataLen)
{
	BYTE FrameHeader[10];
	UINT HeaderSize = 2;
	FrameHeader[0] = OPCode | 0x80;
	if (DataLen < 126)
	{
		FrameHeader[1] = DataLen;
	}
	else if (DataLen<0x10000)
	{
		FrameHeader[1] = 126;
		WORD Len = htons(DataLen);
		FrameHeader[2] = Len & 0xFF;
		FrameHeader[3] = (Len>>8) & 0xFF;
		HeaderSize += 2;
	}
	else
	{
		FrameHeader[1] = 127;
		UINT64 Len = __htonll(DataLen);
		FrameHeader[2] = Len & 0xFF;
		FrameHeader[3] = (Len >> 8) & 0xFF;
		FrameHeader[4] = (Len >> 16) & 0xFF;
		FrameHeader[5] = (Len >> 24) & 0xFF;
		FrameHeader[6] = (Len >> 32) & 0xFF;
		FrameHeader[7] = (Len >> 40) & 0xFF;
		FrameHeader[8] = (Len >> 48) & 0xFF;
		FrameHeader[9] = (Len >> 56) & 0xFF;
		HeaderSize += 8;
	}
	Send(FrameHeader, HeaderSize);
	if (pData&&DataLen)
		Send(pData, DataLen);
	return TRUE;
}