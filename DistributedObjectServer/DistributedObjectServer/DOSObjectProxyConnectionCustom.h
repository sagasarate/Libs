#pragma once

class CDOSObjectProxyServiceCustom;

class CDOSObjectProxyConnectionCustom :
	public IDOSObjectProxyConnectionBase,
	public IDOSObjectProxyConnectionOperator,	
	public CNameObject
{
protected:
	OBJECT_ID									m_ObjectID;
	CDOSObjectProxyServiceCustom *				m_pService;
	CThreadSafeIDStorage<CDOSMessagePacket *>	m_MsgQueue;	
	IDOSObjectProxyConnection *					m_pProxyConnection;
public:
	CDOSObjectProxyConnectionCustom();
	~CDOSObjectProxyConnectionCustom();

	//IDOSObjectProxyConnectionBase
	virtual void Destory();
	virtual UINT AddUseRef();
	virtual void Release();
	virtual bool PushMessage(CDOSMessagePacket * pPacket);

	//IDOSObjectProxyConnectionOperator
	virtual OBJECT_ID GetObjectID();
	virtual const CLIENT_PROXY_CONFIG& GetConfig();

	virtual bool SendMessage(OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);
	virtual bool SendDisconnectNotify(OBJECT_ID * pTargetObjectID, UINT TargetCount);



	BOOL Init(CDOSObjectProxyServiceCustom * pService, UINT ID, IDOSObjectProxyConnection * pProxyConnection);	

	
	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	bool IsConnected()
	{
		return m_pProxyConnection->IsConnected();
	}

	IDOSObjectProxyConnection * GetInterface()
	{
		return m_pProxyConnection;
	}	
protected:
	virtual void OnMessage(CDOSMessagePacket * pPacket);
	virtual void OnSystemMessage(CDOSMessagePacket * pPacket);	
};

