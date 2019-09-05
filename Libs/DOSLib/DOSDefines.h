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
#define GROUP_WEIGHT_UPDATE_TIME		(5000)
//#define DOS_PROXY_KEEP_ALIVE_TIME		(10000)
//#define DOS_PROXY_KEEP_ALIVE_MAX_COUNT	(20)
#define BLACK_LIST_UPDATE_TIME			(60*1000)

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

enum MSG_ENCRYPT_TYPE
{
	MSG_ENCRYPT_NONE,
	MSG_ENCRYPT_DES,
	MSG_ENCRYPT_AES,
	MSG_ENCRYPT_TEA
};

enum CLIENT_PROXY_MODE
{
	CLIENT_PROXY_MODE_DEFAULT,
	CLIENT_PROXY_MODE_CUSTOM,
};

enum OBJECT_GROUP_TYPE
{
	OBJECT_GROUP_TYPE_NORMAL,
	OBJECT_GROUP_TYPE_PRIVATE,
};

struct CLIENT_PROXY_CONFIG
{
	UINT										ProxyType;
	CLIENT_PROXY_MODE							ProxyMode;
	CIPAddress									ListenAddress;
	UINT										ConnectionGroupCount;
	STORAGE_POOL_SETTING						ConnectionPoolSetting;	
	UINT										ConnectionMsgQueueSize;
	UINT										AcceptQueueSize;
	UINT										ParallelAcceptCount;
	UINT										RecvBufferSize;
	UINT										SendBufferSize;
	UINT										UnacceptConnectionKeepTime;
	bool										UseServerInitiativeKeepAlive;
	UINT										KeepAliveTime;
	UINT										KeepAliveCount;

	UINT										RecvFreqProtect;
	UINT										RecvFlowProtect;
	UINT										RecvProtectCheckInterval;
	UINT										ProtectCountToAddBlackList;
	UINT										ProtectBlockTime;
	bool										EnableBlackList;
	CEasyArray<CIPAddress>						InitBlackList;
	STORAGE_POOL_SETTING						BlackListPoolSetting;

	UINT										MsgCompressType;
	UINT										MinMsgCompressSize;
	UINT										MsgEnCryptType;
	CEasyString									SecretKey;

	UINT										GlobalMsgMapSize;
	UINT										MsgMapSize;
	UINT										MaxMsgSize;
	UINT										MaxSendMsgSize;

	bool										EnableGuardThread;
	UINT										GuardThreadKeepAliveTime;
	UINT										GuardThreadKeepAliveCount;

	
	
	CLIENT_PROXY_CONFIG()
	{
		ProxyType = 0;
		ProxyMode = CLIENT_PROXY_MODE_DEFAULT;
		ConnectionGroupCount = 0;
		ConnectionPoolSetting.StartSize = 128;
		ConnectionPoolSetting.GrowSize = 128;
		ConnectionPoolSetting.GrowLimit = 32;
		ConnectionMsgQueueSize = 128;
		GlobalMsgMapSize = 512;
		MsgMapSize = 512;
		MaxMsgSize = 4096;
		MaxSendMsgSize = 10 * 1024 * 1204;
		AcceptQueueSize = DEFAULT_SERVER_ACCEPT_QUEUE;
		ParallelAcceptCount = DEFAULT_PARALLEL_ACCEPT;
		RecvBufferSize = DEFAULT_SERVER_RECV_DATA_QUEUE;
		SendBufferSize = DEFAULT_SERVER_SEND_DATA_QUEUE;
		MsgCompressType = MSG_COMPRESS_NONE;
		MinMsgCompressSize = 0;
		MsgEnCryptType = MSG_ENCRYPT_NONE;
		UnacceptConnectionKeepTime = 30 * 1000;
		UseServerInitiativeKeepAlive = false;
		KeepAliveTime = 30000;
		KeepAliveCount = 10;
		RecvFreqProtect = 0;
		RecvFlowProtect = 0;
		RecvProtectCheckInterval = 0;
		ProtectCountToAddBlackList = 10;
		ProtectBlockTime = 10 * 60;
		EnableBlackList = false;
		EnableGuardThread = false;
		GuardThreadKeepAliveTime = 20 * 1000;
		GuardThreadKeepAliveCount = 5;
	}
};

struct DOS_ROUTER_CONFIG
{
	UINT										RouterID;
	CEasyNetLinkManager::ENL_CONFIG				RouterLinkConfig;
	UINT										MaxRouterSendMsgQueue;
	UINT										RouterMsgProcessLimit;
	bool										StateMsgTransfer;
	bool										EnableGuardThread;
	UINT										GuardThreadKeepAliveTime;
	UINT										GuardThreadKeepAliveCount;
	DOS_ROUTER_CONFIG()
	{
		RouterID = 0;
		MaxRouterSendMsgQueue = 10240;
		RouterMsgProcessLimit = 256;
		StateMsgTransfer = false;
		EnableGuardThread = false;
		GuardThreadKeepAliveTime = 20 * 1000;
		GuardThreadKeepAliveCount = 5;
	}
};

struct DOS_OBJECT_CONFIG
{
	UINT										ObjectGroupCount;
	STORAGE_POOL_SETTING						ObjectPoolSetting;
	UINT										MaxObjectMsgQueue;
	UINT										ObjectAliveTestTime;
	UINT										ObjectAliveCheckTime;
	UINT										ObjectKeepAliveCount;
	bool										StatObjectCPUCost;
	bool										UseRealGroupLoadWeight;
	DOS_GROUP_INIT_FN							pDOSGroupInitFN;
	DOS_GROUP_DESTORY_FN						pDOSGroupDestoryFN;
	bool										EnableGuardThread;
	UINT										GuardThreadKeepAliveTime;
	UINT										GuardThreadKeepAliveCount;

	DOS_OBJECT_CONFIG()
	{
		ObjectGroupCount = 8;
		ObjectPoolSetting.StartSize = 128;
		ObjectPoolSetting.GrowSize = 128;
		ObjectPoolSetting.GrowLimit = 32;
		MaxObjectMsgQueue = 1024;
		ObjectAliveTestTime = 20 * 1000;
		ObjectAliveCheckTime = 5000;
		ObjectKeepAliveCount = 5;
		StatObjectCPUCost = false;
		UseRealGroupLoadWeight = false;
		pDOSGroupInitFN = NULL;
		pDOSGroupDestoryFN = NULL;
		EnableGuardThread = false;
		GuardThreadKeepAliveTime = 20 * 1000;
		GuardThreadKeepAliveCount = 5;
	}
};

struct DOS_MEMORY_POOL_CONFIG
{
	bool										Enable;
	UINT										MemoryPoolBlockSize;
	UINT										MemoryPoolLeveSize;
	UINT										MemoryPoolLevelCount;
	DOS_MEMORY_POOL_CONFIG()
	{
		Enable = true;
		MemoryPoolBlockSize = 64;
		MemoryPoolLeveSize = 10240;
		MemoryPoolLevelCount = 5;
	}
};

struct DOS_CONFIG
{
	CEasyArray<CLIENT_PROXY_CONFIG>				ClientProxyConfigs;
	DOS_ROUTER_CONFIG							RouterConfig;	
	DOS_OBJECT_CONFIG							ObjectConfig;
	DOS_MEMORY_POOL_CONFIG						MemoryPoolConfig;

	DOS_CONFIG()
	{
		
	}
};

class CDOSBaseObject;

struct DOS_OBJECT_REGISTER_INFO
{
	OBJECT_ID			ObjectID;
	LPCSTR				szObjectTypeName;
	int					Weight;
	int					ObjectGroupIndex;
	UINT				MsgQueueSize;
	UINT				MsgProcessLimit;
	UINT				Flag;
	CDOSBaseObject *	pObject;
	

	DOS_OBJECT_REGISTER_INFO()
	{
		ObjectID=0;
		szObjectTypeName = NULL;
		Weight=1;
		ObjectGroupIndex=-1;
		MsgQueueSize=0;
		MsgProcessLimit=0;
		Flag = 0;
		pObject=NULL;		
	}	
};

extern UINT DistinctObjectID(OBJECT_ID * pObjectIDs,UINT Count);

