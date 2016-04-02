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
			g_IsService = true;
		}
	}


	m_gAppInstance->Run();
	return 0;
}




CServerApp::CServerApp(void)
{
	m_WantExist=FALSE;
	m_pServer=NULL;
	m_gAppInstance=this;
}

CServerApp::~CServerApp(void)
{
}



BOOL CServerApp::OnStartUp()
{
	InitSignals();
	return TRUE;
}
void CServerApp::OnShutDown()
{
	if (m_pServer)
		m_pServer->QueryShowDown();
}

int CServerApp::Run()
{
	if(OnStartUp())
	{
		while ((!m_WantExist) && m_pServer && (!m_pServer->IsServerTerminated()))
		{
			DoSleep(1);
		}
		OnShutDown();
	}
	return 0;
}

void CServerApp::InitSignals()
{
	struct sigaction SigAction;

	SigAction.sa_flags = 0;
	SigAction.sa_sigaction = OnQuitSignal;
	if (sigaction(SIGTERM, &SigAction, NULL) != 0)
	{
		PrintImportantLog(0, "注册信号SIGTERM处理失败%d", errno);
	}
	else
	{
		PrintImportantLog(0, "注册信号SIGTERM处理");
	}

	SigAction.sa_flags = SA_RESETHAND;
	sigaddset(&SigAction.sa_mask, SIGABRT);
	sigaddset(&SigAction.sa_mask, SIGFPE);
	sigaddset(&SigAction.sa_mask, SIGSEGV);
	sigaddset(&SigAction.sa_mask, SIGILL);
	SigAction.sa_sigaction = OnExceptionSignal;
	if (sigaction(SIGABRT, &SigAction, NULL) != 0)
	{
		PrintImportantLog(0, "注册信号SIGABRT处理失败%d", errno);
	}
	else
	{
		PrintImportantLog(0, "注册信号SIGABRT处理");
	}

	if (sigaction(SIGFPE, &SigAction, NULL) != 0)
	{
		PrintImportantLog(0, "注册信号SIGFPE处理失败%d", errno);
	}
	else
	{
		PrintImportantLog(0, "注册信号SIGFPE处理");
	}

	if (sigaction(SIGSEGV, &SigAction, NULL) != 0)
	{
		PrintImportantLog(0, "注册信号SIGSEGV处理失败%d", errno);
	}
	else
	{
		PrintImportantLog(0, "注册信号SIGSEGV处理");
	}

	if (sigaction(SIGILL, &SigAction, NULL) != 0)
	{
		PrintImportantLog(0, "注册信号SIGILL处理失败%d", errno);
	}
	else
	{
		PrintImportantLog(0, "注册信号SIGILL处理");
	}
}


void CServerApp::OnQuitSignal(int SignalNum, siginfo_t * pSigInfo, void * pContext)
{
	PrintImportantLog(0, "收到退出信号%d，准备退出", SignalNum);
	if (m_gAppInstance)
		m_gAppInstance->m_WantExist = TRUE;
}

void CServerApp::OnExceptionSignal(int SignalNum, siginfo_t * pSigInfo, void * pContext)
{	
	void *array[MAX_STACK_LAYERS];

	PrintImportantLog(0, "开始处理信号%d", SignalNum);
	size_t size = backtrace(array, MAX_STACK_LAYERS);
	PrintImportantLog(0, "输出调用栈大小:%d", (int)size);
	CFileLogPrinter * pLog = dynamic_cast<CFileLogPrinter *>(CLogManager::GetInstance()->GetChannel(LOG_IMPORTANT_CHANNEL));
	if (pLog)
	{
		CLinuxFileAccessor * pFile = dynamic_cast<CLinuxFileAccessor *>(pLog->GetFile());
		if (pFile)
		{
			backtrace_symbols_fd(array, MAX_STACK_LAYERS, pFile->GetFileDescriptor());
		}
	}
	
	//char ** strings = backtrace_symbols(array, size);
	//for (size_t i = 0; i < size; i++)
	//{
	//	PrintImportantLog(0, "StackTrace:%s", strings[i]);
	//}
	//free(strings);

	PrintImportantLog(0, "输出调用栈完毕");
}
