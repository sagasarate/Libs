/****************************************************************************/
/*                                                                          */
/*      文件名:    NetServerIOCP.h                                          */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CNetService;

class CNetServer :
	public CBaseNetServer
{
protected:
	HANDLE									m_hIOCP;

	CThreadSafeIDStorage<COverLappedObject>	m_OverLappedObjectPool;

	CThreadSafeIDStorage<CIOCPEventRouter>	m_EventRouterPool;
	
	CIOCPThread *							m_pIOCPThreads;
	int										m_IOCPThreadCount;
	int										m_IOCPThreadNumPerCPU;
	int										m_IOCPObjectPoolSize;
	int										m_IOCPObjectPoolGrowSize;
	int										m_IOCPObjectPoolGrowLimit;
	int										m_EventRouterPoolSize;
	int										m_EventRouterPoolGrowSize;
	int										m_EventRouterPoolGrowLimit;

	DECLARE_CLASS_INFO_STATIC(CNetServer);
public:
	CNetServer(void);
	virtual ~CNetServer(void);

	virtual BOOL StartUp(int EventObjectPoolSize=DEFAULT_EVENT_OBJECT_COUNT,
		int ThreadNumberPerCPU=DEFAULT_THREAD_NUMBER_PER_CPU,
		int EventRouterPoolSiz=DEFAULT_EVENT_ROUTER_COUNT,
		int EventObjectPoolGrowSize=DEFAULT_EVENT_OBJECT_POOL_GROW_SIZE,
		int EventObjectPoolGrowLimit=DEFAULT_EVENT_OBJECT_POOL_GROW_LIMIT,
		int EventRouterPoolGrowSize=DEFAULT_EVENT_ROUTER_POOL_GROW_SIZE,
		int EventRouterPoolGrowlimit=DEFAULT_EVENT_ROUTER_POOL_GROW_LIMIT);
	virtual void ShutDown(DWORD Milliseconds=DEFAULT_THREAD_TERMINATE_TIME);


	COverLappedObject * CreateOverLappedObject();
	BOOL DeleteOverLappedObject(COverLappedObject * pOverLappedObject);

	CIOCPEventRouter * CreateEventRouter();
	BOOL DeleteEventRouter(CIOCPEventRouter * pEventRouter);	


	BOOL BindSocket(SOCKET Socket);	
	BOOL BindFile(HANDLE FileHandle);


	void PrintObjectStatus();
protected:
	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();

	virtual BOOL OnStartUp();
	virtual void OnShutDown();
	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
};

