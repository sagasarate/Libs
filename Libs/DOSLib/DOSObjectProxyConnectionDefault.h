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

class CDOSObjectProxyServiceDefault;
class CDOSObjectProxyConnectionGroup;

class CDOSObjectProxyConnectionDefault :
	public CNetConnection
{
public:
	enum STATUS
	{
		STATUS_NONE,
		STATUS_INITED,
		STATUS_CONNECTED,
		STATUS_DISCONNECTED,
		STATUS_DESTORYED,
	};
protected:
	OBJECT_ID									m_ObjectID;
	CDOSObjectProxyServiceDefault *				m_pService;
	CDOSObjectProxyConnectionGroup *			m_pGroup;
	volatile STATUS								m_Status;

	CLIENT_PROXY_CONFIG							m_Config;

	CCycleQueue<CDOSMessagePacket *>			m_MsgQueue;
	CStaticMap<MSG_ID_TYPE, OBJECT_ID>			m_MessageMap;

	UINT										m_KeepAliveCount;
	UINT										m_MaxKeepAliveCount;
	UINT										m_KeepAliveTime;
	CEasyTimer									m_KeepAliveTimer;
	CEasyTimer									m_UnacceptConnectionKeepTimer;
	bool										m_UseServerInitiativeKeepAlive;
	UINT										m_RecentPingDelay;

	bool										m_NeedDelayClose;
	CEasyTimer									m_DelayCloseTimer;

	CEasyBuffer									m_AssembleBuffer;
	CEasyBuffer *								m_pCompressBuffer;
	char *										m_pLZOCompressWorkBuffer;
	

public:
	CDOSObjectProxyConnectionDefault(void);
	virtual ~CDOSObjectProxyConnectionDefault(void);

	virtual void Destory();
	virtual bool PushMessage(CDOSMessagePacket * pPacket);



	bool Init(CDOSObjectProxyServiceDefault * pService, const CLIENT_PROXY_CONFIG& Config, UINT ID);


	virtual void OnRecvData(const BYTE * pData, UINT DataSize);
	virtual void OnConnection(bool IsSucceed);
	virtual void OnDisconnection();
	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	void QueryDisconnect(UINT Delay);

	OBJECT_ID GetObjectID()
	{
		return m_ObjectID;
	}
	STATUS GetStatus()
	{
		return m_Status;
	}
	void SetCompressBuffer(CEasyBuffer * pCompressBuffer, char * pLZOCompressWorkBuffer)
	{
		m_pCompressBuffer = pCompressBuffer;
		m_pLZOCompressWorkBuffer = pLZOCompressWorkBuffer;
	}
	void SetGroup(CDOSObjectProxyConnectionGroup * pGroup)
	{
		m_pGroup = pGroup;
	}
protected:

	CDOSServer * GetServer();
	void OnClientMsg(CDOSSimpleMessage * pMessage);
	void OnClientSystemMsg(CDOSSimpleMessage * pMessage);

	bool OnMessage(const CDOSMessagePacket * pPacket);
	bool OnSystemMessage(const CDOSMessagePacket * pPacket);
	
	bool SendDisconnectNotify();
	void SendKeepAliveMsg();

	OBJECT_ID GetMsgMapObjectID(MSG_ID_TYPE MsgID);
	bool DoRegisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID);
	bool DoUnregisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID);
	void ClearMsgMapByRouterID(UINT RouterID);

	const void * CompressMsg(const void * pData, MSG_LEN_TYPE& DataLen);
	bool EncyptMsg(const void * pData, MSG_LEN_TYPE& DataLen, LPCVOID& pOut);
	MSG_LEN_TYPE DecyptMsg(void * pData, MSG_LEN_TYPE DataLen);
};

