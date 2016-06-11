#pragma once

class CWebSocketProxyConnection :
	public IDOSObjectProxyConnection,
	public CNameObject
{
protected:

	enum WEB_SOCKET_STATUS
	{
		WEB_SOCKET_STATUS_NONE,
		WEB_SOCKET_STATUS_ACCEPTED,
	};
	enum HTTP_REQUEST_PARSE_STATUS
	{
		HRPS_NONE,
		HRPS_RETURN1,
		HRPS_NEW_LINE1,
		HRPS_RETURN2,
		HRPS_NEW_LINE2,
	};

	enum WEB_SOCKET_OP_CODE
	{
		WEB_SOCKET_OP_CODE_CONTINUOUS_DATA = 0x00,
		WEB_SOCKET_OP_CODE_TEXT_DATA = 0x01,
		WEB_SOCKET_OP_CODE_BINARY_DATA = 0x02,
		WEB_SOCKET_OP_CODE_CLOSE = 0x08,
		WEB_SOCKET_OP_CODE_KEEP_ALIVE_PING = 0x09,
		WEB_SOCKET_OP_CODE_KEEP_ALIVE_PONG = 0x0A,
	};

	IDOSObjectProxyConnectionOperator *					m_pOperator;
	CEasyBuffer									m_AssembleBuffer;	

	WEB_SOCKET_STATUS							m_WebSocketStatus;

public:
	CWebSocketProxyConnection();
	~CWebSocketProxyConnection();

	virtual bool Initialize(IDOSObjectProxyConnectionOperator * pOperator);
	virtual void Destory();
	virtual UINT AddUseRef();
	virtual void Release();

	virtual void OnConnection(BOOL IsSucceed);
	virtual void OnDisconnection();
	virtual bool OnMessage(CDOSMessage * pMessage);
	virtual bool OnSystemMessage(CDOSMessage * pMessage);
	virtual bool OnSendKeepAlive();
	virtual void OnRecvData(const BYTE * pData, UINT DataSize);
	virtual OBJECT_ID GetMsgMap(MSG_ID_TYPE MsgID);
	virtual int Update(int ProcessPacketLimit);

protected:
	void ProcessHTTPMsg(const BYTE * pData, UINT DataSize);
	void OnHTTPRequest(const char * szRequest);
	void SendHTTPRespond(int Code, LPCSTR szContent);
	void ParseStringLines(char * szStr, CEasyArray<char *>& StrLines);
	void ProcessWebSocketData(const BYTE * pData, UINT DataSize);
	void OnWebSocketFrame(BYTE OPCode, bool IsFinFrame, bool HaveMask, BYTE * MaskKey, BYTE * pFrameData, UINT FrameLen, UINT DataPos);
	void SendWebSocketCloseMsg();
	void SendWebSocketPingMsg();
	bool SendWebSocketFrame(WEB_SOCKET_OP_CODE OPCode, BYTE * pData, UINT DataLen);
};

