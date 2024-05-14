/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyNetLink.h                                  */
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




class CEasyNetLink :
	public CNameObject
{
protected:
	CEasyNetLinkManager*						m_pManager;
	CENLBaseConnection *						m_pConnection;
	UINT										m_Status;
	UINT										m_ReportID;	
	bool										m_NeedReallocConnectionID;	
	
	


	DECLARE_CLASS_INFO_STATIC(CEasyNetLink);
public:
	CEasyNetLink(void);
	virtual ~CEasyNetLink(void);

	bool Init(CEasyNetLinkManager * pManager, const CIPAddress& ConnectionAddress, UINT ReportID,
		UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail);

	bool Init(CEasyNetLinkManager * pManager, UINT ReportID, UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail, bool NeedReallocConnectionID);

	bool Init(CEasyNetLink * pLink);

	
	CEasyNetLinkManager* GetManager();


	void SetStatus(UINT Status);
	UINT GetStatus();

	UINT GetReportID();

	virtual void OnLinkStart();
	virtual void OnLinkEnd();
	virtual void OnData(const BYTE * pData, UINT DataSize) = 0;

	virtual bool SendData(LPCVOID pData, UINT DataSize);
	

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	

	//void EnableReallocConnectionID(BOOL Enable);
	bool NeedReallocConnectionID();

	CBaseNetConnection * GetConnection();

	void Disconnect();
	bool IsDisconnected();
	bool IsConnected();
	ENL_ACTIVE_TYPE GetActiveType();

	void PrintInfo(UINT LogChannel);
protected:
	virtual CENLBaseConnection * NewConnection(UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail);
	virtual CENLBaseConnection * NewConnection(const CIPAddress& ConnectionAddress, UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize,
		CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail);
};


inline CEasyNetLinkManager* CEasyNetLink::GetManager()
{
	return m_pManager;
}

inline void CEasyNetLink::SetStatus(UINT Status)
{
	m_Status=Status;
}

inline UINT CEasyNetLink::GetStatus()
{
	return m_Status;
}

inline UINT CEasyNetLink::GetReportID()
{
	return m_ReportID;
}

inline bool CEasyNetLink::NeedReallocConnectionID()
{
	return m_NeedReallocConnectionID;
}

inline CBaseNetConnection * CEasyNetLink::GetConnection()
{
	return m_pConnection->GetBaseConnection();
}

inline void CEasyNetLink::Disconnect()
{
	if (m_pConnection)
		m_pConnection->Disconnect();
}
inline bool CEasyNetLink::IsDisconnected()
{
	if (m_pConnection)
		return m_pConnection->IsDisconnected() != FALSE;
	return true;
}
inline bool CEasyNetLink::IsConnected()
{
	if (m_pConnection)
		return m_pConnection->IsConnected() != FALSE;
	return false;
}

inline ENL_ACTIVE_TYPE CEasyNetLink::GetActiveType()
{
	if (m_pConnection)
		return m_pConnection->GetActiveType();
	return ENL_ACTIVE_TYPE_PASSIVE;
}