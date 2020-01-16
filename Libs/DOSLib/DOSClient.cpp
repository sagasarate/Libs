/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSClient.cpp                                            */
/*      创建日期:  2009年10月15日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CDOSClient, CNameObject);

CDOSClient::CDOSClient(void)
{
	m_KeepAliveCount=0;
	m_MaxKeepAliveCount=10;
	m_KeepAliveTime=30000;
	m_RecentPingDelay = 0;

	m_MsgCompressType=MSG_COMPRESS_LZO;
	m_AssembleBuffer.SetTag(_T("CDOSClient"));
	m_SendBuffer.SetTag(_T("CDOSClient"));
	m_MsgFnMap.SetTag(_T("CDOSClient"));
}

CDOSClient::~CDOSClient(void)
{
	Destory();
}


void CDOSClient::Destory()
{
	CNetConnection::Destory();
}

BOOL CDOSClient::Start(UINT SendQueueLen, UINT RecvQueueLen, const CIPAddress& Address, DWORD TimeOut)
{
	if (!m_SendBuffer.Create(SendQueueLen*NET_DATA_BLOCK_SIZE))
		return FALSE;
	if (!m_AssembleBuffer.Create(RecvQueueLen*NET_DATA_BLOCK_SIZE * 2))
		return FALSE;
	if (!CNetConnection::Create(RecvQueueLen, SendQueueLen))
		return FALSE;

	m_KeepAliveTimer.SaveTime();
	m_KeepAliveCount=0;
	m_RecentPingDelay = 0;
	return CNetConnection::Connect(Address,TimeOut);
}

void CDOSClient::Close()
{
	CNetConnection::Disconnect();
}



UINT CDOSClient::GetRouterID()
{
	return 0;
}
OBJECT_ID CDOSClient::GetObjectID()
{
	return 0;
}
int CDOSClient::GetGroupIndex()
{
	return -1;
}
BOOL CDOSClient::SendMessage(OBJECT_ID ReceiverID,MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize)
{
	m_SendBuffer.SetUsedSize(0);
	CDOSSimpleMessage * pSimpleMessage=(CDOSSimpleMessage *)m_SendBuffer.GetBuffer();
	m_SendBuffer.PushBack(NULL,CDOSSimpleMessage::GetMsgHeaderLength());

	pSimpleMessage->Init();
	pSimpleMessage->SetMsgID(MsgID);
	pSimpleMessage->SetDataLength(DataSize);
	pSimpleMessage->SetMsgFlag(MsgFlag);

	if(!m_SendBuffer.PushBack(pData,DataSize))
		return false;

	return Send(pSimpleMessage,pSimpleMessage->GetMsgLength());
}

BOOL CDOSClient::SendMessageMulti(OBJECT_ID * pReceiverIDList,UINT ReceiverCount,bool IsSorted,MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize)
{
	return FALSE;
}

CDOSMessagePacket * CDOSClient::NewMessagePacket(UINT DataSize,UINT ReceiverCount)
{
	MSG_LEN_TYPE PacketLen=CDOSMessagePacket::CaculatePacketLength(DataSize,ReceiverCount);
	MSG_LEN_TYPE PacketRealLen=CDOSMessagePacket::CaculateRealPacketLength(PacketLen);
	if(PacketRealLen>m_SendBuffer.GetBufferSize())
		return NULL;
	CDOSMessagePacket * pMsgPacket=(CDOSMessagePacket *)m_SendBuffer.GetBuffer();
	pMsgPacket->Init();
	pMsgPacket->SetPacketLength(PacketLen);
	pMsgPacket->IncRefCount();
	return pMsgPacket;
}
BOOL CDOSClient::ReleaseMessagePacket(CDOSMessagePacket * pPacket)
{
	pPacket->DecRefCount();
	return TRUE;
}
BOOL CDOSClient::SendMessagePacket(CDOSMessagePacket * pPacket)
{
	CDOSSimpleMessage * pSimpleMessage=pPacket->GetMessage().MakeSimpleMessage();
	return Send(pSimpleMessage,pSimpleMessage->GetMsgLength());
}

BOOL CDOSClient::RegisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)
{
	return FALSE;
}
BOOL CDOSClient::UnregisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)
{
	return FALSE;
}
BOOL CDOSClient::RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID, int MapType)
{
	return FALSE;
}
BOOL CDOSClient::UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID)
{
	return FALSE;
}

BOOL CDOSClient::SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType)
{
	return FALSE;
}

BOOL CDOSClient::AddConcernedObject(OBJECT_ID ObjectID,bool NeedTest)
{
	return FALSE;
}

BOOL CDOSClient::DeleteConcernedObject(OBJECT_ID ObjectID)
{
	return FALSE;
}

BOOL CDOSClient::FindObject(UINT ObjectType)
{
	return FALSE;
}
BOOL CDOSClient::ReportObject(OBJECT_ID TargetID, const void * pObjectInfoData, UINT DataSize)
{
	return FALSE;
}
BOOL CDOSClient::CloseProxyObject(OBJECT_ID ProxyObjectID,UINT Delay)
{
	return FALSE;
}
BOOL CDOSClient::RequestProxyObjectIP(OBJECT_ID ProxyObjectID)
{
	return FALSE;
}

BOOL CDOSClient::RegisterObject(DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo)
{
	return false;
}
void CDOSClient::Release()
{
	CNetConnection::Release();
}

BOOL CDOSClient::QueryShutDown(OBJECT_ID TargetID, BYTE Level, UINT Param)
{
	return FALSE;
}

void CDOSClient::ShutDown(UINT PluginID)
{

}
bool CDOSClient::RegisterCommandReceiver()
{
	return false;
}
bool CDOSClient::UnregisterCommandReceiver()
{
	return false;
}
BOOL CDOSClient::RegisterLogger(UINT LogChannel, LPCTSTR FileName)
{
	return FALSE;
}
BOOL CDOSClient::RegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader)
{
	return FALSE;
}
void CDOSClient::SetServerWorkStatus(BYTE WorkStatus)
{

}
void CDOSClient::OnRecvData(const BYTE * pData, UINT DataSize)
{
	MSG_LEN_TYPE PacketSize=0;
	UINT PeekPos=0;
	if (DataSize>m_AssembleBuffer.GetFreeSize())
	{
		Close();
		PrintDOSLog(_T("(%d)拼包缓冲溢出，连接断开！"),GetID());
		return;
	}
	m_AssembleBuffer.PushBack(pData, DataSize);
	m_AssembleBuffer.Peek(PeekPos,&PacketSize,sizeof(MSG_LEN_TYPE));
	while(m_AssembleBuffer.GetUsedSize()>=PacketSize&&PacketSize)
	{
		if(PacketSize<sizeof(CDOSSimpleMessage::GetMsgHeaderLength()))
		{
			Close();
			PrintDOSLog(_T("(%d)收到非法包，连接断开！"),GetID());
		}
		CDOSSimpleMessage * pMsg=(CDOSSimpleMessage *)m_AssembleBuffer.GetBuffer();
		
		if (pMsg->GetMsgFlag()&DOS_MESSAGE_FLAG_COMPRESSED)
		{
			switch (m_MsgCompressType)
			{
			case MSG_COMPRESS_ZIP_FAST:
			case MSG_COMPRESS_ZIP_NORMAL:
			case MSG_COMPRESS_ZIP_SLOW:
				//{
				//	CDOSSimpleMessage * pNewMsg=(CDOSSimpleMessage *)m_SendBuffer.GetBuffer();
				//	pNewMsg->GetMsgHeader()=pMsg->GetMsgHeader();
				//	uLongf OutLen=m_SendBuffer.GetBufferSize()-sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD);
				//	int Result=uncompress((Bytef *)pNewMsg->GetDataBuffer(),&OutLen,
				//		(Bytef *)pMsg->GetDataBuffer(),pMsg->GetDataLength());
				//	if(Result==Z_OK)
				//	{
				//		pNewMsg->SetDataLength(OutLen);
				//		OnDOSMessage(pNewMsg);
				//	}
				//	else
				//	{
				//		Close();
				//		PrintDOSLog(_T("(%d消息zip解压缩失败，连接断开！"),GetID());
				//	}
				//}
				break;
			default:
			{
				CDOSSimpleMessage * pNewMsg = (CDOSSimpleMessage *)m_SendBuffer.GetBuffer();
				pNewMsg->GetMsgHeader() = pMsg->GetMsgHeader();
				lzo_uint OutLen = m_SendBuffer.GetBufferSize() - sizeof(CDOSSimpleMessage::DOS_SIMPLE_MESSAGE_HEAD);
				int Result = lzo1x_decompress_safe((BYTE *)pMsg->GetDataBuffer(), pMsg->GetDataLength(),
					(BYTE *)pNewMsg->GetDataBuffer(), &OutLen,
					NULL);
				if (Result == LZO_E_OK)
				{
					pNewMsg->SetDataLength((MSG_LEN_TYPE)OutLen);
					pMsg = pNewMsg;
				}
				else
				{
					Close();
					PrintDOSLog( _T("(%d消息lzo解压缩失败，连接断开！"), GetID());
				}
			}
			break;
			}
		}

		if (pMsg->GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
		{
			OnSystemMessage(pMsg);
		}
		else
		{
			OnDOSMessage(pMsg);
		}
		m_KeepAliveCount = 0;
		m_AssembleBuffer.PopFront(NULL,PacketSize);
		PeekPos=0;
		PacketSize=0;
		m_AssembleBuffer.Peek(PeekPos,&PacketSize,sizeof(MSG_LEN_TYPE));
	}
}

BOOL CDOSClient::OnDOSMessage(CDOSSimpleMessage * pMessage)
{
	return FALSE;
}

BOOL CDOSClient::OnSystemMessage(CDOSSimpleMessage * pMessage)
{
	switch (pMessage->GetMsgID())
	{
	case DSM_PROXY_KEEP_ALIVE_PING:
		pMessage->SetMsgID(DSM_PROXY_KEEP_ALIVE_PONG);
		Send(pMessage, pMessage->GetMsgLength());
		return TRUE;
	case DSM_PROXY_KEEP_ALIVE_PONG:
		if (pMessage->GetDataLength() >= sizeof(PING_DATA))
		{
			PING_DATA * pPingData = (PING_DATA *)pMessage->GetDataBuffer();
			m_RecentPingDelay = GetTimeToTime(pPingData->Time, CEasyTimer::GetTime());
		}
		return TRUE;
	}
	return FALSE;
}

int CDOSClient::Update(int ProcessPacketLimit)
{
	if(IsConnected()&&m_KeepAliveTimer.IsTimeOut(m_KeepAliveTime))
	{
		m_KeepAliveTimer.SaveTime();
		m_KeepAliveCount++;
		if(m_KeepAliveCount>=m_MaxKeepAliveCount)
		{
			PrintDOSLog(_T("KeepAlive超时！"));
			m_KeepAliveCount=0;
			Disconnect();
		}
		SendKeepAlivePing();
	}
	return CNetConnection::Update(ProcessPacketLimit);
}

void CDOSClient::SendKeepAlivePing()
{
	CDOSSimpleMessage * pSimpleMessage = (CDOSSimpleMessage *)m_SendBuffer.GetBuffer();
	m_SendBuffer.PushBack(NULL, CDOSSimpleMessage::GetMsgHeaderLength());

	pSimpleMessage->Init();
	pSimpleMessage->SetMsgID(DSM_PROXY_KEEP_ALIVE_PING);
	pSimpleMessage->SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	pSimpleMessage->SetDataLength(sizeof(sizeof(PING_DATA)));
	PING_DATA * pPingData = (PING_DATA *)pSimpleMessage->GetDataBuffer();
	pPingData->Time = CEasyTimer::GetTime();
	pPingData->RecentDelay = m_RecentPingDelay;	

	Send(pSimpleMessage, pSimpleMessage->GetMsgLength());
}