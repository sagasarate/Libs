#include "StdAfx.h"


CEasyArray<OBJECT_ID>			CMapEventNode::m_BroadcastIDBuffer;
CEasyArray<CMapObject *>		CMapEventNode::m_BroadcastPtrBuffer;

CMapEventNode::CMapEventNode(void)
{
	m_pMapArea=NULL;	
	m_GridType=LT_MOVE;
	m_GridX=0;
	m_GridY=0;
}

CMapEventNode::~CMapEventNode(void)
{
}

bool CMapEventNode::Create(CMapArea * pMapArea,const CMainConfig::POOL_CONFIG& ObjectPoolConfig,UINT GridX,UINT GridY)
{
	m_pMapArea=pMapArea;
	if(!m_MapObjectList.Create(ObjectPoolConfig.StartSize,ObjectPoolConfig.GrowSize,ObjectPoolConfig.GrowLimit))
	{
		return false;
	}	
	m_GridX=GridX;
	m_GridY=GridY;	

	return true;
}



bool CMapEventNode::Enter(CMapObject * pMapObject,UINT Dir)
{
	if(pMapObject==NULL)
		return false;
	if(m_MapObjectList.Find(pMapObject->GetObjectID()))
	{
		Log("CMapEventNode::Enter:对象%llu已经在事件格中",pMapObject->GetObjectID());
		return false;
	}
	if(m_MapObjectList.Insert(pMapObject->GetObjectID(),pMapObject))
	{
		//if(pMapObject->GetObjectID()<NPC_ID_START)
		//{
		//	LogDebug("玩家(%llu)进入事件格(%u,%u)",
		//		pMapObject->GetObjectID(),
		//		m_GridX,m_GridY);
		//}
		DoObjectEnterNotify(pMapObject,Dir);
		return true;
	}
	else
	{
		Log("CMapEventNode::Enter:事件格对象列表已满",pMapObject->GetObjectID());
	}
	return false;
}

bool CMapEventNode::Leave(CMapObject * pMapObject,UINT Dir)
{
	if(pMapObject==NULL)
		return false;
	if(m_MapObjectList.Delete(pMapObject->GetObjectID()))
	{		
		//if(pMapObject->GetObjectID()<NPC_ID_START)
		//{
		//	LogDebug("玩家(%llu)离开事件格(%u,%u)",
		//		pMapObject->GetObjectID(),
		//		m_GridX,m_GridY);
		//}
		DoObjectLeaveNotify(pMapObject,Dir);
		return true;			
	}
	else
	{
		Log("CMapEventNode::Leave:对象%llu未在事件格中",pMapObject->GetObjectID());
	}
	return false;
}

CMapEventNode * CMapEventNode::MoveTo(CMapObject * pMapObject,UINT Dir,UINT MoveType)
{
	if(!IsObjectExist(pMapObject))
		return NULL;
	if(Dir<LD_MAX)
	{
		int OffsetX,OffsetY;
		DirToGridOffset(Dir,OffsetX,OffsetY);
		CMapEventNode * pNewNode=m_pMapArea->GetMapEventNode((int)m_GridX+OffsetX,(int)m_GridY+OffsetY);
		if(pNewNode)		
		{
			UINT OppositeDir=GetOppositeDir(Dir);			
			if(pNewNode->Enter(pMapObject,OppositeDir))
			{
				if(Leave(pMapObject,Dir))
				{
					return pNewNode;
				}
			}
		}
	}
	return NULL;
}

int CMapEventNode::Update(int ProcessLimit)
{
	FUNCTION_BEGIN;	
	FUNCTION_END;
	return 0;
}

//int CMapEventNode::PushMsgToCache(MSG_ID_TYPE MsgID,CSmartStruct& DataPacket,int Level)
//{	
//	void * Pos=GetFirstMapObjectPos();
//	int BroadcastCount=0;
//	while(Pos)
//	{
//		CMapObject * pObject=GetNextMapObject(Pos);
//		if(pObject)
//		{
//			if(pObject->CheckFlag(MOF_USE_MSG_CACHE))
//			{
//				pObject->PushMsg(MsgID,DataPacket,Level);
//				BroadcastCount++;
//			}
//		}
//	}
//	return BroadcastCount;
//}

//int CMapEventNode::PushMsgPacketToCache(CSmartStruct& MsgPacket,int Level)
//{
//	void * Pos=GetFirstMapObjectPos();
//	int BroadcastCount=0;
//	while(Pos)
//	{
//		CMapObject * pObject=GetNextMapObject(Pos);
//		if(pObject)
//		{
//			if(pObject->CheckFlag(MOF_USE_MSG_CACHE))
//			{
//				pObject->PushMsgPacket(MsgPacket,Level);
//				BroadcastCount++;
//			}
//		}
//	}
//	return BroadcastCount;
//}



void CMapEventNode::DoMoveStatusNotify(CMapObject * pMapObject,bool IsForce)
{
	char Buffer[MAX_MOVE_STATUS_SIZE];
	CSmartStruct MoveStatus(Buffer,MAX_MOVE_STATUS_SIZE,true);

	MOVE_NOTIFY_GRIDS * pNotifyGrids=m_pMapArea->GetFullNotifyGrids();

	if(pMapObject->PacketMoveStatus(MoveStatus))		
	{	
		
		m_BroadcastIDBuffer.Empty();
		m_BroadcastPtrBuffer.Empty();

		UINT MsgCachedObjectCount=0;

		for(UINT i=0;i<pNotifyGrids->GridCount;i++)
		{
			UINT GridX=GetGridX()+pNotifyGrids->pGrids[i].X;
			UINT GridY=GetGridY()+pNotifyGrids->pGrids[i].Y;
			CMapEventNode * pMapNode=m_pMapArea->GetMapEventNode(GridX,GridY);
			if(pMapNode)
			{	
				pMapNode->FetchObjects(m_BroadcastIDBuffer,m_BroadcastPtrBuffer);
			}
		}

		if(m_BroadcastIDBuffer.GetCount())
		{
			//使用多目标群发
			qsort(m_BroadcastIDBuffer.GetBuffer(),m_BroadcastIDBuffer.GetCount(),sizeof(OBJECT_ID),OBJECT_ID::Compare);
			CMapAreaAckMsgCaller MsgCaller(m_pMapArea->GetOperator(),
				m_pMapArea->GetOperator()->GetObjectID(),
				m_BroadcastIDBuffer.GetBuffer(),m_BroadcastIDBuffer.GetCount());

			MsgCaller.MoveStatusUpdate(pMapObject->GetObjectID(),IsForce,MoveStatus);
		}

		if(m_BroadcastPtrBuffer.GetCount())
		{
			//使用消息缓冲机制
			CMapAreaAckMsgCaller MsgCaller(NULL,0,0);
			char MsgBuffer[MAX_MOVE_STATUS_SIZE+32];
			CSmartStruct MsgPacket(MsgBuffer,MAX_MOVE_STATUS_SIZE+32,true);

			if(MsgCaller.PackMsgMoveStatusUpdate(MsgPacket,pMapObject->GetObjectID(),IsForce,MoveStatus))
			{
				
				for(UINT i=0;i<m_BroadcastPtrBuffer.GetCount();i++)
				{
					m_BroadcastPtrBuffer[i]->PushMsg(
						MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_MOVE_STATUS_UPDATE,true),
						MsgPacket,1);					
				}		
			}
			else
			{
				Log("CMapEventNode::DoMoveStatusNotify:打包消息失败");
			}
		}		
	}
	else
	{
		Log("CMapEventNode::DoMoveStatusNotify:打包移动状态失败");
	}
}

//bool CMapEventNode::SendEventMsgPacketRaw(CSmartStruct& Packet)
//{	
//	MOVE_NOTIFY_GRIDS * pNotifyGrids=m_pMapArea->GetFullNotifyGrids();
//
//	for(UINT i=0;i<pNotifyGrids->GridCount;i++)
//	{
//		UINT GridX=m_GridX+pNotifyGrids->pGrids[i].X;
//		UINT GridY=m_GridY+pNotifyGrids->pGrids[i].Y;
//		CMapEventNode * pMapNode=m_pMapArea->GetMapEventNode(GridX,GridY);
//		if(pMapNode)
//		{
//			pMapNode->PushMsgPacket(Packet,1);
//		}
//	}	
//	return true;
//}

bool CMapEventNode::SendEventMsgPacket(CSmartStruct& Packet)
{
	m_BroadcastIDBuffer.Empty();
	m_BroadcastPtrBuffer.Empty();

	UINT MsgCachedObjectCount=0;

	MOVE_NOTIFY_GRIDS * pNotifyGrids=m_pMapArea->GetFullNotifyGrids();

	for(UINT i=0;i<pNotifyGrids->GridCount;i++)
	{
		UINT GridX=GetGridX()+pNotifyGrids->pGrids[i].X;
		UINT GridY=GetGridY()+pNotifyGrids->pGrids[i].Y;
		CMapEventNode * pMapNode=m_pMapArea->GetMapEventNode(GridX,GridY);
		if(pMapNode)
		{	
			pMapNode->FetchObjects(m_BroadcastIDBuffer,m_BroadcastPtrBuffer);
		}
	}

	
	if(m_BroadcastIDBuffer.GetCount())
	{
		qsort(m_BroadcastIDBuffer.GetBuffer(),m_BroadcastIDBuffer.GetCount(),sizeof(OBJECT_ID),OBJECT_ID::Compare);
		CMapAreaAckMsgCaller MsgCaller(m_pMapArea->GetOperator(),
			m_pMapArea->GetOperator()->GetObjectID(),
			m_BroadcastIDBuffer.GetBuffer(),m_BroadcastIDBuffer.GetCount());

		MsgCaller.EventMsgPacket(Packet);
	}
	

	if(m_BroadcastPtrBuffer.GetCount())
	{
		for(UINT i=0;i<m_BroadcastPtrBuffer.GetCount();i++)
		{
			m_BroadcastPtrBuffer[i]->PushMsgPacket(Packet,1);
		}	
	}
	
	return true;
}

void CMapEventNode::FetchObjects(CEasyArray<OBJECT_ID>& NoMsgCachedIDList,CEasyArray<CMapObject *>& MsgCachedObjectList)
{
	void * Pos=m_MapObjectList.GetFirstObjectPos();
	while(Pos)
	{
		UINT64 Key;
		CMapObject ** ppObject=m_MapObjectList.GetNextObject(Pos,Key);
		if(ppObject)
		{
			if((*ppObject)->CheckFlag(MOF_USE_MSG_CACHE))
			{
				MsgCachedObjectList.Add(*ppObject);
			}
			else
			{
				NoMsgCachedIDList.Add((*ppObject)->GetAreaEventReceiverID());
			}
		}
	}
}

LPVOID CMapEventNode::GetFirstMapObjectPos()
{
	return m_MapObjectList.GetFirstObjectPos();
}
CMapObject * CMapEventNode::GetNextMapObject(LPVOID& Pos)
{
	UINT64 Key;
	CMapObject ** ppMapObject=m_MapObjectList.GetNextObject(Pos,Key);
	if(ppMapObject)
		return *ppMapObject;
	return NULL;
}


UINT CMapEventNode::GetMapObjectCount()
{
	return m_MapObjectList.GetObjectCount();
}



bool CMapEventNode::IsObjectExist(CMapObject * pMapObject)
{
	return m_MapObjectList.Find(pMapObject->GetObjectID())!=NULL;	
}

void CMapEventNode::DoObjectEnterNotify(CMapObject * pMapObject,UINT Dir)
{
	
	//获取通知范围
	MOVE_NOTIFY_GRIDS * pNotifyGrids=NULL;
	Dir=GetOppositeDir(Dir);
	if(Dir<LD_MAX)
	{
		pNotifyGrids=m_pMapArea->GetMoveGrids(Dir);
	}
	else
	{
		pNotifyGrids=m_pMapArea->GetFullNotifyGrids();
	}

	NotifyAppearSingleToAround(pMapObject,pNotifyGrids);
	NotifyAppearAroundToSingle(pMapObject,pNotifyGrids);
}

void CMapEventNode::DoObjectLeaveNotify(CMapObject * pMapObject,UINT Dir)
{

	//获取通知范围
	MOVE_NOTIFY_GRIDS * pNotifyGrids=NULL;
	Dir=GetOppositeDir(Dir);
	if(Dir<LD_MAX)
	{
		pNotifyGrids=m_pMapArea->GetMoveGrids(Dir);
	}
	else
	{
		pNotifyGrids=m_pMapArea->GetFullNotifyGrids();
	}

	NotifyDisappearSingleToAround(pMapObject,pNotifyGrids);
	NotifyDisappearAroundToSingle(pMapObject,pNotifyGrids);

	
}


void CMapEventNode::NotifyAppearSingleToAround(CMapObject * pMapObject,MOVE_NOTIFY_GRIDS * pNotifyGrids)
{
	char Buffer[MAX_MOVE_STATUS_SIZE];
	CSmartStruct MoveStatus(Buffer,MAX_MOVE_STATUS_SIZE,true);
	if(pMapObject->PacketObjectInfo(MoveStatus))		
	{
		m_BroadcastIDBuffer.Empty();
		m_BroadcastPtrBuffer.Empty();

		UINT MsgCachedObjectCount=0;

		for(UINT i=0;i<pNotifyGrids->GridCount;i++)
		{
			UINT GridX=GetGridX()+pNotifyGrids->pGrids[i].X;
			UINT GridY=GetGridY()+pNotifyGrids->pGrids[i].Y;
			CMapEventNode * pMapNode=m_pMapArea->GetMapEventNode(GridX,GridY);
			if(pMapNode)
			{	
				pMapNode->FetchObjects(m_BroadcastIDBuffer,m_BroadcastPtrBuffer);
			}
		}

		if(m_BroadcastIDBuffer.GetCount())
		{
			//使用多目标群发
			qsort(m_BroadcastIDBuffer.GetBuffer(),m_BroadcastIDBuffer.GetCount(),sizeof(OBJECT_ID),OBJECT_ID::Compare);

			CMapAreaAckMsgCaller MsgCaller(m_pMapArea->GetOperator(),
				m_pMapArea->GetOperator()->GetObjectID(),
				m_BroadcastIDBuffer.GetBuffer(),m_BroadcastIDBuffer.GetCount());

			MsgCaller.NewMapObject(pMapObject->GetObjectID(),MoveStatus);
		}

		if(m_BroadcastPtrBuffer.GetCount())
		{
			//使用消息缓冲机制
			CMapAreaAckMsgCaller MsgCaller(NULL,0,0);
			char MsgBuffer[MAX_MOVE_STATUS_SIZE+32];
			CSmartStruct MsgPacket(MsgBuffer,MAX_MOVE_STATUS_SIZE+32,true);
			if(MsgCaller.PackMsgNewMapObject(MsgPacket,pMapObject->GetObjectID(),MoveStatus))
			{
				//将对象出现的消息发送到通知范围
				for(UINT i=0;i<m_BroadcastPtrBuffer.GetCount();i++)
				{
					m_BroadcastPtrBuffer[i]->PushMsg(
						MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_NEW_MAP_OBJECT,true),
						MsgPacket,1);
				}
			}
			else
			{
				Log("CMapEventNode::NotifyAppearSingleToAround:打包消息失败");
			}
		}		
	}
	else
	{
		Log("CMapEventNode::DoObjectEnterNotify:打包移动状态失败");
	}
}

void CMapEventNode::NotifyDisappearSingleToAround(CMapObject * pMapObject,MOVE_NOTIFY_GRIDS * pNotifyGrids)
{
	m_BroadcastIDBuffer.Empty();
	m_BroadcastPtrBuffer.Empty();

	UINT MsgCachedObjectCount=0;

	for(UINT i=0;i<pNotifyGrids->GridCount;i++)
	{
		UINT GridX=GetGridX()+pNotifyGrids->pGrids[i].X;
		UINT GridY=GetGridY()+pNotifyGrids->pGrids[i].Y;
		CMapEventNode * pMapNode=m_pMapArea->GetMapEventNode(GridX,GridY);
		if(pMapNode)
		{	
			pMapNode->FetchObjects(m_BroadcastIDBuffer,m_BroadcastPtrBuffer);
		}
	}

	if(m_BroadcastIDBuffer.GetCount())
	{
		//使用多目标群发
		qsort(m_BroadcastIDBuffer.GetBuffer(),m_BroadcastIDBuffer.GetCount(),sizeof(OBJECT_ID),OBJECT_ID::Compare);
		CMapAreaAckMsgCaller MsgCaller(m_pMapArea->GetOperator(),
			m_pMapArea->GetOperator()->GetObjectID(),
			m_BroadcastIDBuffer.GetBuffer(),m_BroadcastIDBuffer.GetCount());

		MsgCaller.DeleteMapObject(pMapObject->GetObjectID());
	}

	if(m_BroadcastPtrBuffer.GetCount())
	{
		//使用消息缓冲机制

		CMapAreaAckMsgCaller MsgCaller(NULL,0,0);
		char MsgBuffer[MAX_MOVE_STATUS_SIZE+32];
		CSmartStruct MsgPacket(MsgBuffer,MAX_MOVE_STATUS_SIZE+32,true);

		if(MsgCaller.PackMsgDeleteMapObject(MsgPacket,pMapObject->GetObjectID()))
		{
			//将对象消失的消息发送到通知范围
			for(UINT i=0;i<m_BroadcastPtrBuffer.GetCount();i++)
			{
				m_BroadcastPtrBuffer[i]->PushMsg(
					MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_DEL_MAP_OBJECT,true),
					MsgPacket,1);
			}
		}
		else
		{
			Log("CMapEventNode::NotifyDisappearSingleToAround:打包消息失败");
		}
	}	
}


void CMapEventNode::NotifyAppearAroundToSingle(CMapObject * pMapObject,MOVE_NOTIFY_GRIDS * pNotifyGrids)
{	 
	FUNCTION_BEGIN;

	char EventMsgBuffer[MAX_MSG_SIZE];
	CSmartStruct EventMsgPacket(EventMsgBuffer,MAX_MSG_SIZE,true);

	
		
	CMapAreaAckMsgCaller MsgCaller(m_pMapArea->GetOperator(),
		m_pMapArea->GetOperator()->GetObjectID(),pMapObject->GetAreaEventReceiverID());

	char MsgBuffer[MAX_MOVE_STATUS_SIZE+32];
	char Buffer[MAX_MOVE_STATUS_SIZE];
	
	for(UINT i=0;i<pNotifyGrids->GridCount;i++)
	{
		UINT GridX=GetGridX()+pNotifyGrids->pGrids[i].X;
		UINT GridY=GetGridY()+pNotifyGrids->pGrids[i].Y;
		CMapEventNode * pMapNode=m_pMapArea->GetMapEventNode(GridX,GridY);
		if(pMapNode)
		{	
			void * Pos=pMapNode->GetFirstMapObjectPos();
			while(Pos)
			{					
				CMapObject * pObject=pMapNode->GetNextMapObject(Pos);
				if(pObject)//&&pObject!=pMapObject)
				{
					CSmartStruct MsgPacket(MsgBuffer,MAX_MOVE_STATUS_SIZE+32,true);
					CSmartStruct MoveStatus(Buffer,MAX_MOVE_STATUS_SIZE,true);
					
					if(pObject->PacketObjectInfo(MoveStatus))
					{
						if(MsgCaller.PackMsgNewMapObject(MsgPacket,pObject->GetObjectID(),MoveStatus))
						{
							if(pMapObject->CheckFlag(MOF_USE_MSG_CACHE))
							{
								//使用消息缓冲机制
								pMapObject->PushMsg(
									MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_NEW_MAP_OBJECT,true),
									MsgPacket,1);
							}
							else
							{
								UINT NeedLen=CaculateMsgPacketSize(MsgPacket.GetLength());								

								if(EventMsgPacket.GetFreeLen()<NeedLen)
								{
									MsgCaller.EventMsgPacket(EventMsgPacket);
									EventMsgPacket.Clear();
								}	
								if(!PackMsgToMsgPacket(EventMsgPacket,
									MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_NEW_MAP_OBJECT,true),
									MsgPacket))
								{
									Log("CMapObject::NotifyAppearAroundToSingle:消息缓冲溢出");
									return;
								}
							}
						}
						else
						{
							Log("CMapEventNode::NotifyAppearAroundToSingle:打包消息失败");
						}
					}
					else
					{
						Log("CMapEventNode::NotifyAppearAroundToSingle:打包移动状态失败");
					}
				}
			}	
		}
	}
	if(!pMapObject->CheckFlag(MOF_USE_MSG_CACHE))
	{
		if(EventMsgPacket.GetLength())
		{
			MsgCaller.EventMsgPacket(EventMsgPacket);
		}
	}
	
	FUNCTION_END;
}

void CMapEventNode::NotifyDisappearAroundToSingle(CMapObject * pMapObject,MOVE_NOTIFY_GRIDS * pNotifyGrids)
{
	FUNCTION_BEGIN;

	char EventMsgBuffer[MAX_MSG_SIZE];
	CSmartStruct EventMsgPacket(EventMsgBuffer,MAX_MSG_SIZE,true);



	CMapAreaAckMsgCaller MsgCaller(m_pMapArea->GetOperator(),
		m_pMapArea->GetOperator()->GetObjectID(),pMapObject->GetAreaEventReceiverID());

	char MsgBuffer[MAX_MOVE_STATUS_SIZE+32];

	for(UINT i=0;i<pNotifyGrids->GridCount;i++)
	{
		UINT GridX=GetGridX()+pNotifyGrids->pGrids[i].X;
		UINT GridY=GetGridY()+pNotifyGrids->pGrids[i].Y;
		CMapEventNode * pMapNode=m_pMapArea->GetMapEventNode(GridX,GridY);
		if(pMapNode)
		{	
			void * Pos=pMapNode->GetFirstMapObjectPos();
			while(Pos)
			{					
				CMapObject * pObject=pMapNode->GetNextMapObject(Pos);
				if(pObject)//&&pObject!=pMapObject)
				{
					CSmartStruct MsgPacket(MsgBuffer,MAX_MOVE_STATUS_SIZE+32,true);

					
					if(MsgCaller.PackMsgDeleteMapObject(MsgPacket,pObject->GetObjectID()))
					{
						if(pMapObject->CheckFlag(MOF_USE_MSG_CACHE))
						{
							//使用消息缓冲机制
							pMapObject->PushMsg(
								MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_DEL_MAP_OBJECT,true),
								MsgPacket,1);
						}
						else
						{
							UINT NeedLen=CaculateMsgPacketSize(MsgPacket.GetLength());							

							if(EventMsgPacket.GetFreeLen()<NeedLen)
							{
								MsgCaller.EventMsgPacket(EventMsgPacket);
								EventMsgPacket.Clear();
							}	
							if(!PackMsgToMsgPacket(EventMsgPacket,
								MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_DEL_MAP_OBJECT,true),
								MsgPacket))
							{
								Log("CMapObject::NotifyDisappearSingleToAround:消息缓冲溢出");
							}
						}
					}
					else
					{
						Log("CMapEventNode::NotifyDisappearSingleToAround:打包消息失败");
					}
					
				}
			}	
		}
	}
	if(!pMapObject->CheckFlag(MOF_USE_MSG_CACHE))
	{
		if(EventMsgPacket.GetLength())
		{
			MsgCaller.EventMsgPacket(EventMsgPacket);
		}
	}	
	FUNCTION_END;
}


