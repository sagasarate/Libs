/****************************************************************************/
/*                                                                          */
/*      文件名:    DBTransationWorkThread.h                                 */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace DBLib
{

#define DEFAULT_TRANS_QUEUE		128
#define CONNECTION_TEST_TIME	60000

class CDBTransationManager;

class CDBTransationWorkThread :
	public CEasyThread
{
protected:
	CDBTransationManager *					m_pManager;
	IDatabase *								m_pDatabase;
	IDBConnection *							m_pConnection;
	CEasyStringA							m_ConnectString;
	CCycleQueue<CDBTransaction *>			m_TransQueue;
	CCycleQueue<CDBTransaction *>			m_FinishTransQueue;
	CEasyTimer								m_ConnectionTestTimer;

	DECLARE_CLASS_INFO_STATIC(CDBTransationWorkThread)
public:
	CDBTransationWorkThread(CDBTransationManager * pManager);
	~CDBTransationWorkThread(void);

	void Destory();

	bool Init(IDatabase * pDatabase, LPCSTR ConnectStr, int QueueSize = DEFAULT_TRANS_QUEUE);
	bool AddTransaction(CDBTransaction * pDBTansaction);
	CDBTransaction *PopFinishTransaction();

	UINT GetQueueLen()
	{
		return m_TransQueue.GetUsedSize();
	}
	UINT GetFinishQueueLen()
	{
		return m_FinishTransQueue.GetUsedSize();
	}

protected:
	virtual BOOL OnStart();	
	virtual BOOL OnRun();
	virtual void OnTerminate();
};

}