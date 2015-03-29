#include "StdAfx.h"


CMapObject::CMapObject(void)
{
	m_pMapArea=NULL;
	m_Flag=0;
	m_ObjectID=0;
	m_ControllerID=0;
	m_AreaEventReceiverID=0;
	
	m_pMapEventNode=NULL;
	m_pMapCollisionNode=NULL;

	m_MoveUpdateTimer=0;
	m_MsgUpdateTimer=0;

	m_MoveGridBuffer.Resize(MAX_MOVE_GRID);
}

CMapObject::~CMapObject(void)
{
}


bool CMapObject::Init(CMapArea * pMapArea,UINT64 ObjectID,OBJECT_ID ControllerID,OBJECT_ID AreaEventReceiverID,CSmartStruct& MoveStatusPacket,UINT Flag)
{
	m_pMapArea=pMapArea;
	m_ObjectID=ObjectID;
	m_ControllerID=ControllerID;
	m_AreaEventReceiverID=AreaEventReceiverID;
	m_Flag=Flag;

	if(CheckFlag(MOF_USE_MSG_CACHE))
	{
		if(!m_MsgBuffer.Create(MAX_MSG_SIZE+256))
		{
			Log("CMapObject::CMapObject:创建消息缓冲失败");
			return false;
		}
	}
	
	UINT CurTime=CEasyTimer::GetTime();
	m_MoveStatus.SetMoveStatus(CurTime,MoveStatusPacket);

	if(!DoEnterMap())
		return false;	

	if(ObjectID<NPC_ID_START)
	{
		LogDebug("玩家对象(%llu)创建",ObjectID);
	}

	m_MoveUpdateTimer=CEasyTimer::GetTime();
	m_MsgUpdateTimer=m_MoveUpdateTimer;
	return true;;	
}

void CMapObject::Destory()
{
	DoLeaveMap();	
	m_ObjectID=0;
	m_ControllerID=0;
	m_AreaEventReceiverID=0;	
	m_MoveStatus.Clear();

	CNameObject::Destory();
}


bool CMapObject::UpdateMoveStatus(CSmartStruct& MoveStatusPacket)
{	
	if(m_pMapEventNode==NULL||m_pMapCollisionNode==NULL)
		return false;

	BYTE MoveStatus=MoveStatusPacket.GetMember(SST_MS_MOVE_STATUS);
	BYTE RotateStatus=MoveStatusPacket.GetMember(SST_MS_ROTATE_STATUS);
	UINT CurTime=CEasyTimer::GetTime();
	if(MoveStatus!=MS_NONE)
	{
		
		
		float StartXPos=MoveStatusPacket.GetMember(SST_MS_XPOS);
		float StartYPos=MoveStatusPacket.GetMember(SST_MS_YPOS);

		if(MoveStatus!=MS_STOP)
		{
			if(!MoveTo(StartXPos,StartYPos))
			{				
				StopMove();						
				return false;
			}
			
			m_MoveStatus.SetMoveStatus(CurTime,MoveStatusPacket);

			//LogDebug("[%llu]设置坐标到(%g,%g)-(%g,%g)状态=%d",
			//	m_ObjectID,
			//	m_MoveStatus.GetPosX(),m_MoveStatus.GetPosY(),
			//	m_MoveStatus.GetTargetPosX(),m_MoveStatus.GetTargetPosY(),
			//	m_MoveStatus.GetMoveStatus());
			
			if(m_MoveStatus.GetMoveStatus()==MS_STOP)
			{
				Log("CMapObject::UpdateMoveStatus:请求的移动实际未移动，停止移动");
				StopMove();
				return false;
			}						
		}
		else
		{			
			if(!MoveTo(StartXPos,StartYPos))
			{				
				StopMove();						
				return false;
			}
			
			m_MoveStatus.SetMoveStatus(CurTime,MoveStatusPacket);
		}
		
		
	}
	if(RotateStatus!=RS_NONE)
	{
		//LogDebug("设置方向到(%g)状态=%d",
		//	Direction,RotationStatus);
		m_MoveStatus.SetMoveStatus(CurTime,MoveStatusPacket);
	}
	//Log("玩家(0x%llX)(X=%g,Y=%g,D=%g)(M=%d,R=%d)",
	//	m_ObjectID,
	//	m_XPos,m_YPos,m_Direction,m_MoveStatus,m_RotationStatus);

	//if(m_ObjectID<NPC_ID_START)
	//{
	//	LogDebug("玩家(%llu)改变移动状态(%d,(%g,%g)-(%g,%g))(%d)",
	//		m_ObjectID,
	//		m_MoveStatus.GetMoveStatus(),
	//		m_MoveStatus.GetPosX(),
	//		m_MoveStatus.GetPosY(),
	//		m_MoveStatus.GetTargetPosX(),
	//		m_MoveStatus.GetTargetPosY(),
	//		m_MoveStatus.GetRotateStatus());
	//}

	m_pMapEventNode->DoMoveStatusNotify(this,false);

	return true;
}



int CMapObject::Update(UINT CurTime)
{
	FUNCTION_BEGIN;	
	int ProcessCount=0;

	
	
	if(GetTimeToTime(m_MoveUpdateTimer,CurTime)>MAP_OBJECT_MOVE_UPDATE_TIME)
	{
		m_MoveUpdateTimer=CurTime;
		float OldPosX=m_MoveStatus.GetPosX();
		float OldPosY=m_MoveStatus.GetPosY();
		float OldHeight=m_MoveStatus.GetHeight();
		BYTE OldMoveStatus=m_MoveStatus.GetMoveStatus();

		m_MoveStatus.Update(CurTime);

		if(OldMoveStatus>MS_STOP)
		{
			if(!MoveTo(m_MoveStatus.GetPosX(),m_MoveStatus.GetPosY()))
			{
				StopMove(OldPosX,OldPosY,OldHeight);
			}
			else
			{
				//Log("移动到(%g,%f)",
				//	m_MoveStatus.GetPosX(),m_MoveStatus.GetPosY());
			}
		}
		ProcessCount++;
	}
	if(CheckFlag(MOF_USE_MSG_CACHE))
	{
		if(GetTimeToTime(m_MsgUpdateTimer,CurTime)>MAP_OBJECT_MSG_UPDATE_TIME)
		{
			m_MsgUpdateTimer=CurTime;
			if(m_MsgBuffer.GetLength())
			{
				ClearMsgBuffer();
				ProcessCount++;
			}
		}
	}
	return ProcessCount;
	FUNCTION_END;
	return 0;

}

bool CMapObject::PushMsg(MSG_ID_TYPE MsgID,CSmartStruct& DataPacket,int Level)
{
	if(Level==0||(!CheckFlag(MOF_USE_MSG_CACHE)))
	{
		return m_pMapArea->SendMsg(m_AreaEventReceiverID,MsgID,DataPacket.GetData(),DataPacket.GetDataLen());
	}
	else 
	{
		UINT NeedLen=CaculateMsgPacketSize(DataPacket.GetLength());
	
		if(m_MsgBuffer.GetFreeLen()<NeedLen)
		{
			ClearMsgBuffer();
		}	
		if(!PackMsgToMsgPacket(m_MsgBuffer,MsgID,DataPacket))
		{
			Log("CMapObject::PushMsg:消息缓冲溢出");
			return false;
		}
		return true;
	}
}

bool CMapObject::PushMsgPacket(CSmartStruct& MsgPacket,int Level)
{	
	if(CheckFlag(MOF_USE_MSG_CACHE))
	{
		if(m_MsgBuffer.GetFreeLen()<MsgPacket.GetDataLen())
		{
			ClearMsgBuffer();
		}
		if(m_MsgBuffer.Append(MsgPacket))
		{
			return true;
		}
		else
		{
			Log("CMapObject::PushMsgPacket:消息缓冲溢出");
		}
	}
	else
	{
		Log("在非消息缓冲模式不能使用PushMsgPacket");
	}
	return false;
}


bool CMapObject::PacketObjectInfo(CSmartStruct& Packet)
{
	return m_MoveStatus.MakePacket(Packet,MF_MOVE_STATUS);
}

bool CMapObject::PacketMoveStatus(CSmartStruct& Packet)
{	
	return m_MoveStatus.MakePacket(Packet,MF_MOVE_STATUS);
}


bool CMapObject::HideOnMap(BYTE HideType)
{
	switch(HideType)
	{
	case MOHT_SHOW:
		m_Flag&=~MOF_HIDE_MODE;
		if(m_pMapEventNode==NULL)
		{
			return DoEnterMap();
		}
		break;
	case MOHT_HIDE:
		m_Flag|=MOF_HIDE_MODE;
		break;
	case MOHT_LEAVE_MAP:
		m_Flag&=~MOF_HIDE_MODE;
		return DoLeaveMap();
		break;	
	}	
	return true;
}

bool CMapObject::Teleport(float PosX,float PosY,float Height,float Direction)
{
	UINT GridX=m_pMapArea->PosXToCollisionGridX(PosX);
	UINT GridY=m_pMapArea->PosYToCollisionGridY(PosY);

	m_pMapCollisionNode=m_pMapArea->GetMapCollisionNode(GridX,GridY);
	if(m_pMapCollisionNode)
	{
		if(!m_pMapCollisionNode->CanEnter())
			return false;
	}
	else
	{
		return false;
	}

	if(DoLeaveMap())
	{
		UINT CurTime=CEasyTimer::GetTime();
		m_MoveStatus.SetMoveStatus(CurTime,MS_STOP,0,PosX,PosY,Height,PosX,PosY,Height,Direction,RS_STOP,0);
		return DoEnterMap();
	}
	return false;
}


bool CMapObject::MoveTo(float PosX,float PosY)
{

	if(m_pMapEventNode==NULL||m_pMapCollisionNode==NULL)
		return false;

	if(!m_pMapArea->IsPosInMap(PosX,PosY)||m_pMapEventNode==NULL||m_pMapCollisionNode==NULL)
		return false;

	//LogDebug("移动(%0.0f,%0.0f)",
	//	PosX,PosY);
	

	//移动碰撞格

	UINT GridX=m_pMapCollisionNode->GetGridX();
	UINT GridY=m_pMapCollisionNode->GetGridY();
	UINT NewGridX=m_pMapArea->PosXToCollisionGridX(PosX);
	UINT NewGridY=m_pMapArea->PosYToCollisionGridY(PosY);	
	UINT GridCount=m_MoveGridBuffer.GetCount();

	if(GridX!=NewGridX||GridY!=NewGridY)
	{
		//LogDebug("移动碰撞格(%d,%d)到(%d,%d)",
		//	GridX,GridY,NewGridX,NewGridY);

		if(GetGridPath(GridX,GridY,NewGridX,NewGridY,m_MoveGridBuffer.GetBuffer(),GridCount))
		{		
			for(UINT i=0;i<GridCount;i++)
			{
				int DGridX;
				int DGridY;
				if(i==0)
				{
					DGridX=m_MoveGridBuffer[i].X-GridX;
					DGridY=m_MoveGridBuffer[i].Y-GridY;
				}
				else
				{
					DGridX=m_MoveGridBuffer[i].X-m_MoveGridBuffer[i-1].X;
					DGridY=m_MoveGridBuffer[i].Y-m_MoveGridBuffer[i-1].Y;
				}
				int Dir=GridOffsetToDir(DGridX,DGridY);
				if(Dir!=LD_MAX)
				{
					CMapCollisionNode * pNewNode=m_pMapCollisionNode->MoveTo(this,Dir,0);
					if(pNewNode)					
					{					
						m_pMapCollisionNode=pNewNode;
						GridX=m_pMapCollisionNode->GetGridX();
						GridY=m_pMapCollisionNode->GetGridY();				
					}
					else
					{
						return false;
					}

				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			Log("移动距离过大(%g-%g)-(%g-%g),当前碰撞格(%u,%u)",
				m_MoveStatus.GetPosX(),m_MoveStatus.GetPosY(),
				PosX,PosY,GridX,GridY);
			return false;
		}		
	}

	//移动事件格
	GridX=m_pMapEventNode->GetGridX();
	GridY=m_pMapEventNode->GetGridY();

	if(m_pMapArea->CanMoveEventGrid(PosX,PosY,GridX,GridY))
	{
		NewGridX=m_pMapArea->PosXToEventGridX(PosX);
		NewGridY=m_pMapArea->PosYToEventGridY(PosY);	

		//LogDebug("移动事件格(%d,%d)到(%d,%d)",
		//	GridX,GridY,NewGridX,NewGridY);
		

		m_MoveGridBuffer.Resize(MAX_MOVE_GRID);
		GridCount=m_MoveGridBuffer.GetCount();


		if(GetGridPath(GridX,GridY,NewGridX,NewGridY,m_MoveGridBuffer.GetBuffer(),GridCount))
		{		
			for(UINT i=0;i<GridCount;i++)
			{
				int DGridX;
				int DGridY;
				if(i==0)
				{
					DGridX=m_MoveGridBuffer[i].X-GridX;
					DGridY=m_MoveGridBuffer[i].Y-GridY;
				}
				else
				{
					DGridX=m_MoveGridBuffer[i].X-m_MoveGridBuffer[i-1].X;
					DGridY=m_MoveGridBuffer[i].Y-m_MoveGridBuffer[i-1].Y;
				}
				int Dir=GridOffsetToDir(DGridX,DGridY);
				if(Dir!=LD_MAX)
				{
					CMapEventNode * pNewNode=m_pMapEventNode->MoveTo(this,Dir,0);
					if(pNewNode)					
					{					
						m_pMapEventNode=pNewNode;
						GridX=m_pMapEventNode->GetGridX();
						GridY=m_pMapEventNode->GetGridY();								
					}
					else
					{
						break;
					}

				}
				else
				{
					break;
				}
			}
		}
	}
	
	
	return true;
	
}

void CMapObject::StopMove()
{
	if(m_pMapEventNode)
	{
		UINT CurTime=CEasyTimer::GetTime();
		m_MoveStatus.StopMove(CurTime);	
		m_pMapEventNode->DoMoveStatusNotify(this,true);
	}
}

void CMapObject::StopMove(float PosX,float PosY,float Height)
{
	if(m_pMapEventNode)
	{
		UINT CurTime=CEasyTimer::GetTime();
		m_MoveStatus.StopMove(CurTime,PosX,PosY,Height);	
		m_pMapEventNode->DoMoveStatusNotify(this,true);
	}
}

bool CMapObject::GetGridPath(UINT SrcGridX,UINT SrcGridY,UINT DestGridX,UINT DestGridY,MAP_GRID_POS * pGridBuffer,UINT& GridCount)
{
	int DGridX=DestGridX-SrcGridX;
	int DGridY=DestGridY-SrcGridY;
	int AbsDGridX=abs(DGridX);
	int AbsDGridY=abs(DGridY);
	UINT BufferSize=GridCount;
	GridCount=0;
	if(AbsDGridX>0||AbsDGridY>0)
	{
		if(AbsDGridX>AbsDGridY)
		{
			int Inc=DGridX/AbsDGridX;
			int i=Inc;
			for(;i!=DGridX;i+=Inc)
			{
				if(GridCount>=BufferSize)
					return false;
				pGridBuffer[GridCount].X=SrcGridX+i;				
				pGridBuffer[GridCount].Y=SrcGridY+DGridY*i/DGridX;
				GridCount++;				
			}
			if(GridCount>=BufferSize)
				return false;
			pGridBuffer[GridCount].X=SrcGridX+i;				
			pGridBuffer[GridCount].Y=SrcGridY+DGridY*i/DGridX;
			GridCount++;
		}
		else
		{
			int Inc=DGridY/AbsDGridY;
			int i=Inc;
			for(;i!=DGridY;i+=Inc)
			{
				if(GridCount>=BufferSize)
					return false;
				pGridBuffer[GridCount].Y=SrcGridY+i;				
				pGridBuffer[GridCount].X=SrcGridX+DGridX*i/DGridY;
				GridCount++;
			}
			if(GridCount>=BufferSize)
				return false;
			pGridBuffer[GridCount].Y=SrcGridY+i;				
			pGridBuffer[GridCount].X=SrcGridX+DGridX*i/DGridY;
			GridCount++;
		}
	}
	return true;
}

void CMapObject::ClearMsgBuffer()
{
	CMapAreaAckMsgCaller Caller(m_pMapArea->GetOperator(),
		m_pMapArea->GetOperator()->GetObjectID(),m_AreaEventReceiverID);
	Caller.EventMsgPacket(m_MsgBuffer);
	m_MsgBuffer.Clear();
}

bool CMapObject::DoEnterMap()
{
	UINT GridX=m_pMapArea->PosXToCollisionGridX(m_MoveStatus.GetPosX());
	UINT GridY=m_pMapArea->PosYToCollisionGridY(m_MoveStatus.GetPosY());

	m_pMapCollisionNode=m_pMapArea->GetMapCollisionNode(GridX,GridY);
	if(m_pMapCollisionNode)
	{
		if(!m_pMapCollisionNode->Enter(this))
			return false;
	}
	else
	{
		Log("CMapObject::DoEnterMap:无法获取碰撞格");
		return false;
	}

	GridX=m_pMapArea->PosXToEventGridX(m_MoveStatus.GetPosX());
	GridY=m_pMapArea->PosYToEventGridY(m_MoveStatus.GetPosY());

	m_pMapEventNode=m_pMapArea->GetMapEventNode(GridX,GridY);
	if(m_pMapEventNode)
	{
		if(!m_pMapEventNode->Enter(this,LD_MAX))
			return false;
	}	
	else
	{
		Log("CMapObject::DoEnterMap:无法获取事件格");
		return false;
	}
	return true;
}
bool CMapObject::DoLeaveMap()
{
	if(m_pMapCollisionNode)
		m_pMapCollisionNode->Leave(this);
	m_pMapCollisionNode=NULL;
	if(m_pMapEventNode)
		m_pMapEventNode->Leave(this,LD_MAX);
	m_pMapEventNode=NULL;
	return true;
}