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

#define MONO_ASSEMBLY_DOSSYSTEM							"DOSSystem.dll"
#define MONO_NAME_SPACE_DOSSYSTEM						"DOSSystem"
#define MONO_CLASS_NAME_OBJECT_ID						"OBJECT_ID"
#define MONO_CLASS_NAME_DISTRIBUTED_OBJECT_OPERATOR		"DistributedObjectOperator"
#define MONO_CLASS_NAME_DOS_OBJECT_REGISTER_INFO_EX		"DOS_OBJECT_REGISTER_INFO_EX"

#define MONO_CLASS_FIELD_NAME_OBJECT_ID_ID				"ID"

#define MONO_CLASS_FIELD_NAME_DORI_OBJECT_ID			"ObjectID"
#define MONO_CLASS_FIELD_NAME_DORI_WEIGHT				"Weight"
#define MONO_CLASS_FIELD_NAME_DORI_OBJECT_GROUP_INDEX	"ObjectGroupIndex"
#define MONO_CLASS_FIELD_NAME_DORI_MSG_QUEUE_SIZE		"MsgQueueSize"
#define MONO_CLASS_FIELD_NAME_DORI_MSG_PROCESS_LIMIT	"MsgProcessLimit"
#define MONO_CLASS_FIELD_NAME_DORI_OBJECT				"Object"


#define MONO_CLASS_METHOD_NAME_DO_INITIALIZE				"Initialize"
#define MONO_CLASS_METHOD_NAME_DO_DESTORY					"Destory"
#define MONO_CLASS_METHOD_NAME_DO_ONPRETRANSLATEMESSAGE		"OnPreTranslateMessage"
#define MONO_CLASS_METHOD_NAME_DO_ONMESSAGE					"OnMessage"
#define MONO_CLASS_METHOD_NAME_DO_ONSYSTEMMESSAGE			"OnSystemMessage"
#define MONO_CLASS_METHOD_NAME_DO_ONCONCERNEDOBJECTLOST		"OnConcernedObjectLost"
#define MONO_CLASS_METHOD_NAME_DO_ONFINDOBJECT				"OnFindObject"
#define MONO_CLASS_METHOD_NAME_DO_ONOBJECTREPORT			"OnObjectReport"
#define MONO_CLASS_METHOD_NAME_DO_ONPROXYOBJECTIPREPORT		"OnProxyObjectIPReport"
#define MONO_CLASS_METHOD_NAME_DO_ONSHUTDOWN				"OnShutDown"
#define MONO_CLASS_METHOD_NAME_DO_UPDATE					"Update"

#define MONO_CLASS_METHOD_PARAM_DO_INITIALIZE				1
#define MONO_CLASS_METHOD_PARAM_DO_DESTORY					0
#define MONO_CLASS_METHOD_PARAM_DO_ONPRETRANSLATEMESSAGE	1
#define MONO_CLASS_METHOD_PARAM_DO_ONMESSAGE				1
#define MONO_CLASS_METHOD_PARAM_DO_ONSYSTEMMESSAGE			1
#define MONO_CLASS_METHOD_PARAM_DO_ONCONCERNEDOBJECTLOST	1
#define MONO_CLASS_METHOD_PARAM_DO_ONFINDOBJECT				1
#define MONO_CLASS_METHOD_PARAM_DO_ONOBJECTREPORT			2
#define MONO_CLASS_METHOD_PARAM_DO_ONPROXYOBJECTIPREPORT	4
#define MONO_CLASS_METHOD_PARAM_DO_ONSHUTDOWN				1
#define MONO_CLASS_METHOD_PARAM_DO_UPDATE					1


#define MONO_CLASS_METHOD_NAME_OBJECT_ID_CTOR			".ctor"
#define MONO_CLASS_METHOD_PARAM_OBJECT_ID_CTOR			1
#define MONO_CLASS_METHOD_NAME_DOO_CTOR					".ctor"
#define MONO_CLASS_METHOD_PARAM_DOO_CTOR				1

#define MONO_DOMAIN_FINALIZE_TIMEOUT					(5*60*1000)

#define PLUGIN_LOG_CHANNEL_START	12000

#ifdef WIN32
#define USE_CRT_DETAIL_NEW
#endif

#include "../../Libs/Utils/Utils.h"
#include "../../Libs/NetLib/NetLib.h"
#include "../../Libs/ServerFrameWork/ServerFrameWork.h"

#include "../../Libs/DOSLib/DOSLib.h"
#include "../../Libs//MonoWrapper/MonoWrapper.h"

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

extern "C" typedef BOOL(*PLUGIN_INIT_FN)(IDistributedObjectManager* pObjectManager, UINT PluginID, UINT LogChannel);
extern "C" typedef void(*PLUGIN_QUERY_RELEASE_FN)();
extern "C" typedef void(*PLUGIN_RELEASE_FN)();

enum PLUGIN_TYPE
{
	PLUGIN_TYPE_NATIVE,
	PLUGIN_TYPE_CSHARP,
};
enum PLUGIN_LOAD_TYPE
{
	PLUGIN_LOAD_TYPE_ASSEMBLY,
	PLUGIN_LOAD_TYPE_SOURCE_CODE,
};
enum PLUGIN_STATUS
{
	PLUGIN_STATUS_NONE,
	PLUGIN_STATUS_COMPILEING,
	PLUGIN_STATUS_COMPILED,
	PLUGIN_STATUS_RUNNING,
	PLUGIN_STATUS_ERROR,
};

struct MONO_CLASS_INFO_DORI
{
	MonoClass *			pClass;
	MonoClassField *	pFeildObjectID;
	MonoClassField *	pFeildWeight;
	MonoClassField *	pFeildObjectGroupIndex;
	MonoClassField *	pFeildMsgQueueSize;
	MonoClassField *	pFeildMsgProcessLimit;
	MonoClassField *	pFeildObject;
	MONO_CLASS_INFO_DORI()
	{
		pClass = NULL;
		pFeildObjectID = NULL;
		pFeildWeight = NULL;
		pFeildObjectGroupIndex = NULL;
		pFeildMsgQueueSize = NULL;
		pFeildMsgProcessLimit = NULL;
		pFeildObject = NULL;
	}
	bool IsValid()
	{
		return pClass != NULL&&pFeildObjectID != NULL&&pFeildWeight != NULL&&
			pFeildObjectGroupIndex != NULL&&pFeildMsgQueueSize != NULL&&
			pFeildMsgProcessLimit != NULL&&pFeildObject != NULL;
	}
};

struct MONO_DOMAIN_INFO
{
	MonoDomain *						pMonoDomain;
	MonoAssembly *						pMonoAssembly_DOSSystem;
	MonoClass *							pMonoClass_ObjectID;
	MonoClassField *					pMonoClassField_ObjectID_ID;
	MonoMethod *						pMonoClassMethod_ObjectID_Ctor;
	MonoClass *							pMonoClass_DistributedObjectOperator;
	MonoMethod *						pMonoClassMethod_DistributedObjectOperator_Ctor;
	MONO_CLASS_INFO_DORI				MonoClassInfo_DORI;

	MONO_DOMAIN_INFO()
	{
		pMonoDomain = NULL;
		pMonoAssembly_DOSSystem = NULL;
		pMonoClass_ObjectID = NULL;
		pMonoClassField_ObjectID_ID = NULL;
		pMonoClassMethod_ObjectID_Ctor = NULL;
		pMonoClass_DistributedObjectOperator = NULL;
		pMonoClassMethod_DistributedObjectOperator_Ctor = NULL;
	}
};

struct PLUGIN_INFO
{
	PLUGIN_TYPE					PluginType;
	CEasyString					PluginName;
	PLUGIN_LOAD_TYPE			LoadType;
	CEasyArray<CEasyString>		SourceDirs;
	CEasyString					MainClassNameSpace;
	CEasyString					MainClass;

	UINT						ID;
	PLUGIN_STATUS				PluginStatus;
	CEasyString					ModuleFileName;
	HMODULE						hModule;
	PLUGIN_INIT_FN				pInitFN;
	PLUGIN_QUERY_RELEASE_FN		pQueryReleaseFN;
	PLUGIN_RELEASE_FN			pReleaseFN;
	MonoAssembly *				pCSPluginAssembly;
	UINT						hCSMainObj;
	HANDLE						hMCSProcess;
	UINT						LogChannel;
	MONO_DOMAIN_INFO			MonoDomainInfo;

	PLUGIN_INFO()
	{
		PluginType = PLUGIN_TYPE_NATIVE;
		LoadType = PLUGIN_LOAD_TYPE_ASSEMBLY;


		ID = 0;
		PluginStatus = PLUGIN_STATUS_NONE;
		hModule = NULL;
		pInitFN = NULL;
		pQueryReleaseFN = NULL;
		pReleaseFN = NULL;
		pCSPluginAssembly = NULL;
		hCSMainObj = 0;
		hMCSProcess = NULL;
		LogChannel = 0;
	}
};

struct DOS_OBJECT_REGISTER_INFO_FOR_CS
{
	OBJECT_ID				ObjectID;
	int						Weight;
	int						ObjectGroupIndex;
	UINT					MsgQueueSize;
	UINT					MsgProcessLimit;
	MonoObject * 			pObject;
};



struct MONO_CLASS_INFO_DO
{
	MonoClass *			pClass;
	MonoMethod *		pInitializeMethod;
	MonoMethod *		pDestoryMethod;
	MonoMethod *		pOnPreTranslateMessageMethod;
	MonoMethod *		pOnMessageMethod;
	MonoMethod *		pOnSystemMessageMethod;
	MonoMethod *		pOnConcernedObjectLostMethod;
	MonoMethod *		pOnFindObjectMethod;
	MonoMethod *		pOnObjectReportMethod;
	MonoMethod *		pOnProxyObjectIPReportMethod;
	MonoMethod *		pOnShutDownMethod;
	MonoMethod *		pUpdateMethod;
	MONO_CLASS_INFO_DO()
	{
		pClass = NULL;
		pInitializeMethod = NULL;
		pDestoryMethod = NULL;
		pOnPreTranslateMessageMethod = NULL;
		pOnMessageMethod = NULL;
		pOnSystemMessageMethod = NULL;
		pOnConcernedObjectLostMethod = NULL;
		pOnFindObjectMethod = NULL;
		pOnObjectReportMethod = NULL;
		pOnProxyObjectIPReportMethod = NULL;
		pOnShutDownMethod = NULL;
		pUpdateMethod = NULL;
	}
	bool IsValid()
	{
		return pClass != NULL&&pInitializeMethod != NULL&&pDestoryMethod != NULL&&
			pOnPreTranslateMessageMethod != NULL&&pOnMessageMethod != NULL&&
			pOnSystemMessageMethod != NULL&&pOnConcernedObjectLostMethod != NULL&&
			pOnFindObjectMethod != NULL&&pOnObjectReportMethod != NULL&&
			pOnProxyObjectIPReportMethod != NULL&&pOnShutDownMethod != NULL&&
			pUpdateMethod != NULL;
	}
};


#include "DistributedObjectOperator.h"
#include "DistributedObjectManager.h"



#include "DOSMainApp.h"
#include "DOSServerThread.h"
#include "DOSConfig.h"
#include "DOSMainThread.h"


//#define PERFORMANCE_STAT

#ifdef PERFORMANCE_STAT

#define FUNCTION_BEGIN				{static int s_FunctionIndex=0;if(!s_FunctionIndex) s_FunctionIndex=CPerformanceStatistician::GetInstance()->GetFunctionIndex(s_FunctionIndex,__FUNCTION__); CAutoPerformanceCounter tmpPerformanceCounter(s_FunctionIndex);
#define FUNCTION_END				}

#else

#define FUNCTION_BEGIN
#define FUNCTION_END

#endif
