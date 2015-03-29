/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSDefines.h                                             */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define LOG_DOS_CHANNEL					1301
#define LOG_DOS_OBJECT_STATE_CHANNEL	1302
#define THREAD_CPU_COUNT_TIME			(5000)
//#define DOS_PROXY_KEEP_ALIVE_TIME		(10000)
//#define DOS_PROXY_KEEP_ALIVE_MAX_COUNT	(20)

enum MSG_COMPRESS_TYPE
{
	MSG_COMPRESS_NONE,
	MSG_COMPRESS_LZO,
	MSG_COMPRESS_ZIP_FAST,
	MSG_COMPRESS_ZIP_NORMAL,
	MSG_COMPRESS_ZIP_SLOW,
};

struct CLIENT_PROXY_CONFIG
{
	UINT										ProxyType;
	CIPAddress									ListenAddress;
	UINT										MaxConnection;	
	UINT										ConnectionMsgQueueSize;
	UINT										SendBufferSize;
	UINT										SendDelay;
	UINT										SendQueryLimit;	
	UINT										UnacceptConnectionKeepTime;
	bool										UseServerInitiativeKeepAlive;
	UINT										KeepAliveTime;
	UINT										KeepAliveCount;
	UINT										MsgCompressType;
	UINT										MinMsgCompressSize;

	UINT										MsgQueueSize;
	UINT										GlobalMsgMapSize;
	UINT										MsgMapSize;
	UINT										MaxMsgSize;
	
	
	CLIENT_PROXY_CONFIG()
	{
		ProxyType = 0;
		MaxConnection = 128;
		MsgQueueSize = 512;
		ConnectionMsgQueueSize = 128;
		GlobalMsgMapSize = 512;
		MsgMapSize = 512;
		MaxMsgSize = 4096;
		SendBufferSize = 0;
		SendDelay = 0;
		SendQueryLimit = 0;
		MsgCompressType = MSG_COMPRESS_NONE;
		MinMsgCompressSize = 0;
		UnacceptConnectionKeepTime = 30 * 1000;
		UseServerInitiativeKeepAlive = false;
		KeepAliveTime = 30000;
		KeepAliveCount = 10;
	}
};


struct DOS_CONFIG
{
	UINT										RouterID;	
	CEasyString									RouterLinkConfigFileName;	
	UINT										MaxRouterSendMsgQueue;
	UINT										RouterMsgProcessLimit;
	

	CEasyArray<CLIENT_PROXY_CONFIG>				ClientProxyConfigs;

	UINT										MemoryPoolBlockSize;
	UINT										MemoryPoolLeveSize;
	UINT										MemoryPoolLevelCount;

	UINT										ObjectGroupCount;
	UINT										MaxGroupObjectCount;
	UINT										MaxObjectMsgQueue;	
	UINT										ObjectAliveTestTime;
	UINT										ObjectAliveCheckTime;
	UINT										ObjectKeepAliveCount;
	bool										StatObjectCPUCost;

	DOS_CONFIG()
	{
		RouterID=1;	
		MaxRouterSendMsgQueue=10240;
		RouterMsgProcessLimit=32;
		
		MemoryPoolBlockSize=64;
		MemoryPoolLeveSize=10240;
		MemoryPoolLevelCount=5;
		ObjectGroupCount=8;
		MaxGroupObjectCount=1024;
		MaxObjectMsgQueue=1024;
		ObjectAliveTestTime=20*1000;
		ObjectAliveCheckTime=1000;
		ObjectKeepAliveCount=5;
		StatObjectCPUCost=false;
	}
};

class CDOSBaseObject;

struct DOS_OBJECT_REGISTER_INFO
{
	OBJECT_ID			ObjectID;
	int					Weight;
	int					ObjectGroupIndex;
	UINT				MsgQueueSize;
	UINT				MsgProcessLimit;
	CDOSBaseObject *	pObject;
	UINT				Param;

	DOS_OBJECT_REGISTER_INFO()
	{
		ObjectID=0;
		Weight=1;
		ObjectGroupIndex=-1;
		MsgQueueSize=0;
		MsgProcessLimit=0;
		pObject=NULL;
		Param=0;
	}
};

extern UINT DistinctObjectID(OBJECT_ID * pObjectIDs,UINT Count);

inline BOOL PrintDOSLog(DWORD Color,LPCTSTR Format,...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret=CLogManager::GetInstance()->PrintLogVL(LOG_DOS_CHANNEL,ILogPrinter::LOG_LEVEL_NORMAL,Color,Format,vl);
	va_end(vl);
	return ret;
}


inline BOOL PrintDOSDebugLog(DWORD Color,LPCTSTR Format,...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret=CLogManager::GetInstance()->PrintLogVL(LOG_DOS_CHANNEL,ILogPrinter::LOG_LEVEL_DEBUG,Color,Format,vl);
	va_end(vl);
	return ret;
}

inline BOOL PrintDOSObjectStatLog(DWORD Color,LPCTSTR Format,...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret=CLogManager::GetInstance()->PrintLogVL(LOG_DOS_OBJECT_STATE_CHANNEL,ILogPrinter::LOG_LEVEL_NORMAL,Color,Format,vl);
	va_end(vl);
	return ret;
}