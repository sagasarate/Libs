#include "stdafx.h"



CAsyncLogWorkThread::CAsyncLogWorkThread()
{
	m_LogPrinterList.SetTag(_T("CAsyncLogWorkThread"));
}


CAsyncLogWorkThread::~CAsyncLogWorkThread()
{
}


bool CAsyncLogWorkThread::OnRun()
{
	int ProcessCount = 0;
	{
		CAutoLock Lock(m_EasyCriticalSection);
		for (UINT i = 0; i < m_LogPrinterList.GetCount(); i++)
		{
			ProcessCount += m_LogPrinterList[i]->Update();
		}
	}
	if (ProcessCount == 0)
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	return true;
}

bool CAsyncLogWorkThread::AddLogPrinter(ILogPrinter * pLogPrinter)
{
	CAutoLock Lock(m_EasyCriticalSection);

	m_LogPrinterList.Add(pLogPrinter);

	
	return true;
}
bool CAsyncLogWorkThread::RemoveLogPrinter(ILogPrinter * pLogPrinter)
{
	CAutoLock Lock(m_EasyCriticalSection);

	bool HaveDeleted = false;
	for (UINT i = 0; i < m_LogPrinterList.GetCount(); i++)
	{
		if (m_LogPrinterList[i] == pLogPrinter)
		{
			m_LogPrinterList.Delete(i);
			HaveDeleted = true;
			break;
		}
	}	
	
	return HaveDeleted;
}