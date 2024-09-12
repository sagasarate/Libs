/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerLogPrinter.cpp                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

CServerLogPrinter::CServerLogPrinter(CBaseServer * pServer, UINT Mode, int Level, LPCTSTR LogName, int FileLogBufferLen)
	:CAsyncFileLogPrinter(Level, LogName, FileLogBufferLen, NULL, NULL)
{
	m_pServer=pServer;
	SetLogMode(Mode);
}

CServerLogPrinter::~CServerLogPrinter(void)
{
}



void CServerLogPrinter::PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg)
{
	try
	{
		if ((Level&m_LogLevel) == 0)
		{
			return;
		}

		TCHAR MsgBuff[5000];

		CEasyTime CurTime;
		CurTime.FetchLocalTime();

		if (Level == LOG_LEVEL_DEBUG)
		{
			_stprintf_s(MsgBuff, 5000, _T("[%02u-%02u][%02u:%02u:%02u][D]"),
				CurTime.Month(), CurTime.Day(),
				CurTime.Hour(), CurTime.Minute(), CurTime.Second());
		}
		else
		{
			_stprintf_s(MsgBuff, 5000, _T("[%02u-%02u][%02u:%02u:%02u][N]"),
				CurTime.Month(), CurTime.Day(),
				CurTime.Hour(), CurTime.Minute(), CurTime.Second());
		}
		if (Tag)
		{
			_tcsncat_s(MsgBuff, 5000, _T("["), _TRUNCATE);
			_tcsncat_s(MsgBuff, 5000, Tag, _TRUNCATE);
			_tcsncat_s(MsgBuff, 5000, _T("]"), _TRUNCATE);
		}
		_tcsncat_s(MsgBuff, 5000, Msg, _TRUNCATE);

		if ((m_LogOutputMode&LOM_CONSOLE) && m_pServer)
		{
			m_pServer->PrintConsoleLog(Level, MsgBuff + 7);
		}

		_tcsncat_s(MsgBuff, 5000, _T("\r\n"), _TRUNCATE);

		if (m_LogOutputMode&LOM_VS)
		{
#ifdef WIN32
			OutputDebugString(MsgBuff + 7);
#else
			if(!g_IsService)
				printf("%s", MsgBuff + 7);
#endif
		}

		if (m_LogOutputMode&LOM_FILE)
		{
			PushLog(MsgBuff);
		}
	}
	catch (...)
	{
		PrintImportantLog( _T("Log[%s]输出发生异常"), (LPCTSTR)m_LogName, Msg);
	}
}


void CServerLogPrinter::PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl)
{
	try
	{
		if((Level&m_LogLevel)==0)
		{
			return;
		}
		char MsgBuff[5000];

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
		if (Tag)
		{
			_tcsncat_s(MsgBuff, 5000, _T("["), _TRUNCATE);
			_tcsncat_s(MsgBuff, 5000, Tag, _TRUNCATE);
			_tcsncat_s(MsgBuff, 5000, _T("]"), _TRUNCATE);
		}
		size_t PrefixLen = _tcslen(MsgBuff);
		vsprintf_s(MsgBuff + PrefixLen, 4096 - PrefixLen, Format, vl);
		MsgBuff[4095]=0;


		if((m_LogOutputMode&LOM_CONSOLE)&&m_pServer)
		{
			m_pServer->PrintConsoleLog(Level,MsgBuff+7);
		}

		strncat_s(MsgBuff,5000,"\r\n",4096);

		if(m_LogOutputMode&LOM_VS)
		{
#ifdef WIN32
			OutputDebugString(MsgBuff+7);
#else
			if(!g_IsService)
				printf("%s",MsgBuff+7);
#endif
		}

		if(m_LogOutputMode&LOM_FILE)
		{
			PushLog(MsgBuff);
		}

	}
	catch(...)
	{
		PrintImportantLog("Log[%s]输出发生异常[%s]",(LPCTSTR)m_LogName,Format);
	}
}

