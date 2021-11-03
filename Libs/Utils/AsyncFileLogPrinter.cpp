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
	m_FileExt = "log";
	m_RecentLogTime.FetchLocalTime();
	m_pLogFile = NULL;
	m_LogDataBuffer.SetTag(_T("CAsyncFileLogPrinter"));
	m_LogBackupDir = _T("OldLog");
	m_LogBackupDelay = 0;
}

CAsyncFileLogPrinter::CAsyncFileLogPrinter(int Level, LPCTSTR LogName, int FileLogBufferLen, LPCTSTR FileExt, LPCTSTR HeaderStr)
{
	m_LogLevel = Level;
	m_LogName = LogName;
	if (FileExt)
		m_FileExt = FileExt;
	else
		m_FileExt = "log";
	m_HeaderString = HeaderStr;
	m_RecentLogTime.FetchLocalTime();
	m_pLogFile = NULL;
	m_LogDataBuffer.SetTag(_T("CAsyncFileLogPrinter"));
	m_LogBackupDir = _T("OldLog");
	m_LogBackupDelay = 0;
	if (m_LogDataBuffer.Create(FileLogBufferLen))
	{
		m_LogDataBuffer.SetLockMode(false, true);
		ResetLog();
	}
	else
	{
		PrintImportantLog(_T("无法创建Log缓冲%s"), (LPCTSTR)m_LogName);
	}
}

CAsyncFileLogPrinter::~CAsyncFileLogPrinter(void)
{
	SAFE_RELEASE(m_pLogFile);
}

bool CAsyncFileLogPrinter::ResetLog()
{
	CEasyString LogFileName;
	LogFileName.Format(_T("%s.%u-%02u-%02u.%s"),
		(LPCTSTR)m_LogName,
		m_RecentLogTime.Year(),
		m_RecentLogTime.Month(),
		m_RecentLogTime.Day(),
		(LPCTSTR)m_FileExt);

	LogFileName = CFileTools::MakeModuleFullPath(NULL, LogFileName);

	CEasyString FileDir = CFileTools::GetPathDirectory(LogFileName);
	if (!CFileTools::CreateDirEx(FileDir))
	{
		PrintImportantLog(_T("创建目录%s失败"), (LPCTSTR)FileDir);
	}

	int FileOpenMode = IFileAccessor::modeAppend | IFileAccessor::modeWrite |
		IFileAccessor::shareShareRead | IFileAccessor::osWriteThrough;
	

	SAFE_RELEASE(m_pLogFile);

	m_pLogFile = CFileSystemManager::GetInstance()->CreateFileAccessor(FILE_CHANNEL_NORMAL1);
	if (m_pLogFile == NULL)
	{
		PrintImportantLog(_T("无法创建文件存取器"));
		return false;
	}

	if (!m_pLogFile->Open(LogFileName, FileOpenMode))
	{
		PrintImportantLog(_T("无法打开Log文件%s"), (LPCTSTR)LogFileName);
		return false;
	}
	m_pLogFile->Seek(0, IFileAccessor::seekEnd);
	if (m_pLogFile->GetCurPos() == 0)
	{
#ifndef WIN32
		UINT BomHeader = BMT_UTF_8;
		m_pLogFile->Write(&BomHeader, 3);
#elif defined UNICODE
		UINT BomHeader = BMT_UNICODE;
		m_pLogFile->Write(&BomHeader, 2);
#endif
		if (m_HeaderString.GetLength())
		{
			m_pLogFile->Write(m_HeaderString.GetBuffer(), m_HeaderString.GetLength());
			m_pLogFile->Write(_T("\r\n"), sizeof(TCHAR) * 2);
		}
			
	}

	DoLogBackup();
	return true;
}


void CAsyncFileLogPrinter::PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg)
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
		_tcsncat_s(MsgBuff, 5000, _T("\r\n"), _TRUNCATE);

		PushLog(MsgBuff);
	}
	catch (...)
	{
		PrintImportantLog(_T("Log[%s]输出发生异常"), (LPCTSTR)m_LogName, Msg);
	}
}

void CAsyncFileLogPrinter::PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl)
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
		if (Tag)
		{
			_tcsncat_s(MsgBuff, 5000, _T("["), _TRUNCATE);
			_tcsncat_s(MsgBuff, 5000, Tag, _TRUNCATE);
			_tcsncat_s(MsgBuff, 5000, _T("]"), _TRUNCATE);
		}
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

int CAsyncFileLogPrinter::Update()
{
	int ProcessCount = 0;
	int DataSize = m_LogDataBuffer.GetSmoothUsedSize();
	if (DataSize)
	{
		m_pLogFile->Write(m_LogDataBuffer.GetUsedBuffer(), DataSize);
		m_LogDataBuffer.PopFront((LPVOID)NULL, DataSize);
		ProcessCount++;
	}
	else
	{
		CEasyTime CurTime;
		CurTime.FetchLocalTime();

		if (m_RecentLogTime.Year() != CurTime.Year() ||
			m_RecentLogTime.Month() != CurTime.Month() ||
			m_RecentLogTime.Day() != CurTime.Day())
		{
			m_RecentLogTime = CurTime;
			ResetLog();
			ProcessCount++;
		}		
	}
	return ProcessCount;
}

bool CAsyncFileLogPrinter::PushLog(LPCTSTR LogData)
{

	int DataLen = (int)_tcslen(LogData)*sizeof(TCHAR);
	if (m_LogDataBuffer.PushBack((LPVOID)LogData, DataLen))
	{
		return true;
	}
	else
	{
		PrintImportantLog(_T("将日志压入缓冲失败%u/%u[%s]"), m_LogDataBuffer.GetUsedSize(), m_LogDataBuffer.GetBufferSize(), (LPCTSTR)m_LogName);
		return false;
	}
}

void CAsyncFileLogPrinter::DoLogBackup()
{
	if (m_LogBackupDelay)
	{
		CEasyString FilePartten;

		FilePartten.Format(_T("%s*.*"), (LPCTSTR)m_LogName);
		FilePartten = CFileTools::MakeModuleFullPath(NULL, FilePartten);

		if (CFileTools::IsAbsolutePath(m_LogBackupDir))
		{
			CFileTools::CreateDirEx(m_LogBackupDir);
		}
		else
		{
			CEasyString TargetDir;
			TargetDir.Format(_T("%s%c%s"),
				(LPCTSTR)CFileTools::GetPathDirectory(m_LogName), DIR_SLASH, (LPCTSTR)m_LogBackupDir);
			TargetDir = CFileTools::MakeModuleFullPath(NULL, TargetDir);
			CFileTools::CreateDirEx(TargetDir);
		}

		CFileSearcher FileSearcher;

		time_t CurTime = time(NULL);

		FileSearcher.FindFirst(FilePartten);
		while (FileSearcher.FindNext())
		{
			time_t FileTime = FileSearcher.GetLastAccessTime();

			if (FileTime + m_LogBackupDelay * 24 * 60 * 60 < CurTime)
			{
				CEasyString TargetPath;

				if (CFileTools::IsAbsolutePath(m_LogBackupDir))
				{
					TargetPath.Format(_T("%s%c%s"),
						(LPCTSTR)m_LogBackupDir,
						DIR_SLASH, (LPCTSTR)CFileTools::GetPathFileNameExt(FileSearcher.GetFilePath()));
				}
				else
				{
					TargetPath.Format(_T("%s%c%s%c%s"),
						(LPCTSTR)FileSearcher.GetFileDirect(), DIR_SLASH, (LPCTSTR)m_LogBackupDir,
						DIR_SLASH, (LPCTSTR)CFileTools::GetPathFileNameExt(FileSearcher.GetFilePath()));
				}

				if (CFileTools::MoveFile(FileSearcher.GetFilePath(), TargetPath))
				{
					PrintImportantLog(_T("移动文件成功，%s->%s"), (LPCTSTR)FileSearcher.GetFilePath(), (LPCTSTR)TargetPath);
				}
				else
				{
					PrintImportantLog(_T("移动文件失败，%s->%s"), (LPCTSTR)FileSearcher.GetFilePath(), (LPCTSTR)TargetPath);
				}
			}
		}
	}
}