/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerLogPrinter.h                                       */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CServerThread;

class CServerLogPrinter :
	public CAsyncFileLogPrinter
{
protected:
	CBaseServer *			m_pServer;
	UINT					m_LogOutputMode;
	
public:
	enum LOG_OUTPUT_MODE
	{
		LOM_VS=1,
		LOM_CONSOLE=(1<<1),
		LOM_FILE=(1<<2),
	};

	CServerLogPrinter(CBaseServer * pServer,UINT Mode,int Level,LPCTSTR FileName,int FileLogBufferLen=DEFAULT_ASYNC_FILE_LOG_BUFFER_LEN);
	virtual ~CServerLogPrinter(void);

	UINT GetLogMode()
	{
		return m_LogOutputMode;
	}
	

	void SetLogMode(UINT Mode,int Level,LPCTSTR FileName,int FileLogQueueLen=DEFAULT_ASYNC_FILE_LOG_BUFFER_LEN);

	virtual void PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg);
	virtual void PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl);

};
