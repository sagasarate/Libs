/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyNetLinkConnection.h                                  */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

enum ENL_LINK_STATUS_TYPE
{
	ENL_LINK_NONE,
	ENL_LINK_ACCEPTING,
	ENL_LINK_ACCEPTED,	
	ENL_LINK_MAX,
};

class CEasyNetLinkManager;

class CEasyNetLinkConnection :
	public CNetConnection
{
protected:
	CEasyNetLinkManager*		m_pManager;
	UINT						m_Status;
	UINT						m_ReportID;
	BOOL						m_IsKeepConnect;
	CEasyBuffer					m_AssembleBuffer;
	CEasyBuffer					m_SendBuffer;
	CEasyTimer					m_KeepAliveTimer;
	int							m_KeepAliveCount;
	BOOL						m_NeedReallocConnectionID;
	CEasyNetLinkConnection *	m_pStealer;

	CEasyTimer					m_ReconnectTimer;


	DECLARE_CLASS_INFO_STATIC(CEasyNetLinkConnection);
public:
	CEasyNetLinkConnection(void);
	virtual ~CEasyNetLinkConnection(void);

	void SetMaxPacketSize(UINT MaxPacketSize);

	void SetManager(CEasyNetLinkManager* pManager);
	CEasyNetLinkManager* GetManager();

	void SetStatus(UINT Status);
	UINT GetStatus();

	void SetReportID(UINT ID);
	UINT GetReportID();

	void SetKeepConnect(BOOL IsKeep);
	BOOL IsKeepConnect();	

	

	virtual void OnLinkStart();
	virtual void OnLinkEnd();
	virtual void OnData(const CEasyBuffer& DataBuffer);

	void SendData(LPCVOID pData=NULL,int DataSize=0);
	

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	virtual void OnRecvData(const CEasyBuffer& DataBuffer);
	virtual void OnConnection(BOOL IsSucceed);
	virtual void OnDisconnection();

	void SendLinkMsg(DWORD MsgID,LPCVOID pData=NULL,UINT DataSize=0);

	void EnableReallocConnectionID(BOOL Enable);
	BOOL NeedReallocConnectionID();

	virtual bool StealFrom(CNameObject * pObject,UINT Param=0);

	void PrepareSteal(CEasyNetLinkConnection * pStealer);

	void PrintInfo(UINT LogChannel);
};

inline void CEasyNetLinkConnection::SetManager(CEasyNetLinkManager* pManager)
{
	m_pManager=pManager;
}

inline CEasyNetLinkManager* CEasyNetLinkConnection::GetManager()
{
	return m_pManager;
}

inline void CEasyNetLinkConnection::SetStatus(UINT Status)
{
	m_Status=Status;
}

inline UINT CEasyNetLinkConnection::GetStatus()
{
	return m_Status;
}

inline void CEasyNetLinkConnection::SetReportID(UINT ID)
{
	m_ReportID=ID;
}

inline UINT CEasyNetLinkConnection::GetReportID()
{
	return m_ReportID;
}

inline void CEasyNetLinkConnection::SetKeepConnect(BOOL IsKeep)
{
	m_IsKeepConnect=IsKeep;
}

inline BOOL CEasyNetLinkConnection::IsKeepConnect()
{
	return m_IsKeepConnect;
}

inline void CEasyNetLinkConnection::EnableReallocConnectionID(BOOL Enable)
{
	m_NeedReallocConnectionID=Enable;
}
inline BOOL CEasyNetLinkConnection::NeedReallocConnectionID()
{
	return m_NeedReallocConnectionID;
}
inline void CEasyNetLinkConnection::PrepareSteal(CEasyNetLinkConnection * pStealer)
{
	m_pStealer=pStealer;
}

