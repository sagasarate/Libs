/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyNetLinkManager.h                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CEasyNetLinkManager :
	public CNameObject
{
protected:
	CNetServer *								m_pServer;

	CEasyMap<UINT,CEasyNetLinkConnection*>		m_ConnectionMap;
	CEasyArray<CEasyNetLinkConnection*>			m_ConnectionList;	
	

	CEasyMap<UINT,CEasyNetLinkService*>			m_ServiceMap;
	CEasyArray<CEasyNetLinkService*>			m_ServiceList;

	CIDStorage<int>								m_ConnectionIDPool;
	

	DECLARE_CLASS_INFO_STATIC(CEasyNetLinkManager);
public:
	CEasyNetLinkManager(void);
	virtual ~CEasyNetLinkManager(void);

	BOOL Init(CNetServer * pServer,LPCTSTR ConfigFileName);
	BOOL Init(CNetServer * pServer,xml_node& Config);
	BOOL Init(CNetServer * pServer);
	virtual void Destory();

	CEasyNetLinkService * AddService(UINT ID,UINT ReportID,const CIPAddress& ListenAddress,
		BOOL NeedReallocConnectionID,UINT MaxPacketSize=0,bool IsUseListenThread=true,
		int ParallelAcceptCount=DEFAULT_PARALLEL_ACCEPT,UINT AcceptQueueSize=DEFAULT_SERVER_ACCEPT_QUEUE,
		UINT RecvQueueSize=DEFAULT_SERVER_RECV_DATA_QUEUE,UINT SendQueueSize=DEFAULT_SERVER_SEND_DATA_QUEUE);
	BOOL AddConnection(UINT ReportID,const CIPAddress& ConnectionAddress,UINT MaxPacketSize=0,
		UINT RecvQueueSize=DEFAULT_SERVER_RECV_DATA_QUEUE,UINT SendQueueSize=DEFAULT_SERVER_SEND_DATA_QUEUE);

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	virtual CEasyNetLinkConnection * OnCreateConnection(UINT ID);
	virtual CEasyNetLinkService * OnCreateService(UINT ID);

	
	CEasyNetLinkConnection * GetConnection(UINT ID);
	LPVOID GetFirstConnectionPos();
	CEasyNetLinkConnection * GetNextConnection(LPVOID& Pos);
	CEasyNetLinkService * GetService(UINT ID);

	UINT GetConnectionCount();
	CEasyNetLinkConnection * GetConnectionByIndex(UINT Index);


	virtual CEasyNetLinkConnection * CreateAcceptConnection(UINT ReportID);
	virtual CEasyNetLinkConnection * CreateConnection(UINT ID);
	virtual BOOL AcceptConnection(CEasyNetLinkConnection * pConnection);
	virtual BOOL DeleteConnection(CEasyNetLinkConnection * pConnection);

	virtual CEasyNetLinkService * CreateLinkService(UINT ID);
	virtual BOOL DeleteLinkService(CEasyNetLinkService * pService);

	virtual void OnLinkStart(CEasyNetLinkConnection * pConnection);
	virtual void OnLinkEnd(CEasyNetLinkConnection * pConnection);


	void PrintLinkInfo(UINT LogChannel);
};
