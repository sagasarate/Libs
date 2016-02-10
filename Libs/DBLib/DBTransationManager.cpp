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
		IDBConnection * pConnection=m_pDatabase->CreateConnection();
		if(pConnection->Connect(szConnectStr)!=DBERR_SUCCEED)
		{
			PrintDBLog(0xff,"[%u]数据库无法连接，但初始化继续",GetID());
		}
		CDBTransationWorkThread * pThread=new CDBTransationWorkThread(this);
		if(!pThread->Init(pConnection,szConnectStr,QueueSize))
		{
			SAFE_RELEASE(pThread);
			return false;
		}
		m_WorkThreads.Add(pThread);

	}


	m_PerformanceCountTimer.SaveTime();

	PrintDBLog(0xff,"[%u]一共建立了%d个工作线程",GetID(),ThreadCount);

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
		PrintDBLog(0xff,"无工作线程可分配");
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
		m_RealExecTime=0;
		m_ExecTimes=0;
		if(m_Flag&DBTM_FLAG_LOG_PERFORMANCE)
		{
			PrintDBLog(0xff,"[%u]平均执行时间=%g毫秒,每秒执行次数%g",GetID(),m_AvgExecTime,m_ExecTimesPerSec);
		}
	}

	return ProcessCount;
}

bool CDBTransationManager::IsIdle()
{
	int Len=0;
	for(int i=0;i<(int)m_WorkThreads.GetCount();i++)
	{
		Len+=m_WorkThreads[i]->GetQueueLen()+m_WorkThreads[i]->GetFinishQueueLen();
	}
	return Len==0;
}

}
