/****************************************************************************/
/*                                                                          */
/*      文件名:    NetPTCPConnectionIOCP.h                                  */
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

class CNetPTCPConnection :	
	public CBaseTCPConnection,public IIOCPEventHandler
{
protected:	
	CNetSocket								m_Socket;	
	CNetServer*								m_pServer;	
	CIOCPEventRouter *						m_pIOCPEventRouter;
	volatile UINT							m_SendQueryCount;

	DECLARE_CLASS_INFO_STATIC(CNetPTCPConnection);
public:
	CNetPTCPConnection(void);
	virtual ~CNetPTCPConnection(void);

	virtual BOOL OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject);

	virtual BOOL Create(UINT RecvQueueSize=DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize=DEFAULT_SERVER_SEND_DATA_QUEUE);
	virtual BOOL Create(SOCKET Socket,
		UINT RecvQueueSize=DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize=DEFAULT_SERVER_SEND_DATA_QUEUE);
	virtual void Destory();

	CNetSocket * GetSocket();

	BOOL Connect(const CIPAddress& Address,DWORD TimeOut=NO_CONNECTION_TIME_OUT);
	void Disconnect();
	BOOL IsConnected();
	BOOL IsDisconnected();

	virtual BOOL StartWork();

	virtual void OnConnection(BOOL IsSucceed);
	virtual void OnDisconnection();

	BOOL QuerySend(LPCVOID pData,int Size);
	BOOL QueryRecv();

	virtual void OnRecvData(const CEasyBuffer& DataBuffer);

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	virtual UINT GetCurSendQueryCount();
	

	void SetServer(CNetServer* pServer);

	CNetServer* GetServer();		

	void SetRemoteAddress(const CIPAddress& IPAddress);
	void SetLocalAddress(const CIPAddress& IPAddress);
	CIPAddress& GetRemoteAddress();
	CIPAddress& GetLocalAddress();
	void SetAddressPair(const CIPAddressPair& AddressPair);
	CIPAddressPair& GetAddressPair();

};

inline CNetSocket * CNetPTCPConnection::GetSocket()
{
	return &m_Socket;
}


inline void CNetPTCPConnection::SetServer(CNetServer* pServer)
{
	m_pServer=pServer;
}

inline CNetServer* CNetPTCPConnection::GetServer()
{
	return m_pServer;
}

inline void CNetPTCPConnection::SetRemoteAddress(const CIPAddress& IPAddress)
{
	m_Socket.SetRemoteAddress(IPAddress);
}
inline void CNetPTCPConnection::SetLocalAddress(const CIPAddress& IPAddress)
{
	m_Socket.SetLocalAddress(IPAddress);
}
inline CIPAddress& CNetPTCPConnection::GetRemoteAddress()
{
	return m_Socket.GetRemoteAddress();
}
inline CIPAddress& CNetPTCPConnection::GetLocalAddress()
{
	return m_Socket.GetLocalAddress();
}
inline void CNetPTCPConnection::SetAddressPair(const CIPAddressPair& AddressPair)
{
	m_Socket.SetAddressPair(AddressPair);
}
inline CIPAddressPair& CNetPTCPConnection::GetAddressPair()
{
	return m_Socket.GetAddressPair();
}

