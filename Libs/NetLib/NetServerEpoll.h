/****************************************************************************/
/*                                                                          */
/*      文件名:    NetServerEpoll.h                                         */
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
	//CThreadSafeIDStorage<CEpollEventObject>	m_EpollEventObjectPool;

	CThreadSafeIDStorage<CEpollEventRouter>	m_EventRouterPool;
	
	CEasyArray<CEpollThread>				m_EpollThreadList;
	int										m_EpollWordThreadCount;
	int										m_EventObjectPoolSize;
	int										m_EventObjectPoolGrowSize;
	int										m_EventObjectPoolGrowLimit;
	int										m_EventRouterPoolSize;
	int										m_EventRouterPoolGrowSize;
	int										m_EventRouterPoolGrowLimit;

	DECLARE_CLASS_INFO_STATIC(CNetServer);
public:
	CNetServer(void);
	virtual ~CNetServer(void);

	virtual bool StartUp(int EventObjectPoolSize = DEFAULT_EVENT_OBJECT_COUNT,
		int WorkThreadCount = DEFAULT_WORK_THREAD_COUNT,
		int EventRouterPoolSiz=DEFAULT_EVENT_ROUTER_COUNT,
		int EventObjectPoolGrowSize=DEFAULT_EVENT_OBJECT_POOL_GROW_SIZE,
		int EventObjectPoolGrowLimit=DEFAULT_EVENT_OBJECT_POOL_GROW_LIMIT,
		int EventRouterPoolGrowSize=DEFAULT_EVENT_ROUTER_POOL_GROW_SIZE,
		int EventRouterPoolGrowlimit=DEFAULT_EVENT_ROUTER_POOL_GROW_LIMIT);
	virtual void ShutDown(DWORD Milliseconds=DEFAULT_THREAD_TERMINATE_TIME);


	//CEpollEventObject * CreateEventObject();
	//bool DeleteEventObject(CEpollEventObject * pEpollEventObject);

	CEpollEventRouter * CreateEventRouter();
	CEpollEventRouter * GetEventRouter(UINT ID);
	bool DeleteEventRouter(CEpollEventRouter * pEventRouter);


	bool BindSocket(SOCKET Socket,CEpollEventRouter * pEpollEventRouter);	
	//bool ModifySendEvent(SOCKET Socket, CEpollEventRouter * pEpollEventRouter, bool IsSet);
	bool UnbindSocket(SOCKET Socket, CEpollEventRouter * pEpollEventRouter);


	void PrintObjectStatus();
protected:
	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();

	virtual bool OnStartUp();
	virtual void OnShutDown();
	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
};

