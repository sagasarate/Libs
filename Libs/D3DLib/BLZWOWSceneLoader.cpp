#include "StdAfx.h"

namespace D3DLib{

#define MAX_UPDATE_COST 20000000

CBLZWOWSceneLoader::CBLZWOWSceneLoader(void)
{
	m_pDevice=NULL;
	m_pRender=NULL;
	m_CurMapID=-1;
	m_CurMapIsBigAlphaMask;
	m_CurPosX=0;
	m_CurPosY=0;
	m_CurAreaX=0;
	m_CurAreaY=0;
	m_pCurMapInfo=NULL;
	m_LoadSatus=0;	
	m_pLowSceneModel=NULL;

}

CBLZWOWSceneLoader::~CBLZWOWSceneLoader(void)
{
	Destory();
}


bool CBLZWOWSceneLoader::Init(CD3DDevice * pDevice,CD3DSceneRender * pRender)
{
	m_pDevice=pDevice;
	m_pRender=pRender;
	m_SceneAreas.Resize(BLZ_WOW_MAP_WIDTH*BLZ_WOW_MAP_WIDTH);
	m_TaskQueue.Create(10240);
	m_FinishTaskQueue.Create(10240);

	if(!Start())
		return false;
	return true;
}

void CBLZWOWSceneLoader::Destory()
{
	SafeTerminate();
	for(UINT i=0;i<m_SceneAreas.GetCount();i++)
	{
		SAFE_RELEASE(m_SceneAreas[i].pModel);
	}
	m_SceneAreas.Clear();
	SAFE_RELEASE(m_pLowSceneModel);
}

int CBLZWOWSceneLoader::MoveTo(UINT MapID,FLOAT PosX,FLOAT PosY)
{
	if(m_CurMapID!=MapID)
	{
		if(!FirstLoad(MapID,PosX,PosY))		
		{
			return SLR_ERROR;
		}
	}
	else
	{
		int CenterAreaX=(int)(PosX/BLZ_ADT_MAP_AREA_SIZE);
		int CenterAreaY=(int)(-PosY/BLZ_ADT_MAP_AREA_SIZE);
		UINT Index=CenterAreaY*BLZ_WOW_MAP_WIDTH+CenterAreaX;
		if(Index<m_SceneAreas.GetCount())
		{
			if(m_SceneAreas[Index].Status!=MAS_NOT_EXIST)
			{
				m_CurPosX=PosX;
				m_CurPosY=PosY;
				m_CurAreaX=CenterAreaX;
				m_CurAreaY=CenterAreaY;
			}
			else
			{
				return SLR_ERROR;
			}
		}
		else
		{
			return SLR_ERROR;
		}
	}
	CheckTasks();
	CEasyTime Time;
	Time.FetchLocalTime();
	UINT Minutes=Time.Hour()*60+Time.Minute();
	D3DCOLOR Color;
	if(CBLZWOWDatabase::GetInstance()->GetMapLightColor(MapID,CD3DVector3(PosX,0,PosY),DBC_LICI_FOG_COLOR,Minutes,Color))
	{
		m_pRender->SetFogColor(Color);
	}
	if(CBLZWOWDatabase::GetInstance()->GetMapLightColor(MapID,CD3DVector3(PosX,0,PosY),DBC_LICI_WATER_COLOR_LIGHT,Minutes,Color))
	{
		m_pRender->SetWaterColorLight(Color);
	}
	if(CBLZWOWDatabase::GetInstance()->GetMapLightColor(MapID,CD3DVector3(PosX,0,PosY),DBC_LICI_WATER_COLOR_DARK,Minutes,Color))
	{
		m_pRender->SetWaterColorDark(Color);
	}
	if(CBLZWOWDatabase::GetInstance()->GetMapLightColor(MapID,CD3DVector3(PosX,0,PosY),DBC_LICI_SHADOW_COLOR,Minutes,Color))
	{
		m_pRender->SetShadowColor(Color);
	}
	if(CBLZWOWDatabase::GetInstance()->GetMapLightColor(MapID,CD3DVector3(PosX,0,PosY),DBC_LICI_LIGHT_AMBIENT,Minutes,Color))
	{
		for(UINT i=0;i<m_SceneLight.GetCount();i++)
		{
			m_SceneLight[i]->SetAmbient(Color);
		}
	}
	if(CBLZWOWDatabase::GetInstance()->GetMapLightColor(MapID,CD3DVector3(PosX,0,PosY),DBC_LICI_LIGHT_DIFFUSE,Minutes,Color))
	{
		for(UINT i=0;i<m_SceneLight.GetCount();i++)
		{
			m_SceneLight[i]->SetAmbient(Color);
		}
	}
	if(m_Status==SLS_CRUCIAL_WORING)
		return SLR_NEED_WAIT;
	return SLR_READY;
}

BOOL CBLZWOWSceneLoader::OnRun()
{
	TASK_INFO TaskInfo;
	while(m_TaskQueue.PopFront(TaskInfo))
	{
		switch(TaskInfo.TaskType)
		{
		case TASK_AREA_PRE_LOAD:
		case TASK_CRUCIAL_AREA_LOAD:
			LoadArea(TaskInfo);
			break;
		case TASK_AREA_LOAD:
			LoadAreaObjects(TaskInfo);
			break;
		case TASK_AREA_PRE_RELEASE:
			PreReleaseArea(TaskInfo.pMapArea);			
			break;
		case TASK_AREA_RELEASE:
			SAFE_RELEASE(TaskInfo.pMapArea->pModel);
			TaskInfo.pMapArea->Status=MAS_NOT_LOAD;
			PrintD3DDebugLog(_T("区块[%u,%u]释放完毕"),
				TaskInfo.pMapArea->AreaX,
				TaskInfo.pMapArea->AreaY);
			if(!m_FinishTaskQueue.PushBack(TaskInfo))
			{
				PrintD3DLog(_T("任务完成队列溢出"));
			}
			break;
		case TASK_PRE_LOAD_FINISH:
		case TASK_ALL_FINISH:
			if(!m_FinishTaskQueue.PushBack(TaskInfo))
			{
				PrintD3DLog(_T("任务完成队列溢出"));
			}
			break;
		}
	}	
	Sleep(1);
	return TRUE;
}

int CBLZWOWSceneLoader::Update(int ProcessLimit)
{
	CEasyTimerEx CostTimer;

	CostTimer.SaveTime();

	int ProcessCount=0;
	TASK_INFO TaskInfo;
	while(m_FinishTaskQueue.PopFront(TaskInfo))
	{
		switch(TaskInfo.TaskType)
		{
		case TASK_OBJECT_LOAD:
			m_pRender->AddSceneObject(TaskInfo.pModel,false);
			break;
		case TASK_OBJECT_PRE_RELEASE:
			m_pRender->DelObject(TaskInfo.pModel,false);
			break;
		case TASK_OBJECT_RELEASE:
			break;
		case TASK_AREA_PRE_LOAD:
			m_pRender->AddSceneObject(TaskInfo.pMapArea->pModel,false);
			PrintD3DDebugLog(_T("区块[%u,%u]预加载完毕"),
				TaskInfo.pMapArea->AreaX,
				TaskInfo.pMapArea->AreaY);
			TaskInfo.TaskType=TASK_AREA_LOAD;
			if(!m_TaskQueue.PushBack(TaskInfo))
			{
				PrintD3DLog(_T("任务队列溢出"));
			}
			//m_pLowSceneModel->SetAreaVisible(
			//	TaskInfo.pMapArea->AreaX,
			//	TaskInfo.pMapArea->AreaY,false);
			break;
		case TASK_CRUCIAL_AREA_LOAD:
			m_pRender->AddSceneObject(TaskInfo.pMapArea->pModel,false);
			PrintD3DDebugLog(_T("区块[%u,%u]预加载完毕"),
				TaskInfo.pMapArea->AreaX,
				TaskInfo.pMapArea->AreaY);
			TaskInfo.TaskType=TASK_AREA_LOAD;
			if(!m_TaskQueue.PushBack(TaskInfo))
			{
				PrintD3DLog(_T("任务队列溢出"));
			}
			//m_pLowSceneModel->SetAreaVisible(
			//	TaskInfo.pMapArea->AreaX,
			//	TaskInfo.pMapArea->AreaY,false);
			m_Status=SLS_WORING;		
			break;
		case TASK_AREA_LOAD:
			PrintD3DDebugLog(_T("区块[%u,%u]加载完毕"),
				TaskInfo.pMapArea->AreaX,
				TaskInfo.pMapArea->AreaY);
			TaskInfo.pMapArea->pModel->CheckNeedUpdateObjects();
			break;
		case TASK_AREA_PRE_RELEASE:
			m_pRender->DelObject(TaskInfo.pMapArea->pModel,false);
			PrintD3DDebugLog(_T("区块[%u,%u]预释放完毕"),
				TaskInfo.pMapArea->AreaX,
				TaskInfo.pMapArea->AreaY);
			TaskInfo.TaskType=TASK_AREA_RELEASE;
			if(!m_TaskQueue.PushBack(TaskInfo))
			{
				PrintD3DLog(_T("任务队列溢出"));
			}
			//m_pLowSceneModel->SetAreaVisible(
			//	TaskInfo.pMapArea->AreaX,
			//	TaskInfo.pMapArea->AreaY,true);
			break;
		case TASK_AREA_RELEASE:			
			break;
		case TASK_PRE_LOAD_FINISH:
			PrintD3DDebugLog(_T("全部预加载完毕"));
			TaskInfo.TaskType=TASK_ALL_FINISH;
			if(!m_TaskQueue.PushBack(TaskInfo))
			{
				PrintD3DLog(_T("任务队列溢出"));
			}
			break;
		case TASK_ALL_FINISH:
			PrintD3DDebugLog(_T("全部加载完毕"));
			m_Status=SLS_IDLE;
			break;
		}
		ProcessCount++;
		UINT64 CostTime=CostTimer.GetPastTime();		
		if(CostTime>ProcessLimit)
			break;
	}		
	return ProcessCount;
}

void CBLZWOWSceneLoader::AddSceneLight(CD3DLight * pLight)
{
	m_SceneLight.Add(pLight);
}
bool CBLZWOWSceneLoader::DeleteSceneLight(CD3DLight * pLight)
{
	for(UINT i=0;i<m_SceneLight.GetCount();i++)
	{
		if(m_SceneLight[i]==pLight)
		{
			m_SceneLight.Delete(i);
			return true;
		}
	}
	return false;
}

bool CBLZWOWSceneLoader::FirstLoad(UINT MapID,FLOAT PosX,FLOAT PosY)
{
	CBLZWOWDatabase::BLZ_DB_MAP_INFO * pMapInfo=CBLZWOWDatabase::GetInstance()->FindMapInfo(MapID);
	if(pMapInfo)
	{
		CEasyString WDTFileName;
		WDTFileName.Format(_T("%s\\%s\\%s.wdt"),
			(LPCTSTR)WOW_MAP_ADT_FILE_DIR,(LPCTSTR)pMapInfo->InternalName,(LPCTSTR)pMapInfo->InternalName);
		CBLZWDTFile WDTFile;
		if(WDTFile.Load(WDTFileName))
		{
			CEasyString WDLFileName;

			WDLFileName.Format(_T("%s\\%s\\%s.wdl"),
				(LPCTSTR)WOW_MAP_ADT_FILE_DIR,(LPCTSTR)pMapInfo->InternalName,(LPCTSTR)pMapInfo->InternalName);

			SAFE_RELEASE(m_pLowSceneModel);
			m_pLowSceneModel=new CD3DWOWWDLModel();
			m_pLowSceneModel->SetDevice(m_pDevice);

			if(m_pLowSceneModel->LoadFromFile(WDLFileName))
			{
				m_pRender->AddBackGroundObject(m_pLowSceneModel);
			}
			else
			{
				PrintD3DLog(_T("加载%s失败"),(LPCTSTR)WDLFileName);
				SAFE_RELEASE(m_pLowSceneModel);
			}
			
			int CenterAreaX=(int)(PosX/BLZ_ADT_MAP_AREA_SIZE);
			int CenterAreaY=(int)(-PosY/BLZ_ADT_MAP_AREA_SIZE);
			if(WDTFile.IsAreaExist(CenterAreaX,CenterAreaY))
			{
				m_CurMapID=MapID;
				m_CurMapIsBigAlphaMask=WDTFile.IsBigAlphaMask();
				m_pCurMapInfo=pMapInfo;
				m_CurPosX=PosX;
				m_CurPosY=PosY;
				m_CurAreaX=CenterAreaX;
				m_CurAreaY=CenterAreaY;

				

				for(UINT i=0;i<m_SceneAreas.GetCount();i++)
				{
					m_SceneAreas[i].MapID=MapID;
					m_SceneAreas[i].pMapInfo=pMapInfo;
					m_SceneAreas[i].AreaX=i%BLZ_WOW_MAP_WIDTH;
					m_SceneAreas[i].AreaY=i/BLZ_WOW_MAP_WIDTH;

					SAFE_RELEASE(m_SceneAreas[i].pModel);

					if(WDTFile.IsAreaExist(m_SceneAreas[i].AreaX,m_SceneAreas[i].AreaY))
					{
						m_SceneAreas[i].Status=MAS_NOT_LOAD;
					}
					else
					{
						m_SceneAreas[i].Status=MAS_NOT_EXIST;						
					}
				}
				return true;
			}
			else
			{
				PrintD3DLog(_T("所要到达的区域[%u,%u,%u]不存在"),MapID,CenterAreaX,CenterAreaY);
			}
		}
		else
		{
			PrintD3DLog(_T("无法打开文件%s"),(LPCTSTR)WDTFileName);
		}
	}
	else
	{
		PrintD3DLog(_T("地图%u不存在"),MapID);
	}
	return false;
}


bool CBLZWOWSceneLoader::IsAreaInInView(int AreaX,int AreaY)
{
	return (abs(m_CurAreaX-AreaX)<=SCENE_VIEW_IN_BORDER&&abs(m_CurAreaY-AreaY)<=SCENE_VIEW_IN_BORDER);
}

bool CBLZWOWSceneLoader::IsAreaInOutView(int AreaX,int AreaY)
{
	return (abs(m_CurAreaX-AreaX)<=SCENE_VIEW_OUT_BORDER&&abs(m_CurAreaY-AreaY)<=SCENE_VIEW_OUT_BORDER);
}


void CBLZWOWSceneLoader::CheckTasks()
{
	MAP_AREA_INFO * pCrucialLoadArea=NULL;
	CEasyArray<MAP_AREA_INFO *>	LoadAreas;
	CEasyArray<MAP_AREA_INFO *>	ReleaseAreas;

	for(UINT i=0;i<m_SceneAreas.GetCount();i++)
	{
		if(IsAreaInOutView(m_SceneAreas[i].AreaX,m_SceneAreas[i].AreaY))
		{
			if(IsAreaInInView(m_SceneAreas[i].AreaX,m_SceneAreas[i].AreaY))
			{				
				if(m_SceneAreas[i].Status==MAS_NOT_LOAD)
				{
					m_SceneAreas[i].Status=MAS_LOADING;
					if(m_SceneAreas[i].pModel)
					{
						PrintD3DLog(_T("加载区块时发现区块[%u,%u]已有内容"),
							m_SceneAreas[i].AreaX,
							m_SceneAreas[i].AreaY);
					}
					if(m_SceneAreas[i].AreaX==m_CurAreaX&&m_SceneAreas[i].AreaY==m_CurAreaY)
					{
						
						pCrucialLoadArea=m_SceneAreas.GetObject(i);
					}
					else
					{
						
						LoadAreas.Add(m_SceneAreas.GetObject(i));
					}
				}
			}
		}
		else
		{
			if(m_SceneAreas[i].Status==MAS_LOADED&&m_SceneAreas[i].pModel)
			{			
				m_SceneAreas[i].Status=MAS_LOADING;				
				ReleaseAreas.Add(m_SceneAreas.GetObject(i));
			}
		}
	}
	if(pCrucialLoadArea)
	{
		m_Status=SLS_CRUCIAL_WORING;
		TASK_INFO TaskInfo;
		TaskInfo.TaskType=TASK_CRUCIAL_AREA_LOAD;
		TaskInfo.pModel=NULL;
		TaskInfo.pMapArea=pCrucialLoadArea;
		m_TaskQueue.PushBack(TaskInfo);
		PrintD3DDebugLog(_T("开始加载关键区块[%u,%u]"),
			pCrucialLoadArea->AreaX,
			pCrucialLoadArea->AreaY);
	}
	if(LoadAreas.GetCount()||ReleaseAreas.GetCount())
	{
		if(m_Status!=SLS_CRUCIAL_WORING)
			m_Status=SLS_WORING;
		for(UINT i=0;i<LoadAreas.GetCount();i++)
		{
			TASK_INFO TaskInfo;
			TaskInfo.TaskType=TASK_AREA_PRE_LOAD;
			TaskInfo.pModel=NULL;
			TaskInfo.pMapArea=LoadAreas[i];
			m_TaskQueue.PushBack(TaskInfo);
			PrintD3DDebugLog(_T("开始加载区块[%u,%u]"),
				LoadAreas[i]->AreaX,
				LoadAreas[i]->AreaY);
		}

		for(UINT i=0;i<ReleaseAreas.GetCount();i++)
		{
			TASK_INFO TaskInfo;
			TaskInfo.TaskType=TASK_AREA_PRE_RELEASE;
			TaskInfo.pModel=NULL;
			TaskInfo.pMapArea=ReleaseAreas[i];
			m_TaskQueue.PushBack(TaskInfo);
			PrintD3DDebugLog(_T("开始卸载区块[%u,%u]"),
				ReleaseAreas[i]->AreaX,
				ReleaseAreas[i]->AreaY);
		}

		TASK_INFO TaskInfo;
		TaskInfo.TaskType=TASK_PRE_LOAD_FINISH;
		TaskInfo.pModel=NULL;
		TaskInfo.pMapArea=NULL;
		m_TaskQueue.PushBack(TaskInfo);
	}	
}

void CBLZWOWSceneLoader::LoadArea(TASK_INFO& TaskInfo)
{
	PrintD3DDebugLog(_T("区块[%u,%u]加载中..."),
		TaskInfo.pMapArea->AreaX,
		TaskInfo.pMapArea->AreaY);	

	CEasyString AreaFile=WOW_MAP_ADT_FILE_DIR;
	AreaFile.Format(_T("%s\\%s\\%s_%d_%d.adt"),
		WOW_MAP_ADT_FILE_DIR,
		(LPCTSTR)TaskInfo.pMapArea->pMapInfo->InternalName,
		(LPCTSTR)TaskInfo.pMapArea->pMapInfo->InternalName,
		TaskInfo.pMapArea->AreaX,
		TaskInfo.pMapArea->AreaY);

	SAFE_RELEASE(TaskInfo.pMapArea->pModel);

	TaskInfo.pMapArea->pModel=new CD3DWOWADTModel();
	TaskInfo.pMapArea->pModel->SetDevice(m_pDevice);

	if(TaskInfo.pMapArea->pModel->LoadFromFile(AreaFile,m_CurMapIsBigAlphaMask,false))
	{
		if(!m_FinishTaskQueue.PushBack(TaskInfo))
		{
			PrintD3DLog(_T("任务完成队列溢出"));
		}
	}
	else
	{
		PrintD3DLog(_T("CBLZWOWSceneLoader::LoadArea:加载失败%s"),(LPCTSTR)AreaFile);
		SAFE_RELEASE(TaskInfo.pMapArea->pModel);
		TaskInfo.pMapArea->Status=MAS_NOT_EXIST;	
	}

	
}

void CBLZWOWSceneLoader::LoadAreaObjects(TASK_INFO& TaskInfo)
{
	TaskInfo.pMapArea->pModel->LoadObjects();

	for(UINT i=0;i<TaskInfo.pMapArea->pModel->GetChildCount();i++)
	{
		PushObjectFinishEvent(TaskInfo.pMapArea->pModel->GetChildByIndex(i));
	}

	TaskInfo.pMapArea->Status=MAS_LOADED;

	if(!m_FinishTaskQueue.PushBack(TaskInfo))
	{
		PrintD3DLog(_T("任务完成队列溢出"));
	}
}

void CBLZWOWSceneLoader::PreReleaseArea(MAP_AREA_INFO * pMapArea)
{
	pMapArea->Status=MAS_LOADING;
	for(UINT i=0;i<pMapArea->pModel->GetChildCount();i++)
	{
		PushObjectPreReleaseEvent(pMapArea->pModel->GetChildByIndex(i));
	}

	TASK_INFO Info;
	Info.TaskType=TASK_AREA_PRE_RELEASE;
	Info.pMapArea=pMapArea;
	Info.pModel=NULL;
	if(!m_FinishTaskQueue.PushBack(Info))
	{
		PrintD3DLog(_T("任务完成队列溢出"));
	}
}

void CBLZWOWSceneLoader::PushObjectFinishEvent(CD3DObject * pObject)
{
	TASK_INFO Info;
	Info.TaskType=TASK_OBJECT_LOAD;
	Info.pMapArea=NULL;
	Info.pModel=pObject;
	if(!m_FinishTaskQueue.PushBack(Info))
	{
		PrintD3DLog(_T("任务完成队列溢出"));
	}

	for(UINT i=0;i<pObject->GetChildCount();i++)
	{
		PushObjectFinishEvent(pObject->GetChildByIndex(i));
	}
	
}


void CBLZWOWSceneLoader::PushObjectPreReleaseEvent(CD3DObject * pObject)
{
	for(UINT i=0;i<pObject->GetChildCount();i++)
	{
		PushObjectPreReleaseEvent(pObject->GetChildByIndex(i));
	}

	TASK_INFO Info;
	Info.TaskType=TASK_OBJECT_PRE_RELEASE;
	Info.pMapArea=NULL;
	Info.pModel=pObject;
	if(!m_FinishTaskQueue.PushBack(Info))
	{
		PrintD3DLog(_T("任务完成队列溢出"));
	}
}

}