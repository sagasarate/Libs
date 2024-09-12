/****************************************************************************/
/*                                                                          */
/*      文件名:    FileLogPrinter.h                                         */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


enum FILE_LOG_MODE
{
	FILE_LOG_APPEND=1,
	FILE_LOG_SAFE_WRITE=(1<<1),
	FILE_LOG_SPLIT_BY_DAY=(1<<2),
	FILE_LOG_SHARE_WRITE = (1 << 3),
};

class IFileAccessor;

class CFileLogPrinter :
	public ILogPrinter
{
protected:
	TCHAR			m_MsgBuff[40960];
	DWORD			m_Flag;
	IFileAccessor *	m_pFileAccessor;
	CEasyTime		m_RecentLogTime;
	int				m_FileOpenMode;
	CEasyString		m_LogFileName;
	UINT			m_LogLevel;

	CEasyCriticalSection	m_EasyCriticalSection;
public:
	CFileLogPrinter();
	CFileLogPrinter(LPCTSTR FileName,DWORD Flag);
	~CFileLogPrinter(void);

	bool Create(LPCTSTR FileName,DWORD Flag);
	IFileAccessor * GetFile()
	{
		return m_pFileAccessor;
	}
	LPCTSTR GetLogFileName()
	{
		return m_LogFileName;
	}

	virtual void PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg);
	virtual void PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl);
protected:
	bool OpenLogFile();
};
