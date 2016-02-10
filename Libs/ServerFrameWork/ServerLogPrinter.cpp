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

CServerLogPrinter::CServerLogPrinter(IBaseServer * pServer,UINT Mode,int Level,LPCTSTR FileName,int FileLogQueueLen)
{
	m_pServer=pServer;
	SetLogMode(Mode,Level,FileName,FileLogQueueLen);
}

CServerLogPrinter::~CServerLogPrinter(void)
{
}

void CServerLogPrinter::SetLogMode(UINT Mode,int Level,LPCTSTR FileName,int FileLogBufferLen)
{
	m_LogOutputMode=Mode;

	if(m_LogOutputMode==0)
		return;

	if(m_LogOutputMode&LOM_FILE)
	{
		ResetLog(Level,FileName,FileLogBufferLen);
	}
	else
	{
		CloseLog();
	}
}


void CServerLogPrinter::PrintLogVL(int Level,DWORD Color,LPCTSTR Format,va_list vl)
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
			_stprintf_s(MsgBuff,5000,_T("[%02d-%02d][%02d:%02d:%02d][D]"),
				CurTime.Month(),CurTime.Day(),
				CurTime.Hour(),CurTime.Minute(),CurTime.Second());
		}
		else
		{
			_stprintf_s(MsgBuff,5000,_T("[%02d-%02d][%02d:%02d:%02d][N]"),
				CurTime.Month(),CurTime.Day(),
				CurTime.Hour(),CurTime.Minute(),CurTime.Second());
		}


		vsprintf_s(MsgBuff+20,4096-20,Format, vl );
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
			printf("%s",MsgBuff+7);
#endif
		}

		if(m_LogOutputMode&LOM_FILE)
		{
			m_FileLogWorkThread.PushLog(MsgBuff);
		}

	}
	catch(...)
	{
		PrintImportantLog(0,"Log[%s]输出发生异常[%s]",(LPCTSTR)m_LogFileName,Format);
	}
}

