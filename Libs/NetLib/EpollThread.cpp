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
}

CEpollThread::CEpollThread(CNetServer * pServer):CEasyThread()
{
	m_pServer=pServer;
	m_hEpoll = INVALID_HANDLE_VALUE;
}

CEpollThread::~CEpollThread(void)
{
}

BOOL CEpollThread::OnStart()
{
	PrintNetLog(_T("NetLib"),_T("Epoll工作线程启动"));
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
			ULONG64_CONVERTER Param64;

			Param64.QuadPart=Events[i].data.u64;

			CEpollEventRouter * pEpollEventRouter=m_pServer->GetEventRouter(Param64.LowPart);
			if(pEpollEventRouter)
			{
				pEpollEventRouter->OnEpollEvent(Events[i].events,Param64.HighPart);
			}
			else
			{
				PrintNetLog(0,"Epoll事件无法找到接收者");
			}
		}
	}
	else if(EventCount<0)
	{
		PrintNetLog(0,"epoll_wait发生错误%d",errno);
	}


	return TRUE;
}

void CEpollThread::OnTerminate()
{
	PrintNetLog(_T("NetLib"),"Epoll工作线程退出");
}
