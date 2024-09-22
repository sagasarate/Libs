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
#define CONNECTION_FREE_CHECK_BATCH		32
#define MAX_OBJECT_TYPE_NAME_LEN		32



typedef bool (*DOS_GROUP_INIT_FN)(int GroupIndex);
typedef int (*DOS_GROUP_UPDATE_FN)(int GroupIndex, int ProcessLimit);
typedef void (*DOS_GROUP_DESTORY_FN)(int GroupIndex);

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
	MSG_ENCRYPT_CHECK_SUM,
	MSG_ENCRYPT_DES,
	MSG_ENCRYPT_AES,
	MSG_ENCRYPT_TEA
};

enum MSG_ENCRYPT_MODE
{
	MSG_ENCRYPT_MODE_OPTIONAL,
	MSG_ENCRYPT_MODE_FORCE,
};

enum CLIENT_PROXY_MODE
{
	CLIENT_PROXY_MODE_DEFAULT,
	CLIENT_PROXY_MODE_NO_BUFF,
	CLIENT_PROXY_MODE_WEB_SOCKET,
	CLIENT_PROXY_MODE_CUSTOM = 100,
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
	UINT										ProxyMsgQueueSize;
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
	UINT										MaxIdleTimeToFree;

	UINT										RecvFreqProtect;
	UINT										RecvFlowProtect;
	UINT										RecvProtectCheckInterval;
	UINT										ProtectCountToAddBlackList;
	UINT										ProtectBlockTime;
	bool										EnableBlackList;
	CEasyArray<CIPAddress>						InitBlackList;
	STORAGE_POOL_SETTING						BlackListPoolSetting;

	STORAGE_POOL_SETTING						BroadcastGroupPoolSetting;

	UINT										MsgCompressType;
	UINT										MinMsgCompressSize;
	UINT										MsgEnCryptType;
	UINT										MsgEnCryptMode;
	CEasyStringA								SecretKey;

	UINT										GlobalMsgMapSize;
	UINT										MsgMapSize;
	UINT										MaxMsgSize;
	UINT										MinMsgSize;
	bool										DumpMsg;
	UINT										MaxMsgDumpSize;
	UINT										MaxWSFrameSize;

	bool										EnableGuardThread;
	UINT										GuardThreadKeepAliveTime;
	UINT										GuardThreadKeepAliveCount;



	CLIENT_PROXY_CONFIG()
		:InitBlackList(_T("CLIENT_PROXY_CONFIG"))
	{
		ProxyType = 0;
		ProxyMode = CLIENT_PROXY_MODE_DEFAULT;
		ProxyMsgQueueSize = 1024;
		ConnectionGroupCount = 0;
		ConnectionPoolSetting.StartSize = 128;
		ConnectionPoolSetting.GrowSize = 128;
		ConnectionPoolSetting.GrowLimit = 32;
		ConnectionMsgQueueSize = 128;
		GlobalMsgMapSize = 512;
		MsgMapSize = 512;
		MaxMsgSize = 4096;
		MinMsgSize = 0;
		DumpMsg = false;
		MaxMsgDumpSize = 1024;
		MaxWSFrameSize = 30000;
		AcceptQueueSize = DEFAULT_SERVER_ACCEPT_QUEUE;
		ParallelAcceptCount = DEFAULT_PARALLEL_ACCEPT;
		RecvBufferSize = DEFAULT_SERVER_RECV_DATA_QUEUE;
		SendBufferSize = DEFAULT_SERVER_SEND_DATA_QUEUE;
		BroadcastGroupPoolSetting.StartSize = 128;
		BroadcastGroupPoolSetting.GrowSize = 128;
		BroadcastGroupPoolSetting.GrowLimit = 32;
		MsgCompressType = MSG_COMPRESS_NONE;
		MinMsgCompressSize = 0;
		MsgEnCryptType = MSG_ENCRYPT_NONE;
		MsgEnCryptMode = MSG_ENCRYPT_MODE_OPTIONAL;
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
		MaxIdleTimeToFree = 30;
	}
};

struct DOS_ROUTER_CONFIG
{
	UINT										RouterID;
	CEasyNetLinkManager::ENL_CONFIG				RouterLinkConfig;
	UINT										MaxRouterSendMsgQueue;
	UINT										RouterMsgProcessLimit;
	bool										StatMsgTransfer;
	bool										StatMemoryUse;
	bool										EnableGuardThread;
	UINT										GuardThreadKeepAliveTime;
	UINT										GuardThreadKeepAliveCount;
	DOS_ROUTER_CONFIG()
	{
		RouterID = 0;
		MaxRouterSendMsgQueue = 10240;
		RouterMsgProcessLimit = 256;
		StatMsgTransfer = false;
		StatMemoryUse = false;
		EnableGuardThread = false;
		GuardThreadKeepAliveTime = 20 * 1000;
		GuardThreadKeepAliveCount = 5;
	}
};

struct DOS_OBJECT_CONFIG
{
	UINT										ObjectGroupCount;
	UINT										MaxObjectGroupCount;
	UINT										ObjectRegisterQueueSize;
	STORAGE_POOL_SETTING						ObjectPoolSetting;
	UINT										MaxObjectMsgQueue;
	UINT										ObjectAliveTestTime;
	UINT										ObjectAliveCheckTime;
	UINT										ObjectKeepAliveCount;
	bool										StatObjectCPUCost;
	bool										UseRealGroupLoadWeight;
	CEasyArray<DOS_GROUP_INIT_FN>				DOSGroupInitFNList;
	CEasyArray <DOS_GROUP_UPDATE_FN>			DOSGroupUpdateFNList;
	CEasyArray <DOS_GROUP_DESTORY_FN>			DOSGroupDestoryFNList;
	int											ObjectGroupProcessLimit;
	bool										EnableGuardThread;
	UINT										GuardThreadKeepAliveTime;
	UINT										GuardThreadKeepAliveCount;
	STORAGE_POOL_SETTING						TimerQueueSetting;

	DOS_OBJECT_CONFIG()
	{
		ObjectGroupCount = 8;
		MaxObjectGroupCount = 256;
		ObjectRegisterQueueSize = 256;
		ObjectPoolSetting.StartSize = 128;
		ObjectPoolSetting.GrowSize = 128;
		ObjectPoolSetting.GrowLimit = 32;
		MaxObjectMsgQueue = 1024;
		ObjectAliveTestTime = 20 * 1000;
		ObjectAliveCheckTime = 5000;
		ObjectKeepAliveCount = 5;
		StatObjectCPUCost = false;
		UseRealGroupLoadWeight = false;
		ObjectGroupProcessLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT;
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
		:ClientProxyConfigs(_T("CLIENT_PROXY_CONFIG"))
	{

	}
};

class CDOSBaseObject;

struct DOS_OBJECT_REGISTER_INFO
{
	OBJECT_ID			ObjectID;
	char				ObjectTypeName[MAX_OBJECT_TYPE_NAME_LEN + 1];
	int					Weight;
	int					ObjectGroupIndex;
	UINT				MsgQueueSize;
	UINT				MsgProcessLimit;
	UINT				Flag;
	CDOSBaseObject*		pObject;


	DOS_OBJECT_REGISTER_INFO()
	{
		ObjectID = 0;
		ObjectTypeName[0] = 0;
		Weight = 1;
		ObjectGroupIndex = -1;
		MsgQueueSize = 0;
		MsgProcessLimit = 0;
		Flag = 0;
		pObject = NULL;
	}
};

extern UINT DistinctObjectID(OBJECT_ID* pObjectIDs, UINT Count);

