/****************************************************************************/
/*                                                                          */
/*      文件名:    NetConnectionIOCP.h                                      */
/*      创建日期:  2010年02月09日                                           */
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
	public CBaseNetConnection,public IIOCPEventHandler
{
protected:	
	CNetServer*									m_pServer;	
	volatile bool								m_WantClose;
	int											m_CurAddressFamily;
	
	CCycleQueue<COverLappedObject *>			m_OverLappedObjectPool;
	CEasyCriticalSection						m_OverLappedObjectPoolLock;
	CCycleQueue<COverLappedObject *>			m_RecvDataQueue;
	CIOCPEventRouter *							m_pIOCPEventRouter;
	

	DECLARE_CLASS_INFO_STATIC(CNetConnection);
public:
	CNetConnection(void);
	virtual ~CNetConnection(void);

	virtual bool OnIOCPEvent(int EventID, COverLappedObject * pOverLappedObject);

	virtual bool Create(UINT RecvQueueSize = DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize = DEFAULT_SERVER_SEND_DATA_QUEUE);
	virtual bool Create(SOCKET Socket,
		UINT RecvQueueSize,UINT SendQueueSize);
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




protected:
	bool QueryRecv();
	COverLappedObject * AllocOverLappedObject();
	bool ReleaseOverLappedObject(COverLappedObject * pObject);

};



inline void CNetConnection::SetServer(CNetServer* pServer)
{
	m_pServer=pServer;
}

inline CNetServer* CNetConnection::GetServer()
{
	return m_pServer;
}

inline bool CNetConnection::Send(LPCVOID pData, UINT Size)
{
	return SendMulti(&pData, &Size, 1);
}
inline UINT CNetConnection::GetSendBufferFreeSize()
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);

	return m_OverLappedObjectPool.GetUsedSize()*NET_DATA_BLOCK_SIZE;
}