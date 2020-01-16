/****************************************************************************/
/*                                                                          */
/*      文件名:    LogManager.h                                             */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



#define LOG_SYSTEM_CHANNEL				1010100
#define LOG_IMPORTANT_CHANNEL			1010101
#define LOG_NET_CHANNEL					1101
#define LOG_DB_ERROR_CHANNEL			1202
#define LOG_DOS_CHANNEL					1301
#define LOG_DOS_OBJECT_STATE_CHANNEL	1302
#define LOG_DOS_MSG_STATE_CHANNEL		1303
#define LOG_DOS_MEM_STATE_CHANNEL		1304
#define LOG_MONO_CHANNEL				1401

#define IMPORTANT_LOG_FILE_NAME	_T("ImportantError")
#define DEFAULT_ASYNC_LOG_WORK_THREAD_COUNT	2

extern TCHAR LOG_MANAGER_INSTANCE[];

class CLogManager :
	public CNameObject,
	public CStaticObject2<CLogManager,LOG_MANAGER_INSTANCE>
{
protected:
	CStaticMap<UINT,ILogPrinter *>		m_LogChannels;
	CEasyArray<CAsyncLogWorkThread>		m_WorkThreadList;

	DECLARE_CLASS_INFO_STATIC(CLogManager);
public:
	CLogManager(void);
	virtual ~CLogManager(void);

	UINT AddWorkThreadCount(UINT Count);

	UINT GetChannelCount();
	void AddChannel(UINT ChannelID,ILogPrinter * pLogPrinter);
	ILogPrinter * GetChannel(UINT ChannelID);
	bool DelChannel(UINT ChannelID);

	bool PrintLogDirect(UINT ChannelID, int Level, LPCTSTR Tag, LPCTSTR Msg);
	bool PrintLog(UINT ChannelID, int Level, LPCTSTR Tag, LPCTSTR Format, ...);
	bool PrintLogVL(UINT ChannelID, int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl);
};


inline bool PrintSystemLogWithTag(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl, Format);
	bool ret = CLogManager::GetInstance()->PrintLogVL(LOG_SYSTEM_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, Tag, Format, vl);
	va_end(vl);
	return ret;
}
#define PrintSystemLog(_Format, ...)	PrintSystemLogWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)

inline bool PrintImportantLogWithTag(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl, Format);
	bool ret = CLogManager::GetInstance()->PrintLogVL(LOG_IMPORTANT_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, Tag, Format, vl);
	va_end(vl);
	return ret;
}
#define PrintImportantLog(_Format, ...)		PrintImportantLogWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)