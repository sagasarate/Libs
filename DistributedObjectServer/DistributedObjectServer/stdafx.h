// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件

#pragma once

#ifdef WIN32

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 标头中排除不常使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x0501		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0501		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0510 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0501	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类

#include <afxdtctl.h>		// Internet Explorer 4 公共控件的 MFC 支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 公共控件的 MFC 支持
#endif // _AFX_NO_AFXCMN_SUPPORT



#endif



#ifdef _DEBUG
#define CONFIG_FILE_NAME		"ConfigD.xml"
#else
#define CONFIG_FILE_NAME		"Config.xml"
#endif

#define SERVER_INFO_COUNT_TIME					5000
#define MAX_SERVER_TERMINATE_WAIT_TIME			3000000

#define PLUGIN_INIT_FN_NAME				"InitPlugin"
#define PLUGIN_QUERY_RELEASE_FN_NAME	"QueryReleasePlugin"
#define PLUGIN_RELEASE_FN_NAME			"ReleasePlugin"

#define PLUGIN_LOG_CHANNEL_START	12000

#ifdef WIN32
#define USE_CRT_DETAIL_NEW
#endif

#include "../../Libs/ServerFrameWork/ServerFrameWork.h"

#include "../../Libs/DOSLib/DOSLib.h"

#ifndef WIN32

#include <dlfcn.h>

#endif

#if defined(WIN32)&&(!defined(_DEBUG))

#define OBJECT_EXCEPTION_CATCH_START \
	__try\
	{

#define OBJECT_EXCEPTION_CATCH_END \
	}__except(CExceptionParser::ExceptionPrinter(GetExceptionInformation(),m_ObjectID.ID,__FUNCTION__))\
	{\
		throw;\
	}
#else
#define OBJECT_EXCEPTION_CATCH_START
#define OBJECT_EXCEPTION_CATCH_END
#endif

#include "DistributedObjectOperator.h"
#include "DistributedObjectManager.h"

extern "C" typedef BOOL (*PLUGIN_INIT_FN)(IDistributedObjectManager* pObjectManager,UINT PluginID,UINT LogChannel);
extern "C" typedef void (*PLUGIN_QUERY_RELEASE_FN)();
extern "C" typedef void (*PLUGIN_RELEASE_FN)();

#include "DOSServerThread.h"
#include "MainConfig.h"
#include "MainThread.h"


//#define PERFORMANCE_STAT

#ifdef PERFORMANCE_STAT

#define FUNCTION_BEGIN				{static int s_FunctionIndex=0;if(!s_FunctionIndex) s_FunctionIndex=CPerformanceStatistician::GetInstance()->GetFunctionIndex(s_FunctionIndex,__FUNCTION__); CAutoPerformanceCounter tmpPerformanceCounter(s_FunctionIndex);
#define FUNCTION_END				}

#else

#define FUNCTION_BEGIN
#define FUNCTION_END

#endif
