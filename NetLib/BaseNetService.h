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

class CBaseNetService :
	public CNameObject
{
protected:
	CNetSocket								m_Socket;	

	DECLARE_CLASS_INFO_STATIC(CBaseNetService);
public:
	CBaseNetService(void);
	~CBaseNetService(void);

	CNetSocket * GetSocket();

	void SetRemoteAddress(const CIPAddress& IPAddress);
	void SetLocalAddress(const CIPAddress& IPAddress);
	CIPAddress& GetRemoteAddress();
	CIPAddress& GetLocalAddress();
	void SetAddressPair(const CIPAddressPair& AddressPair);
	CIPAddressPair& GetAddressPair();

	bool IsWorking();

	virtual void OnStartUp()=0;
	virtual void OnClose()=0;

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT)=0;

	virtual CBaseNetConnection * CreateConnection(CIPAddress& RemoteAddress)=0;
	virtual bool DeleteConnection(CBaseNetConnection * pConnection) = 0;

	virtual void OnRecvData(const CIPAddress& IPAddress, const BYTE * pData, UINT DataSize) = 0;
};

inline CNetSocket * CBaseNetService::GetSocket()
{
	return &m_Socket;
}

inline void CBaseNetService::SetRemoteAddress(const CIPAddress& IPAddress)
{
	m_Socket.SetRemoteAddress(IPAddress);
}
inline void CBaseNetService::SetLocalAddress(const CIPAddress& IPAddress)
{
	m_Socket.SetLocalAddress(IPAddress);
}
inline CIPAddress& CBaseNetService::GetRemoteAddress()
{
	return m_Socket.GetRemoteAddress();
}
inline CIPAddress& CBaseNetService::GetLocalAddress()
{
	return m_Socket.GetLocalAddress();
}
inline void CBaseNetService::SetAddressPair(const CIPAddressPair& AddressPair)
{
	m_Socket.SetAddressPair(AddressPair);
}
inline CIPAddressPair& CBaseNetService::GetAddressPair()
{
	return m_Socket.GetAddressPair();
}


inline bool CBaseNetService::IsWorking()
{
	if(m_Socket.GetState()==CNetSocket::SS_LISTENING||
		m_Socket.GetState()==CNetSocket::SS_CONNECTED)
		return true;
	else
		return false;
}