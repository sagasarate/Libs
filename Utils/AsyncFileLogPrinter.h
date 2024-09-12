/****************************************************************************/
/*                                                                          */
/*      文件名:    AsyncFileLogPrinter.h                                       */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#define DEFAULT_ASYNC_FILE_LOG_BUFFER_LEN	40960

class CAsyncFileLogPrinter :
	public ILogPrinter
{
protected:
	int							m_LogLevel;

	CEasyString					m_LogName;
	CEasyString					m_FileExt;
	CEasyString					m_HeaderString;
	IFileAccessor *				m_pLogFile;
	CThreadSafeCycleBuffer		m_LogDataBuffer;
	CEasyTime					m_RecentLogTime;
	CEasyString					m_LogBackupDir;
	int							m_LogBackupDelay;
public:
	
	CAsyncFileLogPrinter();
	CAsyncFileLogPrinter(int Level, LPCTSTR LogName, int FileLogBufferLen = DEFAULT_ASYNC_FILE_LOG_BUFFER_LEN, LPCTSTR FileExt = NULL, LPCTSTR HeaderStr = NULL);
	virtual ~CAsyncFileLogPrinter(void);

	
	int GetLogLevel()
	{
		return m_LogLevel;
	}
	void SetLogLevel(int Level)
	{
		m_LogLevel=Level;
	}
	LPCTSTR GetLogName()
	{
		return m_LogName;
	}

	virtual void PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg) override;
	virtual void PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl) override;

	virtual int Update() override;
	virtual bool NeedAsyncUpdate() override
	{
		return true;
	}
	void SetBackup(LPCTSTR BackupDir, int BackupDelay)
	{
		m_LogBackupDir = BackupDir;
		m_LogBackupDelay = BackupDelay;
	}
protected:
	virtual bool ResetLog();
	virtual bool PushLog(LPCTSTR LogData);
	void DoLogBackup();
};
