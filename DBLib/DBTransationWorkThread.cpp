/****************************************************************************/
/*                                                                          */
/*      文件名:    DBTransationWorkThread.cpp                               */
/*      创建日期:  2009年07月06日                                           */
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

IMPLEMENT_CLASS_INFO_STATIC(CDBTransationWorkThread,CEasyThread);
CDBTransationWorkThread::CDBTransationWorkThread(CDBTransationManager * pManager)
{	
	m_pManager=pManager;
	m_pDatabase = NULL;
	m_pConnection = NULL;
	m_TransQueue.SetTag(_T("CDBTransationWorkThread"));
	m_FinishTransQueue.SetTag(_T("CDBTransationWorkThread"));
	m_IsBusy = false;
}

CDBTransationWorkThread::~CDBTransationWorkThread(void)
{

	Destory();
}

void CDBTransationWorkThread::Destory()
{
	CDBTransaction * pDBTansaction=NULL;
	while(m_TransQueue.PopFront(&pDBTansaction))
	{
		SAFE_RELEASE(pDBTansaction);
	}
	while(m_FinishTransQueue.PopFront(&pDBTansaction))
	{
		SAFE_RELEASE(pDBTansaction);
	}
	SAFE_RELEASE(m_pConnection);
}

bool CDBTransationWorkThread::Init(IDatabase * pDatabase, LPCSTR ConnectStr, bool MultiThreadAdd, int QueueSize)
{
	if (pDatabase == NULL)
		return false;

	Destory();

	m_ConnectString=ConnectStr;
	m_pDatabase = pDatabase;	
	m_TransQueue.Create(QueueSize);
	m_FinishTransQueue.Create(QueueSize);
	m_ConnectionTestTimer.SaveTime();

	if (MultiThreadAdd)
		m_TransQueue.SetLockMode(false, true);
	else
		m_TransQueue.SetLockMode(false, false);

	m_FinishTransQueue.SetLockMode(false, false);

	PrintDBLog("工作线程队列长度%d", QueueSize);

	return Start()!=FALSE;
}

bool CDBTransationWorkThread::AddTransaction(CDBTransaction * pDBTansaction)
{
	if(pDBTansaction)
	{
		if(m_TransQueue.PushBack(&pDBTansaction))
		{
			return true;
		}
		else
		{
			PrintDBLog("事务队列已满(%u,%u)",m_TransQueue.GetUsedSize(),m_TransQueue.GetBufferSize());
			return false;
		}
	}
	else
		return false;
}

CDBTransaction * CDBTransationWorkThread::PopFinishTransaction()
{
	CDBTransaction * pDBTansaction=NULL;
	m_FinishTransQueue.PopFront(&pDBTansaction);
	return pDBTansaction;
}

BOOL CDBTransationWorkThread::OnStart()
{
	mysql_thread_init();
	m_pConnection = m_pDatabase->CreateConnection();
	if (m_pConnection->Connect(m_ConnectString) == DBERR_SUCCEED)
	{
		m_pConnection->EnableTransaction(true);
	}
	else
	{
		PrintDBLog("[%u]数据库无法连接", GetID());
	}
	return TRUE;
}
BOOL CDBTransationWorkThread::OnRun()
{
	CDBTransaction* pDBTansaction = NULL;
	//LPVOID Pos=m_TransQueue.GetFirstObjectPos();
	m_IsBusy = true;
	m_TransQueue.PopFront(&pDBTansaction);
	if (pDBTansaction)
	{
		DWORD ExecTime = CEasyTimer::GetTime();
		bool Ret = pDBTansaction->OnExecute(m_pConnection);
		ExecTime = CEasyTimer::GetTimeToTime(ExecTime, CEasyTimer::GetTime());
		m_pManager->AddExecTime(ExecTime);
		if (Ret)
		{
			m_pConnection->Commit();
		}
		else
		{
			PrintDBLog("数据库事务执行出错，回滚");
			m_pConnection->RollBack();
		}
		pDBTansaction->SetResult(Ret);
		if (!m_FinishTransQueue.PushBack(&pDBTansaction))
		{
			PrintDBLog("完成队列已满(%u,%u)", m_FinishTransQueue.GetUsedSize(), m_FinishTransQueue.GetBufferSize());
		}
		//m_TransQueue.PopFront(pDBTansaction);
		m_IsBusy = false;
	}
	else
	{
		m_IsBusy = false;
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}
	if (m_ConnectionTestTimer.IsTimeOut(CONNECTION_TEST_TIME))
	{
		if (!m_pConnection->IsConnected())
		{
			m_IsBusy = true;
			PrintDBLog("连接已断开，重新连接");
			m_pConnection->Disconnect();
			if (m_pConnection->Connect(m_ConnectString) == DBERR_SUCCEED)
			{
				m_pConnection->EnableTransaction(true);
			}
			else
			{
				PrintDBLog("[%u]数据库无法连接", GetID());
			}
			m_IsBusy = false;
		}
		m_ConnectionTestTimer.SaveTime();
	}
	return TRUE;
}
void CDBTransationWorkThread::OnTerminate()
{
	if (m_pConnection)
	{
		m_pConnection->Disconnect();
		SAFE_RELEASE(m_pConnection);
	}
	mysql_thread_end();
}

}
