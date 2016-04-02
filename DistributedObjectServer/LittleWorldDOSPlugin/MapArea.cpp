#include "StdAfx.h"




static char g_SendBuff[MAX_MESSAGE_PACKET_SIZE];

START_DOS_MSG_MAP(CMapArea)
DOS_MSG_MAP(CMapAreaMsgHandler)
END_DOS_MSG_MAP(CMapArea)



CMapArea::CMapArea(void)
{
	FUNCTION_BEGIN;

	INIT_DOS_MSG_MAP;

	m_pOperator=NULL;

	m_EventBroadcastSize=0;
	m_EventGridSize=0;
	m_CollisionGridSize=0;

	m_MapStartPosX=0;
	m_MapStartPosY=0;
	m_MapWidth=0;
	m_MapHeight=0;

	
	m_MapEventGridWidth=0;
	m_MapEventGridHeight=0;
	
	m_MapCollisionGridWidth=0;
	m_MapCollisionGridHeight=0;

	FUNCTION_END;
}

CMapArea::~CMapArea(void)
{
	FUNCTION_BEGIN;
	//if(m_pOperator)
	//	m_pOperator->Release();
	FUNCTION_END;
}

BOOL CMapArea::Initialize(IDistributedObjectOperator * pOperator)
{
	FUNCTION_BEGIN;
	m_pOperator=pOperator;

	m_ObjectPool.Create(CMainConfig::GetInstance()->GetMapAreaConfig().MapObjectPool.StartSize,
		CMainConfig::GetInstance()->GetMapAreaConfig().MapObjectPool.GrowSize,
		CMainConfig::GetInstance()->GetMapAreaConfig().MapObjectPool.GrowLimit);

	
	
	CreateMapGrid();
	InitNotifyGrids();
	

	m_MapNodeUpdateTimer.SaveTime();

	Log("地图区域对象[%llX]已注册",
		m_pOperator->GetObjectID().ID);

	//if(!m_pOperator->InitMsgProc(GD_MAP_MSG_END-GD_MAP_MSG_START))
	//	return FALSE;

	//m_pOperator->RegisterMsgProc(GD_CHAR_MSG_FIND_MAP,(DOS_MSG_PROC)&CMapArea::OnFindMap);
	//m_pOperator->RegisterMsgProc(GD_MAP_MSG_OBJECT_REGISTER,(DOS_MSG_PROC)&CMapArea::OnRegisterObject);
	//m_pOperator->RegisterMsgProc(GD_MAP_MSG_OBJECT_UNREGISTER,(DOS_MSG_PROC)&CMapArea::OnUnregisterObject);
	//m_pOperator->RegisterMsgProc(GD_MAP_MSG_QUERY_UPDATE_OBJECT,(DOS_MSG_PROC)&CMapArea::OnQueryUpdateObject);
	//m_pOperator->RegisterMsgProc(GD_MAP_MSG_QUERY_SEND_EVENT_MSG_PACKET,(DOS_MSG_PROC)&CMapArea::OnQuerySendEventMsgPacket);


	return TRUE;
	FUNCTION_END;
	return FALSE;
}

void CMapArea::Destory()
{
	FUNCTION_BEGIN;
	//m_pOperator->UnregisterMsgProc(GD_CHAR_MSG_FIND_MAP,(DOS_MSG_PROC)&CMapArea::OnFindMap);
	//m_pOperator->UnregisterMsgProc(GD_MAP_MSG_OBJECT_REGISTER,(DOS_MSG_PROC)&CMapArea::OnRegisterObject);
	//m_pOperator->UnregisterMsgProc(GD_MAP_MSG_OBJECT_UNREGISTER,(DOS_MSG_PROC)&CMapArea::OnUnregisterObject);
	//m_pOperator->UnregisterMsgProc(GD_MAP_MSG_QUERY_UPDATE_OBJECT,(DOS_MSG_PROC)&CMapArea::OnQueryUpdateObject);	
	//m_pOperator->UnregisterMsgProc(GD_MAP_MSG_QUERY_SEND_EVENT_MSG_PACKET,(DOS_MSG_PROC)&CMapArea::OnQuerySendEventMsgPacket);


	m_MapEventGridWidth=0;
	m_MapEventGridHeight=0;

	m_MapCollisionGridWidth=0;
	m_MapCollisionGridHeight=0;

	for(int i=0;i<LD_MAX;i++)
	{
		
		SAFE_DELETE_ARRAY(m_MoveGrids[i].pGrids);
	}
	SAFE_DELETE_ARRAY(m_FullNotifyGrids.pGrids);

	m_pOperator=NULL;

	FUNCTION_END;
}

UINT CMapArea::AddUseRef()
{
	return CNameObject::AddUseRef();
}
void CMapArea::Release()
{
	CNameObject::Release();
}


void CMapArea::OnConcernedObjectLost(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

int CMapArea::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	int ProcessCount=0;

	
	UINT CurTime=CEasyTimer::GetTime();
	void * Pos=m_ObjectPool.GetFirstObjectPos();
	while(Pos)
	{
		UINT64 Key;
		CMapObject * pObject=m_ObjectPool.GetNextObject(Pos,Key);
		ProcessCount+=pObject->Update(CurTime);
	}
	

	//if(m_MapNodeUpdateTimer.IsTimeOut(MAP_NODE_UPDATE_TIME))
	//{
	//	m_MapNodeUpdateTimer.SaveTime();
	//	for(UINT i=0;i<m_MapEventGridWidth*m_MapEventGridHeight;i++)
	//	{
	//		ProcessCount+=m_pMapEventGrids[i].Update(ProcessPacketLimit);
	//	}
	//}
	
	return ProcessCount;
	FUNCTION_END;
	return 0;
}

CMapObject * CMapArea::CreateMapObject(UINT64 ObjectID,OBJECT_ID ControllerID,OBJECT_ID AreaEventReceiverID,CSmartStruct& MoveStatusPacket,UINT Flag)
{
	FUNCTION_BEGIN;
	CMapObject * pMapObject=NULL;
	m_ObjectPool.New(ObjectID,&pMapObject);
	if(pMapObject)
	{
		if(pMapObject->Init(this,ObjectID,ControllerID,AreaEventReceiverID,MoveStatusPacket,Flag))
		{
			return pMapObject;
		}
		m_ObjectPool.Delete(ObjectID);
	}
	FUNCTION_END;
	return NULL;
}
CMapObject * CMapArea::GetMapObject(UINT64 ObjectID)
{
	FUNCTION_BEGIN;
	return m_ObjectPool.Find(ObjectID);
	FUNCTION_END;
	return NULL;
}

BOOL CMapArea::DeleteMapObject(CMapObject * pMapObject)
{
	FUNCTION_BEGIN;
	UINT64 ObjectID=pMapObject->GetObjectID();
	pMapObject->Destory();
	return m_ObjectPool.Delete(ObjectID);
	FUNCTION_END;
	return FALSE;
}


int CMapArea::RegisterMapObject(UINT64 ObjectID ,OBJECT_ID ControllerID ,OBJECT_ID AreaEventReceiverID ,CSmartStruct& MoveStatus ,UINT Flag )
{
	FUNCTION_BEGIN;
	
	CMapAreaAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());
	

	CMapObject * pMapObject=GetMapObject(ObjectID);
	if(pMapObject)
	{
		Log("(0x%llX)已经在地图中了！",ObjectID);
		MsgCaller.RegisterMapObjectAck(ObjectID,MORRT_ALREADY_REGISTER);		
		return TRUE;
	}
	pMapObject=CreateMapObject(ObjectID,ControllerID,AreaEventReceiverID,MoveStatus,Flag);
	if(pMapObject==NULL)
	{
		Log("(0x%llX)无法创建地图对象！",ObjectID);
		MsgCaller.RegisterMapObjectAck(ObjectID,MORRT_FAILED);
		return TRUE;
	}	
	
	LogDebug("(0x%llX)进入地图！",ObjectID);	
	MsgCaller.RegisterMapObjectAck(ObjectID,MORRT_OK);

	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}

int CMapArea::UnregisterMapObject(UINT64 ObjectID )
{	
	CMapObject * pMapObject=GetMapObject(ObjectID);
	if(pMapObject)
	{		
		DeleteMapObject(pMapObject);		

		LogDebug("(0x%llX)离开地图！",ObjectID);
	}
	else
	{
		Log("(0x%llu)并不在地图中，无法注销！",ObjectID);
	}
	return COMMON_RESULT_SUCCEED;
}

int CMapArea::UpdateMoveStatus(UINT64 ObjectID ,CSmartStruct& MoveStatus )
{
	CMapObject * pMapObject=GetMapObject(ObjectID);
	if(pMapObject)
	{	
		pMapObject->UpdateMoveStatus(MoveStatus);
	}
	return COMMON_RESULT_SUCCEED;
}

int CMapArea::SendEventMsgPacket(UINT64 ObjectID ,CSmartStruct& EventMsgPacket )
{
	CMapObject * pMapObject=GetMapObject(ObjectID);
	if(pMapObject)
	{
		CMapEventNode * pEventNode=pMapObject->GetMapEventNode();
		if(pEventNode)
		{
			pEventNode->SendEventMsgPacket(EventMsgPacket);
		}
	}
	return COMMON_RESULT_SUCCEED;
}

int CMapArea::HideOnMap(UINT64 ObjectID ,BYTE HideType )
{
	CMapAreaAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());

	CMapObject * pMapObject=GetMapObject(ObjectID);
	if(pMapObject)
	{
		if(pMapObject->HideOnMap(HideType))
		{
			MsgCaller.HideOnMapAck(ObjectID,HideType,HOMRT_OK);
		}
		else
		{
			MsgCaller.HideOnMapAck(ObjectID,HideType,HOMRT_FAILED);
		}
	}
	else
	{
		MsgCaller.HideOnMapAck(ObjectID,HideType,HOMRT_TARGET_NOT_FIND);
	}
	return COMMON_RESULT_SUCCEED;
}
int CMapArea::TeleportInMap(UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir )
{
	CMapAreaAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());
	CMapObject * pMapObject=GetMapObject(ObjectID);
	if(pMapObject)
	{
		if(pMapObject->Teleport(TargetPosX,TargetPosY,TargetHeight,TargetDir))
		{
			MsgCaller.TeleportInMapAck(ObjectID,TargetPosX,TargetPosY,TargetHeight,TargetDir,TIMRT_OK);
		}
		else
		{
			MsgCaller.TeleportInMapAck(ObjectID,TargetPosX,TargetPosY,TargetHeight,TargetDir,TIMRT_FAILED);
		}
	}
	else
	{
		MsgCaller.TeleportInMapAck(ObjectID,TargetPosX,TargetPosY,TargetHeight,TargetDir,TIMRT_TARGET_NOT_FIND);
	}
	return COMMON_RESULT_SUCCEED;
}

BOOL CMapArea::SendMsg(OBJECT_ID TargetID,MSG_ID_TYPE MsgID,LPCVOID pData,UINT DataSize)
{
	FUNCTION_BEGIN;
	return m_pOperator->SendMessage(TargetID,MsgID,0,pData,DataSize);
	FUNCTION_END;
	return FALSE;
}


MOVE_NOTIFY_GRIDS * CMapArea::GetMoveGrids(UINT Dir)
{
	FUNCTION_BEGIN;
	if(Dir<LD_MAX)
	{
		return m_MoveGrids+Dir;
	}
	FUNCTION_END;
	return NULL;
}

MOVE_NOTIFY_GRIDS *  CMapArea::GetFullNotifyGrids()
{
	FUNCTION_BEGIN;
	return &m_FullNotifyGrids;
	FUNCTION_END;
	return NULL;
}

void CMapArea::CreateMapGrid()
{
	FUNCTION_BEGIN;

	m_MapStartPosX=CMainConfig::GetInstance()->GetMapAreaConfig().MapStartX;
	m_MapStartPosY=CMainConfig::GetInstance()->GetMapAreaConfig().MapStartY;
	m_MapWidth=CMainConfig::GetInstance()->GetMapAreaConfig().MapWidth;
	m_MapHeight=CMainConfig::GetInstance()->GetMapAreaConfig().MapHeight;

	m_EventGridSize=CMainConfig::GetInstance()->GetMapAreaConfig().EventGridSize;
	m_CollisionGridSize=CMainConfig::GetInstance()->GetMapAreaConfig().CollisionGridSize;
	m_EventBroadcastSize=(UINT)ceil(CMainConfig::GetInstance()->GetMapAreaConfig().VisibleRange/m_EventGridSize);
	if(m_EventBroadcastSize<MINI_EVENT_BROADCAST_SIZE)
		m_EventBroadcastSize=MINI_EVENT_BROADCAST_SIZE;

	m_MapEventGridWidth=(UINT)(m_MapWidth/m_EventGridSize);
	m_MapEventGridHeight=(UINT)(m_MapHeight/m_EventGridSize);


	CMapEventNode::m_BroadcastIDBuffer.Create(CMainConfig::GetInstance()->GetMapAreaConfig().ObjectPoolInEventGrid.StartSize,
		CMainConfig::GetInstance()->GetMapAreaConfig().ObjectPoolInEventGrid.GrowSize);
	CMapEventNode::m_BroadcastPtrBuffer.Create(CMainConfig::GetInstance()->GetMapAreaConfig().ObjectPoolInEventGrid.StartSize,
		CMainConfig::GetInstance()->GetMapAreaConfig().ObjectPoolInEventGrid.GrowSize);



	m_pMapEventGrids.Resize(m_MapEventGridWidth*m_MapEventGridHeight);
	for(UINT y=0;y<m_MapEventGridHeight;y++)
	{
		for(UINT x=0;x<m_MapEventGridWidth;x++)
		{
			UINT Offset=y*m_MapEventGridWidth+x;

			m_pMapEventGrids[Offset].Create(this,CMainConfig::GetInstance()->GetMapAreaConfig().ObjectPoolInEventGrid,x,y);			
		}
	}

	m_MapCollisionGridWidth=(UINT)(m_MapWidth/m_CollisionGridSize);
	m_MapCollisionGridHeight=(UINT)(m_MapHeight/m_CollisionGridSize);

	m_pMapCollisionGrids.Resize(m_MapCollisionGridWidth*m_MapCollisionGridHeight);
	for(UINT y=0;y<m_MapCollisionGridHeight;y++)
	{
		for(UINT x=0;x<m_MapCollisionGridWidth;x++)
		{
			UINT Offset=y*m_MapCollisionGridWidth+x;

			m_pMapCollisionGrids[Offset].Create(this,x,y);
			for(UINT i=0;i<LD_MAX;i++)
			{
				int OffsetX,OffsetY;
				DirToGridOffset(i,OffsetX,OffsetY);
				CMapCollisionNode * pLinkNode=GetMapCollisionNode(x+OffsetX,y+OffsetY);
				if(pLinkNode)
					m_pMapCollisionGrids[Offset].SetLink(i,LT_MOVE,pLinkNode);
				else
					m_pMapCollisionGrids[Offset].SetLink(i,LT_NONE,NULL);
			}
		}
	}

	LogDebug("地图起始位置(%g,%g),碰撞格(%u)(%u,%u),事件格(%u)(%u,%u)",
		m_MapStartPosX,m_MapStartPosY,
		m_CollisionGridSize,m_MapCollisionGridWidth,m_MapCollisionGridHeight,
		m_EventGridSize,m_MapEventGridWidth,m_MapEventGridHeight);


	FUNCTION_END;
}

void CMapArea::InitNotifyGrids()
{
	FUNCTION_BEGIN;
	for(int i=0;i<LD_MAX;i++)
	{
		m_MoveGrids[i].GridCount=0;
		m_MoveGrids[i].pGrids=new MAP_GRID_POS[(GetEventBroadcastSize()*2+1)*2];
	}
	m_FullNotifyGrids.GridCount=0;
	m_FullNotifyGrids.pGrids=new MAP_GRID_POS[(GetEventBroadcastSize()*2+1)*(GetEventBroadcastSize()*2+1)];
	
	for(int i=-(int)GetEventBroadcastSize();i<=(int)GetEventBroadcastSize();i++)
	{
		m_MoveGrids[LD_IY].pGrids[m_MoveGrids[LD_IY].GridCount].X=i;
		m_MoveGrids[LD_IY].pGrids[m_MoveGrids[LD_IY].GridCount].Y=GetEventBroadcastSize();
		m_MoveGrids[LD_IY].GridCount++;

		m_MoveGrids[LD_IX_IY].pGrids[m_MoveGrids[LD_IX_IY].GridCount].X=i;
		m_MoveGrids[LD_IX_IY].pGrids[m_MoveGrids[LD_IX_IY].GridCount].Y=GetEventBroadcastSize();
		m_MoveGrids[LD_IX_IY].GridCount++;

		m_MoveGrids[LD_DX_IY].pGrids[m_MoveGrids[LD_DX_IY].GridCount].X=i;
		m_MoveGrids[LD_DX_IY].pGrids[m_MoveGrids[LD_DX_IY].GridCount].Y=GetEventBroadcastSize();
		m_MoveGrids[LD_DX_IY].GridCount++;

		m_MoveGrids[LD_DY].pGrids[m_MoveGrids[LD_DY].GridCount].X=i;
		m_MoveGrids[LD_DY].pGrids[m_MoveGrids[LD_DY].GridCount].Y=-(int)GetEventBroadcastSize();
		m_MoveGrids[LD_DY].GridCount++;

		m_MoveGrids[LD_IX_DY].pGrids[m_MoveGrids[LD_IX_DY].GridCount].X=i;
		m_MoveGrids[LD_IX_DY].pGrids[m_MoveGrids[LD_IX_DY].GridCount].Y=-(int)GetEventBroadcastSize();
		m_MoveGrids[LD_IX_DY].GridCount++;

		m_MoveGrids[LD_DX_DY].pGrids[m_MoveGrids[LD_DX_DY].GridCount].X=i;
		m_MoveGrids[LD_DX_DY].pGrids[m_MoveGrids[LD_DX_DY].GridCount].Y=-(int)GetEventBroadcastSize();
		m_MoveGrids[LD_DX_DY].GridCount++;

		m_MoveGrids[LD_IX].pGrids[m_MoveGrids[LD_IX].GridCount].X=GetEventBroadcastSize();
		m_MoveGrids[LD_IX].pGrids[m_MoveGrids[LD_IX].GridCount].Y=i;
		m_MoveGrids[LD_IX].GridCount++;

		m_MoveGrids[LD_DX].pGrids[m_MoveGrids[LD_DX].GridCount].X=-(int)GetEventBroadcastSize();
		m_MoveGrids[LD_DX].pGrids[m_MoveGrids[LD_DX].GridCount].Y=i;
		m_MoveGrids[LD_DX].GridCount++;
	}
	for(int i=-(int)GetEventBroadcastSize();i<=(int)GetEventBroadcastSize();i++)
	{
		if(i!=-(int)GetEventBroadcastSize())
		{
			m_MoveGrids[LD_IX_DY].pGrids[m_MoveGrids[LD_IX_DY].GridCount].X=GetEventBroadcastSize();
			m_MoveGrids[LD_IX_DY].pGrids[m_MoveGrids[LD_IX_DY].GridCount].Y=i;
			m_MoveGrids[LD_IX_DY].GridCount++;

			m_MoveGrids[LD_DX_DY].pGrids[m_MoveGrids[LD_DX_DY].GridCount].X=-(int)GetEventBroadcastSize();
			m_MoveGrids[LD_DX_DY].pGrids[m_MoveGrids[LD_DX_DY].GridCount].Y=i;
			m_MoveGrids[LD_DX_DY].GridCount++;
		}

		if(i!=(int)GetEventBroadcastSize())
		{
			m_MoveGrids[LD_IX_IY].pGrids[m_MoveGrids[LD_IX_IY].GridCount].X=GetEventBroadcastSize();
			m_MoveGrids[LD_IX_IY].pGrids[m_MoveGrids[LD_IX_IY].GridCount].Y=i;
			m_MoveGrids[LD_IX_IY].GridCount++;

			m_MoveGrids[LD_DX_IY].pGrids[m_MoveGrids[LD_DX_IY].GridCount].X=-(int)GetEventBroadcastSize();
			m_MoveGrids[LD_DX_IY].pGrids[m_MoveGrids[LD_DX_IY].GridCount].Y=i;
			m_MoveGrids[LD_DX_IY].GridCount++;
		}
		for(int j=-(int)GetEventBroadcastSize();j<=(int)GetEventBroadcastSize();j++)
		{
			m_FullNotifyGrids.pGrids[(i+GetEventBroadcastSize())*(GetEventBroadcastSize()*2+1)+j+GetEventBroadcastSize()].X=j;
			m_FullNotifyGrids.pGrids[(i+GetEventBroadcastSize())*(GetEventBroadcastSize()*2+1)+j+GetEventBroadcastSize()].Y=i;
			m_FullNotifyGrids.GridCount++;
		}

	}
	FUNCTION_END;
}

