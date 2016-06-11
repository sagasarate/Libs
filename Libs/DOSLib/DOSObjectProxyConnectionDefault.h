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

class CDOSObjectProxyConnectionDefault :
	public IDOSObjectProxyConnectionBase,
	public CNetConnection
{
protected:
	OBJECT_ID									m_ObjectID;
	CDOSObjectProxyServiceDefault *				m_pService;

	CThreadSafeIDStorage<CDOSMessagePacket *>	m_MsgQueue;
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
	

	

public:
	CDOSObjectProxyConnectionDefault(void);
	virtual ~CDOSObjectProxyConnectionDefault(void);

	virtual void Destory();
	virtual UINT AddUseRef();
	virtual void Release();
	virtual bool PushMessage(CDOSMessagePacket * pPacket);



	bool Init(CDOSObjectProxyServiceDefault * pService,UINT ID);


	virtual void OnRecvData(const BYTE * pData, UINT DataSize);
	virtual void OnConnection(BOOL IsSucceed);
	virtual void OnDisconnection();
	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	void QueryDisconnect(UINT Delay);

	OBJECT_ID GetObjectID()
	{
		return m_ObjectID;
	}
protected:

	CDOSServer * GetServer();
	void OnClientMsg(CDOSSimpleMessage * pMessage);
	void OnClientSystemMsg(CDOSSimpleMessage * pMessage);

	bool OnMessage(CDOSMessagePacket * pPacket);
	bool OnSystemMessage(CDOSMessagePacket * pPacket);
	
	bool SendDisconnectNotify();
	void SendKeepAliveMsg();

	OBJECT_ID GetMsgMapObjectID(MSG_ID_TYPE MsgID);
	bool DoRegisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID);
	bool DoUnregisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID);
	void ClearMsgMapByRouterID(UINT RouterID);

	CDOSSimpleMessage * CompressMsg(CDOSSimpleMessage * pMsg);
};

