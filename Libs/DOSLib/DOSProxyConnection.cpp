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

	if(m_AssembleBuffer.GetBufferSize()<m_pService->GetConfig().MaxMsgSize*2)
	{
		if(!m_AssembleBuffer.Create(m_pService->GetConfig().MaxMsgSize*2))
		{
			PrintDOSLog(0xff0000,_T("创建%u大小的拼包缓冲失败！"),
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
				PrintDOSLog(0xff0000,_T("创建%u大小的压缩缓冲失败！"),
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
			PrintDOSLog(0xff0000,_T("CDOSProxyConnection::OnDisconnection:释放消息内存块失败！"));
		}
	}

	if(m_MsgQueue.GetBufferSize()<m_pService->GetConfig().ConnectionMsgQueueSize)
	{
		if (!m_MsgQueue.Create(m_pService->GetConfig().ConnectionMsgQueueSize))
		{
			PrintDOSLog(0xff0000,_T("创建%u大小的消息队列失败！"),
				m_pService->GetConfig().ConnectionMsgQueueSize);
			return FALSE;
		}
	}

	if(m_MessageMap.GetBufferSize()<m_pService->GetConfig().MsgMapSize)
	{
		if (!m_MessageMap.Create(m_pService->GetConfig().MsgMapSize))
		{
			PrintDOSLog(0xff0000,_T("创建%u大小的消息映射表失败！"),
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
	//		PrintDOSLog(0xff0000,_T("CDOSProxyConnection::Destory:释放消息内存块失败！"));
	//	}
	//}
	//m_MessageMap.Clear();
	FUNCTION_END;
}

void CDOSProxyConnection::OnConnection(BOOL IsSucceed)
{
	FUNCTION_BEGIN;
	PrintDOSDebugLog(0xff0000,_T("收到代理对象的连接！IP=%s"),GetRemoteAddress().GetIPString());
	SetSendDelay(m_pService->GetConfig().SendDelay);
	SetSendQueryLimit(m_pService->GetConfig().SendQueryLimit);
	PrintDOSDebugLog(0xff0000,_T("发送延时设置为%u,并发发送限制为%u"),
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
	PrintDOSDebugLog(0xff0000, _T("对象代理(%d)的连接断开！IP=%s"), GetID(), GetRemoteAddress().GetIPString());
	SendDisconnectNotify();

	CDOSMessagePacket * pPacket;
	while(m_MsgQueue.PopFront(pPacket))
	{
		if(!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(0xff0000,_T("CDOSProxyConnection::OnDisconnection:释放消息内存块失败！"));
		}
	}
	m_MessageMap.Clear();
	m_UnacceptConnectionKeepTimer.SaveTime();
	FUNCTION_END;
}

void CDOSProxyConnection::OnRecvData(const BYTE * pData, UINT DataSize)
{
	FUNCTION_BEGIN;
	MSG_LEN_TYPE PacketSize=0;
	UINT PeekPos=0;
	if (!m_AssembleBuffer.PushBack(pData, DataSize))
	{
		Disconnect();
		PrintDOSLog(0xff0000,_T("对象代理(%d)拼包缓冲溢出，连接断开！"),GetID());
	}
	else
	{
		if(m_AssembleBuffer.Peek(PeekPos,&PacketSize,sizeof(MSG_LEN_TYPE)))
		{
			while(m_AssembleBuffer.GetUsedSize()>=PacketSize&&PacketSize)
			{
				if(PacketSize<CDOSSimpleMessage::GetMsgHeaderLength())
				{
					Disconnect();
					PrintDOSLog(0xff0000,_T("对象代理(%d)收到非法包，连接断开！"),GetID());
				}
				OnMsg((CDOSSimpleMessage *)m_AssembleBuffer.GetBuffer());
				m_AssembleBuffer.PopFront(NULL,PacketSize);
				PeekPos=0;
				PacketSize=0;
				if(!m_AssembleBuffer.Peek(PeekPos,&PacketSize,sizeof(MSG_LEN_TYPE)))
					break;
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
			PrintDOSLog(0xff0000,_T("CDOSProxyConnection::PushMessage:释放消息内存块失败！"));
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
		SendOutSideMsg(pPacket);
		if(!GetServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(0xff0000,_T("CDOSProxyConnection::Update:释放消息内存块失败！"));
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
			CDOSSimpleMessage KeepAliveMsg;
			KeepAliveMsg.Init();
			KeepAliveMsg.SetMsgID(DSM_PROXY_KEEP_ALIVE);
			KeepAliveMsg.SetDataLength(0);
			KeepAliveMsg.SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
			Send(&KeepAliveMsg,KeepAliveMsg.GetMsgLength());
		}
		if(m_KeepAliveCount>=m_MaxKeepAliveCount)
		{
			PrintDOSLog(0xff0000,_T("CDOSProxyConnection::Update:KeepAlive超时！"));
			m_KeepAliveCount=0;
			Disconnect();
		}
	}

	if(m_MessageMap.GetObjectCount()==0)
	{
		if(m_UnacceptConnectionKeepTimer.IsTimeOut(m_pService->GetConfig().UnacceptConnectionKeepTime))
		{
			PrintDOSLog(0xff0000,_T("CDOSProxyConnection::Update:未确认连接存在时间过长！"));
			Disconnect();
		}
	}

	if(m_NeedDelayClose)
	{
		if(m_DelayCloseTimer.IsTimeOut())
		{
			PrintDOSLog(0xff0000,_T("CDOSProxyConnection::Update:连接延时关闭！"));
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
		PrintDOSLog(0xff0000,_T("无法找到消息0X%x的接收者！"),MsgID);
		return FALSE;
	}
	FUNCTION_END;
	return FALSE;
}



inline BOOL CDOSProxyConnection::SendOutSideMsg(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	switch(pPacket->GetMessage().GetMsgID())
	{
	case DSM_PROXY_REGISTER_MSG_MAP:
		if(pPacket->GetMessage().GetDataLength()>=sizeof(MSG_ID_TYPE))
		{
			int Count=(pPacket->GetMessage().GetDataLength())/sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs=(MSG_ID_TYPE *)(pPacket->GetMessage().GetDataBuffer());
			for(int i=0;i<Count;i++)
			{
				RegisterMsgMap(pMsgIDs[i],pPacket->GetMessage().GetSenderID());
			}
		}
		return TRUE;
	case DSM_PROXY_UNREGISTER_MSG_MAP:
		if(pPacket->GetMessage().GetDataLength()>=sizeof(MSG_ID_TYPE))
		{
			int Count=(pPacket->GetMessage().GetDataLength())/sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs=(MSG_ID_TYPE *)(pPacket->GetMessage().GetDataBuffer());
			for(int i=0;i<Count;i++)
			{
				UnregisterMsgMap(pMsgIDs[i],pPacket->GetMessage().GetSenderID());
			}
		}
		return TRUE;
	case DSM_PROXY_DISCONNECT:
		if(pPacket->GetMessage().GetDataLength()>=sizeof(UINT))
		{
			UINT Delay=*((UINT *)(pPacket->GetMessage().GetDataBuffer()));
			m_NeedDelayClose=true;
			m_DelayCloseTimer.SetTimeOut(Delay);
			PrintDOSDebugLog(0xff0000,_T("0x%llX请求在%uMS后断开连接！"),pPacket->GetMessage().GetSenderID().ID,Delay);
		}
		return TRUE;
	case DSM_PROXY_GET_IP:
		{
			PROXY_CLIENT_IP_INFO IPInfo;
			IPInfo.IP=GetRemoteAddress().GetIP();
			IPInfo.Port=GetRemoteAddress().GetPort();

			GetServer()->GetRouter()->RouterMessage(m_ObjectID,pPacket->GetMessage().GetSenderID(),
				DSM_PROXY_IP_REPORT,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,&IPInfo,sizeof(IPInfo));
		}
		return TRUE;
	case DSM_ROUTE_LINK_LOST:
		ClearMsgMapByRouterID(pPacket->GetMessage().GetSenderID().RouterID);
		return TRUE;
	case DSM_OBJECT_ALIVE_TEST:
		{
			BYTE IsEcho=1;
			GetServer()->GetRouter()->RouterMessage(m_ObjectID,pPacket->GetMessage().GetSenderID(),
				DSM_OBJECT_ALIVE_TEST,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,&IsEcho,sizeof(IsEcho));
		}
		return TRUE;
	default:
		{
			bool NeedSaveHead=pPacket->GetRefCount()>1;

			CDOSMessage::DOS_MESSAGE_HEAD SaveHeader;

			if(NeedSaveHead)
			{
				SaveHeader=pPacket->GetMessage().GetMsgHeader();
			}
			CDOSSimpleMessage * pSimpleMessage=pPacket->GetMessage().MakeSimpleMessage();

			pSimpleMessage=CompressMsg(pSimpleMessage);
			if(pSimpleMessage==NULL)
			{
				if(NeedSaveHead)
				{
					pPacket->GetMessage().GetMsgHeader()=SaveHeader;
				}
				Disconnect();
				return FALSE;
			}

			BOOL Ret=Send(pSimpleMessage,pSimpleMessage->GetMsgLength());

			if(NeedSaveHead)
			{
				pPacket->GetMessage().GetMsgHeader()=SaveHeader;
			}

			return Ret;
		}
	}
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
			PrintDOSLog(0xff0000,_T("分配消息内存块失败！"));
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
			PrintDOSDebugLog(0xff0000,_T("向[0x%llX]发送代理对象断线通知"),pTargetObjectIDs[i]);
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
	//	PrintDOSDebugLog(0xff0000,_T("0x%llX注册的代理[0x%X]消息映射[0x%X]被[0x%llX]取代！"),*pObjectID,GetID(),MsgID,ObjectID);
	//}
	//else
	//{
	//	PrintDOSDebugLog(0xff0000,_T("0x%llX注册了代理[0x%X]消息映射[0x%X]！"),ObjectID.ID,GetID(),MsgID);
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
			//PrintDOSDebugLog(0xff0000,_T("0x%llX注销了代理[0x%X]消息映射[0x%X]！"),ObjectID.ID,GetID(),MsgID);
			m_UnacceptConnectionKeepTimer.SaveTime();
			return m_MessageMap.Delete(MsgID);
		}
		else
		{
			//PrintDOSDebugLog(0xff0000,_T("0x%llX注销代理[0x%X]消息映射[0x%X],现注册对象是[0x%llX],无法注销！"),ObjectID.ID,GetID(),MsgID,*pObjectID);
			return FALSE;
		}
	}
	else
	{
		PrintDOSDebugLog(0xff0000,_T("0x%llX注销代理[0x%X]消息映射[0x%X],未找到映射记录！"),ObjectID.ID,GetID(),MsgID);
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
		PrintDOSLog(0xff0000,_T("代理[0x%X]已经没有任何消息映射，连接断开！"),GetID());
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
						PrintDOSLog(0xff0000,_T("创建%u大小的压缩缓冲失败，关闭连接！"),
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
					PrintDOSLog(0xff0000,_T("lzo压缩消息失败(%d)，将直接发送"),
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
			//			PrintDOSLog(0xff0000,_T("创建%u大小的压缩缓冲失败，关闭连接！"),
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
			//		PrintDOSLog(0xff0000,_T("zip压缩消息失败(%d)，将直接发送"),
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
