/****************************************************************************/
/*                                                                          */
/*      文件名:    NetServiceIOCP.h                                         */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CNetServer;

class CNetService :	
	public CBaseNetService,public IIOCPEventHandler
{
protected:
	friend class CIOCPListenThread;

	volatile bool								m_WantClose;
	CNetServer *								m_pServer;	
	int											m_CurProtocol;
	int											m_CurAddressFamily;
	bool										m_IPv6Only;
	CCycleQueue<COverLappedObject *>			m_OverLappedObjectPool;
	CEasyCriticalSection						m_OverLappedObjectPoolLock;
	CCycleQueue<COverLappedObject *>			m_AcceptQueue;
	UINT										m_AcceptQueueSize;
	UINT										m_RecvQueueSize;
	UINT										m_SendQueueSize;
	UINT										m_ParallelAcceptCount;
	UINT										m_ParallelRecvCount;
	bool										m_IsUseListenThread;
	CIOCPEventRouter *							m_pIOCPEventRouter;
	CIOCPListenThread *							m_pListenThread;

	

	DECLARE_CLASS_INFO_STATIC(CNetService);
public:
	CNetService();
	virtual ~CNetService(void);

	

	void SetServer(CNetServer * pServer);

	CNetServer * GetServer();

	virtual bool OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject);
	

	virtual bool Create(int Protocol = IPPROTO_TCP,
		UINT AcceptQueueSize = DEFAULT_SERVER_ACCEPT_QUEUE,
		UINT RecvQueueSize = DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize = DEFAULT_SERVER_SEND_DATA_QUEUE,
		UINT ParallelAcceptCount = DEFAULT_PARALLEL_ACCEPT,
		UINT ParallelRecvCount = DEFAULT_PARALLEL_RECV,
		bool IsUseListenThread = false,
		bool IPv6Only = false);
	virtual void Destory();
	
	bool StartListen(const CIPAddress& Address);
	void Close();
	void QueryClose();

	virtual void OnStartUp();
	virtual void OnClose();

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	virtual CBaseNetConnection * CreateConnection(CIPAddress& RemoteAddress);
	virtual bool DeleteConnection(CBaseNetConnection * pConnection);
	

	bool QueryUDPSend(const CIPAddress& IPAddress, LPCVOID pData, int Size);

	virtual void OnRecvData(const CIPAddress& IPAddress, const BYTE * pData, UINT DataSize);

	

	
	
protected:
	bool QueryAccept();
	bool QueryUDPRecv();
	bool AcceptSocket(SOCKET Socket);
	bool AcceptSocketEx(SOCKET Socket, CEasyBuffer * pAcceptData);
	COverLappedObject * AllocOverLappedObject();
	bool ReleaseOverLappedObject(COverLappedObject * pObject);
};



inline void CNetService::SetServer(CNetServer * pServer)
{
	m_pServer=pServer;
}

inline CNetServer * CNetService::GetServer()
{
	return m_pServer;
}


