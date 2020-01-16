#include "stdafx.h"



CTRConnection::CTRConnection()
	:m_AssembleBuffer(_T("CTRConnection"))
	, m_SendBuffer(_T("CTRConnection"))
	, m_MetaDataBuffer(_T("CTRConnection"))
{
	m_pParent = NULL;
	m_SearchID = 0;
	m_AssembleBuffer.Create(65536);
	m_SendBuffer.Create(4096);
	m_FinishHandshake = 0;
	m_MetaDataSize = 0;
	m_CurMetaDataPiece = 0;
}


CTRConnection::~CTRConnection()
{
}

bool CTRConnection::Init(CDHTService * pParent, UINT SearchID, const NODE_ID& PeerID, const NODE_ID& InfoHash, const CIPAddress& Address)
{
	m_pParent = pParent;
	m_SearchID = SearchID;
	m_PeerID = PeerID;
	m_InfoHash = InfoHash;	
	m_AssembleBuffer.SetUsedSize(0);
	m_SendBuffer.SetUsedSize(0);
	m_FinishHandshake = 0;
	m_MetaDataMsgID = 0;
	m_MetaDataSize = 0;
	m_CurMetaDataPiece = 0;
	m_LogTag.Format(_T("DHT(%d)"), GetID());
	//PrintNetDebugLogWithTag(m_LogTag, _T("尝试连接%s"), Address.GetAddressString());
	SetServer(pParent->GetServer());
	if (Connect(Address,20000))
		return true;
	return false;
}

void CTRConnection::OnConnection(bool IsSucceed)
{
	if (IsSucceed)
	{
		//PrintNetLogWithTag( _T("连接%s成功，发送握手1"), GetRemoteAddress().GetAddressString());
		SendHandshake1();
	}
	else
	{
		//PrintNetLogWithTag( _T("连接%s失败"), GetRemoteAddress().GetAddressString());
	}
}
void CTRConnection::OnDisconnection()
{
	//PrintNetLogWithTag( _T("连接%s断开"), GetRemoteAddress().GetAddressString());
}

void CTRConnection::OnRecvData(const BYTE * pData, UINT DataSize)
{
	m_AssembleBuffer.PushBack(pData, DataSize);
	if (m_FinishHandshake == 0)
	{
		if (m_AssembleBuffer.GetUsedSize() >= 68)
		{
			if (memcmp(((BYTE *)m_AssembleBuffer.GetBuffer()) + 1, "BitTorrent protocol", 19) == 0)
			{
				BYTE * pInfoHash = (BYTE *)m_AssembleBuffer.GetBuffer() + 28;
				BYTE * pPeerID = (BYTE *)m_AssembleBuffer.GetBuffer() + 48;
				memcpy(m_RemotePeerID.NodeID, pPeerID, NODE_ID_BYTE_COUNT);
				m_AssembleBuffer.PopFront(NULL, 68);				
				CEasyString PeerStr((char *)pPeerID, NODE_ID_BYTE_COUNT);
				//PrintNetDebugLogWithTag(m_LogTag, _T("收到握手1回应:%s"), (LPCTSTR)PeerStr);
				m_FinishHandshake = 1;
				SendHandshake2();
			}
		}
	}	
	else
	{
		
		while (m_AssembleBuffer.GetUsedSize() >= sizeof(UINT))
		{
			UINT MsgSize = 0;
			UINT PeekPtr = 0;
			m_AssembleBuffer.Peek(PeekPtr, &MsgSize, sizeof(MsgSize));
			MsgSize = ntohl(MsgSize);
			if (m_AssembleBuffer.GetUsedSize() >= MsgSize + sizeof(UINT))
			{
				BYTE MsgID = 0, ExtMsgID = 0;
				m_AssembleBuffer.Peek(PeekPtr, &MsgID, sizeof(MsgID));				
				if (MsgID == 20)
				{
					if (MsgSize >= 2)
					{
						m_AssembleBuffer.Peek(PeekPtr, &ExtMsgID, sizeof(ExtMsgID));
						OnMsg(((BYTE *)m_AssembleBuffer.GetBuffer()) + sizeof(UINT) + 2, MsgSize - 2);
					}
					else
					{
						PrintNetLogWithTag(m_LogTag, _T("消息大小异常Size=%u"), MsgSize);
					}
				}
				else
				{
					PrintNetDebugLogWithTag(m_LogTag, _T("丢弃未知消息MsgID=%u,Size=%u"), MsgID, MsgSize);
				}
				m_AssembleBuffer.PopFront(NULL, MsgSize + sizeof(UINT));
			}
			else
			{
				break;
			}
		}
	}
}
bool CTRConnection::SendHandshake1()
{
	char Buff[68];

	ZeroMemory(Buff, 68);
	Buff[0] = 19;
	memcpy(Buff + 1, "BitTorrent protocol", 19);
	Buff[25] = 0x10;
	memcpy(Buff + 28, m_InfoHash.NodeID, 20);
	memcpy(Buff + 48, m_PeerID.NodeID, 20);
	return Send(Buff, 68) != FALSE;
}
bool CTRConnection::SendHandshake2()
{
	char * pBuff = (char *)m_SendBuffer.GetBuffer();
	UINT BuffSize = m_SendBuffer.GetBufferSize();
	UINT BuffPtr = 0;

	UINT * MsgSize = (UINT *)(pBuff + BuffPtr);
	BuffPtr += sizeof(UINT);
	BuffSize -= sizeof(UINT);

	*(pBuff + BuffPtr) = 20;
	BuffPtr++;
	BuffSize--;

	*(pBuff + BuffPtr) = 0;
	BuffPtr++;
	BuffSize--;

	int Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "d1:md11:ut_metadatai3eee");
	BuffPtr += Len;
	BuffSize -= Len;

	*MsgSize = htonl(BuffPtr - sizeof(UINT));
	return Send(pBuff, BuffPtr) != FALSE;
}
bool CTRConnection::SendRequest(UINT Piece)
{
	char * pBuff = (char *)m_SendBuffer.GetBuffer();
	UINT BuffSize = m_SendBuffer.GetBufferSize();
	UINT BuffPtr = 0;

	UINT * MsgSize = (UINT *)(pBuff + BuffPtr);
	BuffPtr += sizeof(UINT);
	BuffSize -= sizeof(UINT);

	*(pBuff + BuffPtr) = 20;
	BuffPtr++;
	BuffSize--;

	*(pBuff + BuffPtr) = m_MetaDataMsgID;
	BuffPtr++;
	BuffSize--;

	int Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "d8:msg_typei0e5:piecei%uee", Piece);
	BuffPtr += Len;
	BuffSize -= Len;

	*MsgSize = htonl(BuffPtr - sizeof(UINT));
	return Send(pBuff, BuffPtr) != FALSE;
}

void CTRConnection::OnMsg(BYTE * pData, UINT DataSize)
{
	CBencodingValue MsgData;

	UINT ExtDataSize = DataSize;
	const BYTE * pExtData = MsgData.Parser(pData, ExtDataSize);

	if (ExtDataSize >0x8000|| ExtDataSize > DataSize)
	{
		pData[DataSize - 1] = 0;
		PrintNetLogWithTag(m_LogTag, _T("Bencoding解析异常:%s"), pData);
		Disconnect();
		return;
	}

	if (MsgData.GetType() != BENCODING_TYPE_DICTIONARY)
	{
		PrintNetLogWithTag(m_LogTag, _T("1收到不合法的消息格式连接%s断开"), GetRemoteAddress().GetAddressString());
		Disconnect();
	}

	if (m_FinishHandshake == 1)
	{
		if (MsgData.HaveDictValue("m"))
		{
			CBencodingValue& m = MsgData.GetDictValue("m");
			if (m.GetType() == BENCODING_TYPE_DICTIONARY)
			{
				for (UINT i = 0; i < m.GetDictCount(); i++)
				{
					BENCODING_VALUE_PAIR& Pair = m.GetDictPair(i);
					if (Pair.Value->GetType() == BENCODING_TYPE_INTEGER)
					{
						CEasyString Temp = Pair.Key;
						//PrintNetDebugLogWithTag(m_LogTag, _T("扩展消息%s=%d"), (LPCTSTR)Temp, Pair.Value->GetIntValue());
						if (Pair.Key == "ut_metadata")
						{
							m_MetaDataMsgID = (int)Pair.Value->GetIntValue();
						}
					}
				}				
			}			

			if (MsgData.HaveDictValue("metadata_size"))
			{
				CBencodingValue& MetaDataSize = MsgData.GetDictValue("metadata_size");
				m_MetaDataSize = (UINT)MetaDataSize.GetIntValue();
				if (m_MetaDataSize < MAX_TORRENT_SIZE)
				{					
					PrintNetDebugLogWithTag(m_LogTag, _T("MetaDataSize=%u"), m_MetaDataSize);
					m_MetaDataBuffer.Create(m_MetaDataSize);
				}
				else
				{
					PrintNetDebugLogWithTag(m_LogTag, _T("metadata_size=%u大小异常"), m_MetaDataSize);
					m_MetaDataSize = 0;
				}
			}

			//PrintNetDebugLogWithTag(m_LogTag, _T("收到握手2，开始请求数据"));
			m_FinishHandshake = 2;
			SendRequest(m_CurMetaDataPiece);
		}
		else
		{
			PrintNetLogWithTag( _T("2收到不合法的消息格式连接%s断开"), GetRemoteAddress().GetAddressString());
			Disconnect();
		}
	}
	else
	{
		if (MsgData.HaveDictValue("msg_type"))
		{
			int MsgType = (int)MsgData.GetDictValue("msg_type").GetIntValue();
			if (MsgType == 1)
			{
				if (m_CurMetaDataPiece == 0 && MsgData.HaveDictValue("total_size"))
				{
					m_MetaDataSize = (UINT)MsgData.GetDictValue("total_size").GetIntValue();	
					if (m_MetaDataSize < MAX_TORRENT_SIZE)
					{
						m_MetaDataBuffer.Create(m_MetaDataSize);
					}
					else
					{
						PrintNetDebugLogWithTag(m_LogTag, _T("total_size=%u大小异常"), m_MetaDataSize);
						m_MetaDataSize = 0;
					}
				}
				if (m_MetaDataSize)
				{
					PrintNetLogWithTag(m_LogTag, _T("收到MetaData Size=%d"), ExtDataSize);
					m_MetaDataBuffer.PushBack(pExtData, ExtDataSize);
					m_CurMetaDataPiece++;
					if (m_MetaDataBuffer.GetUsedSize() < m_MetaDataSize)
					{
						SendRequest(m_CurMetaDataPiece);
					}
					else
					{
						PrintNetLogWithTag( _T("MetaData接受完毕，连接%s断开"), GetRemoteAddress().GetAddressString());
						Disconnect();
						m_pParent->OnTorrent(m_SearchID, (BYTE *)m_MetaDataBuffer.GetBuffer(), m_MetaDataBuffer.GetUsedSize());
					}
				}
				else
				{
					PrintNetLogWithTag( _T("协议异常，连接%s断开"), GetRemoteAddress().GetAddressString());
					Disconnect();
				}
			}
			else if (MsgType == 2)
			{
				PrintNetLogWithTag( _T("请求被拒绝，连接%s断开"), GetRemoteAddress().GetAddressString());
				Disconnect();
			}
			else
			{
				PrintNetLogWithTag(m_LogTag, _T("异常MsgType=%d"), MsgType);
			}
		}
		else
		{
			PrintNetLogWithTag( _T("3收到不合法的消息格式连接%s断开"), GetRemoteAddress().GetAddressString());
			Disconnect();
		}
	}
	
}