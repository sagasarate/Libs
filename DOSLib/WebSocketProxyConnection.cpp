#include "stdafx.h"



CWebSocketProxyConnection::CWebSocketProxyConnection()
{
	m_WebSocketStatus = WEB_SOCKET_STATUS_NONE;
}


CWebSocketProxyConnection::~CWebSocketProxyConnection()
{
}
void CWebSocketProxyConnection::OnConnection(bool IsSucceed)
{
	if (m_Config.MaxWSFrameSize < 1024)
	{
		PrintDOSLog(_T("最大帧大小(%u)异常！"), m_Config.MaxWSFrameSize);
		m_Config.MaxWSFrameSize = 1024;
	}
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

		//SendProtocolOption();
	}
	else
	{
		PrintDOSLog(_T("连接初始化失败！IP=%s"), GetRemoteAddress().GetIPString());
		m_Status = STATUS_DISCONNECTED;
		m_pService->QueryDestoryConnection(this);
	}
}

bool CWebSocketProxyConnection::OnMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, const void* pData, MSG_LEN_TYPE DataLen)
{
	m_TotalMsgSendCount++;

	if ((m_Config.MsgCompressType != MSG_COMPRESS_NONE) && ((MsgFlag & DOS_MESSAGE_FLAG_NO_COMPRESS) == 0))
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
			MsgFlag |= DOS_MESSAGE_FLAG_COMPRESSED;
			pData = pNewData;
		}
	}	
	return SendMsg(MsgID, MsgFlag, 0, (const BYTE*)pData, DataLen);
}

void CWebSocketProxyConnection::OnRecvData(const BYTE * pData, UINT DataSize)
{
	m_KeepAliveCount = 0;
	m_KeepAliveTimer.SaveTime();
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


void CWebSocketProxyConnection::OnKeepAliveMsg(UINT Timestamp, UINT RecentDelay)
{
	m_RecentPingDelay = RecentDelay;

	PING_DATA PingData;
	PingData.Time = Timestamp;
	PingData.RecentDelay = m_RecentPingDelay;

	SendMsg(DSM_PROXY_KEEP_ALIVE_PONG, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, 0, (const BYTE*)&PingData, sizeof(PingData));
}
void CWebSocketProxyConnection::OnKeepAliveAckMsg(UINT Timestamp)
{

}
void CWebSocketProxyConnection::SendKeepAliveMsg()
{
	SendWebSocketPingMsg();
}
void CWebSocketProxyConnection::SendProtocolOption()
{
	PROTOCOL_OPTION Data;
	Data.Flag = 0;
	if (m_Config.MsgEnCryptType != MSG_ENCRYPT_NONE)
		Data.Flag |= PROTOCOL_OPTION_FLAG_UP_MSG_USE_ENCRYPT;

	SendMsg(DSM_PROTOCOL_OPTION, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,0, (const BYTE*)&Data, sizeof(PROTOCOL_OPTION));

	PrintDOSDebugLog(_T("已发送协议配置0x%X"), Data.Flag);
}




void CWebSocketProxyConnection::ProcessHTTPMsg(const BYTE * pData, UINT DataSize)
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

void CWebSocketProxyConnection::OnHTTPRequest(const char * szRequest)
{
	PrintDOSLog(_T("%s"), szRequest);

	CEasyArray<char *> Lines;
	ParseStringLines((char *)szRequest, Lines);
	CEasyStringA GetField;
	CEasyStringA UpgradeField;
	CEasyStringA ConnectionField;
	CEasyStringA SecWebSocketKeyField;
	CEasyStringA SecWebSocketVersionField;
	CEasyStringA SecWebSocketExtensionsField;
	CEasyStringA HostField;
	CEasyStringA OriginField;
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

		sha1.Update((BYTE*)SecWebSocketKeyField.GetBuffer(), (UINT)SecWebSocketKeyField.GetLength());
		sha1.Final();
		sha1.GetHash(HashBin);
		CEasyStringA HashStr = CBase64::Encode(HashBin, 20);
		SendHTTPRespond(101, HashStr);
		m_WebSocketStatus = WEB_SOCKET_STATUS_ACCEPTED;
	}
	else
	{
		SendHTTPRespond(403, "Not Support");
	}
}

void CWebSocketProxyConnection::SendHTTPRespond(int Code, LPCSTR szContent)
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
			(UINT)strlen(szContent), szContent);
		break;
	default:
		sprintf_s(Buff, 4096, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s",
			(UINT)strlen(szContent), szContent);
		break;
	}
	Send(Buff, (UINT)strlen(Buff));
	if (Code != 101)
	{
		QueryDisconnect(1000);
	}
}

void CWebSocketProxyConnection::ParseStringLines(char * szStr, CEasyArray<char *>& StrLines)
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

void CWebSocketProxyConnection::ProcessWebSocketData(const BYTE * pData, UINT DataSize)
{
	if (m_AssembleBuffer.PushBack(pData, DataSize))
	{
		UINT FrameCount = 0;
		size_t Pos = 0;
		while (m_AssembleBuffer.GetUsedSize() - Pos >= 2)
		{
			BYTE Header1, Header2;
			m_AssembleBuffer.Peek(Pos, &Header1, sizeof(Header1));
			m_AssembleBuffer.Peek(Pos, &Header2, sizeof(Header2));
			BYTE OPCode = Header1 & 0x0F;
			BYTE ReserveCode = (Header1 >> 4) & 0x07;
			bool IsFinFrame = (Header1 & 0x80) != 0;
			bool HaveMask = (Header2 & 0x80) != 0;
			UINT64 DataLen = Header2 & 0x7F;
			if (DataLen == 126)
			{
				m_AssembleBuffer.Peek(Pos, &DataLen, sizeof(WORD));
				DataLen = ntohs((WORD)DataLen);
			}
			else if (DataLen == 127)
			{
				m_AssembleBuffer.Peek(Pos, &DataLen, sizeof(UINT64));
				DataLen = __ntohll(DataLen);
			}
			BYTE MaskKey[4];
			if (HaveMask)
			{
				m_AssembleBuffer.Peek(Pos, MaskKey, sizeof(BYTE) * 4);
			}
			if (m_AssembleBuffer.GetUsedSize() >= Pos + DataLen)
			{
				FrameCount++;
				if (IsFinFrame)
				{
					//结束帧
					if (FrameCount > 1)
					{
						//多帧
						if (!ProcessMultiFrameData(m_AssembleBuffer, FrameCount))
						{
							Disconnect();
							break;
						}
					}
					else
					{
						//单帧
						BYTE* pData = ((BYTE*)m_AssembleBuffer.GetBuffer()) + Pos;
						if (HaveMask)
						{
							for (UINT i = 0; i < DataLen; i++)
							{
								pData[i] = pData[i] ^ MaskKey[i % 4];
							}
						}
						OnWebSocketFrame(OPCode, pData, (UINT)DataLen);
					}
					m_AssembleBuffer.PopFront(NULL, Pos + DataLen);
					FrameCount = 0;
					Pos = 0;
				}
				else
				{
					//继续处理下一帧
					Pos += DataLen;
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		Disconnect();
		PrintDOSLog(_T("对象代理(%d)拼包缓冲溢出，连接断开！"), GetID());
	}
}
bool CWebSocketProxyConnection::ProcessMultiFrameData(CEasyBuffer& DataBuffer, UINT FrameCount)
{
	size_t Pos = 0;
	UINT TotalDataLen = 0;
	BYTE* pFrameData = NULL;
	BYTE OPCode = 0;
	for (UINT i = 0; i < FrameCount; i++)
	{
		if (m_AssembleBuffer.GetUsedSize() - Pos < 2)
		{
			PrintDOSLog(_T("对象代理(%d)帧数据大小异常1！"), GetID());
			return false;
		}
		BYTE Header1, Header2;
		m_AssembleBuffer.Peek(Pos, &Header1, sizeof(Header1));
		m_AssembleBuffer.Peek(Pos, &Header2, sizeof(Header2));
		if (pFrameData == NULL)
		{
			if (WEB_SOCKET_OP_CODE_CONTINUOUS_DATA != (Header1 & 0x0F))
			{
				PrintDOSLog(_T("对象代理(%d)帧数据OPCode()错误！"), GetID(), (Header1 & 0x0F));
				return false;
			}
		}
		else
		{
			OPCode = Header1 & 0x0F;
		}		
		BYTE ReserveCode = (Header1 >> 4) & 0x07;
		bool IsFinFrame = (Header1 & 0x80) != 0;
		bool HaveMask = (Header2 & 0x80) != 0;
		UINT64 DataLen = Header2 & 0x7F;
		if (DataLen == 126)
		{
			m_AssembleBuffer.Peek(Pos, &DataLen, sizeof(WORD));
			DataLen = ntohs((WORD)DataLen);
		}
		else if (DataLen == 127)
		{
			m_AssembleBuffer.Peek(Pos, &DataLen, sizeof(UINT64));
			DataLen = __ntohll(DataLen);
		}
		BYTE MaskKey[4];
		if (HaveMask)
		{
			m_AssembleBuffer.Peek(Pos, MaskKey, sizeof(BYTE) * 4);
		}
		if (m_AssembleBuffer.GetUsedSize() >= Pos + DataLen)
		{
			BYTE* pData = ((BYTE*)m_AssembleBuffer.GetBuffer()) + Pos;
			if (HaveMask)
			{
				for (UINT i = 0; i < DataLen; i++)
				{
					pData[i] = pData[i] ^ MaskKey[i % 4];
				}
			}
			if (pFrameData == NULL)
			{
				pFrameData = pData;
			}
			else
			{
				//移动数据，使得各帧数据连续
				memmove(pFrameData + DataLen, pData, DataLen);
			}
			TotalDataLen += (UINT)DataLen;
		}
		else
		{
			PrintDOSLog(_T("对象代理(%d)帧数据大小异常2！"), GetID());
		}
	}
	OnWebSocketFrame(OPCode, pFrameData, TotalDataLen);
	return true;
}
void CWebSocketProxyConnection::OnWebSocketFrame(BYTE OPCode, BYTE* pData, UINT DataLen)
{
	//PrintDOSDebugLog("Frame:%u %u %s %s Mask=%02X %02X %02X %02X", 
	//	OPCode, DataLen, IsFinFrame ? "true" : "false", HaveMask ? "true" : "false", MaskKey[0], MaskKey[1], MaskKey[2], MaskKey[3]);
	switch (OPCode)
	{
	case WEB_SOCKET_OP_CODE_CONTINUOUS_DATA:
	case WEB_SOCKET_OP_CODE_TEXT_DATA:
	case WEB_SOCKET_OP_CODE_BINARY_DATA:
		{
			
			if (DataLen >= sizeof(WS_MESSAGE_HEAD))
			{
				WS_MESSAGE_HEAD* pHeader = (WS_MESSAGE_HEAD*)pData;
				ProcessClientMsg(pHeader->MsgID, pHeader->MsgFlag, pHeader->CRC, pData + sizeof(WS_MESSAGE_HEAD), DataLen - sizeof(WS_MESSAGE_HEAD));
			}
		}
		break;
	case WEB_SOCKET_OP_CODE_CLOSE:
		{
			SendWebSocketCloseMsg();
			PrintDOSLog(_T("对象代理(%d)收到WebSocket的连接关闭帧！"), GetID());
			QueryDisconnect(1000);
		}
		break;
	case WEB_SOCKET_OP_CODE_KEEP_ALIVE_PING:
		{
			m_KeepAliveCount = 0;
			m_KeepAliveTimer.SaveTime();
			SendWebSocketPongMsg();
		}
		break;
	case WEB_SOCKET_OP_CODE_KEEP_ALIVE_PONG:
	
		break;
	default:
		PrintDOSLog(_T("对象代理(%d)未知的WebSocket的OPCode(%d)，连接断开！"), GetID(), OPCode);
		Disconnect();
	}
}

void CWebSocketProxyConnection::SendWebSocketCloseMsg()
{
	static BYTE CloseFrame[] = { WEB_SOCKET_OP_CODE_CLOSE | 0x80, 0 };
	if (m_WebSocketStatus == WEB_SOCKET_STATUS_ACCEPTED)
		Send(CloseFrame, sizeof(CloseFrame));
}
void CWebSocketProxyConnection::SendWebSocketPingMsg()
{
	static BYTE PingFrame[] = { WEB_SOCKET_OP_CODE_KEEP_ALIVE_PING | 0x80, 0 };
	if (m_WebSocketStatus == WEB_SOCKET_STATUS_ACCEPTED)
		Send(PingFrame, sizeof(PingFrame));
}
void CWebSocketProxyConnection::SendWebSocketPongMsg()
{
	static BYTE Frame[] = { WEB_SOCKET_OP_CODE_KEEP_ALIVE_PONG | 0x80, 0 };
	if (m_WebSocketStatus == WEB_SOCKET_STATUS_ACCEPTED)
		Send(Frame, sizeof(Frame));
}

bool CWebSocketProxyConnection::SendMsg(MSG_ID_TYPE MsgID, WORD MsgFlag, WORD CRC, const BYTE* pData, UINT DataLen)
{
	WS_MESSAGE_HEAD MsgHeader;
	MsgHeader.MsgID = MsgID;
	MsgHeader.MsgFlag = MsgFlag;
	MsgHeader.CRC = CRC;

	UINT TotalDataLen = sizeof(MsgHeader)+ DataLen;
	if (TotalDataLen < m_Config.MaxWSFrameSize)
	{
		return SendWebSocketFrame(WEB_SOCKET_OP_CODE_BINARY_DATA, &MsgHeader, sizeof(MsgHeader), pData, DataLen, true);
	}
	else
	{
		//分帧发送
		UINT SendLen = m_Config.MaxWSFrameSize - sizeof(MsgHeader);
		if (!SendWebSocketFrame(WEB_SOCKET_OP_CODE_BINARY_DATA, &MsgHeader, sizeof(MsgHeader), pData, SendLen, false))
			return false;
		DataLen -= SendLen;
		pData += SendLen;
		while (DataLen)
		{
			if (DataLen > m_Config.MaxWSFrameSize)
				SendLen = m_Config.MaxWSFrameSize;
			else
				SendLen = DataLen;
			DataLen -= SendLen;
			if (!SendWebSocketFrame(WEB_SOCKET_OP_CODE_CONTINUOUS_DATA, pData, SendLen, NULL, 0, DataLen == 0))
				return false;
			pData += SendLen;
		}
		return true;
	}
}
bool CWebSocketProxyConnection::SendWebSocketFrame(WEB_SOCKET_OP_CODE OPCode, const void* pData1, UINT DataLen1, const void* pData2, UINT DataLen2, bool IsFin)
{
	BYTE FrameHeader[10];
	UINT HeaderSize = 2;
	if(IsFin)
		FrameHeader[0] = OPCode | 0x80;
	else
		FrameHeader[0] = OPCode;
	UINT MsgLen = DataLen1+ DataLen2;
	if (MsgLen < 126)
	{
		FrameHeader[1] = MsgLen;
	}
	else if (MsgLen < 0x10000)
	{
		FrameHeader[1] = 126;
		WORD Len = htons(MsgLen);
		FrameHeader[2] = Len & 0xFF;
		FrameHeader[3] = (Len >> 8) & 0xFF;
		HeaderSize += 2;
	}	
	else
	{
		FrameHeader[1] = 127;
		UINT64 Len = __htonll(MsgLen);
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

	LPCVOID DataBuffers[3];
	UINT DataSizes[3];
	UINT BuffCount = 1;
	DataBuffers[0] = FrameHeader;
	DataSizes[0] = HeaderSize;
	if (pData1 && DataLen1)
	{
		DataBuffers[1] = pData1;
		DataSizes[1] = DataLen1;
		BuffCount++;
	}
	if(pData2 && DataLen2)
	{
		DataBuffers[2] = pData2;
		DataSizes[2] = DataLen2;
		BuffCount++;
	}
	return SendMulti(DataBuffers, DataSizes, BuffCount);
}