/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerAppLinux.h                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#include <signal.h>


class CServerApp
{
protected:
	volatile UINT				m_WantExist;
	CBaseServer *				m_pServer;
	static CLinuxFileAccessor	m_ExceptionLogFile;
	static int					m_Pipe[2];
public:
	CServerApp(void);
	~CServerApp(void);

public:

	virtual BOOL OnStartUp();
	virtual void OnShutDown();

	virtual int Run();

protected:
	void GetProgamVersion();
	void InitSignals();
	static void OnQuitSignal(int SignalNum, siginfo_t * pSigInfo, void * pContext);
	static void OnExceptionSignal(int SignalNum, siginfo_t * pSigInfo, void * pContext);
	static bool OutputExceptionAddress(const char * ModulaName, void * Address);
	static void PrintExceptionLog(const char * Format, ...);
	static bool InitSymbolParser();
};

extern bool g_IsService;