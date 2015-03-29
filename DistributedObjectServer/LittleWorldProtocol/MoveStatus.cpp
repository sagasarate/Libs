#include "StdAfx.h"


CMoveStatus::CMoveStatus(void)
{
	Clear();
}

CMoveStatus::~CMoveStatus(void)
{
}

void CMoveStatus::Clear()
{	
	m_ModifyFlag=0;

	m_XPos=0;
	m_YPos=0;
	m_Height=0;
	m_Direction=0;
	m_MoveStatus=MS_NONE;	
	m_MoveSpeed=0;
	m_RotateStatus=RS_NONE;
	m_RotateSpeed=0;
	m_StartXPos=0;
	m_StartYPos=0;
	m_StartHeight=0;
	m_TargetXPos=0;
	m_TargetYPos=0;
	m_TargetHeight=0;
	m_TotalMoveTime=0;
}

void CMoveStatus::SetMoveStatus(UINT CurTime,CSmartStruct& Packet)
{
	ParsePacket(Packet,MF_MOVE_STATUS);
	EnbaleMoveStatusChange(CurTime);
	//float StartXPos=Packet.GetMember(SST_MS_XPOS);
	//float StartYPos=Packet.GetMember(SST_MS_YPOS);
	//float TargetXPos=Packet.GetMember(SST_MS_TARGET_XPOS);
	//float TargetYPos=Packet.GetMember(SST_MS_TARGET_YPOS);
	//float Direction=Packet.GetMember(SST_MS_DIRECTION);	
	//BYTE MoveStatus=Packet.GetMember(SST_MS_MOVE_STATUS);
	//float MoveSpeed=Packet.GetMember(SST_MS_MOVE_SPEED);	
	//BYTE RotateStatus=Packet.GetMember(SST_MS_ROTATE_STATUS);
	//float RotateSpeed=Packet.GetMember(SST_MS_ROTATE_SPEED);

	//SetMoveStatus(CurTime,MoveStatus ,MoveSpeed ,StartXPos ,StartYPos ,TargetXPos ,TargetYPos ,Direction ,RotateStatus ,RotateSpeed);
}

void CMoveStatus::SetMoveStatus(UINT CurTime,BYTE MoveStatus ,float MoveSpeed ,float StartXPos ,float StartYPos ,float StartHeight,float TargetXPos ,float TargetYPos ,float TargetHeight,float Direction ,BYTE RotateStatus ,float RotateSpeed)
{
	if(MoveStatus!=MS_NONE)
	{
		m_XPos=StartXPos;
		m_YPos=StartYPos;
		m_MoveStatus=MoveStatus;
		m_MoveSpeed=MoveSpeed;
		m_TargetXPos=TargetXPos;
		m_TargetYPos=TargetYPos;
		m_TargetHeight=TargetHeight;
		m_StartXPos=StartXPos;
		m_StartYPos=StartYPos;
		m_StartHeight=StartHeight;
		m_MoveStatusTimer.SaveTime(CurTime);
		
		float DX=m_TargetXPos-m_StartXPos;
		float DY=m_TargetYPos-m_StartYPos;
		float DH=m_TargetHeight-m_StartHeight;
		float Dis=sqrt(DX*DX+DY*DY+DH*DH);
		m_TotalMoveTime=Dis/m_MoveSpeed;
		if(m_TotalMoveTime<=0)
		{
			m_MoveStatus=MS_STOP;
		}		
		m_ModifyFlag|=MF_MS_XPOS|MF_MS_YPOS|MF_MS_HEIGHT|MF_MS_MOVE_STATUS|MF_MS_MOVE_SPEED|MF_MS_TARGET_XPOS|MF_MS_TARGET_YPOS|MF_MS_TARGET_HEIGHT;
	}
	if(RotateStatus!=RS_NONE)
	{		
		m_Direction=IdentityRadian(Direction);
		m_RotateStatus=RotateStatus;			
		m_RotateSpeed=RotateSpeed;
		m_RotateStatusTimer.SaveTime(CurTime);
		m_ModifyFlag|=MF_MS_DIRECTION|MF_MS_ROTATE_STATUS|MF_MS_ROTATE_SPEED;
	}
}

void CMoveStatus::SetRotateStatus(UINT CurTime,float Direction ,BYTE RotateStatus ,float RotateSpeed)
{
	if(RotateStatus!=RS_NONE)
	{		
		m_Direction=IdentityRadian(Direction);
		m_RotateStatus=RotateStatus;			
		m_RotateSpeed=RotateSpeed;
		m_RotateStatusTimer.SaveTime(CurTime);
		m_ModifyFlag|=MF_MS_DIRECTION|MF_MS_ROTATE_STATUS|MF_MS_ROTATE_SPEED;
	}
}

void CMoveStatus::StopMove(UINT CurTime)
{
	m_MoveStatus=MS_STOP;
	m_MoveStatusTimer.SaveTime(CurTime);
	m_ModifyFlag|=MF_MS_MOVE_STATUS;
}

void CMoveStatus::StopMove(UINT CurTime,float PosX ,float PosY,float Height)
{
	m_MoveStatus=MS_STOP;
	m_MoveStatusTimer.SaveTime(CurTime);
	m_XPos=PosX;
	m_YPos=PosY;
	m_Height=Height;
	m_ModifyFlag|=MF_MS_XPOS|MF_MS_YPOS|MF_MS_HEIGHT|MF_MS_MOVE_STATUS;
}

void CMoveStatus::EnbaleMoveStatusChange(UINT CurTime)
{
	if((m_ModifyFlag&MF_MS_MOVE_STATUS)&&m_MoveStatus>MS_STOP)
	{
		m_StartXPos=m_XPos;
		m_StartYPos=m_YPos;
		m_StartHeight=m_Height;
		float DX=m_TargetXPos-m_StartXPos;
		float DY=m_TargetYPos-m_StartYPos;
		float DH=m_TargetHeight-m_StartHeight;
		float Dis=sqrt(DX*DX+DY*DY+DH*DH);
		m_TotalMoveTime=Dis/m_MoveSpeed;
		if(m_TotalMoveTime<=0)
		{
			m_MoveStatus=MS_STOP;
		}
		m_MoveStatusTimer.SaveTime(CurTime);
	}
	if((m_ModifyFlag&MF_MS_ROTATE_STATUS)&&m_RotateStatus>RS_STOP)
	{
		m_RotateStatusTimer.SaveTime(CurTime);
	}
}

void CMoveStatus::Update(UINT CurTime)
{
	float RotationStatusSpan=m_RotateStatusTimer.GetPastTime(CurTime)/1000.0f;

	m_RotateStatusTimer.SaveTime(CurTime);

	switch(m_RotateStatus)
	{
	case RS_STOP:
		break;
	case RS_LEFT:
		m_Direction=IdentityRadian(m_Direction+m_RotateSpeed*RotationStatusSpan);
		m_ModifyFlag|=MF_MS_DIRECTION;
		break;
	case RS_RIGHT:
		m_Direction=IdentityRadian(m_Direction-m_RotateSpeed*RotationStatusSpan);
		m_ModifyFlag|=MF_MS_DIRECTION;
		break;
	}	


	if(m_MoveStatus>MS_STOP)
	{
		float MoveStatusTimeSpan=m_MoveStatusTimer.GetPastTime(CurTime)/1000.0f;

		if(MoveStatusTimeSpan<m_TotalMoveTime)
		{
			float S=MoveStatusTimeSpan/m_TotalMoveTime;
			m_XPos=m_StartXPos+(m_TargetXPos-m_StartXPos)*S;
			m_YPos=m_StartYPos+(m_TargetYPos-m_StartYPos)*S;
			m_Height=m_StartHeight+(m_TargetHeight-m_StartHeight)*S;
			m_ModifyFlag|=MF_MS_XPOS|MF_MS_YPOS|MF_MS_HEIGHT;
		}
		else
		{
			m_XPos=m_TargetXPos;
			m_YPos=m_TargetYPos;
			m_MoveStatus=MS_STOP;
			m_ModifyFlag|=MF_MS_XPOS|MF_MS_YPOS|MF_MS_MOVE_STATUS;
		}
	}
}

void CMoveStatus::GetModifyFlag(OBJECT_MEMBER_FLAGS& ModifyFlags,int GetType,const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	switch(GetType)
	{
	case GMFT_COPY:
		ModifyFlags.MemberFlags[OMF_MOVE_STATUS]=m_ModifyFlag&MemberFlags.MemberFlags[OMF_MOVE_STATUS];
		break;
	case GMFT_AND:
		ModifyFlags.MemberFlags[OMF_MOVE_STATUS]&=m_ModifyFlag&MemberFlags.MemberFlags[OMF_MOVE_STATUS];
		break;
	case GMFT_OR:
		ModifyFlags.MemberFlags[OMF_MOVE_STATUS]|=m_ModifyFlag&MemberFlags.MemberFlags[OMF_MOVE_STATUS];
		break;
	}
}
bool CMoveStatus::IsModified(const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	UINT64 ModifyFlag=m_ModifyFlag&MemberFlags.MemberFlags[OMF_MOVE_STATUS];

	return ModifyFlag!=0;
}
void CMoveStatus::ClearModifyFlag(const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	m_ModifyFlag&=~MemberFlags.MemberFlags[OMF_MOVE_STATUS];
}

bool CMoveStatus::MakePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags)
{	
	UINT64 Flag=MemberFlags.MemberFlags[OMF_MOVE_STATUS];

	if(Flag&MF_MS_XPOS)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_XPOS,m_XPos));
	}
	if(Flag&MF_MS_YPOS)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_YPOS,m_YPos));		
	}
	if(Flag&MF_MS_HEIGHT)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_HEIGHT,m_Height));		
	}
	if(Flag&MF_MS_MOVE_STATUS)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_MOVE_STATUS,m_MoveStatus));
	}
	if(Flag&MF_MS_MOVE_SPEED)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_MOVE_SPEED,m_MoveSpeed));
	}	
	if((Flag&MF_MS_TARGET_XPOS)&&m_MoveStatus>MS_STOP)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_TARGET_XPOS,m_TargetXPos));
	}
	if((Flag&MF_MS_TARGET_YPOS)&&m_MoveStatus>MS_STOP)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_TARGET_YPOS,m_TargetYPos));
	}
	if((Flag&MF_MS_TARGET_HEIGHT)&&m_MoveStatus>MS_STOP)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_TARGET_HEIGHT,m_TargetHeight));
	}
	if(Flag&MF_MS_ROTATE_STATUS)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_ROTATE_STATUS,m_RotateStatus));
	}
	if(Flag&MF_MS_DIRECTION)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_DIRECTION,m_Direction));
	}
	if(Flag&MF_MS_ROTATE_SPEED)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_ROTATE_SPEED,m_RotateSpeed));
	}
	return true;
}

bool CMoveStatus::MakeUpdatePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	OBJECT_MEMBER_FLAGS ModifyFlags;
	ZeroMemory(&ModifyFlags,sizeof(ModifyFlags));
	GetModifyFlag(ModifyFlags,GMFT_COPY,MemberFlags);
	return MakePacket(Packet,MemberFlags);
}

bool CMoveStatus::ParsePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	UINT64 Flag=MemberFlags.MemberFlags[OMF_MOVE_STATUS];
	UINT64 ModifyFlag=0;
	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{
		case SST_MS_XPOS:
			m_XPos=Value;
			ModifyFlag|=MF_MS_XPOS;
			break;
		case SST_MS_YPOS:
			m_YPos=Value;
			ModifyFlag|=MF_MS_YPOS;
			break;
		case SST_MS_HEIGHT:
			m_Height=Value;
			ModifyFlag|=MF_MS_HEIGHT;
			break;
		case SST_MS_DIRECTION:
			m_Direction=IdentityRadian(Value);
			ModifyFlag|=MF_MS_DIRECTION;
			break;
		case SST_MS_MOVE_STATUS:
			m_MoveStatus=Value;
			ModifyFlag|=MF_MS_MOVE_STATUS;
			break;
		case SST_MS_MOVE_SPEED:
			m_MoveSpeed=Value;
			ModifyFlag|=MF_MS_MOVE_SPEED;
			break;
		case SST_MS_ROTATE_STATUS:
			m_RotateStatus=Value;
			ModifyFlag|=MF_MS_ROTATE_STATUS;
			break;
		case SST_MS_ROTATE_SPEED:
			m_RotateSpeed=Value;
			ModifyFlag|=MF_MS_ROTATE_SPEED;
			break;
		case SST_MS_TARGET_XPOS:
			m_TargetXPos=Value;
			ModifyFlag|=MF_MS_TARGET_XPOS;
			break;
		case SST_MS_TARGET_YPOS:
			m_TargetYPos=Value;
			ModifyFlag|=MF_MS_TARGET_YPOS;
			break;
		case SST_MS_TARGET_HEIGHT:
			m_TargetHeight=Value;
			ModifyFlag|=MF_MS_TARGET_HEIGHT;
			break;
		}
	}
	m_ModifyFlag|=ModifyFlag;
	return true;
}


float CMoveStatus::IdentityRadian(float Radian)
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