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

CDOSClient::CDOSClient(void)
{
	m_KeepAliveCount=0;
	m_MaxKeepAliveCount=10;
	m_KeepAliveTime=30000;
	m_RecentPingDelay = 0;

	m_MsgCompressType = MSG_COMPRESS_LZO;
	m_MsgEnCryptType = MSG_ENCRYPT_NONE;
	m_TotalMsgSendCount = 0;
	m_DumpMsg = false;
	
	m_AssembleBuffer.SetTag(_T("CDOSClient"));
	m_SendBuffer.SetTag(_T("CDOSClient"));
	m_MsgFnMap.SetTag(_T("CDOSClient"));
	m_EncyptBuffer.SetTag(_T("CDOSClient"));
}

CDOSClient::~CDOSClient(void)
{
	Destory();
}


void CDOSClient::Destory()
{
	CNetConnection::Destory();
}

bool CDOSClient::Start(UINT SendQueueLen, UINT RecvQueueLen, const CIPAddress& Address, DWORD TimeOut)
{
	if (!m_SendBuffer.Create(SendQueueLen*NET_DATA_BLOCK_SIZE))
		return false;
	if (!m_AssembleBuffer.Create(RecvQueueLen*NET_DATA_BLOCK_SIZE * 2))
		return false;
	if (!m_EncyptBuffer.Create((SendQueueLen + 1)*NET_DATA_BLOCK_SIZE))
		return false;
	if (!CNetConnection::Create(RecvQueueLen, SendQueueLen))
		return false;

	m_KeepAliveTimer.SaveTime();
	m_KeepAliveCount=0;
	m_RecentPingDelay = 0;
	m_TotalMsgSendCount = 0;
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
bool CDOSClient::SendMessage(OBJECT_ID ReceiverID, OBJECT_ID SenderID, MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize)
{
	m_TotalMsgSendCount++;
	m_SendBuffer.SetUsedSize(0);
	CDOSSimpleMessage * pSimpleMessage=(CDOSSimpleMessage *)m_SendBuffer.GetBuffer();
	m_SendBuffer.PushBack(NULL,CDOSSimpleMessage::GetMsgHeaderLength());

	pSimpleMessage->Init();
	pSimpleMessage->SetMsgID(MsgID);
	pSimpleMessage->SetDataLength(DataSize);
	pSimpleMessage->SetMsgFlag(MsgFlag);

	if (m_MsgEnCryptType != MSG_ENCRYPT_NONE)
	{
		MSG_LEN_TYPE OutLen = DataSize;
		WORD CRC = 0;
		pData = EncyptMsg(pData, OutLen, CRC, m_TotalMsgSendCount);
		if (pData == NULL)
		{
			PrintDOSLog(_T("消息加密失败"));
			Disconnect();
			return false;
		}
		DataSize = OutLen;
		pSimpleMessage->SetDataLength(OutLen);
		pSimpleMessage->SetCRC(CRC);
		pSimpleMessage->SetMsgFlag(MsgFlag | DOS_MESSAGE_FLAG_ENCRYPT);
	}

	LPCVOID DataBuffers[2];
	UINT DataSizes[2];

	DataBuffers[0] = pSimpleMessage;
	DataSizes[0] = pSimpleMessage->GetMsgHeaderLength();
	DataBuffers[1] = pData;
	DataSizes[1] = DataSize;
		

	if (!SendMulti(DataBuffers, DataSizes, 2))
	{
		PrintDOSLog(_T("发送失败"));
		Disconnect();
		return false;
	}
	return true;
}

bool CDOSClient::SendMessageMulti(OBJECT_ID * pReceiverIDList,UINT ReceiverCount,bool IsSorted, OBJECT_ID SenderID, MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize)
{
	return false;
}

bool CDOSClient::BroadcastMessageToProxyByMask(WORD RouterID, BYTE ProxyType, UINT64 Mask, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize)
{
	return false;
}

bool CDOSClient::BroadcastMessageToProxyByGroup(WORD RouterID, BYTE ProxyType, UINT64 GroupID, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize)
{
	return false;
}

CDOSMessagePacket * CDOSClient::NewMessagePacket(UINT DataSize,UINT ReceiverCount)
{
	MSG_LEN_TYPE PacketLen=CDOSMessagePacket::CaculatePacketLength(DataSize,ReceiverCount);
	MSG_LEN_TYPE PacketRealLen=CDOSMessagePacket::CaculateRealPacketLength(PacketLen);
	if(PacketRealLen>m_SendBuffer.GetBufferSize())
		return NULL;
	CDOSMessagePacket * pMsgPacket=(CDOSMessagePacket *)m_SendBuffer.GetBuffer();
	pMsgPacket->SetAllocSize((UINT)m_SendBuffer.GetBufferSize());
	pMsgPacket->Init();
	pMsgPacket->SetPacketLength(PacketLen);
	pMsgPacket->IncRefCount();
	return pMsgPacket;
}
bool CDOSClient::ReleaseMessagePacket(CDOSMessagePacket * pPacket)
{
	pPacket->DecRefCount();
	return true;
}
bool CDOSClient::SendMessagePacket(CDOSMessagePacket * pPacket)
{
	m_TotalMsgSendCount++;
	CDOSSimpleMessage * pSimpleMessage=pPacket->GetMessage().MakeSimpleMessage();
	LPCVOID pData = pSimpleMessage->GetMsgData();
	UINT DataSize = pSimpleMessage->GetDataLength();
	if (m_MsgEnCryptType != MSG_ENCRYPT_NONE)
	{
		if (m_DumpMsg)
			DumpBinData((const BYTE *)pData, DataSize, 1024);

		MSG_LEN_TYPE OutLen = DataSize;
		WORD CRC = 0;
		pData = EncyptMsg(pData, OutLen, CRC, m_TotalMsgSendCount);
		if (pData == NULL)
		{
			PrintDOSLog(_T("消息加密失败"));
			Disconnect();
			return false;
		}
		DataSize = OutLen;
		if (m_DumpMsg)
		{
			DumpBinData((const BYTE *)pData, DataSize, 1024);
			PrintDOSLog(_T("Msg:0x%X,CRC=0x%X,MsgCount=%d"), pSimpleMessage->GetMsgID(), CRC, m_TotalMsgSendCount);
		}
		pSimpleMessage->SetDataLength(OutLen);
		pSimpleMessage->SetCRC(CRC);
		pSimpleMessage->SetMsgFlag(pSimpleMessage->GetMsgFlag() | DOS_MESSAGE_FLAG_ENCRYPT);
	}

	LPCVOID DataBuffers[2];
	UINT DataSizes[2];

	DataBuffers[0] = pSimpleMessage;
	DataSizes[0] = pSimpleMessage->GetMsgHeaderLength();
	DataBuffers[1] = pData;
	DataSizes[1] = DataSize;


	if (!SendMulti(DataBuffers, DataSizes, 2))
	{
		PrintDOSLog(_T("发送失败"));
		Disconnect();
		return false;
	}
	return true;
}

bool CDOSClient::RegisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)
{
	return false;
}
bool CDOSClient::UnregisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)
{
	return false;
}
bool CDOSClient::RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID, int MapType)
{
	return false;
}
bool CDOSClient::UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID)
{
	return false;
}

bool CDOSClient::SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType)
{
	return false;
}

bool CDOSClient::AddConcernedObject(OBJECT_ID ObjectID,bool NeedTest)
{
	return false;
}

bool CDOSClient::DeleteConcernedObject(OBJECT_ID ObjectID)
{
	return false;
}

bool CDOSClient::FindObject(UINT ObjectType, bool OnlyLocal)
{
	return false;
}
bool CDOSClient::ReportObject(OBJECT_ID TargetID, const void * pObjectInfoData, UINT DataSize)
{
	return false;
}
bool CDOSClient::CloseProxyObject(OBJECT_ID ProxyObjectID,UINT Delay)
{
	return false;
}
bool CDOSClient::RequestProxyObjectIP(OBJECT_ID ProxyObjectID)
{
	return false;
}

bool CDOSClient::RegisterObject(DOS_OBJ_REGISTER_INFO& ObjectRegisterInfo)
{
	return false;
}
void CDOSClient::Release()
{
	CNetConnection::Release();
}

bool CDOSClient::QueryShutDown(OBJECT_ID TargetID, BYTE Level, UINT Param)
{
	return false;
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
bool CDOSClient::RegisterLogger(UINT LogChannel, LPCTSTR FileName)
{
	return false;
}
bool CDOSClient::RegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader)
{
	return false;
}
void CDOSClient::SetServerWorkStatus(BYTE WorkStatus)
{

}
UINT CDOSClient::AddTimer(UINT64 TimeOut, UINT64 Param, bool IsRepeat)
{
	return 0;
}
bool CDOSClient::DeleteTimer(UINT ID)
{
	return false;
}
bool CDOSClient::SetBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 GroupID)
{
	return false;
}
bool CDOSClient::AddBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask)
{
	return false;
}
bool CDOSClient::RemoveBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask)
{
	return false;
}
bool CDOSClient::AddBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID)
{
	return false;
}
bool CDOSClient::RemoveBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID)
{
	return false;
}
void CDOSClient::OnRecvData(const BYTE * pData, UINT DataSize)
{
	MSG_LEN_TYPE PacketSize=0;
	size_t PeekPos=0;
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
				const BYTE * pMsgData = (const BYTE *)pMsg->GetMsgData();
				UINT MsgDataLen = pMsg->GetDataLength();
				if (MsgDataLen >= sizeof(UINT))
				{
					UINT OrginMsgDataLen = *((const UINT *)pMsgData);
					if (OrginMsgDataLen <= OutLen)
					{
						pMsgData += sizeof(UINT);
						MsgDataLen -= sizeof(UINT);
						int Result = lzo1x_decompress_safe(pMsgData, MsgDataLen,
							(BYTE *)pNewMsg->GetMsgData(), &OutLen,
							NULL);
						if (Result == LZO_E_OK)
						{
							pNewMsg->SetDataLength((MSG_LEN_TYPE)OutLen);
							pMsg = pNewMsg;
						}
						else
						{
							Close();
							PrintDOSLog(_T("(%d消息lzo解压缩失败，连接断开！"), GetID());
						}
					}
					else
					{
						Close();
						PrintDOSLog(_T("(%d解压缓冲大小不足(%u<%u)，连接断开！"), GetID(), OutLen, OrginMsgDataLen);
					}
				}
				else
				{
					Close();
					PrintDOSLog(_T("(%d压缩数据格式错误，连接断开！"), GetID());
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

bool CDOSClient::OnDOSMessage(CDOSSimpleMessage * pMessage)
{
	return false;
}

bool CDOSClient::OnSystemMessage(CDOSSimpleMessage * pMessage)
{
	switch (pMessage->GetMsgID())
	{
	case DSM_PROXY_KEEP_ALIVE_PING:
		pMessage->SetMsgID(DSM_PROXY_KEEP_ALIVE_PONG);
		Send(pMessage, pMessage->GetMsgLength());
		return true;
	case DSM_PROXY_KEEP_ALIVE_PONG:
		if (pMessage->GetDataLength() >= sizeof(PING_DATA))
		{
			const PING_DATA * pPingData = (const PING_DATA *)pMessage->GetMsgData();
			m_RecentPingDelay = CEasyTimer::GetTimeToTime(pPingData->Time, CEasyTimer::GetTime());
		}
		return true;
	}
	return false;
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
	PING_DATA * pPingData = (PING_DATA *)pSimpleMessage->GetMsgData();
	pPingData->Time = CEasyTimer::GetTime();
	pPingData->RecentDelay = m_RecentPingDelay;	

	Send(pSimpleMessage, pSimpleMessage->GetMsgLength());
}

const void * CDOSClient::EncyptMsg(const void * pData, MSG_LEN_TYPE& DataLen, WORD& CRC, UINT MsgSequence)
{
	CRC = 0;
	if (m_MsgEnCryptType != MSG_ENCRYPT_NONE && m_SecretKey.GetLength())
	{
		CRC = MakeCRC(pData, DataLen, m_SecretKey.GetBuffer(), (UINT)m_SecretKey.GetLength(), MsgSequence);

		if (m_EncyptBuffer.GetBufferSize() < DataLen)
		{
			PrintDOSLog(_T("数据过大(%u)，超过加密缓冲大小(%u)！"),
				DataLen, m_EncyptBuffer.GetBufferSize());
			return NULL;
		}

		switch (m_MsgEnCryptType)
		{
		case MSG_ENCRYPT_CHECK_SUM:
			return pData;
			break;
		case MSG_ENCRYPT_DES:
			{
				size_t OutLen = m_EncyptBuffer.GetBufferSize();
				if (CCryptTools::DESEncryptECB((const BYTE *)m_SecretKey.GetBuffer(), m_SecretKey.GetLength(),
					(const BYTE *)pData, DataLen, (BYTE *)m_EncyptBuffer.GetBuffer(), OutLen))
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					return m_EncyptBuffer.GetBuffer();
				}
				else
				{
					PrintDOSLog(_T("DES加密消息失败，将直接发送"));
				}
			}
			break;
		case MSG_ENCRYPT_AES:
			{
				size_t OutLen = m_EncyptBuffer.GetBufferSize();
				if (CCryptTools::AESEncryptECB((const BYTE *)m_SecretKey.GetBuffer(), m_SecretKey.GetLength(),
					(const BYTE *)pData, DataLen, (BYTE *)m_EncyptBuffer.GetBuffer(), OutLen))
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					return m_EncyptBuffer.GetBuffer();
				}
				else
				{
					PrintDOSLog(_T("AES加密消息失败，将直接发送"));
				}
			}
			break;
		case MSG_ENCRYPT_TEA:
			{
				size_t OutLen = m_EncyptBuffer.GetBufferSize();
				if (CCryptTools::TEAEncryptECB((const BYTE *)m_SecretKey.GetBuffer(), m_SecretKey.GetLength(), DEFAULT_TEA_CYCLE,
					(const BYTE *)pData, DataLen, (BYTE *)m_EncyptBuffer.GetBuffer(), OutLen))
				{
					DataLen = (MSG_LEN_TYPE)OutLen;
					return m_EncyptBuffer.GetBuffer();
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

void * CDOSClient::DecyptMsg(void * pData, MSG_LEN_TYPE& DataLen, WORD CRC, UINT MsgSequence)
{
	if (m_MsgEnCryptType != MSG_ENCRYPT_NONE && m_SecretKey.GetLength())
	{
		if (m_EncyptBuffer.GetBufferSize() < DataLen)
		{
			PrintDOSLog(_T("数据过大(%u)，超过加密缓冲大小(%u)！"),
				DataLen, m_EncyptBuffer.GetBufferSize());
			return NULL;
		}

		switch (m_MsgEnCryptType)
		{
		case MSG_ENCRYPT_CHECK_SUM:
		{
			WORD TargetCRC = MakeCRC(pData, (UINT)DataLen, m_SecretKey.GetBuffer(), (UINT)m_SecretKey.GetLength(), MsgSequence);
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
			if (CCryptTools::DESDecryptECB((const BYTE *)m_SecretKey.GetBuffer(), m_SecretKey.GetLength(),
				(BYTE *)pData, DataLen, (BYTE *)m_EncyptBuffer.GetBuffer(), OutLen))
			{
				WORD TargetCRC = MakeCRC(pData, (UINT)OutLen, m_SecretKey.GetBuffer(), (UINT)m_SecretKey.GetLength(), MsgSequence);
				if (TargetCRC == CRC)
				{
					DataLen = (UINT)OutLen;
					return m_EncyptBuffer.GetBuffer();
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
			if (CCryptTools::AESDecryptECB((const BYTE *)m_SecretKey.GetBuffer(), m_SecretKey.GetLength(),
				(BYTE *)pData, DataLen, (BYTE *)m_EncyptBuffer.GetBuffer(), OutLen))
			{
				WORD TargetCRC = MakeCRC(pData, (UINT)OutLen, m_SecretKey.GetBuffer(), (UINT)m_SecretKey.GetLength(), MsgSequence);
				if (TargetCRC == CRC)
				{
					DataLen = (UINT)OutLen;
					return m_EncyptBuffer.GetBuffer();
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
			if (CCryptTools::TEADecryptECB((const BYTE *)m_SecretKey.GetBuffer(), m_SecretKey.GetLength(), DEFAULT_TEA_CYCLE,
				(BYTE *)pData, DataLen, (BYTE *)m_EncyptBuffer.GetBuffer(), OutLen))
			{
				WORD TargetCRC = MakeCRC(pData, (UINT)OutLen, m_SecretKey.GetBuffer(), (UINT)m_SecretKey.GetLength(), MsgSequence);
				if (TargetCRC == CRC)
				{
					DataLen = (UINT)OutLen;
					return m_EncyptBuffer.GetBuffer();
				}
				else
				{
					PrintDOSLog(_T("TEA:CRC校验失败"));
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


WORD CDOSClient::MakeCRC(const void * pData, UINT DataLen, const void * pKey, UINT KeyLen, UINT MsgSequence)
{
	CCRCMaker CRCMaker;
	CRCMaker.CRC16_Fast_Init();
	if (pData)
		CRCMaker.CRC16_Fast_Append((const BYTE *)pData, DataLen);
	if (pKey)
		CRCMaker.CRC16_Fast_Append((const BYTE *)pKey, KeyLen);
	CRCMaker.CRC16_Fast_Append((const BYTE *)&MsgSequence, sizeof(UINT));
	return CRCMaker.CRC16_Fast_Final();
}