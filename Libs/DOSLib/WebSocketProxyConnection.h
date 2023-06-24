#pragma once

class CWebSocketProxyConnection :
	public CDOSObjectProxyConnectionDefault
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

#pragma pack(push)

#pragma pack(1)
	struct WS_MESSAGE_HEAD
	{
		MSG_ID_TYPE		MsgID;
		WORD			MsgFlag;
		WORD			CRC;
	};
#pragma pack(pop)
	
	WEB_SOCKET_STATUS							m_WebSocketStatus;

public:
	CWebSocketProxyConnection();
	~CWebSocketProxyConnection();
	

	virtual void OnConnection(bool IsSucceed) override;
	virtual bool OnMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, const void* pData, MSG_LEN_TYPE DataLen) override;
	virtual void OnRecvData(const BYTE * pData, UINT DataSize) override;
protected:
	virtual void OnKeepAliveMsg(UINT Timestamp, UINT RecentDelay) override;
	virtual void OnKeepAliveAckMsg(UINT Timestamp) override;
	virtual void SendKeepAliveMsg() override;
	virtual void SendProtocolOption() override;

	void ProcessHTTPMsg(const BYTE* pData, UINT DataSize);
	void OnHTTPRequest(const char * szRequest);
	void SendHTTPRespond(int Code, LPCSTR szContent);
	void ParseStringLines(char * szStr, CEasyArray<char *>& StrLines);
	void ProcessWebSocketData(const BYTE * pData, UINT DataSize);
	void OnWebSocketFrame(BYTE OPCode, bool IsFinFrame, bool HaveMask, BYTE * MaskKey, BYTE* pData, UINT DataLen);
	void SendWebSocketCloseMsg();
	void SendWebSocketPingMsg();
	void SendWebSocketPongMsg();
	bool SendWebSocketFrame(WEB_SOCKET_OP_CODE OPCode, MSG_ID_TYPE MsgID, WORD MsgFlag, WORD CRC, const void* pData, UINT DataLen);
};


