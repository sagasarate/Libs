/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSProxyConnection.h                                     */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CDOSObjectProxyService;

class CDOSProxyConnection :
	public CNetConnection
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

	OBJECT_ID									m_ObjectID;
	CDOSObjectProxyService						*m_pService;

	CEasyBuffer									m_AssembleBuffer;		
	CThreadSafeIDStorage<CDOSMessagePacket *>	m_MsgQueue;
	CStaticMap<MSG_ID_TYPE,OBJECT_ID>			m_MessageMap;

	UINT										m_KeepAliveCount;
	UINT										m_MaxKeepAliveCount;
	UINT										m_KeepAliveTime;
	CEasyTimer									m_KeepAliveTimer;
	CEasyTimer									m_UnacceptConnectionKeepTimer;
	bool										m_UseServerInitiativeKeepAlive;
	
	bool										m_NeedDelayClose;
	CEasyTimer									m_DelayCloseTimer;

	static CEasyBuffer							m_CompressBuffer;

	bool										m_UseWebSocketProtocol;
	WEB_SOCKET_STATUS							m_WebSocketStatus;

	DECLARE_CLASS_INFO_STATIC(CDOSProxyConnection);
public:
	CDOSProxyConnection(void);
	virtual ~CDOSProxyConnection(void);

	BOOL Init(CDOSObjectProxyService * pService,UINT ID);

	OBJECT_ID GetObjectID();

	virtual void Destory();

	virtual void OnConnection(BOOL IsSucceed);
	virtual void OnDisconnection();

	virtual void OnRecvData(const BYTE * pData, UINT DataSize);
	void OnMsg(CDOSSimpleMessage * pMessage);

	BOOL PushMessage(CDOSMessagePacket * pPacket);

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
protected:
	CDOSServer * GetServer();	
	BOOL OnSystemMessage(CDOSMessagePacket * pPacket);
	BOOL SendInSideMsg(MSG_ID_TYPE MsgID,LPVOID pData=NULL,UINT DataSize=0);	
	BOOL SendOutSideMsg(CDOSMessagePacket * pPacket);
	BOOL SendDisconnectNotify();

	OBJECT_ID GetMsgMapObjectID(MSG_ID_TYPE MsgID);

	BOOL RegisterMsgMap(MSG_ID_TYPE MsgID,OBJECT_ID ObjectID);
	BOOL UnregisterMsgMap(MSG_ID_TYPE MsgID,OBJECT_ID ObjectID);

	void ClearMsgMapByRouterID(UINT RouterID);

	CDOSSimpleMessage * CompressMsg(CDOSSimpleMessage * pMsg);

	void ProcessHTTPMsg(const BYTE * pData, UINT DataSize);
	void OnHTTPRequest(const char * szRequest);
	void SendHTTPRespond(int Code, LPCSTR szContent);
	void ParseStringLines(char * szStr, CEasyArray<char *>& StrLines);
	void ProcessWebSocketData(const BYTE * pData, UINT DataSize);
	void OnWebSocketFrame(BYTE OPCode, bool IsFinFrame, bool HaveMask, BYTE * MaskKey, BYTE * pFrameData, UINT FrameLen, UINT DataPos);
	void SendWebSocketCloseMsg();
	void SendWebSocketPingMsg();
	BOOL SendMsgByWebSocket(CDOSSimpleMessage * pSimpleMessage);
	BOOL SendWebSocketFrame(WEB_SOCKET_OP_CODE OPCode, BYTE * pData, UINT DataLen);
};

inline OBJECT_ID CDOSProxyConnection::GetObjectID()
{
	return m_ObjectID;
}