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



#define LOG_SYSTEM_CHANNEL		1010100
#define LOG_IMPORTANT_CHANNEL	1010101

#define IMPORTANT_LOG_FILE_NAME	_T("ImportantError")

extern TCHAR LOG_MANAGER_INSTANCE[];

class CLogManager :
	public CNameObject,
	public CStaticObject2<CLogManager,LOG_MANAGER_INSTANCE>
{
protected:
	CEasyMap<UINT,ILogPrinter *>	m_LogChannels;

	DECLARE_CLASS_INFO_STATIC(CLogManager);
public:
	CLogManager(void);
	virtual ~CLogManager(void);

	UINT GetChannelCount();
	void AddChannel(UINT ChannelID,ILogPrinter * pLogPrinter);
	ILogPrinter * GetChannel(UINT ChannelID);
	BOOL DelChannel(UINT ChannelID);

	BOOL PrintLog(UINT ChannelID,int Level,DWORD Color,LPCTSTR Format,...);
	BOOL PrintLogVL(UINT ChannelID,int Level,DWORD Color,LPCTSTR Format,va_list vl);
};
