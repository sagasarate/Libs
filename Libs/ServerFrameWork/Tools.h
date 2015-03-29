/****************************************************************************/
/*                                                                          */
/*      文件名:    Tools.h                                                  */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

extern LPCTSTR g_ProgramName;
extern LPCTSTR g_ServiceName;
extern LPCTSTR g_ServiceDesc;
extern WORD g_ProgramVersion[4];
extern int g_ServiceWorkMode;

enum SERVICE_WORK_MODE
{
	SWM_SERVICE,
	SWM_APPLICATION,
};



inline void Log(const char* Format, ...)
{
	
	
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(SERVER_LOG_CHANNEL,ILogPrinter::LOG_LEVEL_NORMAL,0,Format,vl);
	va_end(vl);
	
	
	
}

inline void LogDebug(const char* Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(SERVER_LOG_CHANNEL,ILogPrinter::LOG_LEVEL_DEBUG,0,Format,vl);
	va_end(vl);
}



