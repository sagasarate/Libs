#include "StdAfx.h"


CBaseCharInfo::CBaseCharInfo(void)
{
	Clear();
}

CBaseCharInfo::~CBaseCharInfo(void)
{
}

void CBaseCharInfo::Clear()
{
	m_ModifyFlag=0;

	m_CharID=0;
	m_CharType=0;
	m_CharName.Clear();
	m_Race=0;
	m_Gender=0;

	m_CreatureDisplayID=0;

	m_CharSkinColor=0;
	m_CharHairColor=0;
	m_CharFaceType=0;
	m_CharHairType=0;
	m_CharBeardType=0;
	m_IsCharBald=0;
	m_MapID=0;

	
}

void CBaseCharInfo::GetModifyFlag(OBJECT_MEMBER_FLAGS& ModifyFlags,int GetType,const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	switch(GetType)
	{
	case GMFT_COPY:
		ModifyFlags.MemberFlags[OMF_BASE_CHAR_INFO]=m_ModifyFlag&MemberFlags.MemberFlags[OMF_BASE_CHAR_INFO];
		break;
	case GMFT_AND:
		ModifyFlags.MemberFlags[OMF_BASE_CHAR_INFO]&=m_ModifyFlag&MemberFlags.MemberFlags[OMF_BASE_CHAR_INFO];
		break;
	case GMFT_OR:
		ModifyFlags.MemberFlags[OMF_BASE_CHAR_INFO]|=m_ModifyFlag&MemberFlags.MemberFlags[OMF_BASE_CHAR_INFO];
		break;
	}
}
bool CBaseCharInfo::IsModified(const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	UINT64 ModifyFlag=m_ModifyFlag&MemberFlags.MemberFlags[OMF_BASE_CHAR_INFO];

	return ModifyFlag!=0;
}
void CBaseCharInfo::ClearModifyFlag(const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	m_ModifyFlag&=~MemberFlags.MemberFlags[OMF_BASE_CHAR_INFO];
}

bool CBaseCharInfo::MakePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	UINT64 Flag=MemberFlags.MemberFlags[OMF_BASE_CHAR_INFO];

	if(Flag&MF_BCI_CHAR_ID)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_ID,m_CharID));
	}
	if(Flag&MF_BCI_CHAR_TYPE)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_TYPE,m_CharType));		
	}
	if(Flag&MF_BCI_CHAR_NAME)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_NAME,(LPCTSTR)m_CharName));
	}
	if(Flag&MF_BCI_RACE)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_RACE,m_Race));
	}
	if(Flag&MF_BCI_GENDER)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_GENDER,m_Gender));
	}
	if(Flag&MF_BCI_CREATURE_DISPLAY_ID)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CREATURE_DISPLAY_ID,m_CreatureDisplayID));
	}
	if(Flag&MF_BCI_CHAR_SKIN_COLOR)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_SKIN_COLOR,m_CharSkinColor));
	}
	if(Flag&MF_BCI_CHAR_HAIR_COLOR)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_HAIR_COLOR,m_CharHairColor));
	}
	if(Flag&MF_BCI_CHAR_FACE_TYPE)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_FACE_TYPE,m_CharFaceType));
	}
	if(Flag&MF_BCI_CHAR_HAIR_TYPE)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_HAIR_TYPE,m_CharHairType));
	}
	if(Flag&MF_BCI_CHAR_BEARD_TYPE)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_BEARD_TYPE,m_CharBeardType));
	}
	if(Flag&MF_BCI_IS_CHAR_BALD)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_IS_CHAR_BALD,m_IsCharBald));
	}
	if(Flag&MF_BCI_MAP_ID)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_MAP_ID,m_IsCharBald));
	}

	return true;
}
bool CBaseCharInfo::MakeUpdatePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	OBJECT_MEMBER_FLAGS ModifyFlags;
	ZeroMemory(&ModifyFlags,sizeof(ModifyFlags));
	GetModifyFlag(ModifyFlags,GMFT_COPY,MemberFlags);
	return MakePacket(Packet,MemberFlags);
}

bool CBaseCharInfo::ParsePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags)
{
	UINT64 Flag=MemberFlags.MemberFlags[OMF_BASE_CHAR_INFO];
	UINT64 ModifyFlag=0;
	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{
		case SST_BCI_CHAR_ID:
			m_CharID=Value;
			ModifyFlag|=MF_BCI_CHAR_ID;
			break;
		case SST_BCI_CHAR_TYPE:
			m_CharType=Value;
			ModifyFlag|=MF_BCI_CHAR_TYPE;
			break;
		case SST_BCI_CHAR_NAME:
			m_CharName=(LPCTSTR)Value;
			ModifyFlag|=MF_BCI_CHAR_NAME;
			break;
		case SST_BCI_RACE:
			m_Race=Value;
			ModifyFlag|=MF_BCI_RACE;
			break;
		case SST_BCI_GENDER:
			m_Gender=Value;
			ModifyFlag|=MF_BCI_GENDER;
			break;
		case SST_BCI_CREATURE_DISPLAY_ID:
			m_CreatureDisplayID=Value;
			ModifyFlag|=MF_BCI_CREATURE_DISPLAY_ID;
			break;
		case SST_BCI_CHAR_SKIN_COLOR:
			m_CharSkinColor=Value;
			ModifyFlag|=MF_BCI_CHAR_SKIN_COLOR;
			break;
		case SST_BCI_CHAR_HAIR_COLOR:
			m_CharHairColor=Value;
			ModifyFlag|=MF_BCI_CHAR_HAIR_COLOR;
			break;
		case SST_BCI_CHAR_FACE_TYPE:
			m_CharFaceType=Value;
			ModifyFlag|=MF_BCI_CHAR_FACE_TYPE;
			break;
		case SST_BCI_CHAR_HAIR_TYPE:
			m_CharHairType=Value;
			ModifyFlag|=MF_BCI_CHAR_HAIR_TYPE;
			break;
		case SST_BCI_CHAR_BEARD_TYPE:
			m_CharBeardType=Value;
			ModifyFlag|=MF_BCI_CHAR_BEARD_TYPE;
			break;
		case SST_BCI_IS_CHAR_BALD:
			m_IsCharBald=Value;
			ModifyFlag|=MF_BCI_IS_CHAR_BALD;
			break;
		case SST_BCI_MAP_ID:
			m_MapID=Value;
			ModifyFlag|=MF_BCI_MAP_ID;
			break;
		}
	}
	m_ModifyFlag|=ModifyFlag;
	return true;
}