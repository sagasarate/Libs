/****************************************************************************/
/*                                                                          */
/*      文件名:    NetSocket.h                                              */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



class CNetSocket :
	public CNameObject
{
public:
	enum SOCKET_STATE
	{
		SS_UNINITED,
		SS_UNUSED,
		SS_ACCEPTED,
		SS_CONNECTING,
		SS_CONNECTED,
		SS_LISTENING,
		SS_DISCONNECTED
	};	
protected:		
	volatile SOCKET				m_Socket;
	volatile SOCKET_STATE		m_State;
	BOOL						m_IsBlocking;
	CEasyTimer					m_ConnectTimer;
	CIPAddressPair				m_IPAddressPair;

	DECLARE_CLASS_INFO_STATIC(CNetSocket);
public:	

	CNetSocket(void);
	CNetSocket(SOCKET Socket);
	virtual ~CNetSocket(void);

	BOOL	MakeSocket( int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP );
#ifdef WIN32
	int		GetProtocol();
#endif

	BOOL	SetSocket( SOCKET Socket );
	BOOL	SetSocket( CNetSocket & Socket );
	BOOL	StealSocket( CNetSocket & Socket );

	int		SetOption(int Level,int Option,char * pValue,socklen_t ValueLen);
	int		GetOption(int Level,int Option,char * pValue,socklen_t& ValueLen);

	bool	SetRecvBufferSize(int Size);
	bool	SetSendBufferSize(int Size);
	int		GetRecvBufferSize();
	int		GetSendBufferSize();

	BOOL	Connect(const CIPAddress& Address);
	BOOL	ConnectNoBlocking(const  CIPAddress& Address ,DWORD dwTimeOut);
	BOOL	Connected();
	BOOL	IsConnected();
	BOOL	IsDisconnected();

	BOOL	Bind(const CIPAddress& Address);

	BOOL	Listen(const CIPAddress& Address);
	

	BOOL	EnableBlocking(BOOL Enable);	
	BOOL	IsBlocking();

	BOOL	Select( BOOL * pbRead, BOOL * pbWrite, BOOL * pbExcept, DWORD dwTimeOut );

#ifdef WIN32
	BOOL	WMsgSelect(HWND hWnd,unsigned int wMsg,long lEvent);
	BOOL	EventSelect(WSAEVENT hEventObject,long lEvent);
#endif

	SOCKET	GetSocket();

	int GetState();
	void SetState(int State );


	void	Close();
	void	Clear();

	
	int	Send(LPVOID lpBuffer, int iSize);
	int	Recv(LPVOID lpBuffer, int iSize);
	BOOL Accept(CNetSocket & Socket );

	int SendTo(const CIPAddress& Address,LPVOID lpBuffer, int iSize);	
	int RecvFrom(CIPAddress& Address,LPVOID lpBuffer, int iSize);
	

#ifdef WIN32
	BOOL SendOverlapped( LPVOID lpData, int nDatasize, DWORD &dwBytesSent, DWORD dwFlag, LPOVERLAPPED lpOverlapped);
	BOOL RecvOverlapped( LPVOID lpDataBuf, int nBufsize, DWORD &dwBytesReceived, DWORD &dwFlag, LPOVERLAPPED lpOverlapped );
	BOOL AcceptOverlapped( SOCKET AcceptSocket, LPVOID lpDataBuf, DWORD dwRecvBufferLength, DWORD &dwBytesReceived, LPOVERLAPPED lpOverlapped);

	BOOL SendToOverlapped(const CIPAddress& Address,LPVOID lpData, int nDatasize, DWORD &dwBytesSent, DWORD dwFlag, LPOVERLAPPED lpOverlapped);
	BOOL RecvFromOverlapped(CIPAddress& Address,int& AddresLen,LPVOID lpDataBuf, int nBufsize, DWORD &dwBytesReceived, DWORD &dwFlag, LPOVERLAPPED lpOverlapped );
#endif

	void SetRemoteAddress(const CIPAddress& IPAddress);
	void SetLocalAddress(const CIPAddress& IPAddress);
	CIPAddress& GetRemoteAddress();
	CIPAddress& GetLocalAddress();
	void SetAddressPair(const CIPAddressPair& AddressPair);
	CIPAddressPair& GetAddressPair();

#ifdef WIN32
	static bool CNetSocket::NetStartup();
	static void CNetSocket::NetCleanup();
#endif

	virtual bool StealFrom(CNameObject * pObject,UINT Param=0);
};

inline BOOL CNetSocket::SetSocket( CNetSocket & Socket )
{
	if( !SetSocket( Socket.GetSocket() ))
		return FALSE; 
	SetState( Socket.GetState()); 
	SetAddressPair( Socket.GetAddressPair());
	m_IsBlocking=Socket.m_IsBlocking;
	m_ConnectTimer=Socket.m_ConnectTimer;
	return TRUE;
}

inline BOOL CNetSocket::StealSocket( CNetSocket & Socket )
{ 
	if( !SetSocket( Socket ) ) 
		return FALSE;
	Socket.m_Socket=INVALID_SOCKET;
	Socket.Clear();
	return TRUE;
}

inline BOOL CNetSocket::IsConnected()
{
	return GetState()==SS_CONNECTED;
}

inline BOOL CNetSocket::IsDisconnected()
{
	return (GetState()!=SS_CONNECTED)&&(GetState()!=SS_CONNECTING);
}

inline BOOL CNetSocket::IsBlocking()
{
	return m_IsBlocking;
}

inline SOCKET CNetSocket::GetSocket()
{ 
	return m_Socket;
}

inline int CNetSocket::GetState()
{ 
	return m_State;
}

inline void CNetSocket::SetState( int State)
{ 
	m_State=(SOCKET_STATE)State;
}

inline void CNetSocket::Clear()
{ 
	SetSocket( INVALID_SOCKET );
	m_IPAddressPair.Clear();
}

inline void CNetSocket::SetRemoteAddress(const CIPAddress& IPAddress)
{
	m_IPAddressPair.SetRemoteIPAddress(IPAddress);	
}
inline void CNetSocket::SetLocalAddress(const CIPAddress& IPAddress)
{
	m_IPAddressPair.SetLocalIPAddress(IPAddress);
}
inline CIPAddress& CNetSocket::GetRemoteAddress()
{
	return m_IPAddressPair.GetRemoteIPAddress();
}
inline CIPAddress& CNetSocket::GetLocalAddress()
{
	return m_IPAddressPair.GetLocalIPAddress();
}
inline void CNetSocket::SetAddressPair(const CIPAddressPair& AddressPair)
{
	m_IPAddressPair=AddressPair;
}
inline CIPAddressPair& CNetSocket::GetAddressPair()
{
	return m_IPAddressPair;
}