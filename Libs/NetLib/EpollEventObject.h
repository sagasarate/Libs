/****************************************************************************/
/*                                                                          */
/*      文件名:    EpollEventObject.h                                       */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

enum EVENT_TYPE
{
	IO_NOTSET,
	IO_RECV,
	IO_SEND,
	IO_ACCEPT,	
	IO_ERROR,
	IO_CLOSE,
	
};

class CNetServer;

class CEpollEventObject 
{
protected:
	UINT						m_ID;	
	SOCKET						m_AcceptSocket;	
	CIPAddress					m_Address;
	socklen_t					m_AddressLen;
	CEasyBuffer					m_DataBuff;	
	int							m_TypeID;	
	UINT						m_ParentID;	
	UINT						m_ErrorCode;
	UINT						m_EventID;
	CNetServer *				m_pCreator;	
	
public:
	CEpollEventObject(void);
	~CEpollEventObject(void);

	void SetID(UINT ID);
	UINT GetID();

	BOOL Create(CNetServer * pCreator);
	void Destory();
	
	CEasyBuffer * GetDataBuff();	


	void SetType(int Type);
	int GetType();

	void SetAcceptSocket(SOCKET Socket);
	SOCKET GetAcceptSocket();

	void SetAddress(const CIPAddress& Address);
	CIPAddress& GetAddress();
	socklen_t& GetAddressLen();

	void SetParentID(UINT ID);
	UINT GetParentID();	

	void SetErrorCode(UINT ErrorCode);
	UINT GetErrorCode();

	void SetEventID(UINT EventID);
	UINT GetEventID();

	void Release();
};

inline void CEpollEventObject::SetID(UINT ID)
{
	m_ID=ID;
}
inline UINT CEpollEventObject::GetID()
{
	return m_ID;
}

inline CEasyBuffer * CEpollEventObject::GetDataBuff()
{
	return &m_DataBuff;
}


inline void CEpollEventObject::SetType(int Type)
{
	m_TypeID=Type;
}
inline int CEpollEventObject::GetType()
{
	return m_TypeID;
}
inline void CEpollEventObject::SetAcceptSocket(SOCKET Socket)
{
	m_AcceptSocket=Socket;
}
inline SOCKET CEpollEventObject::GetAcceptSocket()
{
	return m_AcceptSocket;
}
inline void CEpollEventObject::SetAddress(const CIPAddress& Address)
{
	m_Address=Address;
}
inline CIPAddress& CEpollEventObject::GetAddress()
{
	return m_Address;
}

inline socklen_t& CEpollEventObject::GetAddressLen()
{
	return m_AddressLen;
}

inline void CEpollEventObject::SetParentID(UINT ID)
{
	m_ParentID=ID;
}
inline UINT CEpollEventObject::GetParentID()
{
	return m_ParentID;
}

inline void CEpollEventObject::SetErrorCode(UINT ErrorCode)
{
	m_ErrorCode=ErrorCode;
}

inline UINT CEpollEventObject::GetErrorCode()
{
	return m_ErrorCode;
}

inline void CEpollEventObject::SetEventID(UINT EventID)
{
	m_EventID=EventID;
}
inline UINT CEpollEventObject::GetEventID()
{
	return m_EventID;
}