#include "stdafx.h"

const LPCTSTR DEFULT_HEADER_STRING=_T("Time,Tag,");

CCSVFileLogPrinter::CCSVFileLogPrinter(void)
{
}
CCSVFileLogPrinter::CCSVFileLogPrinter(int Level, LPCTSTR LogName, LPCTSTR HeaderStr, int FileLogBufferLen)
	:CAsyncFileLogPrinter(Level, LogName, FileLogBufferLen, _T("csv"), HeaderStr ? CEasyString(DEFULT_HEADER_STRING) + HeaderStr : _T(""))
{
	
}

CCSVFileLogPrinter::~CCSVFileLogPrinter(void)
{
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

		PushLog(MsgBuff);


	}
	catch (...)
	{
		PrintImportantLog(_T("Log[%s]输出发生异常[%s]"), (LPCTSTR)m_LogName, Msg);
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

		PushLog(MsgBuff);


	}
	catch(...)
	{
		PrintImportantLog(_T("Log[%s]输出发生异常[%s]"),(LPCTSTR)m_LogName,Format);
	}
}
