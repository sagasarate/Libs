#include "StdAfx.h"

START_DOS_MSG_MAP(CNPCManager)
DOS_MSG_MAP(CMapAreaAckMsgHandler)
END_DOS_MSG_MAP(CNPCManager)

CNPCManager::CNPCManager(void)
{
	FUNCTION_BEGIN;
	INIT_DOS_MSG_MAP;
	m_pOperator=NULL;
	m_CurMapID=0;
	FUNCTION_END;
}

CNPCManager::~CNPCManager(void)
{
	FUNCTION_BEGIN;
	//if(m_pOperator)
	//	m_pOperator->Release();
	FUNCTION_END;
}

BOOL CNPCManager::Initialize(IDistributedObjectOperator * pOperator)
{
	FUNCTION_BEGIN;
	m_pOperator=pOperator;
	
	if(!m_NPCPool.Create(CMainConfig::GetInstance()->GetNPCManagerConfig().NPCPool.StartSize,
		CMainConfig::GetInstance()->GetNPCManagerConfig().NPCPool.GrowSize,
		CMainConfig::GetInstance()->GetNPCManagerConfig().NPCPool.GrowLimit))
		return FALSE;
	
	
	m_FindTimer.SaveTime();

	Log("NPC管理对象[%llX]已注册",
		m_pOperator->GetObjectID().ID);

	return TRUE;
	FUNCTION_END;
	return FALSE;
}
void CNPCManager::Destory()
{
	FUNCTION_BEGIN;


	void * Pos=m_NPCPool.GetFirstObjectPos();
	while(Pos)
	{
		CBaseNPC * pNPCController=m_NPCPool.GetNext(Pos);
		pNPCController->ForceRelease();
	}

	m_pOperator=NULL;

	FUNCTION_END;
}

BOOL CNPCManager::CreateNPC(UINT64 CharID,LPCTSTR CharName,float XPos,float YPos,float Height,float Direction)
{
	CBaseNPC * pNPC=NULL;
	UINT ID=m_NPCPool.NewObject(&pNPC);
	if(pNPC)
	{	
		if(pNPC->Init(this,ID,m_CurMapID,CharID,CharName,XPos,YPos,Height,Direction))
		{
			DOS_OBJECT_REGISTER_INFO_EX RegisterInfo;
			RegisterInfo.ObjectID.ObjectTypeID=LWOT_NPC;
			RegisterInfo.pObject=pNPC;
			if(m_pOperator->RegisterObject(RegisterInfo))
			{
				//Log("创建了NPC对象[%llu]%s(%g,%g)",CharID,CharName,XPos,YPos);
				return TRUE;
			}
		}
		DeleteNPC(ID);
	}
	Log("创建NPC对象失败[%llu]%s",CharID,CharName);
	return FALSE;
}

BOOL CNPCManager::DeleteNPC(UINT PoolID)
{
	//Log("删除NPC对象[%llu]",PoolID);
	return m_NPCPool.DeleteObject(PoolID);
}


void CNPCManager::OnConcernedObjectLost(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CNPCManager::OnObjectReport(OBJECT_ID ObjectID,const CSmartStruct& ObjectInfo)
{
	FUNCTION_BEGIN;
	if(ObjectID.ObjectTypeID==LWOT_MAP_AREA)
	{
		m_CurMapID=ObjectID;
		Log("NPCManager找到地图对象[%llX]！",m_CurMapID.ID);
	}
	if(ObjectID.ObjectTypeID==LWOT_CHAR_QUERY_CENTER)
	{
		m_CharQueryCenterID=ObjectID;
		Log("NPCManager找到CharQueryCenter[%llX]！",m_CharQueryCenterID.ID);
	}
	//if(m_CurMapID.ObjectTypeID&&m_CharQueryCenterID.ID)
	//{
	//	CMainConfig::BORN_POINT BornPoint;

	//	CMainConfig::GetInstance()->GetRandomBornPoint(BornPoint);

	//	int XDim=20;
	//	int YDim=20;
	//	int Dis=16;
	//	CEasyString Name;
	//	for(int y=0;y<YDim;y++)
	//	{
	//		for(int x=0;x<XDim;x++)
	//		{
	//			Name.Format("NPC%03d",NPC_ID_START+y*XDim+x);
	//			CreateNPC(NPC_ID_START+y*XDim+x,Name,BornPoint.PosX+(x-XDim/2)*Dis,BornPoint.PosY+(y-YDim/2)*Dis,0,0);
	//		}
	//	}
	//}
	FUNCTION_END;
}

int CNPCManager::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	if(m_FindTimer.IsTimeOut(OBJECT_FIND_TIME))
	{
		m_FindTimer.SaveTime();

		if(m_CurMapID.ID==0)
		{
			m_pOperator->FindObject(LWOT_MAP_AREA);			
		}

		if(m_CharQueryCenterID.ID==0)
		{
			m_pOperator->FindObject(LWOT_CHAR_QUERY_CENTER);			
		}
	}
	FUNCTION_END;
	return 0;
}


