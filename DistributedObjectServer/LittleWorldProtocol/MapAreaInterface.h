#pragma once

//
class IMapArea:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	RegisterMapObject
	用途:	
	参数:
    ObjectID				
    ControllerID				
    AreaEventReceiverID				
    MoveStatus				
    Flag				
	返回值:无特别意义
	*************************************************/
	virtual int RegisterMapObject(UINT64 ObjectID ,OBJECT_ID ControllerID ,OBJECT_ID AreaEventReceiverID ,CSmartStruct& MoveStatus ,UINT Flag ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	UnregisterMapObject
	用途:	
	参数:
    ObjectID				
	返回值:无特别意义
	*************************************************/
	virtual int UnregisterMapObject(UINT64 ObjectID ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	UpdateMoveStatus
	用途:	
	参数:
    ObjectID				
    MoveStatus				
	返回值:无特别意义
	*************************************************/
	virtual int UpdateMoveStatus(UINT64 ObjectID ,CSmartStruct& MoveStatus ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	SendEventMsgPacket
	用途:	
	参数:
    ObjectID				
    EventMsgPacket				
	返回值:无特别意义
	*************************************************/
	virtual int SendEventMsgPacket(UINT64 ObjectID ,CSmartStruct& EventMsgPacket ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	HideOnMap
	用途:	
	参数:
    ObjectID				
    HideType				
	返回值:无特别意义
	*************************************************/
	virtual int HideOnMap(UINT64 ObjectID ,BYTE HideType ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	TeleportInMap
	用途:	
	参数:
    ObjectID				
    TargetPosX				
    TargetPosY				
    TargetHeight				
    TargetDir				
	返回值:无特别意义
	*************************************************/
	virtual int TeleportInMap(UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir ) {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum REGISTERMAPOBJECT_MEMBER_IDS
	{
		REGISTERMAPOBJECT_MEMBER_OBJECTID=1,
		REGISTERMAPOBJECT_MEMBER_CONTROLLERID=2,
		REGISTERMAPOBJECT_MEMBER_AREAEVENTRECEIVERID=3,
		REGISTERMAPOBJECT_MEMBER_MOVESTATUS=7,
		REGISTERMAPOBJECT_MEMBER_FLAG=8,
	};

	enum UNREGISTERMAPOBJECT_MEMBER_IDS
	{
		UNREGISTERMAPOBJECT_MEMBER_OBJECTID=1,
	};

	enum UPDATEMOVESTATUS_MEMBER_IDS
	{
		UPDATEMOVESTATUS_MEMBER_OBJECTID=1,
		UPDATEMOVESTATUS_MEMBER_MOVESTATUS=11,
	};

	enum SENDEVENTMSGPACKET_MEMBER_IDS
	{
		SENDEVENTMSGPACKET_MEMBER_OBJECTID=1,
		SENDEVENTMSGPACKET_MEMBER_EVENTMSGPACKET=2,
	};

	enum HIDEONMAP_MEMBER_IDS
	{
		HIDEONMAP_MEMBER_OBJECTID=1,
		HIDEONMAP_MEMBER_HIDETYPE=2,
	};

	enum TELEPORTINMAP_MEMBER_IDS
	{
		TELEPORTINMAP_MEMBER_OBJECTID=1,
		TELEPORTINMAP_MEMBER_TARGETPOSX=2,
		TELEPORTINMAP_MEMBER_TARGETPOSY=3,
		TELEPORTINMAP_MEMBER_TARGETHEIGHT=5,
		TELEPORTINMAP_MEMBER_TARGETDIR=4,
	};


};
