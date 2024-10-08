﻿/****************************************************************************/
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
	m_WantTerminate=false;
}

CEasyThread::~CEasyThread()
{
	SafeTerminate(DEFAULT_THREAD_TERMINATE_TIME);	
}


bool CEasyThread::Start(bool IsSuspended,DWORD StartWaitTime)
{	
	int Flag = 0;	

	if(IsWorking())
		return false;	
	
	Flag = CREATE_SUSPENDED;		

	m_Status=THREAD_STATUS_STARTING;
	m_WantTerminate=false;

	HANDLE hThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		CEasyThread::ThreadProc,
		(void*)this,
		Flag,
		(UINT *)&m_ThreadID);
	if (hThread == 0)
		return false;

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
	
	return true;
}


bool CEasyThread::Resume()
{
	if (ResumeThread(m_hThread) != (DWORD)-1)
	{
		//m_Status=THREAD_STATUS_WORKING;
		return true;
	}
	else
		return false;
}


bool CEasyThread::Suspend()
{
	if (SuspendThread(m_hThread) != (DWORD)-1)
	{
		m_Status=THREAD_STATUS_SUSPENDED;
		return true;
	}
	else
		return false;
}


void CEasyThread::Terminate()
{
	if (!IsTerminated())
	{
		m_WantTerminate = true;
		m_Status = THREAD_STATUS_ENDING;
	}
		
}


void CEasyThread::ForceTerminate()
{
	if (!IsTerminated())
	{
		PrintImportantLog(_T("线程%u被强行终止"), m_ThreadID);
		TerminateThread(m_hThread, 0);
		OnTerminate();
	}
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

bool CEasyThread::WaitForWorking(DWORD Milliseconds)
{	
	if (IsWorking())
		return true;
	if(Milliseconds==INFINITE)
	{	
		while(m_Status==THREAD_STATUS_STARTING)
		{
			DoSleep(10);
		}
		return true;
	}
	else
	{		
		CEasyTimer Timer;
		Timer.SetTimeOut(Milliseconds);
		while(true)
		{			
			if(m_Status!=THREAD_STATUS_STARTING)
				return IsWorking();			
			if (Timer.IsTimeOut())
				break;
			DoSleep(10);
		}
		return false;
	}
}

bool CEasyThread::WaitForTerminate(DWORD Milliseconds)
{
	if (IsTerminated())
		return true;
	if(Milliseconds==INFINITE)
	{	
		while(m_Status!=THREAD_STATUS_TERMINATED)
		{
			DoSleep(10);
		}
		return true;
	}
	else
	{		
		CEasyTimer Timer;
		Timer.SetTimeOut(Milliseconds);
		while(true)
		{			
			if (IsTerminated())
				return true;
			if (Timer.IsTimeOut())
				break;
			DoSleep(10);
		}
		return false;
	}
}

bool CEasyThread::SetThreadPriority(int Priority)
{
	return ::SetThreadPriority(m_hThread,Priority);
}
int CEasyThread::GetThreadPriority()
{
	return ::GetThreadPriority(m_hThread);
}

int CEasyThread::GetExitCode()
{
	DWORD ExitCode;
	if (GetExitCodeThread(m_hThread, &ExitCode))
	{
		if (ExitCode == STILL_ACTIVE)
			return -2;
		else
			return (int)ExitCode;
	}
	return -1;
}

bool CEasyThread::OnStart()
{
	return true;
}

void CEasyThread::Execute()
{
	while((!m_WantTerminate)&&(OnRun()))
	{
	}	
}

bool CEasyThread::OnRun()
{
	return true;
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
		if (pThread->m_hThread)
		{
			CloseHandle(pThread->m_hThread);
			pThread->m_hThread = NULL;
		}
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