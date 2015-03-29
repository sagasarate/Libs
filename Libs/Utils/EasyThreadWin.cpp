/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyThreadWin.cpp                                        */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

#include <process.h>

IMPLEMENT_CLASS_INFO_STATIC(CEasyThread,CNameObject);

CEasyThread::CEasyThread():CNameObject()
{
	m_ThreadID=0;	
	m_hThread=NULL;		
	m_Status=THREAD_STATUS_TERMINATED;
	m_WantTerminate=FALSE;
}

CEasyThread::~CEasyThread()
{
	SafeTerminate(DEFAULT_THREAD_TERMINATE_TIME);	
}


BOOL CEasyThread::Start(BOOL IsSuspended,DWORD StartWaitTime)
{	
	int Flag = 0;	

	if(IsWorking())
		return FALSE;	
	
	Flag = CREATE_SUSPENDED;		

	m_Status=THREAD_STATUS_STARTING;
	m_WantTerminate=FALSE;

	HANDLE hThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		CEasyThread::ThreadProc,
		(void*)this,
		Flag,
		(UINT *)&m_ThreadID);
	if (hThread == 0)
		return FALSE;

	m_hThread = hThread;

	if (IsSuspended)
	{
		m_Status=THREAD_STATUS_SUSPENDED;
	}
	else
	{
		Resume();
		WaitForWorking(StartWaitTime);
	}
	
	return TRUE;
}


BOOL CEasyThread::Resume()
{
	if (ResumeThread(m_hThread) != (DWORD)-1)
	{
		m_Status,THREAD_STATUS_WORKING;
		return TRUE;
	}
	else
		return FALSE;
}


BOOL CEasyThread::Suspend()
{
	if (SuspendThread(m_hThread) != (DWORD)-1)
	{
		m_Status=THREAD_STATUS_SUSPENDED;
		return TRUE;
	}
	else
		return FALSE;
}


void CEasyThread::Terminate()
{
	m_WantTerminate=TRUE;
	if(m_Status!=THREAD_STATUS_TERMINATED)
		m_Status=THREAD_STATUS_ENDING;
}


void CEasyThread::ForceTerminate()
{
	TerminateThread(m_hThread,0);
	OnTerminate();
	if(m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread=NULL;
	}
	m_ThreadID=0;
}

void CEasyThread::SafeTerminate(DWORD Milliseconds)
{	
	Terminate();
	if(!WaitForTerminate(Milliseconds))
		ForceTerminate();
	if(m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread=NULL;
	}
	m_ThreadID=0;
}

BOOL CEasyThread::WaitForWorking(DWORD Milliseconds)
{	
	if(Milliseconds==INFINITE)
	{	
		while(m_Status==THREAD_STATUS_STARTING)
		{
			DoSleep(10);
		}
		return TRUE;
	}
	else
	{		
		CEasyTimer Timer;
		Timer.SetTimeOut(Milliseconds);
		while(!Timer.IsTimeOut())
		{			
			if(m_Status!=THREAD_STATUS_STARTING)
				return TRUE;
			DoSleep(10);
		}
		return FALSE;
	}
}

BOOL CEasyThread::WaitForTerminate(DWORD Milliseconds)
{
	if(Milliseconds==INFINITE)
	{	
		while(m_Status!=THREAD_STATUS_TERMINATED)
		{
			DoSleep(10);
		}
		return TRUE;
	}
	else
	{		
		CEasyTimer Timer;
		Timer.SetTimeOut(Milliseconds);
		while(!Timer.IsTimeOut())
		{			
			if(m_Status==THREAD_STATUS_TERMINATED)
				return TRUE;
			DoSleep(10);
		}
		return FALSE;
	}
}

BOOL CEasyThread::SetThreadPriority(int Priority)
{
	return ::SetThreadPriority(m_hThread,Priority);
}
int CEasyThread::GetThreadPriority()
{
	return ::GetThreadPriority(m_hThread);
}

BOOL CEasyThread::OnStart()
{
	return TRUE;
}

void CEasyThread::Execute()
{
	while((!m_WantTerminate)&&(OnRun()))
	{
	}	
}

BOOL CEasyThread::OnRun()
{
	return TRUE;
}

void CEasyThread::OnTerminate()
{
	
}


UINT __stdcall CEasyThread::ThreadProc(LPVOID pParam)
{
//#ifdef USE_THREAD_EXCEPTION_CATCH
//	__try
//	{
//#endif
	
		CEasyThread * pThread=(CEasyThread *)pParam;	
		
		if(pThread->OnStart())
		{
			pThread->m_Status=THREAD_STATUS_WORKING;
			pThread->Execute();
		}
		pThread->m_Status=THREAD_STATUS_ENDING;
		pThread->OnTerminate();
		pThread->m_Status=THREAD_STATUS_TERMINATED;	
		_endthreadex(0);			
		
		return 0;
//#ifdef USE_THREAD_EXCEPTION_CATCH
//	}__except(CExceptionParser::ExceptionHander(GetExceptionInformation()))
//	{
//		exit(0);
//	}	
//	return 0;
//#endif
}