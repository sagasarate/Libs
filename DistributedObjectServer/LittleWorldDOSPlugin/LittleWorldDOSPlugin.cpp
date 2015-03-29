// TestGameServer.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

extern "C" UTILS_DLL_EXPORT BOOL InitPlugin(IDistributedObjectManager* pObjectManager);
extern "C" UTILS_DLL_EXPORT void ReleasePlugin();


CPlayerManager * g_pPlayerManager=NULL;
CMapArea * g_pMapArea=NULL;
CNPCManager * g_pNPCManager=NULL;
CCharQueryCenter * g_pCharQueryCenter=NULL;

BOOL InitPlugin(IDistributedObjectManager* pObjectManager)
{	
	//_CrtSetBreakAlloc(155);
	SAFE_DELETE(g_pPlayerManager);
	SAFE_DELETE(g_pMapArea);
	SAFE_DELETE(g_pPlayerManager);
	SAFE_RELEASE(g_pCharQueryCenter);

	if(!CMainConfig::GetInstance()->LoadConfig(MakeModuleFullPath(NULL,CONFIG_FILE_NAME)))
	{
		Log("加载配置文件[%s]失败",CONFIG_FILE_NAME);
		return FALSE;
	}

	

	DOS_OBJECT_REGISTER_INFO_EX RegisterInfo;

	if(CMainConfig::GetInstance()->GetMapAreaConfig().Enable)
	{
		g_pMapArea=new CMapArea;
		
		RegisterInfo.pObject=g_pMapArea;
		RegisterInfo.ObjectID.ObjectTypeID=LWOT_MAP_AREA;
		RegisterInfo.Weight=1024;
		RegisterInfo.MsgProcessLimit=1024;	
		pObjectManager->RegisterObject(RegisterInfo);
	}

	if(CMainConfig::GetInstance()->GetPlayerManagerConfig().Enable)
	{
		g_pPlayerManager=new CPlayerManager;

		RegisterInfo.pObject=g_pPlayerManager;
		RegisterInfo.ObjectID.ObjectTypeID=LWOT_PLAYER_MANAGER;
		RegisterInfo.Weight=1;
		RegisterInfo.MsgProcessLimit=0;	
		pObjectManager->RegisterObject(RegisterInfo);
	}

	if(CMainConfig::GetInstance()->GetNPCManagerConfig().Enable)
	{
		g_pNPCManager=new CNPCManager;
		
		RegisterInfo.pObject=g_pNPCManager;
		RegisterInfo.ObjectID.ObjectTypeID=LWOT_NPC_MANAGER;
		RegisterInfo.Weight=1;
		RegisterInfo.MsgProcessLimit=0;
		pObjectManager->RegisterObject(RegisterInfo);
	}

	if(CMainConfig::GetInstance()->GetCharQueryCenterConfig().Enable)
	{
		g_pCharQueryCenter=new CCharQueryCenter;

		RegisterInfo.pObject=g_pCharQueryCenter;
		RegisterInfo.ObjectID.ObjectTypeID=LWOT_CHAR_QUERY_CENTER;
		RegisterInfo.Weight=1;
		RegisterInfo.MsgProcessLimit=0;
		pObjectManager->RegisterObject(RegisterInfo);
	}
	
	return TRUE;
}

void ReleasePlugin()
{
	SAFE_DELETE(g_pNPCManager);
	SAFE_DELETE(g_pMapArea);
	SAFE_DELETE(g_pPlayerManager);	
}