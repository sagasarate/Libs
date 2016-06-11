/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyNetLinkService.h                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CENLConnection;

class CEasyNetLinkService :
	public CNetService
{
protected:
	CEasyNetLinkManager*						m_pManager;
	UINT										m_ReportID;
	bool										m_NeedReallocConnectionID;
	UINT										m_RecvQueueSize;
	UINT										m_SendQueueSize;
	UINT										m_MaxPacketSize;
	CEasyNetLinkManager::DATA_COMPRESS_TYPE		m_DataCompressType;
	UINT										m_MinCompressSize;
	CEasyArray<CIPPattern>						m_IPList;

	DECLARE_CLASS_INFO_STATIC(CEasyNetLinkService);
public:
	CEasyNetLinkService(void);
	virtual ~CEasyNetLinkService(void);		

	bool Init(CEasyNetLinkManager* pManager, UINT ReportID, const CIPAddress& ListenAddress,
		bool NeedReallocConnectionID, bool IsUseListenThread,
		int ParallelAcceptCount, UINT AcceptQueueSize,
		UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize,
		CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize);


	virtual CBaseNetConnection * CreateConnection(CIPAddress& RemoteAddress);
	virtual BOOL DeleteConnection(CBaseNetConnection * pConnection);

	CEasyNetLinkManager* GetManager();

	UINT GetReportID();

	bool NeedReallocConnectionID();

	void SetIPList(const CEasyArray<CIPPattern>& IPList);

	void PrintInfo(UINT LogChannel);

	
};


inline CEasyNetLinkManager* CEasyNetLinkService::GetManager()
{
	return m_pManager;
}

inline UINT CEasyNetLinkService::GetReportID()
{
	return m_ReportID;
}

inline bool CEasyNetLinkService::NeedReallocConnectionID()
{
	return m_NeedReallocConnectionID;
}

inline void CEasyNetLinkService::SetIPList(const CEasyArray<CIPPattern>& IPList)
{
	m_IPList=IPList;
}