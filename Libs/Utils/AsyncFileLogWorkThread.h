/****************************************************************************/
/*                                                                          */
/*      文件名:    AsyncFileLogWorkThread.h                                      */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define ASYNC_FILE_LOG_DATA_BLOCK_SIZE	128


struct FILE_LOG_DATA_BLOCK
{
	TCHAR Data[ASYNC_FILE_LOG_DATA_BLOCK_SIZE];
	int DataSize;
	CEasyCriticalSection Lock;
};


class CAsyncFileLogWorkThread :
	public CEasyThread
{
protected:
	CEasyString								m_LogFileName;
	CEasyString								m_LogFileExtName;
	CEasyString								m_LogFileHeaderString;
	IFileAccessor							*m_pLogFile;
	CThreadSafeCycleBuffer					m_LogDataBuffer;	
	CEasyTime								m_RecentLogTime;

	DECLARE_CLASS_INFO_STATIC(CAsyncFileLogWorkThread);
public:
	CAsyncFileLogWorkThread();
	BOOL Init(LPCTSTR FileName,LPCTSTR FileExtName,int LogBufferLen);
	virtual ~CAsyncFileLogWorkThread(void);

	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();

	BOOL PushLog(LPCTSTR LogData);

	LPCTSTR GetLogFileName()
	{
		return m_LogFileName;
	}
	void SetLogFileHeaderString(LPCTSTR szString)
	{
		m_LogFileHeaderString=szString;
		m_LogFileHeaderString="Time,"+m_LogFileHeaderString+"\r\n";
	}
	
protected:
	void CreateDir(LPCTSTR FilePath);	
};
