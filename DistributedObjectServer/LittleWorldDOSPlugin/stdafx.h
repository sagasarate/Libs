// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifdef WIN32
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

#include <afxwin.h>         // MFC 核心和标准组件

#endif




#include "../../Libs/Utils/Utils.h"
#include "../../Libs/NetLib/NetLib.h"
#include "../../Libs/DOSLib/DOSLib.h"

// TODO: 在此处引用程序需要的其他头文件
#include <math.h>

#define MAX_MAP_GRID_OBJECT_COUNT			100
#define MAP_OBJECT_MOVE_UPDATE_TIME			(500)
#define MAP_OBJECT_MSG_UPDATE_TIME			(500)
#define OBJECT_FIND_TIME					(5000)
#define NPC_STATUS_UPDATE_TIME				(100)
#define MAX_TARGET_UPDATE_TIME				(800)

#define MAX_MOVE_GRID						256
#define MINI_EVENT_BROADCAST_SIZE			2

#define MAX_MESSAGE_PACKET_SIZE				65536

#ifdef WIN32
#define USE_CRT_DETAIL_NEW
#endif

#define CONFIG_FILE_NAME					"LittleWorldDOSPlugin.xml"

enum LD_DIR
{
	LD_IX,
	LD_IX_DY,
	LD_DY,
	LD_DX_DY,
	LD_DX,
	LD_DX_IY,
	LD_IY,
	LD_IX_IY,
	LD_MAX,
};
enum LD_TYPE
{
	LT_NONE,
	LT_MOVE,
	LT_JUMP,
	LT_FALL,
};


inline UINT GetOppositeDir(UINT Dir)
{
	switch(Dir)
	{
	case LD_IX:
		return LD_DX;
	case LD_IX_DY:
		return LD_DX_IY;
	case LD_DY:
		return LD_IY;
	case LD_DX_DY:
		return LD_IX_IY;
	case LD_DX:
		return LD_IX;
	case LD_DX_IY:
		return LD_IX_DY;
	case LD_IY:
		return LD_DY;
	case LD_IX_IY:
		return LD_DX_DY;
	}
	return LD_MAX;
}

inline UINT GridOffsetToDir(int XOffset,int YOffset)
{
	static int s_GridOffsetToDir[3][3]={	{LD_DX_IY	,LD_IY		,LD_IX_IY},
	{LD_DX		,LD_MAX		,LD_IX},
	{LD_DX_DY	,LD_DY		,LD_IX_DY}
	};
	XOffset=XOffset+1;
	YOffset=1-YOffset;
	return s_GridOffsetToDir[YOffset][XOffset];
}

inline void DirToGridOffset(UINT Dir,int& XOffset,int& YOffset)
{
	switch(Dir)
	{
	case LD_IX:
		XOffset=1;
		YOffset=0;
		break;
	case LD_IX_DY:
		XOffset=1;
		YOffset=-1;
		break;
	case LD_DY:
		XOffset=0;
		YOffset=-1;
		break;
	case LD_DX_DY:
		XOffset=-1;
		YOffset=-1;
		break;
	case LD_DX:
		XOffset=-1;
		YOffset=0;
		break;
	case LD_DX_IY:
		XOffset=-1;
		YOffset=1;
		break;
	case LD_IY:
		XOffset=0;
		YOffset=1;
		break;
	case LD_IX_IY:
		XOffset=1;
		YOffset=1;
		break;
	default:
		XOffset=0;
		YOffset=0;
		break;
	}
}

#define SERVER_LOG_CHANNEL			8001

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

class CMapEventNode;
struct MAP_GRID_POS
{
	int			X;
	int			Y;
	CMapEventNode *	pMapNode;
};

struct MOVE_NOTIFY_GRIDS
{
	UINT			GridCount;
	MAP_GRID_POS *	pGrids;
};

#include "../LittleWorldProtocol/LittleWorldProtocol.h"

#include "MainConfig.h"

#include "Character.h"
#include "BaseNPC.h"
#include "Player.h"

#include "PlayerManager.h"



#include "NPCManager.h"

#include "MapObject.h"
#include "MapEventNode.h"
#include "MapCollisionNode.h"
#include "MapArea.h"

#include "CharQueryCenter.h"

//#define PERFORMANCE_STAT


#ifdef PERFORMANCE_STAT

#define FUNCTION_BEGIN				{static int s_FunctionIndex=0;if(!s_FunctionIndex) s_FunctionIndex=CPerformanceStatistician::GetInstance()->GetFunctionIndex(s_FunctionIndex,__FUNCTION__); CAutoPerformanceCounter tmpPerformanceCounter(s_FunctionIndex);
#define FUNCTION_END				}

#else

#define FUNCTION_BEGIN
#define FUNCTION_END

#endif

