/****************************************************************************/
/*                                                                          */
/*      文件名:    DBLib.h                                                  */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define DBLIB_VERSION	"2.0.0.0"


//typedef unsigned long ulong;

inline BOOL PrintDBLogWithTag(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl, Format);
	BOOL ret = CLogManager::GetInstance()->PrintLogVL(LOG_DB_ERROR_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, Tag, Format, vl);
	va_end(vl);
	return ret;
}

inline BOOL PrintDBDebugLogWithTag(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl, Format);
	BOOL ret = CLogManager::GetInstance()->PrintLogVL(LOG_DB_ERROR_CHANNEL, ILogPrinter::LOG_LEVEL_DEBUG, Tag, Format, vl);
	va_end(vl);
	return ret;
}

#define PrintDBLog(_Format, ...)	PrintDBLogWithTag(__FUNCTION__, _Format, ##__VA_ARGS__)
#define PrintDBDebugLog(_Format, ...)	PrintDBDebugLogWithTag(__FUNCTION__, _Format, ##__VA_ARGS__)

#include "DBTypes.h"
#include "DBValue.h"
#include "DBInterfaces.h"

#include "DBParameterSet.h"
#include "DBStaticRecordSet.h"
#include "DBFakeRecordSet.h"


#ifdef WIN32

#include <sql.h>
#include <sqlext.h>
#include <WinSock2.h>
#include <MSWSock.h>




#include "ODBCRecordSet.h"
#include "ODBCConnection.h"
#include "ODBCDataBase.h"

#ifdef _WIN64

#include "../OtherInclude/MySQL/x64/win/mysql.h"

#else

#include "../OtherInclude/MySQL/x86/win/mysql.h"

#endif

#else

#ifdef __x86_64__

#include "../OtherInclude/MySQL/x64/linux/mysql.h"


#else

#include "../OtherInclude/MySQL/x86/linux/mysql.h"

#endif

#endif






#include "MySQLDynamicRecordSet.h"
#include "MySQLRecordSet.h"
#include "MySQLConnection.h"
#include "MySQLDatabase.h"

#include "DBTransaction.h"
#include "DBTransationWorkThread.h"
#include "DBTransationManager.h"


using namespace DBLib;


