/****************************************************************************/
/*                                                                          */
/*      文件名:    NetServiceEpoll.h                                        */
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
	public CBaseNetService,public IEpollEventHandler
{
protected:
	volatile bool								m_WantClose;
	volatile bool								m_IsEventProcessing;
	CNetServer *								m_pServer;	
	int											m_CurProtocol;	
	int											m_CurAddressFamily;
	bool										m_IPv6Only;
	CCycleQueue<CEpollEventObject>				m_RecvQueue;
	CCycleQueue<CEpollEventObject>				m_SendQueue;
	CCycleQueue<SOCKET>							m_AcceptQueue;
	CEasyCriticalSection						m_SendLock;
	int											m_AcceptQueueSize;
	int											m_RecvQueueSize;
	int											m_SendQueueSize;
	int											m_ParallelAcceptCount;
	int											m_ParallelRecvCount;
	CEpollEventRouter *							m_pEpollEventRouter;

	

	DECLARE_CLASS_INFO_STATIC(CNetService);
public:
	CNetService();
	virtual ~CNetService(void);

	

	void SetServer(CNetServer * pServer);

	CNetServer * GetServer();

	virtual bool OnEpollEvent(UINT EventID);
	

	virtual bool Create(int Protocol = IPPROTO_TCP,
		int AcceptQueueSize=DEFAULT_SERVER_ACCEPT_QUEUE,
		int RecvQueueSize=DEFAULT_SERVER_RECV_DATA_QUEUE,
		int SendQueueSize=DEFAULT_SERVER_SEND_DATA_QUEUE,
		int ParallelAcceptCount=DEFAULT_PARALLEL_ACCEPT,
		int ParallelRecvCount=DEFAULT_PARALLEL_RECV,
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
	

	bool QueryUDPSend(const CIPAddress& IPAddress,LPCVOID pData,int Size);

	virtual void OnRecvData(const CIPAddress& IPAddress, const BYTE * pData, UINT DataSize);

	

	
	
protected:	
	
	bool AcceptSocket(SOCKET Socket);

	void DoAcceptSocket();
	void DoUDPRecv();
	void DoUDPSend();
};



inline void CNetService::SetServer(CNetServer * pServer)
{
	m_pServer=pServer;
}

inline CNetServer * CNetService::GetServer()
{
	return m_pServer;
}


