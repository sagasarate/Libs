/****************************************************************************/
/*                                                                          */
/*      文件名:    FileLogPrinter.cpp                                       */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

CFileLogPrinter::CFileLogPrinter()
{
	m_Flag=0;
	m_pFileAccessor=NULL;
	m_FileOpenMode=0;
	m_LogLevel=0;

}

CFileLogPrinter::CFileLogPrinter(LPCTSTR FileName,DWORD Flag)
{
	Create(FileName,Flag);
}

bool CFileLogPrinter::Create(LPCTSTR FileName,DWORD Flag)
{
	CEasyString LogFileName;

	m_pFileAccessor=CFileSystemManager::GetInstance()->CreateFileAccessor(FILE_CHANNEL_NORMAL1);
	if(m_pFileAccessor==NULL)
		return false;

#ifdef _DEBUG
	m_LogLevel=ILogPrinter::LOG_LEVEL_DEBUG|ILogPrinter::LOG_LEVEL_NORMAL;
#else
	m_LogLevel=ILogPrinter::LOG_LEVEL_NORMAL;
#endif

	m_Flag=Flag;
	m_LogFileName=FileName;
	if(m_Flag&FILE_LOG_SPLIT_BY_DAY)
	{
		m_RecentLogTime.FetchLocalTime();
		LogFileName.Format(_T("%s.%u-%02u-%02u.log"),
			(LPCTSTR)m_LogFileName,
			m_RecentLogTime.Year(),
			m_RecentLogTime.Month(),
			m_RecentLogTime.Day());
	}
	else
	{
		LogFileName.Format(_T("%s.log"),
			(LPCTSTR)m_LogFileName);
	}
	if(m_Flag&FILE_LOG_APPEND)
		m_FileOpenMode=IFileAccessor::modeOpenAlways|IFileAccessor::modeAppend;
	else
		m_FileOpenMode=IFileAccessor::modeCreateAlways;
	m_FileOpenMode|=IFileAccessor::modeWrite|IFileAccessor::shareShareRead;
	if(m_Flag&FILE_LOG_SAFE_WRITE)
		m_FileOpenMode|=IFileAccessor::osWriteThrough;
	return m_pFileAccessor->Open(LogFileName,m_FileOpenMode)==TRUE;
}

CFileLogPrinter::~CFileLogPrinter(void)
{
	SAFE_RELEASE(m_pFileAccessor);
}

void CFileLogPrinter::PrintLogVL(int Level,DWORD Color,LPCTSTR Format,va_list vl)
{
	CAutoLock Lock(m_EasyCriticalSection);

	if((m_LogLevel&Level)==0)
		return;

	if(m_pFileAccessor==NULL)
		return;

	CEasyTime CurTime;
	CurTime.FetchLocalTime();
	if(m_Flag&FILE_LOG_SPLIT_BY_DAY)
	{
		if(!CEasyTime::IsSameDate(m_RecentLogTime,CurTime))
		{
			CEasyString LogFileName;

			m_pFileAccessor->Close();
			m_RecentLogTime=CurTime;
			LogFileName.Format(_T("%s.%u-%02u-%02u.log"),
				(LPCTSTR)m_LogFileName,
				m_RecentLogTime.Year(),
				m_RecentLogTime.Month(),
				m_RecentLogTime.Day());
			m_pFileAccessor->Open(LogFileName,m_FileOpenMode);
			if(m_Flag&FILE_LOG_APPEND)
				m_pFileAccessor->Seek(0,IFileAccessor::seekEnd);
		}
	}
	CurTime.Format(m_MsgBuff,40960,_T("[%m-%d %H:%M:%S]:"));

	m_pFileAccessor->Write(m_MsgBuff,_tcslen(m_MsgBuff));
	_vstprintf_s( m_MsgBuff,40960, Format, vl );
	m_pFileAccessor->Write(m_MsgBuff,_tcslen(m_MsgBuff));
	m_pFileAccessor->Write(_T("\r\n"),2);

}
