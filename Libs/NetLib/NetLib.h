/****************************************************************************/
/*                                                                          */
/*      文件名:    NetLib.h                                                 */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define NETLIB_VERSION	"2.0.0.0"

#ifdef WIN32
#include <WinSock2.h>
#include <MSWSock.h>

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

#define EWOULDBLOCK		WSAEWOULDBLOCK

typedef int	socklen_t;

#else

#include <netdb.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


#define INVALID_SOCKET						(-1)
//#define INVALID_HANDLE_VALUE				(UINT)(-1)
#define SOCKET_ERROR						(-1)
#define DEFAULT_EPOLL_EVENT_WAIT_NUM		(32)


#define closesocket(s)						close(s)

typedef int SOCKET;

#endif


#define MAX_LISTEN_BACKLOG						64

#define DEFAULT_THREAD_NUMBER_PER_CPU			2
#define LOG_NET_CHANNEL							1101
#define DEFAULT_SERVER_ACCEPT_QUEUE				1024
#define DEFAULT_SERVER_RECV_DATA_QUEUE			4096
#define DEFAULT_SERVER_SEND_DATA_QUEUE			4096
#define DEFAULT_FILE_ACCESSOR_DATA_QUEUE		1024
#define DEFAULT_FILE_ACCESSOR_REQUEST_QUEUE		1024
#define DEFAULT_FILE_ACCESSOR_ERROR_QUEUE		1024
#define MAX_ONCE_SEND_COUNT						64
#define DEFAULT_PARALLEL_ACCEPT					64
#define DEFAULT_PARALLEL_RECV					64
#define MAX_EVENT_OBJECT						10240
#define DEFAULT_EVENT_ROUTER_COUNT				10240
#define DEFAULT_EVENT_OBJECT_POOL_GROW_SIZE		0
#define DEFAULT_EVENT_OBJECT_POOL_GROW_LIMIT	0
#define DEFAULT_EVENT_ROUTER_POOL_GROW_SIZE		0
#define DEFAULT_EVENT_ROUTER_POOL_GROW_LIMIT	0
#define MAX_DATA_PACKET_SIZE					4096
#define DEFAULT_SERVER_PROCESS_PACKET_LIMIT		32


#define MAX_EASY_NET_LINK_MSG_SIZE			65535
#define MAX_EASY_NET_LINK_DATA_SIZE			(MAX_EASY_NET_LINK_MSG_SIZE-sizeof(EASY_NET_LINK_MSG_HEAD))

#define EASY_LINK_RECONNECT_TIME			(10*1000)


struct EASY_NET_LINK_MSG_HEAD
{
	DWORD Size;					//消息包大小，包括头部
	DWORD MsgID;				//消息ID
};

struct EASY_NET_LINK_MSG
{
	EASY_NET_LINK_MSG_HEAD	Header;		//消息头部
	char					Data[1];	//消息体数据
};

enum EASY_NET_LINK_MSG_ID
{
	EASY_NET_LINK_MSG_LINK_START,		
	EASY_NET_LINK_MSG_KEEP_ALIVE,
	EASY_NET_LINK_MSG_USER_DATA,
};

struct EASY_NET_LINK_INFO
{
	UINT ID;
};


inline BOOL PrintNetLog(DWORD Color,LPCTSTR Format,...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret=CLogManager::GetInstance()->PrintLogVL(LOG_NET_CHANNEL,ILogPrinter::LOG_LEVEL_NORMAL,Color,Format,vl);
	va_end(vl);
	return ret;
}

inline BOOL PrintNetDebugLog(DWORD Color,LPCTSTR Format,...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret=CLogManager::GetInstance()->PrintLogVL(LOG_NET_CHANNEL,ILogPrinter::LOG_LEVEL_DEBUG,Color,Format,vl);
	va_end(vl);
	return ret;
}



#include "IPAddress.h"
#include "IPPattern.h"
#include "NetSocket.h"

#include "BaseTCPConnection.h"
#include "BaseService.h"
#include "BaseServer.h"


#ifdef WIN32

#include "IIOCPEventHandler.h"
#include "IOCPEventRouter.h"
#include "OverLappedObject.h"
#include "IOCPThread.h"
#include "IOCPListenThread.h"
#include "NetServerIOCP.h"
#include "NetConnectionIOCP.h"
#include "NetPTCPConnectionIOCP.h"
#include "NetServiceIOCP.h"
#include "NetPUDPServiceIOCP.h"
#include "IOCPFileAccessor.h"

#include "EasyFileServerConnection.h"
#include "EasyFileService.h"

#else

#include "EpollEventObject.h"
#include "IEpollEventHandler.h"
#include "EpollEventRouter.h"
#include "EpollThread.h"
#include "NetServerEpoll.h"
#include "NetServiceEpoll.h"
#include "NetConnectionEpoll.h"
#include "NetPTCPConnectionEpoll.h"

#endif

#include "EasyNetLinkConnection.h"
#include "EasyNetLinkService.h"
#include "EasyNetLinkManager.h"

