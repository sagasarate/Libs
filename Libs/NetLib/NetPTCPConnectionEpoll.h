/****************************************************************************/
/*                                                                          */
/*      文件名:    NetPTCPConnectionEpoll.h                                 */
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

class CNetPTCPConnection :
	public CBaseTCPConnection,public IEpollEventHandler
{
protected:
	CNetServer*									m_pServer;
	CThreadSafeIDStorage<CEpollEventObject *>	m_SendQueue;
	CEpollEventRouter *							m_pEpollEventRouter;

	CEasyCriticalSection						m_RecvLock;
	CEasyCriticalSection						m_SendLock;

	DECLARE_CLASS_INFO_STATIC(CNetPTCPConnection);
public:
	CNetPTCPConnection(void);
	virtual ~CNetPTCPConnection(void);

	virtual BOOL OnEpollEvent(UINT EventID);

	virtual BOOL Create(UINT RecvQueueSize=DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize=DEFAULT_SERVER_SEND_DATA_QUEUE);
	virtual BOOL Create(SOCKET Socket,
		UINT RecvQueueSize=DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize=DEFAULT_SERVER_SEND_DATA_QUEUE);
	virtual void Destory();



	BOOL Connect(const CIPAddress& Address,DWORD TimeOut=NO_CONNECTION_TIME_OUT);
	void Disconnect();


	BOOL StartWork();

	virtual void OnConnection(BOOL IsSucceed);
	virtual void OnDisconnection();

	BOOL QuerySend(LPCVOID pData, UINT Size);


	virtual void OnRecvData(const CEasyBuffer& DataBuffer);

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);


	void SetServer(CNetServer* pServer);

	CNetServer* GetServer();



protected:
	void DoRecv();
	void DoSend();
};



inline void CNetPTCPConnection::SetServer(CNetServer* pServer)
{
	m_pServer=pServer;
}

inline CNetServer* CNetPTCPConnection::GetServer()
{
	return m_pServer;
}





