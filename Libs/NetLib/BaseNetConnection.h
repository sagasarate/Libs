/****************************************************************************/
/*                                                                          */
/*      文件名:    BaseTCPConnection.h                                      */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CBaseNetConnection :
	public CNameObject
{
protected:
	CNetSocket								m_Socket;

	DECLARE_CLASS_INFO_STATIC(CBaseNetConnection);
public:
	CBaseNetConnection(void);
	~CBaseNetConnection(void);

	CNetSocket * GetSocket();

	void SetRemoteAddress(const CIPAddress& IPAddress);
	void SetLocalAddress(const CIPAddress& IPAddress);
	CIPAddress& GetRemoteAddress();
	CIPAddress& GetLocalAddress();
	void SetAddressPair(const CIPAddressPair& AddressPair);
	CIPAddressPair& GetAddressPair();

	bool IsConnected();
	bool IsDisconnected();

	virtual bool Create(UINT RecvQueueSize, UINT SendQueueSize) = 0;
	virtual bool Create(SOCKET Socket, UINT RecvQueueSize, UINT SendQueueSize) = 0;

	virtual bool StartWork() = 0;

	virtual void OnConnection(bool IsSucceed) = 0;
	virtual void OnDisconnection()=0;

	virtual void OnRecvData(const BYTE * pData, UINT DataSize) = 0;

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT)=0;


	bool PrintConnectionLogWithTag(LPCTSTR Tag, LPCTSTR Format, ...);

	bool PrintConnectionDebugLogWithTag(LPCTSTR Tag, LPCTSTR Format, ...);
};

inline CNetSocket * CBaseNetConnection::GetSocket()
{
	return &m_Socket;
}

inline void CBaseNetConnection::SetRemoteAddress(const CIPAddress& IPAddress)
{
	m_Socket.SetRemoteAddress(IPAddress);
}
inline void CBaseNetConnection::SetLocalAddress(const CIPAddress& IPAddress)
{
	m_Socket.SetLocalAddress(IPAddress);
}
inline CIPAddress& CBaseNetConnection::GetRemoteAddress()
{
	return m_Socket.GetRemoteAddress();
}
inline CIPAddress& CBaseNetConnection::GetLocalAddress()
{
	return m_Socket.GetLocalAddress();
}
inline void CBaseNetConnection::SetAddressPair(const CIPAddressPair& AddressPair)
{
	m_Socket.SetAddressPair(AddressPair);
}
inline CIPAddressPair& CBaseNetConnection::GetAddressPair()
{
	return m_Socket.GetAddressPair();
}

inline bool CBaseNetConnection::IsConnected()
{
	return m_Socket.IsConnected();
}

inline bool CBaseNetConnection::IsDisconnected()
{
	return m_Socket.IsDisconnected();
}

inline bool CBaseNetConnection::PrintConnectionLogWithTag(LPCTSTR szFunction, LPCTSTR Format, ...)
{
	TCHAR Tag[256];
	_stprintf_s(Tag, 256, _T("%s(%u)"), szFunction, GetID());

	va_list vl;
	va_start(vl, Format);
	bool ret = CLogManager::GetInstance()->PrintLogVL(LOG_NET_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, Tag, Format, vl);
	va_end(vl);
	return ret;
}

inline bool CBaseNetConnection::PrintConnectionDebugLogWithTag(LPCTSTR szFunction, LPCTSTR Format, ...)
{
	TCHAR Tag[256];
	_stprintf_s(Tag, 256, _T("%s(%u)"), szFunction, GetID());

	va_list vl;
	va_start(vl, Format);
	bool ret = CLogManager::GetInstance()->PrintLogVL(LOG_NET_CHANNEL, ILogPrinter::LOG_LEVEL_DEBUG, Tag, Format, vl);
	va_end(vl);
	return ret;
}