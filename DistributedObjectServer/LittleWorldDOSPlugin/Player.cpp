#include "StdAfx.h"

START_DOS_MSG_MAP(CPlayer)
DOS_MSG_MAP(CPlayerCharDBAckMsgHandler)
DOS_MSG_MAP(CPlayerCharOperatorMsgHandler)
END_DOS_MSG_MAP(CPlayer)
CPlayer::CPlayer(void)
{
	FUNCTION_BEGIN;
	INIT_DOS_MSG_MAP;
	m_pManager=NULL;
	m_ManagerID=0;
	m_PoolID=0;
	m_ClientProxyID=0;
	m_CharDBID=0;
	m_AccountID=0;
	m_Status=0;
	FUNCTION_END;
}

CPlayer::~CPlayer(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


BOOL CPlayer::Init(CPlayerManager * pManager,OBJECT_ID ManagerID,UINT PoolID,OBJECT_ID MapID,OBJECT_ID CharQueryCenterID,OBJECT_ID CharDBID,OBJECT_ID ClientProxyID,LPCTSTR szAccountName,LPCTSTR szPassword)
{
	FUNCTION_BEGIN;
	if(!CCharacter::Init())
		return FALSE;
	m_pManager=pManager;
	m_ManagerID=ManagerID;
	m_PoolID=PoolID;
	m_MapID=MapID;
	m_CharQueryCenterID=CharQueryCenterID;
	m_CharDBID=CharDBID;
	m_ClientProxyID=ClientProxyID;
	m_AccountName=szAccountName;
	m_Password=szPassword;
	m_AccountID=0;
	m_Status=STATUS_LOGIN;
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

BOOL CPlayer::Initialize(IDistributedObjectOperator * pOperator)
{
	FUNCTION_BEGIN;
	if(!CCharacter::Initialize(pOperator))
		return FALSE;

	m_pOperator->AddConcernedObject(m_ClientProxyID,true);
	m_pOperator->AddConcernedObject(m_CharDBID,true);
	//m_pOperator->AddConcernedObject(m_MapID,true);

	CPlayerCharDBMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_CharDBID);

	MsgCaller.AccountLogin(m_AccountName,m_Password,"");

	return TRUE;
	FUNCTION_END;
	return FALSE;
}
void CPlayer::Destory()
{
	FUNCTION_BEGIN;
	SavePlayerData();

	CCharacter::Destory();

	if(m_pManager)
		m_pManager->DeletePlayer(m_PoolID);
	FUNCTION_END;
}

void CPlayer::ForceRelease()
{
	FUNCTION_BEGIN;
	if(m_pOperator)
		m_pOperator->Release();
	FUNCTION_END;
}

void CPlayer::OnConcernedObjectLost(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	if(ObjectID==m_ClientProxyID)
	{
		OnClientDisconnect();
	}
	if(ObjectID==m_CharDBID||ObjectID==m_MapID)
	{
		DisconnectClient();
	}
	FUNCTION_END;
}

int CPlayer::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	return CCharacter::Update(ProcessPacketLimit);
	FUNCTION_END;
}

int CPlayer::MapChangeConfirm(int Result )
{
	FUNCTION_BEGIN;
	if(m_Status==STATUS_CHAR_OPERATE)
	{
		if(Result==CMRT_OK)
		{
			if(CCharacter::AddCharToMap(m_MapID,m_CharQueryCenterID,m_pOperator->GetObjectID(),MOF_USE_MSG_CACHE,
				m_BaseCharInfo.GetCharID(),m_BaseCharInfo.GetCharName(),CT_PLAYER,
				m_MoveStatus.GetPosX(),m_MoveStatus.GetPosY(),m_MoveStatus.GetHeight(),m_MoveStatus.GetDirection()))
			{
				return COMMON_RESULT_SUCCEED;
			}
			else
			{
				LogDebug("CPlayer::MapChangeConfirm:帐户[%s]角色[0x%llX]加入地图失败",
					(LPCTSTR)m_AccountName,m_BaseCharInfo.GetCharID());
				DisconnectClient();
			}
		}
		else
		{
			LogDebug("CPlayer::MapChangeConfirm:客户端未确认地图加载%d",Result);
			DisconnectClient();
		}
	}
	else
	{
		return CCharacter::MapChangeConfirm(Result);
	}
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}

int CPlayer::RegisterMapObjectAck(UINT64 ObjectID ,int Result )
{
	FUNCTION_BEGIN;
	if(Result!=MORRT_OK)
	{
		Log("注册地图对象失败(%d)，断开客户端",Result);
		DisconnectClient();
	}
	else
	{

		MSG_ID_TYPE MsgIDs[]={
			MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_CHAR_INFO,false),
			MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_MOVE,false),
			MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_FLY,false),
			MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_MAP_CHANGE,false),
			MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_JUMP,false),
		};
		m_pOperator->RegisterMsgMap(m_ClientProxyID,MsgIDs,sizeof(MsgIDs)/sizeof(MSG_ID_TYPE));

		//CCharQueryCenterMsgCaller CQCMsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ManagerID);

		//CQCMsgCaller.RegisterChar(m_BaseCharInfo.GetCharID());



		m_Status=STATUS_GAMING;


	}
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}

void CPlayer::OnClientDisconnect()
{
	FUNCTION_BEGIN;
	LogDebug("[%llX][%s]断线",m_BaseCharInfo.GetCharID(),m_BaseCharInfo.GetCharName());

	

	CCharQueryCenterMsgCaller CQCMsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ManagerID);

	CQCMsgCaller.UnregisterChar(m_BaseCharInfo.GetCharID());

	m_pOperator->Release();
	FUNCTION_END;
}

void CPlayer::DisconnectClient()
{
	FUNCTION_BEGIN;
	m_pOperator->CloseProxyObject(m_ClientProxyID,1000);
	FUNCTION_END;
}

void CPlayer::SavePlayerData()
{
	FUNCTION_BEGIN;
	CPlayerCharDBMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_CharDBID);

	char Buffer[MAX_PLAYER_CHAR_INFO_SIZE];
	CSmartStruct CharData(Buffer,MAX_PLAYER_CHAR_INFO_SIZE,true);
	if(m_MoveStatus.MakePacket(CharData,MF_MOVE_STATUS)&&m_BaseCharInfo.MakePacket(CharData,MF_BASE_CHAR_INFO))
	{
		MsgCaller.SaveCharData(m_AccountID,m_BaseCharInfo.GetCharID(),CharData);
	}
	
	FUNCTION_END;
}

int CPlayer::AccountLoginAck(int Result ,UINT64 AccountID ,CSmartStruct& AccountData )
{
	FUNCTION_BEGIN;
	CPlayerManagerAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);

	LogDebug("CPlayer::AccountLoginAck:帐户[%s]登陆返回结果%d",
		(LPCTSTR)m_AccountName,Result);

	if(Result==PLRT_OK)
	{
		MSG_ID_TYPE MsgIDs[]={
			MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_GET_CHAR_LIST,false),
			MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_CREATE_CHAR,false),
			MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_DELETE_CHAR,false),
			MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_SELECT_CHAR,false),
		};
		m_pOperator->RegisterMsgMap(m_ClientProxyID,MsgIDs,sizeof(MsgIDs)/sizeof(MSG_ID_TYPE));

		m_AccountID=AccountID;
		MsgCaller.PlayerLoginAck(Result,AccountID);
		m_Status=STATUS_CHAR_OPERATE;
	}
	else
	{
		MsgCaller.PlayerLoginAck(PLRT_FAILED,0);
	}
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::AccountLogoutAck(int Result ,UINT64 AccountID )
{
	FUNCTION_BEGIN;
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::GetAccountCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData )
{
	FUNCTION_BEGIN;
	LogDebug("CPlayer::GetAccountCharAck:帐户[%s]请求角色列表返回结果%d",
		(LPCTSTR)m_AccountName,Result);

	CPlayerCharOperatorAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);
	MsgCaller.GetCharListAck(Result,CharData);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::CreateCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData )
{
	FUNCTION_BEGIN;
	LogDebug("CPlayer::CreateCharAck:帐户[%s]请求创建角色返回结果%d",
		(LPCTSTR)m_AccountName,Result);

	CPlayerCharOperatorAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);
	MsgCaller.CreateCharAck(Result,CharData);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::DeleteCharAck(int Result ,UINT64 AccountID ,UINT64 CharID )
{
	FUNCTION_BEGIN;
	LogDebug("CPlayer::DeleteCharAck:帐户[%s]请求删除角色返回结果%d",
		(LPCTSTR)m_AccountName,Result);

	CPlayerCharOperatorAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);
	MsgCaller.DeleteCharAck(Result,CharID);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::GetCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData )
{
	FUNCTION_BEGIN;

	LogDebug("CPlayer::GetCharDataAck:帐户[%s]请求获取角色[0x%llX]数据返回结果%d",
		(LPCTSTR)m_AccountName,CharID,Result);

	CPlayerCharOperatorAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);

	if(Result==CMRT_OK)
	{
		if(m_MoveStatus.ParsePacket(CharData,MF_MOVE_STATUS)&&m_BaseCharInfo.ParsePacket(CharData,MF_BASE_CHAR_INFO))
		{
			MSG_ID_TYPE MsgIDs[]={
				MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_MAP_CHANGE,false),
			};
			m_pOperator->RegisterMsgMap(m_ClientProxyID,MsgIDs,sizeof(MsgIDs)/sizeof(MSG_ID_TYPE));

			MsgCaller.SelectCharAck(CMRT_OK,m_BaseCharInfo.GetCharID(),CharData);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			LogDebug("CPlayer::GetCharDataAck:帐户[%s]解包角色[0x%llX]数据失败",
				(LPCTSTR)m_AccountName,CharID);

			Result=CMRT_FAILED;
		}
	}

	CSmartStruct Empty;

	MsgCaller.SelectCharAck(Result,CharID,Empty);


	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::SaveCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID )
{
	FUNCTION_BEGIN;
	LogDebug("0x%llX保存角色0x%llX结果%d",AccountID,CharID,Result);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}


int CPlayer::GetCharList()
{
	FUNCTION_BEGIN;
	LogDebug("CPlayer::GetCharList:[%s]请求获取角色列表",
		(LPCTSTR)m_AccountName);
	CPlayerCharOperatorAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);
	if(m_Status==STATUS_CHAR_OPERATE)
	{
		CPlayerCharDBMsgCaller DBMsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_CharDBID);

		DBMsgCaller.GetAccountChar(m_AccountID);
		return COMMON_RESULT_SUCCEED;
	}
	CSmartStruct Empty;
	MsgCaller.GetCharListAck(CMRT_FAILED,Empty);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}


int CPlayer::CreateChar(CSmartStruct& CharCreateData )
{
	FUNCTION_BEGIN;
	LogDebug("CPlayer::CreateChar:[%s]请求创建角色",
		(LPCTSTR)m_AccountName);

	CSmartStruct Empty;

	CPlayerCharOperatorAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);
	if(m_Status==STATUS_CHAR_OPERATE)
	{
		LPCTSTR CharName=(LPCTSTR)CharCreateData.GetMember(SST_BCI_CHAR_NAME);
		BYTE Race=CharCreateData.GetMember(SST_BCI_RACE);
		BYTE Gender=CharCreateData.GetMember(SST_BCI_GENDER);
		BYTE SkinColor=CharCreateData.GetMember(SST_BCI_CHAR_SKIN_COLOR);
		BYTE HairColor=CharCreateData.GetMember(SST_BCI_CHAR_HAIR_COLOR);
		BYTE FaceType=CharCreateData.GetMember(SST_BCI_CHAR_FACE_TYPE);
		BYTE HairType=CharCreateData.GetMember(SST_BCI_CHAR_HAIR_TYPE);
		BYTE BeardType=CharCreateData.GetMember(SST_BCI_CHAR_BEARD_TYPE);
		BYTE IsCharBald=CharCreateData.GetMember(SST_BCI_IS_CHAR_BALD);
		UINT64 MapID=0;
		float PosX=0;
		float PosY=0;
		float PosHeight=0;
		float Direction=0;

		if(Race>0)
		{
			CMainConfig::BORN_POINT BornPoint;

			ZeroMemory(&BornPoint,sizeof(BornPoint));

			CMainConfig::GetInstance()->GetRandomBornPoint(BornPoint);

			char Buffer[MAX_MOVE_STATUS_SIZE];
			CSmartStruct Packet(Buffer,MAX_MOVE_STATUS_SIZE,true);

			Packet.AddMember(SST_BCI_CHAR_NAME,CharName);
			Packet.AddMember(SST_BCI_RACE,Race);
			Packet.AddMember(SST_BCI_GENDER,Gender);
			Packet.AddMember(SST_BCI_CHAR_SKIN_COLOR,SkinColor);
			Packet.AddMember(SST_BCI_CHAR_HAIR_COLOR,HairColor);
			Packet.AddMember(SST_BCI_CHAR_FACE_TYPE,FaceType);
			Packet.AddMember(SST_BCI_CHAR_HAIR_TYPE,HairType);
			Packet.AddMember(SST_BCI_CHAR_BEARD_TYPE,BeardType);
			Packet.AddMember(SST_BCI_IS_CHAR_BALD,IsCharBald);
			Packet.AddMember(SST_BCI_MAP_ID,BornPoint.MapID);
			Packet.AddMember(SST_MS_XPOS,BornPoint.PosX);
			Packet.AddMember(SST_MS_YPOS,BornPoint.PosY);
			Packet.AddMember(SST_MS_HEIGHT,BornPoint.Height);
			Packet.AddMember(SST_MS_DIRECTION,BornPoint.Dir);


			CPlayerCharDBMsgCaller DBMsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_CharDBID);

			DBMsgCaller.CreateChar(m_AccountID,Packet);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			MsgCaller.CreateCharAck(PCCRT_CHAR_DATA_ERROR,Empty);
			return COMMON_RESULT_SUCCEED;
		}
	}
	
	MsgCaller.CreateCharAck(CMRT_FAILED,Empty);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::DeleteChar(UINT64 CharID )
{
	FUNCTION_BEGIN;
	LogDebug("CPlayer::DeleteChar:[%s]请求删除角色[%llu]",
		(LPCTSTR)m_AccountName,CharID);
	CPlayerCharOperatorAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);
	if(m_Status==STATUS_CHAR_OPERATE)
	{
		CPlayerCharDBMsgCaller DBMsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_CharDBID);

		DBMsgCaller.DeleteChar(m_AccountID,CharID);
		return COMMON_RESULT_SUCCEED;
	}
	MsgCaller.DeleteCharAck(CMRT_FAILED,CharID);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::SelectChar(UINT64 CharID )
{
	FUNCTION_BEGIN;
	LogDebug("CPlayer::SelectChar:[%s]请求选择角色[%llu]",
		(LPCTSTR)m_AccountName,CharID);
	CPlayerCharOperatorAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);
	if(m_Status==STATUS_CHAR_OPERATE)
	{
		CPlayerCharDBMsgCaller DBMsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_CharDBID);

		DBMsgCaller.GetCharData(m_AccountID,CharID);
		return COMMON_RESULT_SUCCEED;
	}
	CSmartStruct Empty;
	MsgCaller.SelectCharAck(CMRT_FAILED,CharID,Empty);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}


int CPlayer::NewMapObject(UINT64 ObjectID ,CSmartStruct& MoveStatus )
{
	FUNCTION_BEGIN;

	CMapAreaAckMsgCaller  MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);

	MsgCaller.NewMapObject(ObjectID,MoveStatus);

	return COMMON_RESULT_FAILED;

	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::DeleteMapObject(UINT64 ObjectID )
{
	FUNCTION_BEGIN;

	CMapAreaAckMsgCaller  MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);

	MsgCaller.DeleteMapObject(ObjectID);

	return COMMON_RESULT_FAILED;

	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& MoveStatus )
{
	FUNCTION_BEGIN;

	CMapAreaAckMsgCaller  MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);

	MsgCaller.MoveStatusUpdate(ObjectID,IsForce,MoveStatus);

	if(ObjectID==m_BaseCharInfo.GetCharID())
	{
		UINT CurTime=CEasyTimer::GetTime();
		m_MoveStatus.SetMoveStatus(CurTime,MoveStatus);
	}

	return COMMON_RESULT_FAILED;

	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayer::EventMsgPacket(CSmartStruct& EventMsg )
{
	FUNCTION_BEGIN;

	CMapAreaAckMsgCaller  MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_ClientProxyID);

	MsgCaller.EventMsgPacket(EventMsg);

	void * Pos=EventMsg.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct Value=EventMsg.GetNextMember(Pos,MemberID);
		if(MemberID==SST_GDMP_MSG)
		{
			UINT MsgID=Value.GetMember(SST_GDM_MSG_ID);
			CSmartStruct MsgPacket=Value.GetMember(SST_GDM_MSG_DATA);

			if(MsgID==MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_MOVE_STATUS_UPDATE,true))
			{
				UINT64 ObjectID = MsgPacket.GetMember(MOVESTATUSUPDATE_MEMBER_OBJECTID);
				if(ObjectID==m_BaseCharInfo.GetCharID())
				{
					CSmartStruct MoveStatus = MsgPacket.GetMember(MOVESTATUSUPDATE_MEMBER_MOVESTATUS);

					UINT CurTime=CEasyTimer::GetTime();
					m_MoveStatus.SetMoveStatus(CurTime,MoveStatus);
				}
			}
		}
	}

	return COMMON_RESULT_FAILED;

	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}