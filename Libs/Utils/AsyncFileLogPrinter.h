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
	CAsyncFileLogWorkThread		m_FileLogWorkThread;
	CEasyCriticalSection		m_EasyCriticalSection;
	CEasyString					m_LogFileName;
	int							m_LogLevel;
public:
	
	CAsyncFileLogPrinter();
	CAsyncFileLogPrinter(int Level,LPCTSTR FileName,int FileLogBufferLen=DEFAULT_ASYNC_FILE_LOG_BUFFER_LEN);
	virtual ~CAsyncFileLogPrinter(void);

	
	int GetLogLevel()
	{
		return m_LogLevel;
	}
	void SetLogLevel(int Level)
	{
		m_LogLevel=Level;
	}
	LPCTSTR GetLogFileName()
	{
		return m_LogFileName;
	}

	void ResetLog(int Level,LPCTSTR FileName,int FileLogQueueLen=DEFAULT_ASYNC_FILE_LOG_BUFFER_LEN);
	void CloseLog();

	virtual void PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg);
	virtual void PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl);

	

};
