/****************************************************************************/
/*                                                                          */
/*      文件名:    AsyncFileLogPrinter.cpp                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

CAsyncFileLogPrinter::CAsyncFileLogPrinter()
{
	m_LogLevel=0;
}

CAsyncFileLogPrinter::CAsyncFileLogPrinter(int Level,LPCTSTR FileName,int FileLogQueueLen)
{
	ResetLog(Level,FileName,FileLogQueueLen);
}

CAsyncFileLogPrinter::~CAsyncFileLogPrinter(void)
{
	m_FileLogWorkThread.SafeTerminate();
}

void CAsyncFileLogPrinter::ResetLog(int Level,LPCTSTR FileName,int FileLogBufferLen)
{
	m_LogLevel=Level;
	m_LogFileName=FileName;

	CAutoLock Lock(m_EasyCriticalSection);

	m_FileLogWorkThread.Init(FileName,_T("log"),FileLogBufferLen);
	if(!m_FileLogWorkThread.IsWorking())
		m_FileLogWorkThread.Start();

}

void CAsyncFileLogPrinter::CloseLog()
{
	m_FileLogWorkThread.SafeTerminate();
}


void CAsyncFileLogPrinter::PrintLogVL(int Level,DWORD Color,LPCTSTR Format,va_list vl)
{
	try
	{
		if((Level&m_LogLevel)==0)
		{
			return;
		}
		TCHAR MsgBuff[5000];

		CEasyTime CurTime;
		CurTime.FetchLocalTime();

		if(Level==LOG_LEVEL_DEBUG)
		{
			_stprintf_s(MsgBuff,5000,_T("[%02u-%02u][%02u:%02u:%02u][D]"),
				CurTime.Month(),CurTime.Day(),
				CurTime.Hour(),CurTime.Minute(),CurTime.Second());
		}
		else
		{
			_stprintf_s(MsgBuff,5000,_T("[%02u-%02u][%02u:%02u:%02u][N]"),
				CurTime.Month(),CurTime.Day(),
				CurTime.Hour(),CurTime.Minute(),CurTime.Second());
		}


		_vstprintf_s(MsgBuff+20,4096-20,Format, vl );
		MsgBuff[4095]=0;
		_tcsncat_s(MsgBuff,5000,_T("\r\n"),4096);

		m_FileLogWorkThread.PushLog(MsgBuff);


	}
	catch(...)
	{
		PrintImportantLog(0,_T("Log[%s]输出发生异常[%s]"),(LPCTSTR)m_LogFileName,Format);
	}
}


void CAsyncFileLogPrinter::PrintLogDirect(int Level,DWORD Color,LPCTSTR szMsg)
{
	if((Level&m_LogLevel)==0)
	{
		return;
	}

	m_FileLogWorkThread.PushLog(szMsg);
}
