/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyThreadWin.h                                          */
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
	ET_PRIORITY_TIME_CRITICAL = THREAD_PRIORITY_TIME_CRITICAL,
	ET_PRIORITY_HIGHEST = THREAD_PRIORITY_HIGHEST,
	ET_PRIORITY_ABOVE_NORMA = THREAD_PRIORITY_ABOVE_NORMAL,
	ET_PRIORITY_NORMAL = THREAD_PRIORITY_NORMAL,
	ET_PRIORITY_BELOW_NORMA = THREAD_PRIORITY_BELOW_NORMAL,
	ET_PRIORITY_LOWEST = THREAD_PRIORITY_LOWEST,
	ET_PRIORITY_IDLE = THREAD_PRIORITY_IDLE,
	ET_PRIORITY_ERROR_RETURN = THREAD_PRIORITY_ERROR_RETURN,
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
	UINT			m_ThreadID;
	HANDLE			m_hThread;
	volatile UINT	m_Status;
	volatile bool	m_WantTerminate;




	DECLARE_CLASS_INFO_STATIC(CEasyThread)
public:
	CEasyThread();
	virtual ~CEasyThread();

	///启动线程
	bool Start(bool IsSuspended = false, DWORD StartWaitTime = DEFAULT_THREAD_STARTUP_TIME);

	///继续线程
	bool Resume();

	/// 挂起线程
	bool Suspend();

	///终止线程
	void Terminate();

	//强制结束
	void ForceTerminate();
	void SafeTerminate(DWORD Milliseconds = DEFAULT_THREAD_TERMINATE_TIME);

	//线程是否被结束了
	bool IsTerminated();

	//线程是否被挂起
	bool IsSuspend();

	//线程是否运行中
	bool IsWorking();

	///获得线程状态
	int GetStatus();

	///等待线程开始工作
	bool WaitForWorking(DWORD Milliseconds = INFINITE);

	///等待线程退出
	bool WaitForTerminate(DWORD Milliseconds = INFINITE);


	HANDLE GetThreadHandle();
	UINT GetThreadID();

	bool SetThreadPriority(int Priority);
	int GetThreadPriority();

	int GetExitCode();

protected:
	virtual bool OnStart();
	virtual void Execute();
	virtual bool OnRun();
	virtual void OnTerminate();


	///线程例程
	static UINT __stdcall ThreadProc(LPVOID pParam);
};

inline bool CEasyThread::IsTerminated()
{
	if (m_Status == THREAD_STATUS_TERMINATED)
	{
		return true;
	}
	else
	{
		if (GetExitCode() != THREAD_EXIT_CODE_STILL_ACTIVE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
inline bool CEasyThread::IsSuspend()
{
	return m_Status == THREAD_STATUS_SUSPENDED;
}
inline bool CEasyThread::IsWorking()
{
	return m_Status == THREAD_STATUS_WORKING;
}
inline int CEasyThread::GetStatus()
{
	return m_Status;
}

inline HANDLE CEasyThread::GetThreadHandle()
{
	return m_hThread;
}

inline UINT CEasyThread::GetThreadID()
{
	return m_ThreadID;
}


inline UINT GetCurThreadID()
{
	return (UINT)GetCurrentThreadId();
}