/****************************************************************************/
/*                                                                          */
/*      文件名:    NetConnectionEpoll.h                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#define NO_CONNECTION_TIME_OUT		0


class CNetService;

class CNetConnection :
	public CBaseNetConnection,public IEpollEventHandler
{
protected:
	CNetServer*									m_pServer;
	volatile bool								m_WantClose;
	volatile bool								m_IsEventProcessing;
	int											m_CurAddressFamily;
	bool										m_UseSafeDisconnect;
	CCycleBuffer								m_RecvQueue;
	CCycleBuffer								m_SendQueue;
	//CThreadSafeIDStorage<CEpollEventObject *>	m_RecvQueue;
	//CThreadSafeIDStorage<CEpollEventObject *>	m_SendQueue;
	CEpollEventRouter *							m_pEpollEventRouter;

	//CEasyCriticalSection						m_RecvLock;
	CEasyCriticalSection						m_SendLock;

	DECLARE_CLASS_INFO_STATIC(CNetConnection);
public:
	CNetConnection(void);
	virtual ~CNetConnection(void);

	virtual bool OnEpollEvent(UINT EventID);

	virtual bool Create(UINT RecvQueueSize = DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize=DEFAULT_SERVER_SEND_DATA_QUEUE);
	virtual bool Create(SOCKET Socket, UINT RecvQueueSize = DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize = DEFAULT_SERVER_SEND_DATA_QUEUE);
	virtual void Destory();
	void Close();


	bool Connect(const CIPAddress& Address, DWORD TimeOut = NO_CONNECTION_TIME_OUT);
	void Disconnect();
	void QueryDisconnect();


	bool StartWork();


	bool Send(LPCVOID pData, UINT Size);
	bool SendMulti(LPCVOID * pDataBuffers, const UINT * pDataSizes, UINT BufferCount);
	UINT GetSendBufferFreeSize();

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);


	void SetServer(CNetServer* pServer);

	CNetServer* GetServer();

	void EnableSafeDisconnect(bool Enable);


protected:
	void DoRecv();
	void DoSend();
	UINT TrySend(LPCVOID pData, UINT Size);
};




inline void CNetConnection::SetServer(CNetServer* pServer)
{
	m_pServer=pServer;
}

inline CNetServer* CNetConnection::GetServer()
{
	return m_pServer;
}


inline void CNetConnection::EnableSafeDisconnect(bool Enable)
{
	m_UseSafeDisconnect=Enable;
}

inline bool CNetConnection::Send(LPCVOID pData, UINT Size)
{
	return SendMulti(&pData, &Size, 1);
}
inline UINT CNetConnection::GetSendBufferFreeSize()
{
	CAutoLockEx Lock(m_SendLock);

	return m_SendQueue.GetFreeSize();
}