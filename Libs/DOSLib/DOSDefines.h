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


#define THREAD_CPU_COUNT_TIME			(5000)
//#define DOS_PROXY_KEEP_ALIVE_TIME		(10000)
//#define DOS_PROXY_KEEP_ALIVE_MAX_COUNT	(20)

typedef bool (*DOS_GROUP_INIT_FN)(UINT GroupIndex);
typedef bool(*DOS_GROUP_DESTORY_FN)(UINT GroupIndex);

enum MSG_COMPRESS_TYPE
{
	MSG_COMPRESS_NONE,
	MSG_COMPRESS_LZO,
	MSG_COMPRESS_ZIP_FAST,
	MSG_COMPRESS_ZIP_NORMAL,
	MSG_COMPRESS_ZIP_SLOW,
};

enum CLIENT_PROXY_MODE
{
	CLIENT_PROXY_MODE_DEFAULT,
	CLIENT_PROXY_MODE_CUSTOM,
};

struct CLIENT_PROXY_CONFIG
{
	UINT										ProxyType;
	CLIENT_PROXY_MODE							ProxyMode;
	CIPAddress									ListenAddress;
	UINT										MaxConnection;	
	UINT										ConnectionMsgQueueSize;
	UINT										AcceptQueueSize;
	UINT										ParallelAcceptCount;
	UINT										RecvBufferSize;
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
		ProxyMode = CLIENT_PROXY_MODE_DEFAULT;
		MaxConnection = 128;
		MsgQueueSize = 512;
		ConnectionMsgQueueSize = 128;
		GlobalMsgMapSize = 512;
		MsgMapSize = 512;
		MaxMsgSize = 4096;
		AcceptQueueSize = DEFAULT_SERVER_ACCEPT_QUEUE;
		ParallelAcceptCount = DEFAULT_PARALLEL_ACCEPT;
		RecvBufferSize = DEFAULT_SERVER_RECV_DATA_QUEUE;
		SendBufferSize = DEFAULT_SERVER_SEND_DATA_QUEUE;
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
	CEasyNetLinkManager::ENL_CONFIG				RouterLinkConfig;
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
	DOS_GROUP_INIT_FN							pDOSGroupInitFN;
	DOS_GROUP_DESTORY_FN						pDOSGroupDestoryFN;

	DOS_CONFIG()
	{
		RouterID=0;	
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
		pDOSGroupInitFN = NULL;
		pDOSGroupDestoryFN = NULL;
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
	//UINT				Param;

	DOS_OBJECT_REGISTER_INFO()
	{
		ObjectID=0;
		Weight=1;
		ObjectGroupIndex=-1;
		MsgQueueSize=0;
		MsgProcessLimit=0;
		pObject=NULL;
		//Param=0;
	}
};

extern UINT DistinctObjectID(OBJECT_ID * pObjectIDs,UINT Count);

inline BOOL PrintDOSLog(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret = CLogManager::GetInstance()->PrintLogVL(LOG_DOS_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, Tag, Format, vl);
	va_end(vl);
	return ret;
}


inline BOOL PrintDOSDebugLog(LPCTSTR Tag, LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret = CLogManager::GetInstance()->PrintLogVL(LOG_DOS_CHANNEL, ILogPrinter::LOG_LEVEL_DEBUG, Tag, Format, vl);
	va_end(vl);
	return ret;
}

inline BOOL PrintDOSObjectStatLog(LPCTSTR Format, ...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret = CLogManager::GetInstance()->PrintLogVL(LOG_DOS_OBJECT_STATE_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, NULL, Format, vl);
	va_end(vl);
	return ret;
}