// TestGameServer.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"


extern "C" UTILS_DLL_EXPORT BOOL InitPlugin(IDistributedObjectManager* pObjectManager);
extern "C" UTILS_DLL_EXPORT void ReleasePlugin();


CPlayerCharDataDBAdapter * g_pPlayerCharDataDBAdapter=NULL;
//CMapArea * g_pMapArea=NULL;
//CNPCManager * g_pNPCManager=NULL;

CPlayerCharDataDBAdapter * GetPlayerCharDataDBAdapter()
{
	return g_pPlayerCharDataDBAdapter;
}

BOOL InitPlugin(IDistributedObjectManager* pObjectManager)
{
	//_CrtSetBreakAlloc(155);
	SAFE_DELETE(g_pPlayerCharDataDBAdapter);

	if(!CMainConfig::GetInstance()->LoadConfig(MakeModuleFullPath(NULL,CONFIG_FILE_NAME)))
	{
		Log("加载配置文件[%s]失败",CONFIG_FILE_NAME);
		return FALSE;
	}


	DOS_OBJECT_REGISTER_INFO_EX RegisterInfo;

	if(CMainConfig::GetInstance()->IsPlayerCharDataDBEnable())
	{
		g_pPlayerCharDataDBAdapter=new CPlayerCharDataDBAdapter;

		RegisterInfo.pObject=g_pPlayerCharDataDBAdapter;
		RegisterInfo.ObjectID.ObjectTypeID=LWOT_PLATER_CHAR_DATA_DB_ADAPTER;
		RegisterInfo.Weight=1024;
		RegisterInfo.MsgProcessLimit=1024;
		pObjectManager->RegisterObject(RegisterInfo);
	}


	return TRUE;
}

void ReleasePlugin()
{
	SAFE_DELETE(g_pPlayerCharDataDBAdapter);
}
