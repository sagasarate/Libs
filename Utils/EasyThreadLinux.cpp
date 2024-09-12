/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyThreadLinux.cpp                                      */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"
#include <sys/syscall.h>
#include <execinfo.h>

CEasyCriticalSection CEasyThread::m_GetCallStackLock;
pthread_t CEasyThread::m_GetCallStackCaller = 0;
pthread_t CEasyThread::m_GetCallStackTarget = 0;
LPVOID * CEasyThread::m_CallStackSaveBuffer = NULL;
UINT CEasyThread::m_CallStackMaxDepth = 0;

IMPLEMENT_CLASS_INFO_STATIC(CEasyThread,CNameObject);

CEasyThread::CEasyThread():CNameObject()
{
	m_ThreadHandle = 0;
	m_ThreadID=0;
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

	struct sigaction sa;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = SuspendSignalHandler;
	sigaction(SIG_THREAD_SUSPEND, &sa, NULL);

	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = ResumeSignalHandler;
	sigaction(SIG_THREAD_RESUME, &sa, NULL);

	m_Status=THREAD_STATUS_STARTING;
	m_WantTerminate=FALSE;

	if(pthread_create(&m_ThreadHandle,NULL,ThreadProc,this)!=0)
	{
		return FALSE;
	}
	//pthread_detach(m_ThreadHandle);
	WaitForWorking(StartWaitTime);

	return TRUE;
}


BOOL CEasyThread::Resume()
{
	if (m_Status == THREAD_STATUS_SUSPENDED)
	{
		pthread_kill(m_ThreadHandle, SIG_THREAD_RESUME);
		m_Status = THREAD_STATUS_WORKING;
		return TRUE;
	}
	return FALSE;
}


BOOL CEasyThread::Suspend()
{
	if (m_Status == THREAD_STATUS_WORKING)
	{
		pthread_kill(m_ThreadHandle, SIG_THREAD_SUSPEND);
		m_Status = SIG_THREAD_SUSPEND;
		return TRUE;
	}
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
	if(!IsTerminated())
	{
		PrintImportantLog("线程%u被强行终止", (UINT)m_ThreadHandle);
		pthread_cancel(m_ThreadHandle);
		OnTerminate();
	}
	m_ThreadHandle = 0;
	m_ThreadID = 0;
}

void CEasyThread::SafeTerminate(DWORD Milliseconds)
{
	Terminate();
	if(!WaitForTerminate(Milliseconds))
		ForceTerminate();
	m_ThreadHandle = 0;
	m_ThreadID = 0;
}

BOOL CEasyThread::WaitForWorking(DWORD Milliseconds)
{
	if (IsWorking())
		return TRUE;
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
		while(true)
		{
			if(m_Status!=THREAD_STATUS_STARTING)
				return IsWorking();
			if (Timer.IsTimeOut())
				break;
			DoSleep(10);
		}
		return FALSE;
	}
}

BOOL CEasyThread::WaitForTerminate(DWORD Milliseconds)
{
	if (IsTerminated())
		return TRUE;
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
		while(true)
		{
			if(IsTerminated())
				return TRUE;
			if (Timer.IsTimeOut())
				break;
			DoSleep(10);
		}
		return FALSE;
	}
}

BOOL CEasyThread::SetThreadPriority(int Priority)
{
	return FALSE;
}

int CEasyThread::GetThreadPriority()
{
	return ET_PRIORITY_ERROR_RETURN;
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


LPVOID CEasyThread::ThreadProc(LPVOID pParam)
{
#ifdef USE_THREAD_EXCEPTION_CATCH
	__try
	{
#endif
		pthread_detach(pthread_self());

		CEasyThread * pThread=(CEasyThread *)pParam;

		pThread->m_ThreadID = syscall(SYS_gettid);

		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

		if(pThread->OnStart())
		{
			pThread->m_Status=THREAD_STATUS_WORKING;
			pThread->Execute();
		}
		pThread->m_Status=THREAD_STATUS_ENDING;
		pThread->OnTerminate();
		pThread->m_Status=THREAD_STATUS_TERMINATED;

		return NULL;
#ifdef USE_THREAD_EXCEPTION_CATCH
	}__except(CExceptionParser::ExceptionHander(GetExceptionInformation()))
	{
		exit(0);
	}
	return 0;
#endif
}


int CEasyThread::GetCallStack(HANDLE hThread, LPVOID * CallStacks, UINT MaxDepth)
{
	if (hThread == 0 || (pthread_t)hThread == pthread_self())
	{
		return backtrace(CallStacks, MaxDepth);
	}

	CAutoLock Lock(m_GetCallStackLock);

	m_GetCallStackCaller = pthread_self();
	m_GetCallStackTarget = (pthread_t)hThread;
	m_CallStackSaveBuffer = CallStacks;
	m_CallStackMaxDepth = MaxDepth;

	struct sigaction sa;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = CallStackSignalHandler;
	sigaction(SIG_THREAD_GET_CALL_STACK, &sa, NULL);

	if (pthread_kill(m_GetCallStackTarget, SIG_THREAD_GET_CALL_STACK) != 0)
		return 0;

	{
		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIG_THREAD_GET_CALL_STACK);

		// wait for CALLSTACK_SIG on this thread
		sigsuspend(&mask);
	}

	m_GetCallStackCaller = 0;
	m_GetCallStackTarget = 0;
	CallStacks = NULL;
	return m_CallStackMaxDepth;
}

void CEasyThread::CallStackSignalHandler(int signr, siginfo_t *info, void *secret)
{
	pthread_t CurThread = pthread_self();
	
	if (!pthread_equal(CurThread, m_GetCallStackTarget))
		return;

	m_CallStackMaxDepth = backtrace(m_CallStackSaveBuffer, m_CallStackMaxDepth);

	for (UINT i = 1; i < m_CallStackMaxDepth; i++)
	{
		if (m_CallStackSaveBuffer[i] != NULL)
			continue;

		// Found it at stack[i]. Thus remove the first i.
		const int IgnoreTopFramesNum = i;
		m_CallStackMaxDepth -= IgnoreTopFramesNum;
		memmove(m_CallStackSaveBuffer, m_CallStackSaveBuffer + IgnoreTopFramesNum, m_CallStackMaxDepth * sizeof(LPVOID));
		break;
	}

	pthread_kill(m_GetCallStackCaller, SIG_THREAD_GET_CALL_STACK);
}

void CEasyThread::SuspendSignalHandler(int signr, siginfo_t *info, void *secret)
{
	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask, SIG_THREAD_RESUME);
	sigsuspend(&mask);
}

void CEasyThread::ResumeSignalHandler(int signr, siginfo_t *info, void *secret)
{
}