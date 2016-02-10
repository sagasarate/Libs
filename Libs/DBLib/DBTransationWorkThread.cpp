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
	m_pConnection=NULL;
	m_pManager=pManager;
}

CDBTransationWorkThread::~CDBTransationWorkThread(void)
{

	Destory();
}

void CDBTransationWorkThread::Destory()
{
	CDBTransaction * pDBTansaction=NULL;
	while(m_TransQueue.PopFront(pDBTansaction))
	{
		SAFE_RELEASE(pDBTansaction);
	}
	while(m_FinishTransQueue.PopFront(pDBTansaction))
	{
		SAFE_RELEASE(pDBTansaction);
	}
	SAFE_RELEASE(m_pConnection);
}

bool CDBTransationWorkThread::Init(IDBConnection * pConnection,LPCSTR ConnectStr,int QueueSize)
{
	if(pConnection==NULL)
		return false;

	Destory();

	m_ConnectString=ConnectStr;
	m_pConnection=pConnection;
	m_pConnection->EnableTransaction(true);
	m_TransQueue.Create(QueueSize);
	m_FinishTransQueue.Create(QueueSize);
	m_ConnectionTestTimer.SaveTime();

	PrintDBLog(0xff,"工作线程队列长度%d",QueueSize);

	return Start()!=FALSE;
}

bool CDBTransationWorkThread::AddTransaction(CDBTransaction * pDBTansaction)
{
	if(pDBTansaction)
	{
		if(m_TransQueue.PushBack(pDBTansaction))
		{
			return true;
		}
		else
		{
			PrintDBLog(0xff,"事务队列已满(%u,%u)",m_TransQueue.GetObjectCount(),m_TransQueue.GetBufferSize());
			return false;
		}
	}
	else
		return false;
}

CDBTransaction * CDBTransationWorkThread::PopFinishTransaction()
{
	CDBTransaction * pDBTansaction=NULL;
	m_FinishTransQueue.PopFront(pDBTansaction);
	return pDBTansaction;
}

BOOL CDBTransationWorkThread::OnStart()
{
	return TRUE;
}
BOOL CDBTransationWorkThread::OnRun()
{
	CDBTransaction * pDBTansaction=NULL;
	//LPVOID Pos=m_TransQueue.GetFirstObjectPos();
	m_TransQueue.PopFront(pDBTansaction);
	if(pDBTansaction)
	{
		DWORD ExecTime=CEasyTimer::GetTime();
		bool Ret=pDBTansaction->OnExecute(m_pConnection);
		ExecTime=GetTimeToTime(ExecTime,CEasyTimer::GetTime());
		m_pManager->AddExecTime(ExecTime);
		if(Ret)
		{
			m_pConnection->Commit();
		}
		else
		{
			PrintDBLog(0xff,"数据库事务执行出错，回滚");
			m_pConnection->RollBack();
		}
		pDBTansaction->SetResult(Ret);
		m_FinishTransQueue.PushBack(pDBTansaction);
		//m_TransQueue.PopFront(pDBTansaction);
	}
	else
	{
		DoSleep(1);
	}
	if(m_ConnectionTestTimer.IsTimeOut(CONNECTION_TEST_TIME))
	{
		if(!m_pConnection->IsConnected())
		{
			PrintDBLog(0xff,"连接已断开，重新连接");
			m_pConnection->Disconnect();
			m_pConnection->Connect((LPCSTR)m_ConnectString);
		}
		m_ConnectionTestTimer.SaveTime();
	}
	return TRUE;
}
void CDBTransationWorkThread::OnTerminate()
{

}

}
