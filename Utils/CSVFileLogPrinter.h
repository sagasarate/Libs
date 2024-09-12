#pragma once

class CCSVFileLogPrinter :
	public CAsyncFileLogPrinter
{
public:
	CCSVFileLogPrinter(void);	
	CCSVFileLogPrinter(int Level, LPCTSTR LogName, LPCTSTR HeaderStr, int FileLogBufferLen = DEFAULT_ASYNC_FILE_LOG_BUFFER_LEN);
	~CCSVFileLogPrinter(void);

	

	virtual void PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg);
	virtual void PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl);
};
