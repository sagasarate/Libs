#include "StdAfx.h"

#define MIN_TRACE_DISTANCE		2.0f

CBaseNPC::CBaseNPC(void)
{
	FUNCTION_BEGIN;
	INIT_DOS_MSG_MAP;

	m_pOperator=NULL;
	m_pManager=NULL;
	m_PoolID=0;	
	m_SendedMoveStatus=MS_NONE;
	m_TargetCharID=0;
	m_AIStatus=AI_STATUS_IDLE;
	m_BornPosX=0;
	m_BornPosY=0;

	FUNCTION_END;
}

CBaseNPC::~CBaseNPC(void)
{
	FUNCTION_BEGIN;
	//if(m_pOperator)
	//	m_pOperator->Release();
	FUNCTION_END;
}

BOOL CBaseNPC::Init(CNPCManager * pManager,UINT PoolID,OBJECT_ID MapID,UINT64 CharID,LPCTSTR CharName,float XPos,float YPos,float Height,float Direction)
{
	FUNCTION_BEGIN;
	if(!CCharacter::Init())
		return FALSE;

	m_MapID=MapID;
	m_BaseCharInfo.SetCharID(CharID);	
	m_BaseCharInfo.SetCharName(CharName);
	UINT CurTime=CEasyTimer::GetTime();
	m_MoveStatus.SetMoveStatus(CurTime,MS_STOP,0,XPos,YPos,Height,XPos,YPos,Height,
		Direction,RS_STOP,0);


	m_pManager=pManager;
	m_PoolID=PoolID;
	m_SendedMoveStatus=MS_NONE;
	m_TargetCharID=0;
	m_AIStatus=AI_STATUS_IDLE;
	m_BornPosX=XPos;
	m_BornPosY=YPos;
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

BOOL CBaseNPC::Initialize(IDistributedObjectOperator * pOperator)
{
	FUNCTION_BEGIN;

	m_AreaEventReceiverID=pOperator->GetObjectID();

	if(!CCharacter::Initialize(pOperator))
		return FALSE;
	
	if(!AddCharToMap(m_MapID,m_CharQueryCenterID,0,0,m_BaseCharInfo.GetCharID(),m_BaseCharInfo.GetCharName(),
		CT_NPC,m_MoveStatus.GetPosX(),m_MoveStatus.GetPosY(),m_MoveStatus.GetHeight(),m_MoveStatus.GetDirection()))
		return FALSE;
	
	m_StatusUpdateTimer.SaveTime();

	m_AIStatus=AI_STATUS_IDLE;
	m_AIStatusTimer.SetTimeOut(5000);

	srand(CEasyTimer::GetTime());
		
	return TRUE;

	FUNCTION_END;
	return FALSE;
}
void CBaseNPC::Destory()
{
	FUNCTION_BEGIN;
	
	CCharacter::Destory();

	if(m_pManager)
		m_pManager->DeleteNPC(m_PoolID);
	FUNCTION_END;
}

void CBaseNPC::ForceRelease()
{
	FUNCTION_BEGIN;
	if(m_pOperator)
		m_pOperator->Release();
	FUNCTION_END;
}


int CBaseNPC::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	if(m_StatusUpdateTimer.IsTimeOut(NPC_STATUS_UPDATE_TIME))
	{
		m_StatusUpdateTimer.SaveTime();
		UINT CurTime=m_StatusUpdateTimer.GetSavedTime();		
		m_MoveStatus.Update(CurTime);
		//Log("当前坐标(%g,%g)",
		//	m_MoveStatus.GetPosX(),m_MoveStatus.GetPosY());
		if(m_TargetCharID)
			m_TargetMoveStatus.Update(CurTime);
		switch(m_AIStatus)
		{
		case AI_STATUS_IDLE:
			if(m_AIStatusTimer.IsTimeOut())
			{
				SelectMoveTarget(CurTime);
				//DoJump();
			}
			break;
		case AI_STATUS_MOVE:
			if(m_AIStatusTimer.IsTimeOut())
			{
				m_MoveStatus.StopMove(CurTime);
				SendMoveStatus(true,true);
				//LogDebug("当前位置(%g,%g)已到达目标(%g,%g)移动结束",
				//	m_NPCStatusInfo.XPos,
				//	m_NPCStatusInfo.YPos,
				//	m_NPCStatusInfo.TargetXPos,
				//	m_NPCStatusInfo.TargetYPos);
				m_AIStatus=AI_STATUS_IDLE;
				m_AIStatusTimer.SetTimeOut(5000);
			}
			break;
		case AI_STATUS_JUMP:
			if(m_AIStatusTimer.IsTimeOut())
			{				
				m_AIStatus=AI_STATUS_IDLE;
				m_AIStatusTimer.SetTimeOut(5000);
			}
			break;
		}
		//if(m_TargetInfo.CharID)
		//{
		//	CaculateStatus(CurTime,m_TargetInfo);
		//	TraceTarget(CurTime);
		//}
		
		return 1;
	}
	FUNCTION_END;
	return 0;
}

int CBaseNPC::RegisterMapObjectAck(UINT64 ObjectID ,int Result )
{
	return COMMON_RESULT_SUCCEED;
}
int CBaseNPC::UnregisterMapObjectAck(UINT64 ObjectID ,int Result )
{
	return COMMON_RESULT_SUCCEED;
}
int CBaseNPC::UpdateMoveStatusAck(UINT64 ObjectID ,int Result )
{
	return COMMON_RESULT_SUCCEED;
}
int CBaseNPC::NewMapObject(UINT64 ObjectID ,CSmartStruct& MoveStatus )
{
	
	//if(ObjectID!=m_CharID&&ObjectID<1000&&m_TargetCharID==0)
	//{
	//	m_TargetCharID=ObjectID;
	//	UINT CurTime=CEasyTimer::GetTime();
	//	m_TargetMoveStatus.SetMoveStatus(CurTime,MoveStatus);
	//	
	//	LogDebug("角色(%llu)被选定为目标",
	//		m_TargetCharID);
	//}
	return COMMON_RESULT_SUCCEED;
}
int CBaseNPC::DeleteMapObject(UINT64 ObjectID )
{
	//if(m_TargetCharID==ObjectID)
	//{
	//	LogDebug("目标(%llu)消失",m_TargetCharID);
	//	m_TargetMoveStatus.Clear();
	//	UINT CurTime=CEasyTimer::GetTime();
	//	m_MoveStatus.StopMove(CurTime);
	//	SendMoveStatus(true,true);
	//}
	return COMMON_RESULT_SUCCEED;
}
int CBaseNPC::MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& CharData )
{
	UINT CurTime=CEasyTimer::GetTime();
	if(m_BaseCharInfo.GetCharID()==ObjectID)
		m_MoveStatus.SetMoveStatus(CurTime,CharData);
	if(m_TargetCharID==ObjectID)
		m_TargetMoveStatus.SetMoveStatus(CurTime,CharData);
	return COMMON_RESULT_SUCCEED;
}


float CBaseNPC::IdentityRadian(float Radian)
{
	while(Radian<0)
	{
		Radian+=TWO_PI;
	}
	while(Radian>=TWO_PI)
	{
		Radian-=TWO_PI;
	}
	return Radian;
}


float CBaseNPC::TraceTarget(UINT CurTime)
{
	float DX=m_TargetMoveStatus.GetPosX()-m_MoveStatus.GetPosX();
	float DY=m_TargetMoveStatus.GetPosY()-m_MoveStatus.GetPosY();
	float Dis=sqrt(DX*DX+DY*DY);
	
	if(Dis>MIN_TRACE_DISTANCE)
	{
		float Dir=acos(DX/Dis);
		if(DY<0)
		{
			Dir=TWO_PI-Dir;
		}

		bool NeedQueryMove=false;
		if(m_MoveStatus.GetMoveStatus()<=MS_STOP)
		{
			NeedQueryMove=true;
		}
		else
		{
			float DX=m_TargetMoveStatus.GetPosX()-m_MoveStatus.GetTargetPosX();
			float DY=m_TargetMoveStatus.GetPosY()-m_MoveStatus.GetTargetPosY();
			float Dis=sqrt(DX*DX+DY*DY);
			if(Dis>MIN_TRACE_DISTANCE)
			{
				NeedQueryMove=true;
			}
		}		
		if(NeedQueryMove)
		{
			
			m_MoveStatus.SetMoveStatus(CurTime,MS_FORWARD,RUN_SPEED,
				m_MoveStatus.GetPosX(),
				m_MoveStatus.GetPosY(),
				m_MoveStatus.GetHeight(),
				m_MoveStatus.GetPosX()+(Dis-MIN_TRACE_DISTANCE/2)*cos(Dir),
				m_MoveStatus.GetPosY()+(Dis-MIN_TRACE_DISTANCE/2)*sin(Dir),
				m_MoveStatus.GetHeight(),
				Dir,RS_STOP,ROTATE_SPEED);
			
			SendMoveStatus(true,true);
		}
	}	
	else
	{
		if(m_SendedMoveStatus>MS_STOP)
		{			
			if(m_TargetMoveStatus.GetMovePastTime(CurTime)>=MAX_TARGET_UPDATE_TIME)
			{
				m_TargetMoveStatus.StopMove(CurTime);				
				if(Dis>0)
				{
					float Dir=acos(DX/Dis);
					if(DY<0)
					{
						Dir=TWO_PI-Dir;
					}			
					m_MoveStatus.SetRotateStatus(CurTime,Dir,RS_STOP,ROTATE_SPEED);
				}				
				SendMoveStatus(true,true);
			}			
		}
		else if(Dis>0)
		{
			float Dir=acos(DX/Dis);
			if(DY<0)
			{
				Dir=TWO_PI-Dir;
			}
			if(abs(m_MoveStatus.GetDirection()-Dir)>0.1f)
			{
				m_MoveStatus.SetRotateStatus(CurTime,Dir,RS_STOP,ROTATE_SPEED);
				SendMoveStatus(false,true);			
			}
		}
	}
	return Dis;
}



void CBaseNPC::SendMoveStatus(bool SendMove,bool SendRotation)
{
	char Buffer[MAX_MOVE_STATUS_SIZE];
	CSmartStruct QueryStatus(Buffer,MAX_MOVE_STATUS_SIZE,true);

	m_SendedMoveStatus=m_MoveStatus.GetMoveStatus();

	OBJECT_MEMBER_FLAGS MemberFlags;
	if(SendMove&&SendRotation)
		MemberFlags=MF_MOVE_STATUS;
	else if(SendMove)
		MemberFlags=MF_MOVE_STATUS_ONLY_MOVE;
	else
		MemberFlags=MF_MOVE_STATUS_ONLY_ROTATE;

	m_MoveStatus.MakePacket(QueryStatus,MemberFlags);

	CMapAreaMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_MapID);

	MsgCaller.UpdateMoveStatus(m_BaseCharInfo.GetCharID(),QueryStatus);
	
	//LogDebug("[%llu]更新移动状态[%d](%g,%g)-(%g,%g)(V=%g),[%d](Dir=%g)",
	//	m_CharID,m_NPCStatusInfo.MoveStatus,
	//	m_NPCStatusInfo.XPos,m_NPCStatusInfo.YPos,
	//	m_NPCStatusInfo.TargetXPos,m_NPCStatusInfo.TargetYPos,
	//	m_NPCStatusInfo.MoveSpeed,
	//	m_NPCStatusInfo.RotationStatus,m_NPCStatusInfo.Direction);
}

void CBaseNPC::SelectMoveTarget(UINT CurTime)
{
	float TargetXPos=m_BornPosX+GetRand(-20,20);
	float TargetYPos=m_BornPosY+GetRand(-20,20);
	float DX=TargetXPos-m_MoveStatus.GetPosX();
	float DY=TargetYPos-m_MoveStatus.GetPosY();
	float Dis=sqrt(DX*DX+DY*DY);
	if(Dis>5)
	{
		float Dir=acos(DX/Dis);
		if(DY<0)
		{
			Dir=TWO_PI-Dir;
		}
		m_MoveStatus.SetMoveStatus(CurTime,MS_FORWARD,WALK_SPEED,
			m_MoveStatus.GetPosX(),
			m_MoveStatus.GetPosY(),
			m_MoveStatus.GetHeight(),
			TargetXPos,
			TargetYPos,
			m_MoveStatus.GetHeight(),
			Dir,RS_STOP,ROTATE_SPEED);

		m_AIStatus=AI_STATUS_MOVE;
		UINT MoveTime=m_MoveStatus.GetTotalMoveTime()*1000;
		m_AIStatusTimer.SetTimeOut(MoveTime);
	}
	else
	{
		m_AIStatus=AI_STATUS_IDLE;
		m_AIStatusTimer.SetTimeOut(5000);
	}

	//LogDebug("当前位置(%g,%g)目标(%g,%g)开始移动，移动时间(%u)",
	//	m_NPCStatusInfo.XPos,
	//	m_NPCStatusInfo.YPos,
	//	m_NPCStatusInfo.TargetXPos,
	//	m_NPCStatusInfo.TargetYPos,
	//	MoveTime);

	SendMoveStatus(true,true);
}

void CBaseNPC::DoJump()
{
	m_AIStatus=AI_STATUS_JUMP;
	m_AIStatusTimer.SetTimeOut(5000);

	//CSmartStruct MsgPacket(128);

	//UINT BufferSize;
	//void * pBuffer=MsgPacket.PrepareMember(BufferSize);
	//CSmartStruct Msg(pBuffer,BufferSize,true);	

	//{
	//	if(!Msg.AddMember(SST_GDM_MSG_ID,GD_CHAR_MSG_QUERY_JUMP_RESULT))
	//	{
	//		Log("CNPCController::DoJump:消息缓冲溢出");
	//		return;
	//	}	

	//	BufferSize;
	//	pBuffer=Msg.PrepareMember(BufferSize);
	//	CSmartStruct Data(pBuffer,BufferSize,true);
	//	{
	//		if(!Data.AddMember(SST_GDVI_MO_ID,m_CharID))
	//		{
	//			Log("CNPCController::DoJump:消息缓冲溢出");
	//			return;
	//		}
	//		if(!Data.AddMember(SST_GDVI_RESULT,(BYTE)0))
	//		{
	//			Log("CNPCController::DoJump:消息缓冲溢出");
	//			return;
	//		}	
	//	}
	//	if(!Msg.FinishMember(SST_GDM_MSG_DATA,Data.GetDataLen()))
	//	{
	//		Log("CNPCController::DoJump:消息缓冲溢出");
	//		return;
	//	}
	//}

	//if(!MsgPacket.FinishMember(SST_GDMP_MSG,Msg.GetDataLen()))
	//{
	//	Log("CNPCController::DoJump:消息缓冲溢出");
	//	return;
	//}
	//
	//m_pOperator->SendMessage(m_CharID,GD_MAP_MSG_QUERY_SEND_EVENT_MSG_PACKET,MsgPacket.GetData(),MsgPacket.GetDataLen());
}