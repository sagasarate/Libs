/****************************************************************************/
/*                                                                          */
/*      文件名:    EpollThread.cpp                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"



IMPLEMENT_CLASS_INFO_STATIC(CEpollThread,CEasyThread);

CEpollThread::CEpollThread() :CEasyThread()
{
	m_pServer = NULL;
	m_hEpoll = INVALID_HANDLE_VALUE;
	m_BindSocketCount = 0;
}

CEpollThread::CEpollThread(CNetServer * pServer):CEasyThread()
{
	m_pServer=pServer;
	m_hEpoll = INVALID_HANDLE_VALUE;
	m_BindSocketCount = 0;
}

CEpollThread::~CEpollThread(void)
{
}

bool CEpollThread::Init(CNetServer * pServer)
{
	m_pServer = pServer;
	m_BindSocketCount = 0;
	return true;
}

bool CEpollThread::BindSocket(SOCKET Socket, CEpollEventRouter * pEpollEventRouter, bool UseLTMode)
{
	if (m_hEpoll == INVALID_HANDLE_VALUE)
	{
		PrintNetLog( "(%d)Epoll没有初始化,无法绑定Socket！", GetID());
		return false;
	}
	if (Socket == INVALID_SOCKET)
	{
		PrintNetLog( "(%d)Socket没有初始化,无法绑定Socket！", GetID());
		return false;
	}

	epoll_event EpollEvent;
	ZeroMemory(&EpollEvent, sizeof(EpollEvent));
	UINT64_CONVERTER Param64;
	Param64.LowPart = (DWORD)pEpollEventRouter->GetID();
	Param64.HighPart = (DWORD)(pEpollEventRouter->GetSessionID());
	EpollEvent.data.u64 = Param64.QuadPart;
	if (UseLTMode)
		EpollEvent.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
	else
		EpollEvent.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET;
	if (epoll_ctl(m_hEpoll, EPOLL_CTL_ADD, Socket, &EpollEvent) == 0)
	{
		AtomicInc(&m_BindSocketCount);
		return true;
	}		
	else
	{
		return false;
	}
		
}

//bool CEpollThread::ModifySendEvent(SOCKET Socket, CEpollEventRouter * pEpollEventRouter, bool IsSet)
//{
//	epoll_event EpollEvent;
//	UINT64_CONVERTER Param64;
//	Param64.LowPart = (DWORD)pEpollEventRouter->GetID();
//	Param64.HighPart = (DWORD)(pEpollEventRouter->GetSessionID());
//	EpollEvent.data.u64 = Param64.QuadPart;
//	if (IsSet)
//		EpollEvent.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
//	else
//		EpollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP;
//	if (epoll_ctl(m_hEpoll, EPOLL_CTL_MOD, Socket, &EpollEvent) == 0)
//		return true;
//	else
//		return false;
//}

bool CEpollThread::UnbindSocket(SOCKET Socket)
{
	if (Socket == INVALID_SOCKET)
	{
		PrintNetLog( "(%d)Socket没有初始化,无法解绑Socket！", GetID());
		return false;
	}
	if (epoll_ctl(m_hEpoll, EPOLL_CTL_DEL, Socket, NULL) == 0)
	{
		AtomicDec(&m_BindSocketCount);
		return true;
	}		
	else
	{
		return false;
	}
		
}

BOOL CEpollThread::OnStart()
{
	PrintNetLog(_T("Epoll工作线程启动"));

	if (m_hEpoll != INVALID_HANDLE_VALUE)
	{
		close(m_hEpoll);
		m_hEpoll = INVALID_HANDLE_VALUE;
	}

	m_hEpoll = epoll_create(DEFAULT_EVENT_ROUTER_COUNT);
	if (m_hEpoll == INVALID_HANDLE_VALUE)
	{
		PrintNetLog( "(%d)创建Epoll失败(%d)！", GetID(), GetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL CEpollThread::OnRun()
{

	epoll_event Events[DEFAULT_EPOLL_EVENT_WAIT_NUM];

	int EventCount=epoll_wait(m_hEpoll,Events,DEFAULT_EPOLL_EVENT_WAIT_NUM,100);

	if(EventCount>0)
	{
		for(int i=0;i<EventCount;i++)
		{
			UINT64_CONVERTER Param64;

			Param64.QuadPart=Events[i].data.u64;

			CEpollEventRouter * pEpollEventRouter=m_pServer->GetEventRouter(Param64.LowPart);
			if(pEpollEventRouter)
			{
				pEpollEventRouter->OnEpollEvent(Events[i].events,Param64.HighPart);
			}
			else
			{
				PrintNetLog("Epoll事件无法找到接收者");
			}
		}
	}
	else if(EventCount<0)
	{
		PrintNetLog("epoll_wait发生错误%d",errno);
	}


	return TRUE;
}

void CEpollThread::OnTerminate()
{
	if (m_hEpoll != INVALID_HANDLE_VALUE)
	{
		close(m_hEpoll);
		m_hEpoll = INVALID_HANDLE_VALUE;
	}
	PrintNetLog("Epoll工作线程退出");
}
