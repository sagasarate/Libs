/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyThreadLinux.h                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define DEFAULT_THREAD_TERMINATE_TIME		60000
#define DEFAULT_THREAD_STARTUP_TIME			60000

enum THREAD_STATUS
{
	THREAD_STATUS_TERMINATED,
	THREAD_STATUS_STARTING,
	THREAD_STATUS_SUSPENDED,
	THREAD_STATUS_WORKING,
	THREAD_STATUS_ENDING,
};

enum THREAD_PRIORITY
{
	ET_PRIORITY_TIME_CRITICAL=0,
	ET_PRIORITY_HIGHEST,
	ET_PRIORITY_ABOVE_NORMA,
	ET_PRIORITY_NORMAL,
	ET_PRIORITY_BELOW_NORMA,
	ET_PRIORITY_LOWEST,
	ET_PRIORITY_IDLE,
	ET_PRIORITY_ERROR_RETURN=-1,
};


enum THREAD_EXIT_CODE
{
	THREAD_EXIT_CODE_OK = 0,
	THREAD_EXIT_CODE_FETCH_EXIT_CODE_ERROR = -1,
	THREAD_EXIT_CODE_STILL_ACTIVE = -2,
};

class CEasyThread :
	public CNameObject
{
protected:
	pthread_t		m_ThreadID;	
	volatile UINT	m_Status;
	volatile UINT	m_WantTerminate;

	DECLARE_CLASS_INFO_STATIC(CEasyThread)
public:
	CEasyThread();
	virtual ~CEasyThread();

	///启动线程
	BOOL Start(BOOL IsSuspended = FALSE,DWORD StartWaitTime=DEFAULT_THREAD_STARTUP_TIME);

	///继续线程
	BOOL Resume();

	/// 挂起线程
	BOOL Suspend();

	///终止线程
	void Terminate();

	//强制结束
	void ForceTerminate();
	void SafeTerminate(DWORD Milliseconds=DEFAULT_THREAD_TERMINATE_TIME);

	//线程是否被结束了
	BOOL IsTerminated();

	//线程是否被挂起
	BOOL IsSuspend();

	//线程是否运行中
	BOOL IsWorking();

	///获得线程状态
	int GetStatus();

	///等待线程开始工作
	BOOL WaitForWorking(DWORD Milliseconds=INFINITE);

	///等待线程退出
	BOOL WaitForTerminate(DWORD Milliseconds=INFINITE);


	HANDLE GetThreadHandle();
	UINT GetThreadID();

	BOOL SetThreadPriority(int Priority);
	int GetThreadPriority();

protected:
	virtual BOOL OnStart();
	virtual void Execute();
	virtual BOOL OnRun();
	virtual void OnTerminate();

	///线程例程
	static LPVOID ThreadProc(LPVOID pParam);
};

inline BOOL CEasyThread::IsTerminated()
{
	return m_Status==THREAD_STATUS_TERMINATED;
}
inline BOOL CEasyThread::IsSuspend()
{
	return m_Status==THREAD_STATUS_SUSPENDED;
}
inline BOOL CEasyThread::IsWorking()
{
	return m_Status==THREAD_STATUS_WORKING;
}
inline int CEasyThread::GetStatus()
{
	return m_Status;
}

inline HANDLE CEasyThread::GetThreadHandle()
{
	return (HANDLE)m_ThreadID;
}

inline UINT CEasyThread::GetThreadID()
{
	return m_ThreadID;
}