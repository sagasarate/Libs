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

class CEasyNetLink;
class CEasyNetLinkService;

class CEasyNetLinkManager :
	public CNameObject
{
public:
	enum DATA_COMPRESS_TYPE
	{
		DATA_COMPRESS_TYPE_NONE,
		DATA_COMPRESS_TYPE_LZO,
		DATA_COMPRESS_TYPE_MAX,
	};

	const static UINT DEFAULT_MIN_COMPRESS_SIZE = 1024;

	struct ENL_SERVICE
	{
		UINT					ServiceID;
		UINT					ReportID;
		bool					NeedReallocConnectionID;
		CIPAddress				ListenAddress;
		UINT					MaxPacketSize;
		bool					IsUseListenThread;
		int						ParallelAcceptCount;
		UINT					AcceptQueueSize;
		UINT					RecvQueueSize;
		UINT					SendQueueSize;
		DATA_COMPRESS_TYPE		DataCompressType;
		UINT					MinCompressSize;
		CEasyArray<CIPPattern>	IPPatternList;

		ENL_SERVICE()
		{
			ServiceID = 0;
			ReportID = 0;
			NeedReallocConnectionID = false;
			MaxPacketSize = NET_DATA_BLOCK_SIZE;
			IsUseListenThread = true;
			ParallelAcceptCount = DEFAULT_PARALLEL_ACCEPT;
			AcceptQueueSize = DEFAULT_SERVER_ACCEPT_QUEUE;
			RecvQueueSize = DEFAULT_SERVER_RECV_DATA_QUEUE;
			SendQueueSize = DEFAULT_SERVER_SEND_DATA_QUEUE;
			DataCompressType = DATA_COMPRESS_TYPE_NONE;
			MinCompressSize = DEFAULT_MIN_COMPRESS_SIZE;
		}
	};

	struct ENL_SERVICE_CONFIG
	{
		CEasyArray<ENL_SERVICE>		ServiceList;
	};

	struct ENL_CONNECTION
	{
		UINT				ReportID;
		CIPAddress			Address;
		UINT				MaxPacketSize;
		UINT				RecvQueueSize;
		UINT				SendQueueSize;
		DATA_COMPRESS_TYPE	DataCompressType;
		UINT				MinCompressSize;

		ENL_CONNECTION()
		{
			ReportID = 0;
			MaxPacketSize = NET_DATA_BLOCK_SIZE;
			RecvQueueSize = DEFAULT_SERVER_RECV_DATA_QUEUE;
			SendQueueSize = DEFAULT_SERVER_SEND_DATA_QUEUE;
			DataCompressType = DATA_COMPRESS_TYPE_NONE;
			MinCompressSize = DEFAULT_MIN_COMPRESS_SIZE;
		}
	};

	struct ENL_CONNECTION_CONFIG
	{
		CEasyArray<ENL_CONNECTION>	ConnectionList;
	};

	struct ENL_CONFIG
	{
		UINT					ServerID;
		UINT					ReallocIDRange;
		ENL_SERVICE_CONFIG		ServiceConfig;
		ENL_CONNECTION_CONFIG	ConnectionConfig;
		ENL_CONFIG()
		{
			ServerID = 0;
			ReallocIDRange = 2048;
		}
	};

	
protected:
	CNetServer *								m_pServer;

	CStaticMap<UINT,CEasyNetLink*>				m_LinkMap;
	CEasyArray<CEasyNetLink*>					m_LinkList;	
	

	CStaticMap<UINT, CEasyNetLinkService*>		m_ServiceMap;
	CEasyArray<CEasyNetLinkService*>			m_ServiceList;

	CIDStorage<int>								m_LinkIDPool;
	

	DECLARE_CLASS_INFO_STATIC(CEasyNetLinkManager);
public:
	CEasyNetLinkManager(void);
	virtual ~CEasyNetLinkManager(void);

	bool Init(CNetServer * pServer, LPCTSTR ConfigFileName);
	bool Init(CNetServer * pServer, xml_node& Config);
	bool Init(CNetServer * pServer, const ENL_CONFIG& Config);
	bool Init(CNetServer * pServer);
	static bool LoadConfig(LPCTSTR ConfigFileName, ENL_CONFIG& Config);
	static bool LoadConfig(xml_node& XmlRoot, ENL_CONFIG& Config);
	virtual void Destory();

	CEasyNetLinkService * AddService(UINT ID, UINT ReportID, const CIPAddress& ListenAddress,
		bool NeedReallocConnectionID, UINT MaxPacketSize = 0, bool IsUseListenThread = true,
		int ParallelAcceptCount = DEFAULT_PARALLEL_ACCEPT, UINT AcceptQueueSize = DEFAULT_SERVER_ACCEPT_QUEUE,
		UINT RecvQueueSize = DEFAULT_SERVER_RECV_DATA_QUEUE, UINT SendQueueSize = DEFAULT_SERVER_SEND_DATA_QUEUE,
		DATA_COMPRESS_TYPE DataCompressType = DATA_COMPRESS_TYPE_NONE, UINT MinCompressSize = DEFAULT_MIN_COMPRESS_SIZE);
	bool AddLink(UINT ReportID, const CIPAddress& ConnectionAddress, UINT MaxPacketSize = 0,
		UINT RecvQueueSize=DEFAULT_SERVER_RECV_DATA_QUEUE,UINT SendQueueSize=DEFAULT_SERVER_SEND_DATA_QUEUE,
		DATA_COMPRESS_TYPE DataCompressType = DATA_COMPRESS_TYPE_NONE, UINT MinCompressSize = DEFAULT_MIN_COMPRESS_SIZE);

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	virtual CEasyNetLink * OnCreateLink(UINT ID) = 0;
	virtual CEasyNetLinkService * OnCreateService(UINT ID);

	
	CEasyNetLink * GetLink(UINT ID);
	LPVOID GetFirstLinkPos();
	CEasyNetLink * GetNextLink(LPVOID& Pos);
	CEasyNetLinkService * GetService(UINT ID);

	UINT GetLinkCount();
	CEasyNetLink * GetLinkByIndex(UINT Index);


	virtual CEasyNetLink * CreateAcceptLink();
	virtual CEasyNetLink * CreateLink(UINT ID);
	virtual bool AcceptLink(UINT LinkID, CEasyNetLink * pLink);
	virtual void DeleteLink(CEasyNetLink * pLink);

	virtual CEasyNetLinkService * CreateLinkService(UINT ID);
	virtual bool DeleteLinkService(CEasyNetLinkService * pService);

	virtual void OnLinkStart(CEasyNetLink * pLink);
	virtual void OnLinkEnd(CEasyNetLink * pLink);

	CNetServer * GetServer();

	void PrintLinkInfo(UINT LogChannel);
};

inline CNetServer * CEasyNetLinkManager::GetServer()
{
	return m_pServer;
}