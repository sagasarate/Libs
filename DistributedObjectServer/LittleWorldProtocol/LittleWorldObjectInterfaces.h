#pragma once

#include "DataDefines.h"
#include "MoveStatus.h"
#include "BaseCharInfo.h"

enum LITTLE_WORLD_OBJECT_INTERFACES
{
	INTERFACE_LW_MAP_AREA=1,
	INTERFACE_LW_CHARACTER,
	INTERFACE_LW_PLAYER_MANAGER,
	INTERFACE_LW_CHAR_QUERY_CENTER,
	INTERFACE_LW_PLAYER_CHAR_DB,
	INTERFACE_LW_PLAYER_CHAR_OPERATOR,
};


inline UINT CaculateMsgPacketSize(UINT MsgDataLen)
{
	UINT NeedLen=CSmartStruct::GetFixMemberSize(sizeof(MSG_ID_TYPE));
	NeedLen+=CSmartStruct::GetStructMemberSize(MsgDataLen);		
	NeedLen+=CSmartStruct::GetStructMemberSize(0);
	return NeedLen;
}


inline bool PackMsgToMsgPacket(CSmartStruct& Packet,MSG_ID_TYPE MsgID,CSmartStruct& MsgData)
{
	UINT BufferSize;
	void * pBuffer=Packet.PrepareMember(BufferSize);
	CSmartStruct Msg(pBuffer,BufferSize,true);
	if(!Msg.AddMember(SST_GDM_MSG_ID,MsgID))
	{
		return false;
	}
	if(!Msg.AddMember(SST_GDM_MSG_DATA,MsgData))
	{
		return false;
	}
	if(!Packet.FinishMember(SST_GDMP_MSG,Msg.GetDataLen()))
	{
		return false;
	}
	return true;
}


