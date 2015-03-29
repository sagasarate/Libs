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
	public CBaseService,public IIOCPEventHandler
{
protected:
	volatile BOOL								m_WantClose;
	CNetServer *								m_pServer;	
	int											m_CurProtocol;	
	CThreadSafeIDStorage<COverLappedObject *>	m_AcceptQueue;
	int											m_AcceptQueueSize;
	int											m_RecvQueueSize;
	int											m_SendQueueSize;
	int											m_ParallelAcceptCount;
	int											m_ParallelRecvCount;
	bool										m_IsUseListenThread;
	CIOCPEventRouter *							m_pIOCPEventRouter;
	CIOCPListenThread *							m_pListenThread;

	

	DECLARE_CLASS_INFO_STATIC(CNetService);
public:
	CNetService();
	virtual ~CNetService(void);

	

	void SetServer(CNetServer * pServer);

	CNetServer * GetServer();

	virtual BOOL OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject);
	

	virtual BOOL Create(int Protocol = IPPROTO_TCP,
		int AcceptQueueSize=DEFAULT_SERVER_ACCEPT_QUEUE,
		int RecvQueueSize=DEFAULT_SERVER_RECV_DATA_QUEUE,
		int SendQueueSize=0,
		int ParallelAcceptCount=DEFAULT_PARALLEL_ACCEPT,
		int ParallelRecvCount=DEFAULT_PARALLEL_RECV,
		bool IsUseListenThread=false);
	virtual void Destory();
	
	BOOL StartListen(const CIPAddress& Address);
	void Close();
	void QueryClose();

	virtual void OnStartUp();
	virtual void OnClose();

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	virtual CBaseTCPConnection * CreateConnection(CIPAddress& RemoteAddress);
	virtual BOOL DeleteConnection(CBaseTCPConnection * pConnection);
	

	BOOL QueryUDPSend(const CIPAddress& IPAddress,LPCVOID pData,int Size);

	virtual void OnRecvData(const CIPAddress& IPAddress,const CEasyBuffer& DataBuffer);

	

	
	
protected:
	BOOL QueryAccept();
	BOOL QueryUDPRecv();
	BOOL AcceptSocket(SOCKET Socket);
	BOOL AcceptSocketEx(SOCKET Socket,CEasyBuffer * pAcceptData);
};



inline void CNetService::SetServer(CNetServer * pServer)
{
	m_pServer=pServer;
}

inline CNetServer * CNetService::GetServer()
{
	return m_pServer;
}


