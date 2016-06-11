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
	m_IOCPThreadCount=0;
	m_IOCPObjectPoolSize=DEFAULT_EVENT_OBJECT_COUNT;
	m_IOCPObjectPoolGrowSize=DEFAULT_EVENT_OBJECT_POOL_GROW_SIZE;
	m_IOCPObjectPoolGrowLimit=DEFAULT_EVENT_OBJECT_POOL_GROW_LIMIT;
	m_EventRouterPoolSize=DEFAULT_EVENT_ROUTER_COUNT;
	m_EventRouterPoolGrowSize=DEFAULT_EVENT_ROUTER_POOL_GROW_SIZE;
	m_EventRouterPoolGrowLimit=DEFAULT_EVENT_ROUTER_POOL_GROW_LIMIT;
	m_IOCPThreadNumPerCPU=DEFAULT_THREAD_NUMBER_PER_CPU;	
}

CNetServer::~CNetServer(void)
{
	ShutDown();	
	CNetSocket::NetCleanup();
}

BOOL CNetServer::StartUp(int EventObjectPoolSize,
						 int ThreadNumberPerCPU,
						 int EventRouterPoolSiz,
						 int EventObjectPoolGrowSize,
						 int EventObjectPoolGrowLimit,
						 int EventRouterPoolGrowSize,
						 int EventRouterPoolGrowlimit)
{
	m_IOCPObjectPoolSize=EventObjectPoolSize;
	m_EventRouterPoolSize=EventRouterPoolSiz;
	m_IOCPThreadNumPerCPU=ThreadNumberPerCPU;
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

BOOL CNetServer::OnStart()
{
	if(!CBaseNetServer::OnStart())
		return FALSE;

	if(m_hIOCP!=NULL)
		return FALSE;

	CNetSocket::NetStartup();

	m_OverLappedObjectPool.Create(m_IOCPObjectPoolSize,m_IOCPObjectPoolGrowSize,m_IOCPObjectPoolGrowLimit);
	m_EventRouterPool.Create(m_EventRouterPoolSize,m_EventRouterPoolGrowSize,m_EventRouterPoolGrowLimit);

	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
	if( m_hIOCP == NULL )
	{
		PrintNetLog(_T("NetLib"),_T("(%d)创建完成端口失败(%d)！"),GetID(),GetLastError());		
		return FALSE;
	}

	SYSTEM_INFO	si;
	GetSystemInfo( &si );
	m_IOCPThreadCount = si.dwNumberOfProcessors * m_IOCPThreadNumPerCPU;

	m_pIOCPThreads = new CIOCPThread[m_IOCPThreadCount];

	for( int i = 0;i < m_IOCPThreadCount;i ++ )
	{
		m_pIOCPThreads[i].SetIOCPHandle(m_hIOCP);
		m_pIOCPThreads[i].Start();
	}	
	if(!OnStartUp())
	{		
		return FALSE;
	}
	return TRUE;
}

BOOL CNetServer::OnRun()
{
	if(!CBaseNetServer::OnRun())
		return FALSE;

	if(Update()==0)
	{
		DoSleep(1);
	}
	return TRUE;
}

void CNetServer::OnTerminate()
{	
	if( m_pIOCPThreads )
	{
		for( int i = 0;i < m_IOCPThreadCount;i ++ )
		{			
			m_pIOCPThreads[i].SafeTerminate();
		}
		delete[] m_pIOCPThreads;
		m_pIOCPThreads = NULL;
	}		

	OnShutDown();

	CloseHandle( m_hIOCP );
	m_hIOCP = NULL;	
	if(m_OverLappedObjectPool.GetObjectCount())
	{		
		PrintNetLog(_T("NetLib"),_T("(%d)关闭，开始统计OverLappedObject使用状况！"),GetID());
		PrintObjectStatus();
	}

	m_OverLappedObjectPool.Destory();	
	m_EventRouterPool.Destory();

	CNetSocket::NetCleanup();
}

COverLappedObject * CNetServer::CreateOverLappedObject()
{
	

	COverLappedObject * pOverLappedObject=NULL;

	pOverLappedObject=m_OverLappedObjectPool.NewObject();
	if(pOverLappedObject)
	{
		if(pOverLappedObject->GetParentID())
		{
			PrintImportantLog(0,_T("分配了未释放的OverLappedObject"));
		}
		pOverLappedObject->Create(this);
		
		return pOverLappedObject;
	}
	PrintNetLog(_T("NetLib"),_T("(%d)Server无法创建COverLappedObject！"),GetID());



	return NULL;
}

BOOL CNetServer::DeleteOverLappedObject(COverLappedObject * pOverLappedObject)
{	
	pOverLappedObject->Destory();
	if(!m_OverLappedObjectPool.DeleteObject(pOverLappedObject->GetID()))
	{
		PrintNetLog(_T("NetLib"),_T("(%d)Server无法删除COverLappedObject(%d)！"),GetID(),pOverLappedObject->GetID());
		return FALSE;
	}	
	
	return TRUE;
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
	PrintNetLog(_T("NetLib"),_T("(%d)Server无法创建CIOCPEventRouter！"),GetID());
	return NULL;
}

BOOL CNetServer::DeleteEventRouter(CIOCPEventRouter * pEventRouter)
{

	pEventRouter->Destory();
	if(!m_EventRouterPool.DeleteObject(pEventRouter->GetID()))
	{
		PrintNetLog(_T("NetLib"),_T("(%d)Server无法删除CIOCPEventRouter(%d)！"),GetID(),pEventRouter->GetID());
		return FALSE;
	}

	return TRUE;
}


BOOL CNetServer::BindSocket(SOCKET Socket)
{
	if(m_hIOCP == NULL)
	{
		PrintNetLog(_T("NetLib"),_T("(%d)完成端口没有初始化,无法绑定Socket！"),GetID());		
		return FALSE;
	}
	if(Socket == INVALID_SOCKET)
	{
		PrintNetLog(_T("NetLib"),_T("(%d)Socket没有初始化,无法绑定Socket！"),GetID());		
		return FALSE;
	}
	HANDLE hPort = CreateIoCompletionPort((HANDLE)Socket, m_hIOCP, 0, 0 );
	return TRUE;
}

BOOL CNetServer::BindFile(HANDLE FileHandle)
{
	if(m_hIOCP == NULL)
	{
		PrintNetLog(_T("NetLib"),_T("(%d)完成端口没有初始化,无法绑定FileHandle！"),GetID());		
		return FALSE;
	}
	if(FileHandle == INVALID_HANDLE_VALUE)
	{
		PrintNetLog(_T("NetLib"),_T("(%d)FileHandle没有初始化,无法绑定FileHandle！"),GetID());		
		return FALSE;
	}
	HANDLE hPort = CreateIoCompletionPort(FileHandle, m_hIOCP, 0, 0 );
	return TRUE;
}

BOOL CNetServer::OnStartUp()
{
	return TRUE;
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

	int AcceptCount=0;
	int RecvCount=0;
	int SendCount=0;
	int OtherCount=0;
	
	LPVOID Pos=m_OverLappedObjectPool.GetFirstObjectPos();
	while(Pos)
	{
		COverLappedObject * pObject=m_OverLappedObjectPool.GetNext(Pos);
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

	PrintNetLog(0,_T("有%d(%u,%u,%u)个OverLapped对象使用中,其中Accept=%d,Recv=%d,Send=%d,Other=%d"),
		m_OverLappedObjectPool.GetObjectCount(),
		UsedCount,FreeCount,UsedCount+FreeCount,
		AcceptCount,
		RecvCount,
		SendCount,
		OtherCount);
	PrintNetLog(0,_T("有%d个EventRouter使用中"),m_EventRouterPool.GetObjectCount());
}