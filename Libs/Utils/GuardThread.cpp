/****************************************************************************/
/*                                                                          */
/*      文件名:    GuardThread.cpp                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CGuardThread,CEasyThread);

#define DEFAULT_GUARD_KEEPALIVE_TIME	(20*1000)
#define DEFAULT_MAX_LOST_ALIVE_COUNT	(6)

volatile bool CGuardThread::m_Enable = true;

void CGuardThread::Enable(bool Enable)
{
	m_Enable = Enable;
}

CGuardThread::CGuardThread(void)
{
	m_KeepAliveTime=DEFAULT_GUARD_KEEPALIVE_TIME;
	m_MaxLostAliveCount=DEFAULT_MAX_LOST_ALIVE_COUNT;
	m_TargetThreadID=0;
	m_TargetThreadHandle = NULL;
	m_RecentKeepAliveTime=CEasyTimer::GetTime();
	m_LostAliveCount=0;
}

CGuardThread::~CGuardThread(void)
{
}


BOOL CGuardThread::OnStart()
{	
	PrintImportantLog(_T("守护线程启动，目标线程[%u]"), m_TargetThreadID);
	return TRUE;
}

BOOL CGuardThread::OnRun()
{
	if (m_Enable)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		UINT CurTime=CEasyTimer::GetTime();
		if(GetTimeToTime(m_RecentKeepAliveTime,CurTime)>m_KeepAliveTime)
		{


			m_RecentKeepAliveTime=CurTime;
			m_LostAliveCount++;
			PrintImportantLog(_T("检测到线程[%u]失去响应,第%d次"),m_TargetThreadID,m_LostAliveCount);
			if(m_LostAliveCount>m_MaxLostAliveCount)
			{
				PrintImportantLog(_T("检测到线程[%u]死锁"),m_TargetThreadID);
				DumpThreadCallStack();
				assert(false);
			}
		}
	}
	DoSleep(100);
	return TRUE;
}

void CGuardThread::OnTerminate()
{

}

void CGuardThread::PrintDumpLog(IFileAccessor * pLogFile, LPCTSTR Format, ...)
{
	va_list	vl;
	va_start(vl, Format);
	TCHAR Buff[4096];
	int Len = _stprintf_s(Buff, 4096, Format, vl);
	_tcsncat_s(Buff, 4096, _T("\r\n"), _TRUNCATE);
	Len += 2;
	pLogFile->Write(Buff, Len);
	va_end(vl);
}

void CGuardThread::DumpThreadCallStack()
{
#ifndef WIN32
	void * CallStacks[20];
	int CallStackSize = CEasyThread::GetCallStack(m_TargetThreadHandle, CallStacks, 20);

	PrintImportantLog("call stack size:%d", CallStackSize);
	char ExceptionFileName[MAX_PATH];
	char ExeFilePath[MAX_PATH];


	//先在重要日志输出一遍原始地址
	for (int i = 0; i < CallStackSize; i++)
	{
		PrintImportantLog("%d:RawAddress:%p", i, CallStacks[i]);

		Dl_info DLInfo;
		if (dladdr(CallStacks[i], &DLInfo))
		{
			PrintImportantLog("BaseAddress:%p at %s", DLInfo.dli_fbase, DLInfo.dli_fname);
		}
	}

	//恢复对SIGCHLD的屏蔽
	signal(SIGCHLD, SIG_DFL);

	sprintf_s(ExceptionFileName, MAX_PATH, "/proc/%d/exe", getpid());
	int Len = readlink(ExceptionFileName, ExeFilePath, MAX_PATH);
	if (Len > 0)
	{
		CEasyTime CurTime;
		CurTime.FetchLocalTime();

		sprintf_s(ExceptionFileName, MAX_PATH, "%s.CallStack.%d-%02d-%02d_%02d-%02d-%02d.log",
			ExeFilePath, CurTime.Year(), CurTime.Month(), CurTime.Day(), CurTime.Hour(), CurTime.Minute(), CurTime.Second());

		CLinuxFileAccessor LogFile;
		if (LogFile.Open(ExceptionFileName, IFileAccessor::modeCreateAlways | IFileAccessor::modeWrite | IFileAccessor::osWriteThrough))
		{
			PrintDumpLog(&LogFile, "ThreadID:%u", m_TargetThreadID);
			//输出原始地址
			for (int i = 0; i < CallStackSize; i++)
			{


				Dl_info DLInfo;
				if (dladdr(CallStacks[i], &DLInfo))
				{
					if ((UINT64)DLInfo.dli_fbase == 0x400000)
					{
						PrintDumpLog(&LogFile, "%d:RawAddress:%p,ModuleAddress:%p,ModuleBase:%p at %s",
							i, CallStacks[i], CallStacks[i], DLInfo.dli_fbase, DLInfo.dli_fname);
					}
					else
					{
						PrintDumpLog(&LogFile, "%d:RawAddress:%p,ModuleAddress:%p,ModuleBase:%p at %s",
							i, CallStacks[i], (void *)((char *)CallStacks[i] - (char *)DLInfo.dli_fbase), DLInfo.dli_fbase, DLInfo.dli_fname);
					}
				}
				else
				{
					PrintDumpLog(&LogFile, "%d:RawAddress:%p", i, CallStacks[i]);
				}
				PrintDumpLog(&LogFile, "-------------------------------------------------------\r\n");
			}

			int CallPipe[2];
			if (pipe(CallPipe) == 0)
			{
				char Buff[1024];

				for (int i = 0; i < CallStackSize; i++)
				{
					PrintDumpLog(&LogFile, "%d:RawAddress:%p", i, CallStacks[i]);

					Dl_info DLInfo;
					if (dladdr(CallStacks[i], &DLInfo))
					{
						PrintDumpLog(&LogFile, "ModuleBase:%p at %s", DLInfo.dli_fbase, DLInfo.dli_fname);
						if ((UINT64)DLInfo.dli_fbase == 0x400000)
						{
							if (Addr2Line(DLInfo.dli_fname, CallStacks[i], CallPipe, Buff, 1024))
							{
								PrintDumpLog(&LogFile, "%s:%s", DLInfo.dli_fname, Buff);
							}
						}
						else
						{
							if (Addr2Line(DLInfo.dli_fname, (void *)((char *)CallStacks[i] - (char *)DLInfo.dli_fbase), CallPipe, Buff, 1024))
							{
								PrintDumpLog(&LogFile, "%s:%s", DLInfo.dli_fname, Buff);
							}
						}
					}
					PrintDumpLog(&LogFile, "-------------------------------------------------------\r\n");
				}
			}
			else
			{
				PrintImportantLog("创建输出管道失败%d", errno);
			}

			LogFile.Close();
		}
		else
		{
			PrintImportantLog("无法创建文件%s", ExceptionFileName);
		}
	}
	else
	{
		PrintImportantLog("无法获取进程映像文件名");
	}

	PrintImportantLog("输出调用栈完毕");
#endif
}