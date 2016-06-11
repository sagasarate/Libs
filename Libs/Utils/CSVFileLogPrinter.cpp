#include "stdafx.h"


CCSVFileLogPrinter::CCSVFileLogPrinter(void)
{
}

CCSVFileLogPrinter::~CCSVFileLogPrinter(void)
{
}

bool CCSVFileLogPrinter::Init(int Level,LPCTSTR FileName,LPCTSTR LogHeader,int FileLogBufferLen)
{
	m_LogLevel=Level;
	m_LogFileName=FileName;

	CAutoLock Lock(m_EasyCriticalSection);

	m_FileLogWorkThread.SetLogFileHeaderString(LogHeader);
	m_FileLogWorkThread.Init(FileName,_T("csv"),FileLogBufferLen);
	if(!m_FileLogWorkThread.IsWorking())
		m_FileLogWorkThread.Start();

	return true;
}
void CCSVFileLogPrinter::PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg)
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


		_stprintf_s(MsgBuff, 5000, _T("\"%02u-%02u %02u:%02u:%02u\","),
			CurTime.Month(), CurTime.Day(),
			CurTime.Hour(), CurTime.Minute(), CurTime.Second());

		if (Tag)
		{
			_tcsncat_s(MsgBuff, 5000, Tag, _TRUNCATE);			
		}
		_tcsncat_s(MsgBuff, 5000, _T(","), _TRUNCATE);
		
		_tcsncat_s(MsgBuff, 5000, Msg, _TRUNCATE);
		_tcsncat_s(MsgBuff, 5000, _T("\r\n"), _TRUNCATE);

		m_FileLogWorkThread.PushLog(MsgBuff);


	}
	catch (...)
	{
		PrintImportantLog(0, _T("Log[%s]输出发生异常[%s]"), (LPCTSTR)m_LogFileName, Msg);
	}
}
void CCSVFileLogPrinter::PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl)
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


		_stprintf_s(MsgBuff,5000,_T("\"%02u-%02u %02u:%02u:%02u\","),
			CurTime.Month(),CurTime.Day(),
			CurTime.Hour(),CurTime.Minute(),CurTime.Second());

		if (Tag)
		{
			_tcsncat_s(MsgBuff, 5000, Tag, _TRUNCATE);
		}
		_tcsncat_s(MsgBuff, 5000, _T(","), _TRUNCATE);

		size_t PrefixLen = _tcslen(MsgBuff);

		_vstprintf_s(MsgBuff + PrefixLen, 4096 - PrefixLen, Format, vl);
		MsgBuff[4095]=0;
		_tcsncat_s(MsgBuff, 5000, _T("\r\n"), _TRUNCATE);

		m_FileLogWorkThread.PushLog(MsgBuff);


	}
	catch(...)
	{
		PrintImportantLog(0,_T("Log[%s]输出发生异常[%s]"),(LPCTSTR)m_LogFileName,Format);
	}
}
