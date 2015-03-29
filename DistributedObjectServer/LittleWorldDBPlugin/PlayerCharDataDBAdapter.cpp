#include "StdAfx.h"


#define CHAR_ID_SEED_FETCH_TIME		5000

START_DOS_MSG_MAP(CPlayerCharDataDBAdapter)
DOS_MSG_MAP(CPlayerCharDBMsgHandler)
END_DOS_MSG_MAP(CPlayerCharDataDBAdapter)

CPlayerCharDataDBAdapter::CPlayerCharDataDBAdapter(void)
{
	FUNCTION_BEGIN;

	INIT_DOS_MSG_MAP;
	m_pOperator=NULL;
	m_pDBTransManager=NULL;
	m_pDatabase=NULL;
	m_CharIDSeed=0;
	FUNCTION_END;
}

CPlayerCharDataDBAdapter::~CPlayerCharDataDBAdapter(void)
{
	Destory();
}

BOOL CPlayerCharDataDBAdapter::Initialize(IDistributedObjectOperator * pOperator)
{
	FUNCTION_BEGIN;
	m_pOperator=pOperator;

	if(CMainConfig::GetInstance()->GetPlayerCharDataDBConfig().DBType==CMainConfig::DB_TYPE_MSSQL)
	{
#ifdef WIN32
		m_pDatabase=new CODBCDatabase();
		((CODBCDatabase *)m_pDatabase)->Init();
#else
		Log("Linux版不支持MSSQL");
		return FALSE;
#endif
	}
	else
	{
		m_pDatabase=new CMySQLDatabase();
	}


	m_pDBTransManager=new CDBTransationManager();
	if(!m_pDBTransManager->Init(m_pDatabase,
		CMainConfig::GetInstance()->GetPlayerCharDataDBConfig().DBConnectString,
		CMainConfig::GetInstance()->GetPlayerCharDataDBConfig().ThreadCount,
		CMainConfig::GetInstance()->GetPlayerCharDataDBConfig().QueueSize,
		CMainConfig::GetInstance()->GetPlayerCharDataDBConfig().IsLogPerformance?CDBTransationManager::DBTM_FLAG_LOG_PERFORMANCE:0))
	{
		Log("CPlayerCharDataDBAdapter::Initialize:数据库初始化失败");
	}

	m_CharIDSeedFetchTimer.SaveTime();

	Log("PlayerCharDataDBAdapter初始化成功");

	return TRUE;
	FUNCTION_END;
	return FALSE;
}
void CPlayerCharDataDBAdapter::Destory()
{
	FUNCTION_BEGIN;

	SAFE_RELEASE(m_pDBTransManager);
	SAFE_RELEASE(m_pDatabase);
	m_pOperator=NULL;


	FUNCTION_END;
}


int CPlayerCharDataDBAdapter::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	int ProcessCount=0;

	ProcessCount+=m_pDBTransManager->Update(CMainConfig::GetInstance()->GetPlayerCharDataDBConfig().RunLimit);

	if(m_CharIDSeed==0)
	{
		if(m_CharIDSeedFetchTimer.IsTimeOut(CHAR_ID_SEED_FETCH_TIME))
		{
			m_CharIDSeedFetchTimer.SaveTime();
			CDBTransGetCharIDSeed * pDBTrans=new CDBTransGetCharIDSeed();
			if(!m_pDBTransManager->AddTransaction(pDBTrans))
			{
				Log("CPlayerCharDataDBAdapter::Update:提交数据库事务CDBTransGetCharIDSeed失败");
				SAFE_RELEASE(pDBTrans);
			}
		}
	}

	return ProcessCount;
	FUNCTION_END;
	return 0;
}

bool CPlayerCharDataDBAdapter::UnpackCharCreateInfo(const CSmartStruct& Packet,CHAR_CREATE_INFO& CreateInfo)
{
	FUNCTION_BEGIN;
	CreateInfo.CharName=(LPCTSTR)Packet.GetMember(SST_BCI_CHAR_NAME);
	CreateInfo.Race=Packet.GetMember(SST_BCI_RACE);
	CreateInfo.Gender=Packet.GetMember(SST_BCI_GENDER);
	CreateInfo.SkinColor=Packet.GetMember(SST_BCI_CHAR_SKIN_COLOR);
	CreateInfo.HairColor=Packet.GetMember(SST_BCI_CHAR_HAIR_COLOR);
	CreateInfo.FaceType=Packet.GetMember(SST_BCI_CHAR_FACE_TYPE);
	CreateInfo.HairType=Packet.GetMember(SST_BCI_CHAR_HAIR_TYPE);
	CreateInfo.BeardType=Packet.GetMember(SST_BCI_CHAR_BEARD_TYPE);
	CreateInfo.IsCharBald=Packet.GetMember(SST_BCI_IS_CHAR_BALD);
	CreateInfo.MapID=Packet.GetMember(SST_BCI_MAP_ID);
	CreateInfo.PosX=Packet.GetMember(SST_MS_XPOS);
	CreateInfo.PosY=Packet.GetMember(SST_MS_YPOS);
	CreateInfo.PosHeight=Packet.GetMember(SST_MS_HEIGHT);
	CreateInfo.Direction=Packet.GetMember(SST_MS_DIRECTION);
	return true;
	FUNCTION_END;
	return false;
}

bool CPlayerCharDataDBAdapter::PackPlayerCharInfo(CSmartStruct& Packet,const BASE_CHAR_INFO& BaseCharInfo)
{
	FUNCTION_BEGIN;
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_ID,BaseCharInfo.CharID));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_NAME,(LPCTSTR)BaseCharInfo.CharName));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_RACE,BaseCharInfo.Race));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_GENDER,BaseCharInfo.Gender));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_SKIN_COLOR,BaseCharInfo.SkinColor));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_HAIR_COLOR,BaseCharInfo.HairColor));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_FACE_TYPE,BaseCharInfo.FaceType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_HAIR_TYPE,BaseCharInfo.HairType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_CHAR_BEARD_TYPE,BaseCharInfo.BeardType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_IS_CHAR_BALD,BaseCharInfo.IsCharBald));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_BCI_MAP_ID,BaseCharInfo.MapID));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_XPOS,BaseCharInfo.PosX));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_YPOS,BaseCharInfo.PosY));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_HEIGHT,BaseCharInfo.PosHeight));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_MS_DIRECTION,BaseCharInfo.Direction));
	return true;
	FUNCTION_END;
	return false;
}
bool CPlayerCharDataDBAdapter::PackPlayerCharInfo(CSmartStruct& Packet,const PLAYER_CHAR_INFO& PlayerCharInfo)
{
	FUNCTION_BEGIN;
	if(!PackPlayerCharInfo(Packet,PlayerCharInfo.BaseCharInfo))
		return false;
	return true;
	FUNCTION_END;
	return false;
}
bool CPlayerCharDataDBAdapter::UnpackPlayerCharInfo(const CSmartStruct& Packet,PLAYER_CHAR_INFO& PlayerCharInfo)
{
	FUNCTION_BEGIN;
	PlayerCharInfo.BaseCharInfo.CharName=(LPCTSTR)Packet.GetMember(SST_BCI_CHAR_NAME);
	PlayerCharInfo.BaseCharInfo.Race=Packet.GetMember(SST_BCI_RACE);
	PlayerCharInfo.BaseCharInfo.Gender=Packet.GetMember(SST_BCI_GENDER);
	PlayerCharInfo.BaseCharInfo.SkinColor=Packet.GetMember(SST_BCI_CHAR_SKIN_COLOR);
	PlayerCharInfo.BaseCharInfo.HairColor=Packet.GetMember(SST_BCI_CHAR_HAIR_COLOR);
	PlayerCharInfo.BaseCharInfo.FaceType=Packet.GetMember(SST_BCI_CHAR_FACE_TYPE);
	PlayerCharInfo.BaseCharInfo.HairType=Packet.GetMember(SST_BCI_CHAR_HAIR_TYPE);
	PlayerCharInfo.BaseCharInfo.BeardType=Packet.GetMember(SST_BCI_CHAR_BEARD_TYPE);
	PlayerCharInfo.BaseCharInfo.IsCharBald=Packet.GetMember(SST_BCI_IS_CHAR_BALD);
	PlayerCharInfo.BaseCharInfo.MapID=Packet.GetMember(SST_BCI_MAP_ID);
	PlayerCharInfo.BaseCharInfo.PosX=Packet.GetMember(SST_MS_XPOS);
	PlayerCharInfo.BaseCharInfo.PosY=Packet.GetMember(SST_MS_YPOS);
	PlayerCharInfo.BaseCharInfo.PosHeight=Packet.GetMember(SST_MS_HEIGHT);
	PlayerCharInfo.BaseCharInfo.Direction=Packet.GetMember(SST_MS_DIRECTION);
	return true;
	FUNCTION_END;
	return false;
}

bool CPlayerCharDataDBAdapter::PackPlayerCharInfoList(CSmartStruct& Packet,const CEasyArray<BASE_CHAR_INFO>& CharList)
{
	FUNCTION_BEGIN;
	for(UINT i=0;i<CharList.GetCount();i++)
	{
		UINT BuffSize=0;
		void * pBuffer=Packet.PrepareMember(BuffSize);
		CSmartStruct CharInfo(pBuffer,BuffSize,true);
		if(!PackPlayerCharInfo(CharInfo,CharList.GetObjectConst(i)))
			return false;
		Packet.FinishMember(SST_CIL_CHAR_INFO,CharInfo.GetDataLen());
	}

	return true;
	FUNCTION_END;
	return false;
}

int CPlayerCharDataDBAdapter::AccountLogin(LPCTSTR szAccountName ,LPCTSTR szPassword ,LPCTSTR szClientIP )
{
	FUNCTION_BEGIN;
	LogDebug("[%s]请求登陆",
		szAccountName);

	CDBTransAccountLogin * pDBTrans=new CDBTransAccountLogin(GetCurMsgSenderID(),szAccountName,szPassword);
	if(!m_pDBTransManager->AddTransaction(pDBTrans))
	{
		Log("CPlayerCharDataDBAdapter::AccountLogin:提交数据库事务失败");
		SAFE_RELEASE(pDBTrans);
		CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());
		MsgCaller.AccountLogoutAck(PLRT_FAILED,0);
	}
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayerCharDataDBAdapter::AccountLogout(UINT64 AccountID )
{
	FUNCTION_BEGIN;
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayerCharDataDBAdapter::GetAccountChar(UINT64 AccountID )
{
	FUNCTION_BEGIN;
	LogDebug("[0x%llX]请求获取角色列表",
		AccountID);

	CDBTransGetAccountChar * pDBTrans=new CDBTransGetAccountChar(GetCurMsgSenderID(),AccountID);
	if(m_pDBTransManager->AddTransaction(pDBTrans))
	{
		return COMMON_RESULT_SUCCEED;
	}
	else
	{
		Log("CPlayerCharDataDBAdapter::GetAccountChar:提交数据库事务失败");
		SAFE_RELEASE(pDBTrans);
	}

	CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());
	CSmartStruct Empty;
	MsgCaller.GetAccountCharAck(CMRT_FAILED,AccountID,Empty);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayerCharDataDBAdapter::CreateChar(UINT64 AccountID ,CSmartStruct& CharData )
{
	FUNCTION_BEGIN;
	LogDebug("[0x%llX]请求创建角色",
		AccountID);
	CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());
	if(m_CharIDSeed)
	{
		CHAR_CREATE_INFO CreateInfo;
		CreateInfo.AccountID=AccountID;
		CreateInfo.CharID=m_CharIDSeed;
		m_CharIDSeed++;
		UnpackCharCreateInfo(CharData,CreateInfo);
		CDBTransCreateChar * pDBTrans=new CDBTransCreateChar(GetCurMsgSenderID(),CreateInfo);
		if(m_pDBTransManager->AddTransaction(pDBTrans))
		{
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			Log("CPlayerCharDataDBAdapter::CreateChar:提交数据库事务失败");
			SAFE_RELEASE(pDBTrans);
		}
	}
	else
	{
		Log("CPlayerCharDataDBAdapter::CreateChar:[0x%llX]请求创建角色时,CharIDSeed还未准备好");
	}
    CSmartStruct Empty;
	MsgCaller.CreateCharAck(PCCRT_FAILED,AccountID,Empty);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayerCharDataDBAdapter::DeleteChar(UINT64 AccountID ,UINT64 CharID )
{
	FUNCTION_BEGIN;	
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayerCharDataDBAdapter::GetCharData(UINT64 AccountID ,UINT64 CharID )
{
	FUNCTION_BEGIN;
	LogDebug("[0x%llX]请求获取角色[0x%llX]的数据",
		AccountID,CharID);

	CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());

	CDBTransGetCharData * pDBTrans=new CDBTransGetCharData(GetCurMsgSenderID(),AccountID,CharID);
	if(m_pDBTransManager->AddTransaction(pDBTrans))
	{
		return COMMON_RESULT_SUCCEED;
	}
	else
	{
		Log("CPlayerCharDataDBAdapter::GetCharData:提交数据库事务失败");
		SAFE_RELEASE(pDBTrans);
	}
    CSmartStruct Empty;
	MsgCaller.GetCharDataAck(CMRT_FAILED,AccountID,CharID,Empty);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}
int CPlayerCharDataDBAdapter::SaveCharData(UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData )
{
	FUNCTION_BEGIN;
	LogDebug("[0x%llX]请求保存角色0x%llX",
		AccountID,CharID);

	CSmartStruct Empty;
	CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());

	PLAYER_CHAR_INFO CharInfo;
	if(UnpackPlayerCharInfo(CharData,CharInfo))
	{
		CharInfo.AccountID=AccountID;
		CharInfo.BaseCharInfo.CharID=CharID;
		CDBTransSaveCharData * pDBTrans=new CDBTransSaveCharData(GetCurMsgSenderID(),CharInfo);
		if(m_pDBTransManager->AddTransaction(pDBTrans))
		{
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			Log("CPlayerCharDataDBAdapter::SaveCharData:提交数据库事务失败");
			SAFE_RELEASE(pDBTrans);
		}
	}
	else
	{
		Log("CPlayerCharDataDBAdapter::SaveCharData:解包角色数据失败");
	}

	MsgCaller.SaveCharDataAck(CMRT_FAILED,AccountID,CharID);
	return COMMON_RESULT_SUCCEED;
	FUNCTION_END;
	return COMMON_RESULT_FAILED;
}

void CPlayerCharDataDBAdapter::OnDBRetGetCharIDSeed(UINT64 CharIDSeed)
{
	FUNCTION_BEGIN;
	LogDebug("CPlayerCharDataDBAdapter::OnDBRetGetCharIDSeed:获取CharIDSeed=0x%llX",CharIDSeed);
	m_CharIDSeed=CharIDSeed;
	FUNCTION_END;
}

void CPlayerCharDataDBAdapter::OnDBRetAccountLogin(OBJECT_ID CallerID,LPCTSTR szAccountName,UINT64 AccountID)
{
	FUNCTION_BEGIN;
	LogDebug("[%s]请求登陆，数据库返回0x%llX",
		szAccountName,AccountID);

    CSmartStruct Empty;
	CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),CallerID);
	if(AccountID)
		MsgCaller.AccountLoginAck(PLRT_OK,AccountID,Empty);
	else
		MsgCaller.AccountLoginAck(PLRT_FAILED,AccountID,Empty);
	FUNCTION_END;
}

void CPlayerCharDataDBAdapter::OnDBRetCreateChar(OBJECT_ID CallerID,int Result,PLAYER_CHAR_INFO& PlayerCharInfo)
{
	FUNCTION_BEGIN;
	LogDebug("[0x%llX]请求登陆，数据库返回%d",
		PlayerCharInfo.AccountID,Result);
    CSmartStruct Empty;
	CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),CallerID);
	if(Result==PCCRT_OK)
	{
		char Buff[MAX_PLAYER_CHAR_INFO_SIZE];
		CSmartStruct CharInfo(Buff,MAX_PLAYER_CHAR_INFO_SIZE,true);
		if(PackPlayerCharInfo(CharInfo,PlayerCharInfo))
		{
			MsgCaller.CreateCharAck(PCCRT_OK,PlayerCharInfo.AccountID,CharInfo);
		}
		else
		{
			Log("CPlayerCharDataDBAdapter::OnDBRetCreateChar:打包角色数据失败");

			MsgCaller.CreateCharAck(PCCRT_FAILED,PlayerCharInfo.AccountID,Empty);
		}
	}
	else
	{
		MsgCaller.CreateCharAck(Result,PlayerCharInfo.AccountID,Empty);
	}
	FUNCTION_END;
}

void CPlayerCharDataDBAdapter::OnDBRetGetAccountChar(OBJECT_ID CallerID,int Result,UINT64 AccountID,const CEasyArray<BASE_CHAR_INFO> CharList)
{
	FUNCTION_BEGIN;
	LogDebug("[0x%llX]请求登陆，数据库返回%d",
		AccountID,Result);
    CSmartStruct Empty;
	CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),CallerID);
	if(Result==PCCRT_OK)
	{
		char Buff[MAX_PLAYER_CHAR_INFO_SIZE*MAX_ACCOUNT_CHAR+32];
		CSmartStruct CharInfoList(Buff,MAX_PLAYER_CHAR_INFO_SIZE*MAX_ACCOUNT_CHAR+32,true);

		if(PackPlayerCharInfoList(CharInfoList,CharList))
		{
			MsgCaller.GetAccountCharAck(CMRT_OK,AccountID,CharInfoList);
		}
		else
		{
			Log("CPlayerCharDataDBAdapter::OnDBRetCreateChar:打包角色数据失败");
			MsgCaller.GetAccountCharAck(CMRT_FAILED,AccountID,Empty);
		}
	}
	else
	{
		MsgCaller.GetAccountCharAck(Result,AccountID,Empty);
	}
	FUNCTION_END;
}

void CPlayerCharDataDBAdapter::OnDBRetGetCharData(OBJECT_ID CallerID,int Result,const PLAYER_CHAR_INFO& CharInfo)
{
	FUNCTION_BEGIN;
	LogDebug("[0x%llX]请求请求获取角色[0x%llX]的数据，数据库返回%d",
		CharInfo.AccountID,CharInfo.BaseCharInfo.CharID,Result);
    CSmartStruct Empty;
	CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),CallerID);
	if(Result==PCCRT_OK)
	{
		char Buff[MAX_PLAYER_CHAR_INFO_SIZE];
		CSmartStruct CharInfoPacket(Buff,MAX_PLAYER_CHAR_INFO_SIZE,true);

		if(PackPlayerCharInfo(CharInfoPacket,CharInfo))
		{
			MsgCaller.GetCharDataAck(CMRT_OK,CharInfo.AccountID,CharInfo.BaseCharInfo.CharID,CharInfoPacket);
		}
		else
		{
			Log("CPlayerCharDataDBAdapter::OnDBRetGetCharData:打包角色数据失败");
			MsgCaller.GetCharDataAck(CMRT_FAILED,CharInfo.AccountID,CharInfo.BaseCharInfo.CharID,Empty);
		}
	}
	else
	{
		MsgCaller.GetCharDataAck(Result,CharInfo.AccountID,CharInfo.BaseCharInfo.CharID,Empty);
	}
	FUNCTION_END;
}


void CPlayerCharDataDBAdapter::OnDBRetSaveCharData(OBJECT_ID CallerID,int Result,const PLAYER_CHAR_INFO& CharInfo)
{
	FUNCTION_BEGIN;
	LogDebug("[0x%llX]请求保存角色[0x%llX]的数据，数据库返回%d",
		CharInfo.AccountID,CharInfo.BaseCharInfo.CharID,Result);
	CPlayerCharDBAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),CallerID);
	MsgCaller.SaveCharDataAck(Result,CharInfo.AccountID,CharInfo.BaseCharInfo.CharID);
	if(Result==PCCRT_OK)
	{
		
	}
	else
	{
		
	}
	FUNCTION_END;
}