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

void CCSVFileLogPrinter::PrintLogVL(int Level,DWORD Color,LPCTSTR Format,va_list vl)
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


		_stprintf_s(MsgBuff,5000,_T("\"%02d-%02d %02d:%02d:%02d\","),
			CurTime.Month(),CurTime.Day(),
			CurTime.Hour(),CurTime.Minute(),CurTime.Second());


		_vstprintf_s(MsgBuff+17,4096-17,Format, vl );
		MsgBuff[4095]=0;
		_tcsncat_s(MsgBuff,5000,_T("\r\n"),4096);

		m_FileLogWorkThread.PushLog(MsgBuff);


	}
	catch(...)
	{
		PrintImportantLog(0,_T("Log[%s]输出发生异常[%s]"),(LPCTSTR)m_LogFileName,Format);
	}
}
