/****************************************************************************/
/*                                                                          */
/*      文件名:    NetServerIOCP.cpp                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO_STATIC(CNetServer,CBaseNetServer);

CNetServer::CNetServer(void):CBaseNetServer()
{
	CNetSocket::NetStartup();
	m_hIOCP=NULL;	
	m_pIOCPThreads=NULL;	
	m_IOCPObjectPoolSize=DEFAULT_EVENT_OBJECT_COUNT;
	m_IOCPObjectPoolGrowSize=DEFAULT_EVENT_OBJECT_POOL_GROW_SIZE;
	m_IOCPObjectPoolGrowLimit=DEFAULT_EVENT_OBJECT_POOL_GROW_LIMIT;
	m_EventRouterPoolSize=DEFAULT_EVENT_ROUTER_COUNT;
	m_EventRouterPoolGrowSize=DEFAULT_EVENT_ROUTER_POOL_GROW_SIZE;
	m_EventRouterPoolGrowLimit=DEFAULT_EVENT_ROUTER_POOL_GROW_LIMIT;
	m_IOCPWorkThreadCount = DEFAULT_WORK_THREAD_COUNT;
	m_OverLappedObjectPool.SetTag(_T("CNetServer"));
	m_EventRouterPool.SetTag(_T("CNetServer"));
}

CNetServer::~CNetServer(void)
{
	ShutDown();	
	CNetSocket::NetCleanup();
}

bool CNetServer::StartUp(int EventObjectPoolSize,
						int WorkThreadCount,
						int EventRouterPoolSiz,
						int EventObjectPoolGrowSize,
						int EventObjectPoolGrowLimit,
						int EventRouterPoolGrowSize,
						int EventRouterPoolGrowlimit)
{
	m_IOCPObjectPoolSize=EventObjectPoolSize;
	m_EventRouterPoolSize=EventRouterPoolSiz;
	m_IOCPWorkThreadCount = WorkThreadCount;
	m_IOCPObjectPoolGrowSize=EventObjectPoolGrowSize;
	m_IOCPObjectPoolGrowLimit=EventObjectPoolGrowLimit;
	m_EventRouterPoolGrowSize=EventRouterPoolGrowSize;
	m_EventRouterPoolGrowLimit=EventRouterPoolGrowlimit;
	return Start();
}

void CNetServer::ShutDown(DWORD Milliseconds)
{	
	SafeTerminate(Milliseconds);	
}

bool CNetServer::OnStart()
{
	if(!CBaseNetServer::OnStart())
		return false;

	if(m_hIOCP!=NULL)
		return false;

	CNetSocket::NetStartup();

	m_OverLappedObjectPool.Create(m_IOCPObjectPoolSize,m_IOCPObjectPoolGrowSize,m_IOCPObjectPoolGrowLimit);
	m_EventRouterPool.Create(m_EventRouterPoolSize,m_EventRouterPoolGrowSize,m_EventRouterPoolGrowLimit);

	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
	if( m_hIOCP == NULL )
	{
		PrintNetLog(_T("(%d)创建完成端口失败(%d)！"),GetID(),GetLastError());		
		return false;
	}

	if (m_IOCPWorkThreadCount <= 0)
	{
		PrintNetLog(_T("非法的IOCP工作线程数量"));
		return false;
	}

	m_pIOCPThreads = MONITORED_NEW_ARRAY(_T("CNetService"), CIOCPThread, m_IOCPWorkThreadCount);

	for (int i = 0; i < m_IOCPWorkThreadCount; i++)
	{
		m_pIOCPThreads[i].SetIOCPHandle(m_hIOCP);
		m_pIOCPThreads[i].Start();
	}	
	PrintNetLog(_T("已创建%d个IOCP工作线程"), m_IOCPWorkThreadCount);
	if(!OnStartUp())
	{		
		return false;
	}
	return true;
}

bool CNetServer::OnRun()
{
	if(!CBaseNetServer::OnRun())
		return false;

	if(Update()==0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}
	return true;
}

void CNetServer::OnTerminate()
{	
	if( m_pIOCPThreads )
	{
		for (int i = 0; i < m_IOCPWorkThreadCount; i++)
		{			
			m_pIOCPThreads[i].SafeTerminate();
		}
		MONITORED_DELETE_ARRAY(m_pIOCPThreads);
		m_pIOCPThreads = NULL;
	}		

	OnShutDown();

	CloseHandle( m_hIOCP );
	m_hIOCP = NULL;	

	CAutoLock Lock(m_OverLappedObjectPoolLock);

	if(m_OverLappedObjectPool.GetObjectCount())
	{		
		PrintNetLog(_T("(%d)关闭，开始统计OverLappedObject使用状况！"),GetID());
		PrintObjectStatus();
	}

	m_OverLappedObjectPool.Destory();	
	m_EventRouterPool.Destory();

	CNetSocket::NetCleanup();
}

UINT CNetServer::AllocOverLappedObject(CCycleQueue<COverLappedObject *>& Queue, UINT AllocCount, UINT ParantID)
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);

	COverLappedObject * pOverLappedObject=NULL;
	UINT Count = 0;

	while (Count<AllocCount)
	{
		pOverLappedObject = m_OverLappedObjectPool.NewObject();
		if (pOverLappedObject)
		{
			if (pOverLappedObject->GetParentID())
			{
				PrintImportantLog(_T("分配了未释放的OverLappedObject"));
			}
			pOverLappedObject->Create(this);
			pOverLappedObject->SetParentID(ParantID);
			pOverLappedObject->SetStatus(OVERLAPPED_OBJECT_STATUS_IDLE);

			Queue.PushBack(&pOverLappedObject);
			Count++;
		}
		else
		{
			PrintNetLog(_T("(%d)无法分配COverLappedObject(%u/%u)"), GetID(),
				m_OverLappedObjectPool.GetObjectCount(), m_OverLappedObjectPool.GetBufferSize());
			break;
		}
	}

	return Count;
}
bool CNetServer::ReleaseOverLappedObject(CCycleQueue<COverLappedObject *>& Queue)
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);
	COverLappedObject * pOverLappedObject = NULL;
	while (Queue.PopFront(&pOverLappedObject))
	{
		pOverLappedObject->SetParentID(0);
		pOverLappedObject->SetStatus(OVERLAPPED_OBJECT_STATUS_FREE);
		if (!m_OverLappedObjectPool.DeleteObject(pOverLappedObject->GetID()))
		{
			PrintNetLog(_T("(%d)Server无法删除COverLappedObject(%d)！"), GetID(), pOverLappedObject->GetID());
		}
	}
	return true;
}
bool CNetServer::ReleaseOverLappedObject(UINT ID)
{		
	CAutoLock Lock(m_OverLappedObjectPoolLock);
	if (!m_OverLappedObjectPool.DeleteObject(ID))
	{
		PrintNetLog( _T("(%d)Server无法删除COverLappedObject(%d)！"), GetID(), ID);
		return false;
	}	
	
	return true;
}

CIOCPEventRouter * CNetServer::CreateEventRouter()
{

	CIOCPEventRouter * pEventRouter=NULL;
	UINT ID=0;	
	
	ID=m_EventRouterPool.NewObject(&pEventRouter);

	if(pEventRouter)
	{
		pEventRouter->SetID(ID);
		return pEventRouter;
	}
	PrintNetLog(_T("(%d)Server无法创建CIOCPEventRouter！"),GetID());
	return NULL;
}

bool CNetServer::DeleteEventRouter(CIOCPEventRouter * pEventRouter)
{

	pEventRouter->Destory();
	if(!m_EventRouterPool.DeleteObject(pEventRouter->GetID()))
	{
		PrintNetLog(_T("(%d)Server无法删除CIOCPEventRouter(%d)！"),GetID(),pEventRouter->GetID());
		return false;
	}

	return true;
}


bool CNetServer::BindSocket(SOCKET Socket)
{
	if(m_hIOCP == NULL)
	{
		PrintNetLog(_T("(%d)完成端口没有初始化,无法绑定Socket！"),GetID());		
		return false;
	}
	if(Socket == INVALID_SOCKET)
	{
		PrintNetLog(_T("(%d)Socket没有初始化,无法绑定Socket！"),GetID());		
		return false;
	}
	HANDLE hPort = CreateIoCompletionPort((HANDLE)Socket, m_hIOCP, 0, 0 );
	return true;
}

bool CNetServer::BindFile(HANDLE FileHandle)
{
	if(m_hIOCP == NULL)
	{
		PrintNetLog(_T("(%d)完成端口没有初始化,无法绑定FileHandle！"),GetID());		
		return false;
	}
	if(FileHandle == INVALID_HANDLE_VALUE)
	{
		PrintNetLog(_T("(%d)FileHandle没有初始化,无法绑定FileHandle！"),GetID());		
		return false;
	}
	HANDLE hPort = CreateIoCompletionPort(FileHandle, m_hIOCP, 0, 0 );
	return true;
}

bool CNetServer::OnStartUp()
{
	return true;
}
void CNetServer::OnShutDown()
{
	
}

int CNetServer::Update(int ProcessPacketLimit)
{
	int ProcessCount=0;	
	return ProcessCount;
}


void CNetServer::PrintObjectStatus()
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);

	int AcceptCount=0;
	int RecvCount=0;
	int SendCount=0;
	int OtherCount=0;
	
	LPVOID Pos=m_OverLappedObjectPool.GetFirstObjectPos();
	while(Pos)
	{
		COverLappedObject * pObject = m_OverLappedObjectPool.GetNextObject(Pos);
		switch(pObject->GetType())
		{
		case IO_RECV:
			RecvCount++;			
			break;
		case IO_SEND:
			SendCount++;			
			break;
		case IO_ACCEPT:
			AcceptCount++;			
			break;
		default:
			OtherCount++;			
		}
	}

	UINT UsedCount,FreeCount;

	m_OverLappedObjectPool.Verfy(UsedCount,FreeCount);

	PrintNetLog(_T("有%d(%u,%u,%u)个OverLapped对象使用中,其中Accept=%d,Recv=%d,Send=%d,Other=%d"),
		m_OverLappedObjectPool.GetObjectCount(),
		UsedCount,FreeCount,UsedCount+FreeCount,
		AcceptCount,
		RecvCount,
		SendCount,
		OtherCount);
	PrintNetLog(_T("有%d个EventRouter使用中"),m_EventRouterPool.GetObjectCount());
}