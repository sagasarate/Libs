/****************************************************************************/
/*                                                                          */
/*      文件名:    BaseService.h                                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CBaseService :
	public CNameObject
{
protected:
	CNetSocket								m_Socket;	

	DECLARE_CLASS_INFO_STATIC(CBaseService);
public:
	CBaseService(void);
	~CBaseService(void);

	CNetSocket * GetSocket();

	void SetRemoteAddress(const CIPAddress& IPAddress);
	void SetLocalAddress(const CIPAddress& IPAddress);
	CIPAddress& GetRemoteAddress();
	CIPAddress& GetLocalAddress();
	void SetAddressPair(const CIPAddressPair& AddressPair);
	CIPAddressPair& GetAddressPair();

	BOOL IsWorking();

	virtual void OnStartUp()=0;
	virtual void OnClose()=0;

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT)=0;

	virtual CBaseTCPConnection * CreateConnection(CIPAddress& RemoteAddress)=0;
	virtual BOOL DeleteConnection(CBaseTCPConnection * pConnection)=0;

	virtual void OnRecvData(const CIPAddress& IPAddress,const CEasyBuffer& DataBuffer)=0;
};

inline CNetSocket * CBaseService::GetSocket()
{
	return &m_Socket;
}

inline void CBaseService::SetRemoteAddress(const CIPAddress& IPAddress)
{
	m_Socket.SetRemoteAddress(IPAddress);
}
inline void CBaseService::SetLocalAddress(const CIPAddress& IPAddress)
{
	m_Socket.SetLocalAddress(IPAddress);
}
inline CIPAddress& CBaseService::GetRemoteAddress()
{
	return m_Socket.GetRemoteAddress();
}
inline CIPAddress& CBaseService::GetLocalAddress()
{
	return m_Socket.GetLocalAddress();
}
inline void CBaseService::SetAddressPair(const CIPAddressPair& AddressPair)
{
	m_Socket.SetAddressPair(AddressPair);
}
inline CIPAddressPair& CBaseService::GetAddressPair()
{
	return m_Socket.GetAddressPair();
}


inline BOOL CBaseService::IsWorking()
{
	if(m_Socket.GetState()==CNetSocket::SS_LISTENING||
		m_Socket.GetState()==CNetSocket::SS_CONNECTED)
		return TRUE;
	else
		return FALSE;
}