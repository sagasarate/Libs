/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerFrameWork.h                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#include "../Protocol/BaseProtocol.h"
#include "../Protocol/ServerConsoleProtocol.h"




#define SERVER_LOG_CHANNEL					8001

#define SERVER_STATUS_LOG_CHANNEL			8003

#define SYSTEM_NET_LINK_CONFIG_FILE_NAME	"SysLink.XML"


#define MAX_CONTROL_PANEL_MSG_LEN			20480						//控制台可显示的最多消息

#define SERVER_STATUS_BLOCK_SIZE			10240

#define SERVER_INFO_COUNT_TIME				5000

#define SERVER_STATUS_FORMAT_INFO_SIZE	(MAX_SERVER_STATUS_NAME_LEN+64)

#include "IBaseServer.h"


#include "SystemConfig.h"

#include "ControlPanel.h"


#include "ServerLogPrinter.h"

#include "SystemNetLink.h"
#include "SystemNetLinkManager.h"
#include "SystemControlPort.h"




#include "ServerThread.h"

#include "MainGuardThread.h"

#include "Tools.h"

#ifdef WIN32

#include "ServerFrameWorkResource.h"
#include "DlgServerStatus.h"
#include "ServerConsoleDlg.h"
#include "NTService.h"
#include "ServerAppWin.h"

//#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Version.lib")

#else

	#include "ServerAppLinux.h"

#endif
