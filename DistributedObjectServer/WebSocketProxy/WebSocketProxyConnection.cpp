#include "stdafx.h"



CWebSocketProxyConnection::CWebSocketProxyConnection()
{
	m_pOperator = NULL;
	m_WebSocketStatus = WEB_SOCKET_STATUS_NONE;
}


CWebSocketProxyConnection::~CWebSocketProxyConnection()
{
}


bool CWebSocketProxyConnection::Initialize(IDOSObjectProxyConnectionOperator * pOperator)
{
	m_pOperator = pOperator;
	if (m_AssembleBuffer.GetBufferSize() < m_pOperator->GetConfig().MaxMsgSize * 2)
	{
		if (!m_AssembleBuffer.Create(m_pOperator->GetConfig().MaxMsgSize * 2))
		{
			PrintDOSLog(_T("DOSLib"), _T("创建%u大小的拼包缓冲失败！"),
				m_pOperator->GetConfig().MaxMsgSize * 2);
			return false;
		}
	}
	m_WebSocketStatus = WEB_SOCKET_STATUS_NONE;
	return true;
}
void CWebSocketProxyConnection::Destory()
{
	CNameObject::Destory();
}
UINT CWebSocketProxyConnection::AddUseRef()
{
	return CNameObject::AddUseRef();
}
void CWebSocketProxyConnection::Release()
{
	CNameObject::Release();
}

void CWebSocketProxyConnection::OnConnection(BOOL IsSucceed)
{

}
void CWebSocketProxyConnection::OnDisconnection()
{

}
bool CWebSocketProxyConnection::OnMessage(CDOSMessage * pMessage)
{
	return SendWebSocketFrame(WEB_SOCKET_OP_CODE_BINARY_DATA, (BYTE *)pMessage->GetDataBuffer(), pMessage->GetDataLength());
}
bool CWebSocketProxyConnection::OnSystemMessage(CDOSMessage * pMessage)
{
	return false;
}
bool CWebSocketProxyConnection::OnSendKeepAlive()
{
	SendWebSocketPingMsg();
	return true;
}
void CWebSocketProxyConnection::OnRecvData(const BYTE * pData, UINT DataSize)
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

OBJECT_ID CWebSocketProxyConnection::GetMsgMap(MSG_ID_TYPE MsgID)
{
	return m_pOperator->GetMsgMap(MsgID);
}
int CWebSocketProxyConnection::Update(int ProcessPacketLimit)
{
	return 0;
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
			strlen(szContent), szContent);
		break;
	default:
		sprintf_s(Buff, 4096, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s",
			strlen(szContent), szContent);
		break;
	}
	m_pOperator->SendData(Buff, strlen(Buff));
	if (Code != 101)
	{
		m_pOperator->Disconnect(1000);
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
		m_pOperator->Disconnect(0);
		PrintDOSLog(_T("DOSLib"), _T("对象代理(%d)拼包缓冲溢出，连接断开！"), GetID());
	}
}

void CWebSocketProxyConnection::OnWebSocketFrame(BYTE OPCode, bool IsFinFrame, bool HaveMask, BYTE * MaskKey, BYTE * pFrameData, UINT FrameLen, UINT DataPos)
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
		if (DataLen)
		{
			m_pOperator->SendInSideMsg(0, 0, pData, DataLen);
		}
	}
	break;
	case WEB_SOCKET_OP_CODE_CLOSE:
	{
		SendWebSocketCloseMsg();
		m_pOperator->Disconnect(1000);
	}
	case WEB_SOCKET_OP_CODE_KEEP_ALIVE_PING:
	{
		m_pOperator->ResetKeepAlive();
		*pFrameData = ((*pFrameData) & 0x0F) | WEB_SOCKET_OP_CODE_KEEP_ALIVE_PONG;
		m_pOperator->SendData(pFrameData, FrameLen);
	}
	break;
	case WEB_SOCKET_OP_CODE_KEEP_ALIVE_PONG:
	{
		m_pOperator->ResetKeepAlive();
	}
	break;
	default:
		PrintDOSLog(_T("DOSLib"), _T("对象代理(%d)未知的WebSocket的OPCode(%d)，连接断开！"), GetID(), OPCode);
		m_pOperator->Disconnect(0);
	}
}

void CWebSocketProxyConnection::SendWebSocketCloseMsg()
{
	static BYTE CloseFrame[] = { WEB_SOCKET_OP_CODE_CLOSE | 0x80, 0 };
	if (m_WebSocketStatus == WEB_SOCKET_STATUS_ACCEPTED)
		m_pOperator->SendData(CloseFrame, sizeof(CloseFrame));
}
void CWebSocketProxyConnection::SendWebSocketPingMsg()
{
	static BYTE PingFrame[] = { WEB_SOCKET_OP_CODE_KEEP_ALIVE_PING | 0x80, 0 };
	if (m_WebSocketStatus == WEB_SOCKET_STATUS_ACCEPTED)
		m_pOperator->SendData(PingFrame, sizeof(PingFrame));
}



bool CWebSocketProxyConnection::SendWebSocketFrame(WEB_SOCKET_OP_CODE OPCode, BYTE * pData, UINT DataLen)
{
	BYTE FrameHeader[10];
	UINT HeaderSize = 2;
	FrameHeader[0] = OPCode | 0x80;
	if (DataLen < 126)
	{
		FrameHeader[1] = DataLen;
	}
	else if (DataLen < 0x10000)
	{
		FrameHeader[1] = 126;
		WORD Len = htons(DataLen);
		FrameHeader[2] = Len & 0xFF;
		FrameHeader[3] = (Len >> 8) & 0xFF;
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
	m_pOperator->SendData(FrameHeader, HeaderSize);
	if (pData&&DataLen)
		m_pOperator->SendData(pData, DataLen);
	return TRUE;
}