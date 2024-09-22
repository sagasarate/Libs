/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerAppLinux.cpp                                       */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

#include <execinfo.h>
#include <malloc.h>
#include <sys/wait.h>
#include  <termios.h>


#define MAX_STACK_LAYERS    20

CServerApp	* m_gAppInstance=NULL;

bool g_IsService = false;



int main(int argc, char *argv[])
{
	if (argc>1)
	{
		if (strcmp(argv[1], "-d") == 0)
		{
			daemon(1, 1);
			PrintImportantLog("已启用Daemon");
			g_IsService = true;
		}
	}

	//mallopt(M_ARENA_MAX, 1);


	m_gAppInstance->Run();
	return 0;
}


CLinuxFileAccessor CServerApp::m_ExceptionLogFile;
int	CServerApp::m_Pipe[2] = { 0, 0 };

CServerApp::CServerApp(void)
{
	m_WantExist=false;
	m_pServer=NULL;
	m_gAppInstance=this;
}

CServerApp::~CServerApp(void)
{
}



bool CServerApp::OnStartUp()
{
	InitSignals();	
	return true;
}
void CServerApp::OnShutDown()
{
	if (m_pServer)
		m_pServer->QueryShowDown();
}


int kbhit(void)
{
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);
}

int CServerApp::Run()
{
	char KeyBuffer[2048];
	int BufferPtr = 0;
	if(OnStartUp())
	{
		//struct termios oldt, newt;

		//tcgetattr(STDIN_FILENO, &oldt);
		//newt = oldt;
		//newt.c_lflag &= ~(ICANON);
		//tcsetattr(STDIN_FILENO, TCSANOW, &newt);


		while ((!m_WantExist) && m_pServer && (!m_pServer->IsServerTerminated()))
		{
			//经常收到异常字符，暂时关闭控制台命令输入功能
			//if (kbhit())
			//{
			//	KeyBuffer[BufferPtr++] = getchar();
			//	if (KeyBuffer[BufferPtr - 1] == '\n' || BufferPtr >= 2000)
			//	{
			//		//输入了回车或者缓冲满了，提交命令
			//		KeyBuffer[BufferPtr - 1] = 0;
			//		m_pServer->PushConsoleCmd(KeyBuffer);
			//		BufferPtr = 0;
			//	}
			//}
			DoSleep(100);
		}
		OnShutDown();
	}

	CSystemConfig::ReleaseInstance();
	CPerformanceStatistician::ReleaseInstance();
	CLogManager::ReleaseInstance();
	CFileSystemManager::ReleaseInstance();
#ifdef USE_MONITORED_NEW
	CMemoryAllocatee::GetInstance()->ReportLeakStat();
	CMemoryAllocatee::ReleaseInstance();
#endif
	return 0;
}

void CServerApp::InitSignals()
{
	struct sigaction SigAction;

	signal(SIGCHLD, SIG_IGN);
	PrintImportantLog("已忽略SIGCHLD处理");

	signal(SIGPIPE, SIG_IGN);
	PrintImportantLog("已忽略SIGPIPE处理");

	//屏蔽此信号会导致mono gc死锁
	//signal(SIGPWR, SIG_IGN);
	//PrintImportantLog("已忽略SIGPWR处理");

	ZeroMemory(&SigAction, sizeof(SigAction));
	SigAction.sa_flags = 0;
	SigAction.sa_sigaction = OnQuitSignal;
	if (sigaction(SIGTERM, &SigAction, NULL) != 0)
	{
		PrintImportantLog("注册信号SIGTERM处理失败%d", errno);
	}
	else
	{
		PrintImportantLog( "注册信号SIGTERM处理");
	}

	SigAction.sa_flags = SA_RESETHAND;
	sigaddset(&SigAction.sa_mask, SIGABRT);
	sigaddset(&SigAction.sa_mask, SIGFPE);
	sigaddset(&SigAction.sa_mask, SIGSEGV);
	sigaddset(&SigAction.sa_mask, SIGILL);
	SigAction.sa_sigaction = OnExceptionSignal;
	if (sigaction(SIGABRT, &SigAction, NULL) != 0)
	{
		PrintImportantLog("注册信号SIGABRT处理失败%d", errno);
	}
	else
	{
		PrintImportantLog("注册信号SIGABRT处理");
	}

	if (sigaction(SIGFPE, &SigAction, NULL) != 0)
	{
		PrintImportantLog("注册信号SIGFPE处理失败%d", errno);
	}
	else
	{
		PrintImportantLog("注册信号SIGFPE处理");
	}

	if (sigaction(SIGSEGV, &SigAction, NULL) != 0)
	{
		PrintImportantLog("注册信号SIGSEGV处理失败%d", errno);
	}
	else
	{
		PrintImportantLog("注册信号SIGSEGV处理");
	}

	if (sigaction(SIGILL, &SigAction, NULL) != 0)
	{
		PrintImportantLog("注册信号SIGILL处理失败%d", errno);
	}
	else
	{
		PrintImportantLog("注册信号SIGILL处理");
	}
}


void CServerApp::OnQuitSignal(int SignalNum, siginfo_t * pSigInfo, void * pContext)
{
	PrintImportantLog("收到退出信号%d，准备退出", SignalNum);
	if (m_gAppInstance)
		m_gAppInstance->m_WantExist = true;
}

void CServerApp::OnExceptionSignal(int SignalNum, siginfo_t * pSigInfo, void * pContext)
{
	void * CallStacks[MAX_STACK_LAYERS];

	//关闭守护线程检测
	CGuardThread::Enable(false);

	if(pSigInfo)
		PrintImportantLog("开始处理信号%d在线程%u的0x%p", SignalNum, pthread_self(), pSigInfo->si_addr);
	else
		PrintImportantLog("开始处理信号%d在线程%u", SignalNum, pthread_self());
	size_t CallStackSize = backtrace(CallStacks, MAX_STACK_LAYERS);
	PrintImportantLog("输出调用栈大小:%d", (int)CallStackSize);
	char ExceptionFileName[MAX_PATH + 1];
	char ExeFilePath[MAX_PATH + 1];

	//恢复对SIGCHLD的屏蔽
	signal(SIGCHLD, SIG_DFL);

	//先在重要日志输出一遍原始地址
	for (size_t i = 0; i < CallStackSize; i++)
	{
		PrintImportantLog("%d:RawAddress:%p", i, CallStacks[i]);

		Dl_info DLInfo;
		if (dladdr(CallStacks[i], &DLInfo))
		{
			PrintImportantLog("BaseAddress:%p at %s", DLInfo.dli_fbase, DLInfo.dli_fname);
		}
	}

	sprintf_s(ExceptionFileName, MAX_PATH, "/proc/%d/exe", getpid());
	int Len = readlink(ExceptionFileName, ExeFilePath, MAX_PATH);
	if (Len > 0)
	{
		ExeFilePath[Len] = 0;
		CEasyTime CurTime;
		CurTime.FetchLocalTime();

		sprintf_s(ExceptionFileName, MAX_PATH, "%s.Exception.%d-%02d-%02d_%02d-%02d-%02d.log",
			ExeFilePath, CurTime.Year(), CurTime.Month(), CurTime.Day(), CurTime.Hour(), CurTime.Minute(), CurTime.Second());

		if (m_ExceptionLogFile.Open(ExceptionFileName, IFileAccessor::modeCreateAlways | IFileAccessor::modeWrite | IFileAccessor::osWriteThrough))
		{
			PrintExceptionLog("SignalCode:%d at 0x%p", SignalNum, pSigInfo->si_addr);
			//输出原始地址
			for (size_t i = 0; i < CallStackSize; i++)
			{
				

				Dl_info DLInfo;
				if (dladdr(CallStacks[i], &DLInfo))
				{
					if ((UINT64)DLInfo.dli_fbase == 0x400000)
					{
						PrintExceptionLog("%d:RawAddress:%p,ModuleAddress:%p,ModuleBase:%p at %s",
							i, CallStacks[i], CallStacks[i], DLInfo.dli_fbase, DLInfo.dli_fname);
					}
					else
					{
						PrintExceptionLog("%d:RawAddress:%p,ModuleAddress:%p,ModuleBase:%p at %s",
							i, CallStacks[i], (void *)((char *)CallStacks[i] - (char *)DLInfo.dli_fbase), DLInfo.dli_fbase, DLInfo.dli_fname);
					}					
				}
				else
				{
					PrintExceptionLog("%d:RawAddress:%p", i, CallStacks[i]);
				}
				PrintExceptionLog("-------------------------------------------------------\r\n");
			}

			InitSymbolParser();
			for (size_t i = 0; i < CallStackSize; i++)
			{
				PrintExceptionLog("%d:RawAddress:%p", i, CallStacks[i]);

				Dl_info DLInfo;
				if (dladdr(CallStacks[i], &DLInfo))
				{
					PrintExceptionLog("ModuleBase:%p at %s", DLInfo.dli_fbase, DLInfo.dli_fname);
					if ((UINT64)DLInfo.dli_fbase == 0x400000)
						OutputExceptionAddress(DLInfo.dli_fname, CallStacks[i]);
					else
						OutputExceptionAddress(DLInfo.dli_fname, (void *)((char *)CallStacks[i] - (char *)DLInfo.dli_fbase));
				}

				PrintExceptionLog("-------------------------------------------------------\r\n");
			}

			m_ExceptionLogFile.Close();
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
}

bool CServerApp::OutputExceptionAddress(const char * ModulaName, void * Address)
{
	char Buff[1024];
	if (Addr2Line(ModulaName, Address, m_Pipe, Buff, 1024))
	{
		PrintExceptionLog("%s:%s", ModulaName, Buff);
		return true;
	}
	else
	{
		PrintExceptionLog("Parser Error");
		return false;
	}
}

void CServerApp::PrintExceptionLog(const char * Format, ...)
{
	va_list	vl;
	va_start(vl, Format);
	char Buff[4096];
	int Len = vsprintf_s(Buff, 4096, Format, vl);
	strcat_s(Buff, 4096, _T("\r\n"));
	Len += 2;
	m_ExceptionLogFile.Write(Buff, Len);
	va_end(vl);
}
bool CServerApp::InitSymbolParser()
{
	if (pipe(m_Pipe) == 0)
	{
		return true;
	}
	else
	{
		PrintImportantLog("创建输出管道失败%d", errno);
		return false;
	}
}
