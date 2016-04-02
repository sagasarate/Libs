#pragma once

class CCSVFileLogPrinter :
	public CAsyncFileLogPrinter
{
public:
	CCSVFileLogPrinter(void);	
	~CCSVFileLogPrinter(void);

	bool Init(int Level,LPCTSTR FileName,LPCTSTR LogHeader,int FileLogBufferLen=DEFAULT_ASYNC_FILE_LOG_BUFFER_LEN);

	virtual void PrintLogDirect(int Level, DWORD Color, LPCTSTR Msg);
	virtual void PrintLogVL(int Level,DWORD Color,LPCTSTR Format,va_list vl);
};
