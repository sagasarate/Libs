/****************************************************************************/
/*                                                                          */
/*      文件名:    DBTransationManager.cpp                                  */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


namespace DBLib
{

IMPLEMENT_CLASS_INFO_STATIC(CDBTransationManager,CNameObject);
CDBTransationManager::CDBTransationManager(void)
{
	m_pDatabase=NULL;
	m_ExecTimes=0;
	m_RealExecTime=0;
	m_AvgExecTime=0;
	m_ExecTimesPerSec=0;
	m_WorkThreads.SetTag(_T("CDBTransationManager"));
}

CDBTransationManager::~CDBTransationManager(void)
{
	Destory();
}

void CDBTransationManager::Destory()
{
	for(int i=0;i<(int)m_WorkThreads.GetCount();i++)
	{
		if(m_WorkThreads[i])
			m_WorkThreads[i]->SafeTerminate(DEFAULT_THREAD_TERMINATE_TIME);
		SAFE_RELEASE(m_WorkThreads[i]);
	}
	m_WorkThreads.Clear();
}

bool CDBTransationManager::Init(IDatabase * pDatabase,LPCSTR szConnectStr,int ThreadCount,int QueueSize,UINT Flag)
{
	if(pDatabase==NULL)
		return false;
	Destory();
	m_pDatabase=pDatabase;
	m_Flag=Flag;

	for(int i=0;i<ThreadCount;i++)
	{		
		CDBTransationWorkThread * pThread = MONITORED_NEW(_T("CDBTransationManager"), CDBTransationWorkThread, this);
		if (!pThread->Init(pDatabase, szConnectStr, (Flag&DBTM_FLAG_ADD_TRANS_MULTI_THREAD) != 0, QueueSize))
		{
			SAFE_RELEASE(pThread);
			return false;
		}
		m_WorkThreads.Add(pThread);

	}


	m_PerformanceCountTimer.SaveTime();

	PrintDBLog("[%u]一共建立了%d个工作线程",GetID(),ThreadCount);

	return true;
}

CDBTransationWorkThread * CDBTransationManager::AllocWorkThread()
{
	CDBTransationWorkThread * pThread=NULL;
	UINT Len=0xffffffff;
	for(int i=0;i<(int)m_WorkThreads.GetCount();i++)
	{
		if(m_WorkThreads[i]->GetQueueLen()<Len)
		{
			pThread=m_WorkThreads[i];
			Len=m_WorkThreads[i]->GetQueueLen();
		}
	}
	return pThread;
}

bool CDBTransationManager::AddTransaction(CDBTransaction * pDBTansaction)
{
	CDBTransationWorkThread * pThread=AllocWorkThread();
	if(pThread)
	{
		return pThread->AddTransaction(pDBTansaction);
	}
	else
	{
		PrintDBLog("无工作线程可分配");
		return false;
	}
}



int CDBTransationManager::Update(int ProcessLimit)
{
	int ProcessCount=0;
	while(ProcessCount<ProcessLimit)
	{
		int Count=0;
		for(int i=0;i<(int)m_WorkThreads.GetCount();i++)
		{
			CDBTransaction * pDBTansaction=m_WorkThreads[i]->PopFinishTransaction();
			if(pDBTansaction)
			{
				pDBTansaction->OnFinish();
				SAFE_RELEASE(pDBTansaction);
				ProcessCount++;
				Count++;
			}
		}
		if(Count==0)
			break;
	}

	if(m_PerformanceCountTimer.IsTimeOut(PERFORMANCE_COUNT_TIME))
	{
		m_PerformanceCountTimer.SaveTime();
		m_ExecTimesPerSec=(float)m_ExecTimes*1000/PERFORMANCE_COUNT_TIME;
		if(m_ExecTimes)
			m_AvgExecTime=(float)m_RealExecTime/m_ExecTimes;
		else
			m_AvgExecTime=0;	
		UINT TotalQueueLen = 0;
		UINT TotalFinishQueueLen = 0;
		for (int i = 0; i < (int)m_WorkThreads.GetCount(); i++)
		{
			TotalQueueLen += m_WorkThreads[i]->GetQueueLen();
			TotalFinishQueueLen += m_WorkThreads[i]->GetFinishQueueLen();
			if (m_WorkThreads[i]->IsJam())
				PrintDBLog("线程[%u]发生拥堵", m_WorkThreads[i]->GetThreadID());
		}
		if(m_Flag&DBTM_FLAG_LOG_PERFORMANCE)
		{
			PrintDBLog("[%s]总执行次数%u,平均执行时间=%0.2f毫秒,每秒执行次数%0.2f,任务队列总长%u,完成队列总长%u", 
				GetName(), m_ExecTimes, m_AvgExecTime, m_ExecTimesPerSec, TotalQueueLen, TotalFinishQueueLen);
		}
		m_RealExecTime = 0;
		m_ExecTimes = 0;
	}

	return ProcessCount;
}

bool CDBTransationManager::IsIdle()
{
	for(int i=0;i<(int)m_WorkThreads.GetCount();i++)
	{
		if (m_WorkThreads[i]->IsBusy() || (m_WorkThreads[i]->GetQueueLen() > 0) || (m_WorkThreads[i]->GetFinishQueueLen() > 0))
			return false;
	}
	return true;
}

bool CDBTransationManager::HaveJam()
{
	for (int i = 0; i < (int)m_WorkThreads.GetCount(); i++)
	{
		if (m_WorkThreads[i]->IsJam())
			return true;
	}
	return false;
}

}
