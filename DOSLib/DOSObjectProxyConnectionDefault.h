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
	CEasyBuffer *								m_pEncyptBuffer;
	char *										m_pLZOCompressWorkBuffer;

	UINT										m_RecvCount;
	UINT										m_RecvFlow;
	CEasyTimer									m_RecvProtectCheckTimer;
	UINT										m_TotalMsgSendCount;
	UINT										m_TotalMsgRecvCount;

	UINT										m_ReleaseTime;

	volatile UINT64								m_BroadcastMask;
	CThreadSafeStaticMap<UINT64, UINT64>		m_BroadcastGroupList;

public:
	CDOSObjectProxyConnectionDefault(void);
	virtual ~CDOSObjectProxyConnectionDefault(void);

	virtual void Destory();
	bool PushMessage(CDOSMessagePacket * pPacket);



	bool Init(CDOSObjectProxyServiceDefault * pService, const CLIENT_PROXY_CONFIG& Config, UINT ID);


	virtual void OnRecvData(const BYTE * pData, UINT DataSize) override;
	virtual void OnConnection(bool IsSucceed) override;
	virtual void OnDisconnection() override;
	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT) override;

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
	void SetEncyptBuffer(CEasyBuffer * pEncyptBuffer)
	{
		m_pEncyptBuffer = pEncyptBuffer;
	}
	void SetGroup(CDOSObjectProxyConnectionGroup * pGroup)
	{
		m_pGroup = pGroup;
	}
	UINT GetReleaseTime()
	{
		return m_ReleaseTime;
	}
	void SetBroadcastMask(UINT64 Mask)
	{
		m_BroadcastMask = Mask;
	}
	void AddBroadcastMask(UINT64 Mask)
	{
		AtomicOr(&m_BroadcastMask, Mask);
	}
	void RemoveBroadcastMask(UINT64 Mask)
	{
		AtomicAnd(&m_BroadcastMask, ~Mask);
	}
	UINT64 GetBroadcastMask()
	{
		return m_BroadcastMask;
	}
	bool AddBroadcastGroup(UINT64 GroupID)
	{
		return m_BroadcastGroupList.Insert(GroupID, GroupID) != 0;
	}
	bool RemoveBroadcastGroup(UINT64 GroupID)
	{
		if (GroupID == (UINT64)(-1))
		{
			m_BroadcastGroupList.Clear();
			return true;
		}
		else
		{
			return m_BroadcastGroupList.Delete(GroupID);
		}		
	}
	bool IsInBroadcastGroup(UINT64 GroupID)
	{
		return m_BroadcastGroupList.Find(GroupID) != NULL;
	}
protected:

	CDOSServer * GetServer();
	void ProcessClientMsg(MSG_ID_TYPE MsgID, WORD MsgFlag, WORD CRC, const void* pData, MSG_LEN_TYPE DataLen);
	void OnClientMsg(MSG_ID_TYPE MsgID, WORD MsgFlag, WORD CRC, const void* pData, MSG_LEN_TYPE DataLen);
	void OnClientSystemMsg(MSG_ID_TYPE MsgID, WORD MsgFlag, WORD CRC, const void* pData, MSG_LEN_TYPE DataLen);

	virtual bool OnMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, const void * pData, MSG_LEN_TYPE DataLen);
	bool OnSystemMessage(const CDOSMessagePacket * pPacket);
	
	bool SendDisconnectNotify();
	virtual void OnKeepAliveMsg(UINT Timestamp, UINT RecentDelay);
	virtual void OnKeepAliveAckMsg(UINT Timestamp);
	virtual void SendKeepAliveMsg();
	virtual void SendProtocolOption();

	bool DoRegisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID);
	bool DoUnregisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID);
	void ClearMsgMapByRouterID(UINT RouterID);

	const void * CompressMsg(const void * pData, MSG_LEN_TYPE& DataLen);
	const void * EncyptMsg(const void * pData, MSG_LEN_TYPE& DataLen, WORD& CRC, UINT MsgSequence);
	void * DecyptMsg(void * pData, MSG_LEN_TYPE& DataLen, WORD CRC, UINT MsgSequence);
	WORD MakeCRC(const void * pData, UINT DataLen, const void * pKey, UINT KeyLen, UINT MsgSequence);
	void LogConnection(LPCTSTR szFunction, LPCTSTR Format, ...);
	void LogDebugConnection(LPCTSTR szFunction, LPCTSTR Format, ...);
};

inline void CDOSObjectProxyConnectionDefault::LogConnection(LPCTSTR szFunction, LPCTSTR Format, ...)
{
	va_list vl;

	TCHAR Tag[256];
	_stprintf_s(Tag, 256, _T("%s(%u)"), szFunction, GetID());

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(LOG_DOS_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, Tag, Format, vl);
	va_end(vl);
}

inline void CDOSObjectProxyConnectionDefault::LogDebugConnection(LPCTSTR szFunction, LPCTSTR Format, ...)
{
	va_list vl;

	TCHAR Tag[256];
	_stprintf_s(Tag, 256, _T("%s(%u)"), szFunction, GetID());

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(LOG_DOS_CHANNEL, ILogPrinter::LOG_LEVEL_DEBUG, Tag, Format, vl);
	va_end(vl);
}