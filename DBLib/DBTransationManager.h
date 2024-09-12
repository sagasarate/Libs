/****************************************************************************/
/*                                                                          */
/*      文件名:    DBTransationManager.h                                    */
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

#define DEFAULT_TRANS_THREAD	4
#define PERFORMANCE_COUNT_TIME	(2*60*1000)



class CDBTransationManager
	:public CNameObject
{
public:
	enum FLAG
	{
		DBTM_FLAG_LOG_PERFORMANCE = 1,
		DBTM_FLAG_ADD_TRANS_MULTI_THREAD = (1 << 1),
	};
protected:
	CEasyArray<CDBTransationWorkThread *>	m_WorkThreads;
	IDatabase *								m_pDatabase;
	UINT									m_Flag;

	CEasyTimer								m_PerformanceCountTimer;
	volatile UINT							m_ExecTimes;
	volatile UINT							m_RealExecTime;

	float									m_AvgExecTime;
	float									m_ExecTimesPerSec;

	
	
	DECLARE_CLASS_INFO_STATIC(CDBTransationManager)
public:
	CDBTransationManager(void);
	~CDBTransationManager(void);

	void Destory();

	bool Init(IDatabase * pDatabase,LPCSTR szConnectStr,int ThreadCount=DEFAULT_TRANS_THREAD,int QueueSize=DEFAULT_TRANS_QUEUE,UINT Flag=0);

	void ModifyFlag(UINT Add,UINT Remove)
	{
		m_Flag|=Add;
		m_Flag&=~Remove;
	}

	CDBTransationWorkThread * AllocWorkThread();
	
	bool AddTransaction(CDBTransaction * pDBTansaction);

	int Update(int ProcessLimit=DEFAULT_PROCESS_LIMIT);

	bool IsIdle();
	bool HaveJam();

	void AddExecTime(UINT ExecTime)
	{
		AtomicInc(&m_ExecTimes);
		AtomicAdd(&m_RealExecTime,ExecTime);
	}

	float GetAvgExecTime()
	{
		return m_AvgExecTime;
	}
	float GetExecTimesPerSec()
	{
		return m_ExecTimesPerSec;
	}
};

}