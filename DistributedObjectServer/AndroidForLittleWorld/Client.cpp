#include "StdAfx.h"


START_DOS_MSG_MAP_CLIENT(CClient)
DOS_MSG_MAP_CLIENT(CPlayerManagerAckMsgHandler)
DOS_MSG_MAP_CLIENT(CPlayerCharOperatorAckMsgHandler)
DOS_MSG_MAP_CLIENT(CCharacterAckMsgHandler)
DOS_MSG_MAP_CLIENT(CMapAreaAckMsgHandler)
END_DOS_MSG_MAP_CLIENT(CClient)

CClient::CClient(void)
{
	INIT_DOS_MSG_MAP;

	m_ID=0;
	m_pGroup=NULL;
	m_pServer=NULL;
	m_pMainControlView=NULL;
	m_pClientInfoView=NULL;
	m_MainControlViewIndex=0;
	m_Status=STATUS_NONE;
	m_AccountID=0;
	m_SelfCharID=0;
	m_QueryMoveStatus=MS_NONE;
	m_IsInSleep=false;
}

CClient::~CClient(void)
{
	if(m_pClientInfoView)
	{
		m_pClientInfoView->SetClient(NULL);
		m_pClientInfoView=NULL;
	}
	ClearCharList();
}

bool CClient::Init(CClientGroup * pGroup,CNetServer * pServer,CMainControlView * pMainControlView,CLIENT_CONFIG& ClientConfig)
{
	m_pGroup=pGroup;
	m_pServer=pServer;
	m_pMainControlView=pMainControlView;
	m_ClientConfig=ClientConfig;

	m_pMainControlView->AddClient(this);

	if(!m_VarList.Create(128))
		return false;
	if(!m_FunList.Create(128))
		return false;

	CESFunctionLib::GetInstance()->AddFunction(&m_FunList);

	m_FunList.AddCFunction("Login",2,this,&CClient::SFLogin);
	m_FunList.AddCFunction("GetCharList",0,this,&CClient::SFGetCharList);
	m_FunList.AddCFunction("CreateChar",3,this,&CClient::SFCreateChar);
	m_FunList.AddCFunction("DeleteChar",1,this,&CClient::SFDeleteChar);
	m_FunList.AddCFunction("SelectChar",1,this,&CClient::SFSelectChar);
	m_FunList.AddCFunction("Fly",2,this,&CClient::SFFly);
	m_FunList.AddCFunction("Move",2,this,&CClient::SFMove);
	m_FunList.AddCFunction("GetCurPosX",0,this,&CClient::SFGetCurPosX);
	m_FunList.AddCFunction("GetCurPosY",0,this,&CClient::SFGetCurPosY);
	m_FunList.AddCFunction("Sleep",1,this,&CClient::SFSleep);

	if(!m_AroundCharList.Create(MAX_CHAR_LIST_COUNT))
		return false;

	if(!m_CommandQueue.Create(EVENT_QUEUE_SIZE))
		return false;

	m_ESThreadPool.Resize(SCRITP_THREAD_COUNT);
	for(UINT i=0;i<m_ESThreadPool.GetCount();i++)
	{
		m_ESThreadPool[i].SetVariableList(&m_VarList);
		m_ESThreadPool[i].SetFunctionList(&m_FunList);
		m_ESThreadPool[i].SetScript(&m_Script);
	}
	SetServer(pServer);
	return true;
}

void CClient::StartRun()
{
	CWinFileAccessor ScriptFile;

	if(ScriptFile.Open(m_ClientConfig.ScriptFileName,IFileAccessor::modeOpen|IFileAccessor::modeRead|IFileAccessor::shareShareAll))
	{
		UINT Size=ScriptFile.GetSize();
		char * pScriptText=new char[Size+1];
		ScriptFile.Read(pScriptText,Size);
		ScriptFile.Close();
		pScriptText[Size]=0;
		int LastLine=0;
		int RetCode=m_Script.PushScript(pScriptText,&m_VarList,&m_FunList,LastLine);
		delete[] pScriptText;
		if(RetCode==0)
		{			
			SetStatus(STATUS_DISCONNECT);
			m_StatusTimer.SetTimeOut(m_ClientConfig.FirstConnectDelay);
			
		}
		else
		{
			PrintSystemLog("编译脚本失败在行[%d]:%s",
				LastLine,ESGetErrorMsg(RetCode));
		}
		m_CharMoveStatusUpdateTimer.SaveTime();

	}
	else
	{
		PrintSystemLog("无法打开脚本文件[%s]",m_ClientConfig.ScriptFileName);
	}
}

void CClient::OnConnection(BOOL IsSucceed)
{
	if(IsSucceed)
	{
		PrintLog("开始连接服务器%s:%u成功",
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());
		SetStatus(STATUS_LOGIN);
		DoLogin();

	}
	else
	{
		PrintLog("开始连接服务器%s:%u失败",
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());
		SetStatus(STATUS_DISCONNECT);
		m_StatusTimer.SetTimeOut(m_ClientConfig.ReconnectDelay);
	}
}
void CClient::OnDisconnection()
{
	PrintLog("与服务器%s:%u的连接断开",
		GetRemoteAddress().GetIPString(),
		GetRemoteAddress().GetPort());
	SetStatus(STATUS_DISCONNECT);
	for(UINT i=0;i<m_ESThreadPool.GetCount();i++)
	{
		m_ESThreadPool[i].Reset();
	}
	m_StatusTimer.SetTimeOut(m_ClientConfig.ReconnectDelay);
}
int CClient::Update(int ProcessPacketLimit)
{
	int ProcessCount=CDOSClient::Update(ProcessPacketLimit);

	UINT CurTime=CEasyTimer::GetTime();
	switch(m_Status)
	{
	case STATUS_DISCONNECT:
		if(m_StatusTimer.IsTimeOut())
		{
			PrintLog("开始连接服务器%s:%u",
				m_ClientConfig.ServerAddress.GetIPString(),
				m_ClientConfig.ServerAddress.GetPort());
			SetStatus(STATUS_CONNECTING);
			Start(MAX_MSG_PACKET_SIZE,m_ClientConfig.ServerAddress,CONNECT_TIMEOUT);
		}
		break;
	case STATUS_CONNECTING:
		break;
	case STATUS_LOGIN:
	case STATUS_GAMING:
		break;
	}

	if(m_CharMoveStatusUpdateTimer.IsTimeOut(CHAR_MOVE_STATUS_UPDATE_TIME))
	{
		m_CharMoveStatusUpdateTimer.SaveTime();

		
		MAP_CHAR_INFO * pCharInfo=m_pGroup->GetCharInfo(m_SelfCharID);
		if(pCharInfo)
		{
			if(m_QueryMoveStatus>MS_STOP&&pCharInfo->MoveStatus.GetMoveStatus()<=MS_STOP)
			{
				CCharacterMsgCaller MsgCaller(this,0,0);

				m_QueryMoveStatus=MS_NONE;
				MsgCaller.QueryMove(MS_STOP,
					pCharInfo->MoveStatus.GetPosX(),pCharInfo->MoveStatus.GetPosY(),0,
					pCharInfo->MoveStatus.GetPosX(),pCharInfo->MoveStatus.GetPosY(),0,
					pCharInfo->MoveStatus.GetRotateStatus(),pCharInfo->MoveStatus.GetDirection());
				ContinueScript(INTERRUPT_TYPE_MOVE,1);
			}
		}
	}

	int Limit=ProcessPacketLimit;
	CEasyString Command;
	while(m_CommandQueue.PopFront(Command))
	{
		m_CommandScript.Clear();
		int LastLine=0;
		int RetCode=m_CommandScript.PushScript(Command,&m_VarList,&m_FunList,LastLine);		
		if(RetCode==0)
		{			
			CESThread * pESThread=AllocEmptyCommandESThread();
			if(pESThread)
			{		
				RetCode=m_ScriptExecutor.ExecScript(*pESThread);
				if(RetCode)
				{
					PrintLog("执行命令脚本出错在行[%u]:%s",
						pESThread->GetLastLine(),ESGetErrorMsg(pESThread->GetResultCode()));
				}
				else
				{
					PrintLog("执行命令脚本结果:%s",
						BolanToString(pESThread->GetResult()));
				}
			}
		}
		else
		{
			PrintSystemLog("编译命令脚本失败在行[%d]:%s",
				LastLine,ESGetErrorMsg(RetCode));
		}

		ProcessCount++;
		Limit--;
		if(Limit<=0)
			break;
	}

	if(m_IsInSleep&&m_SleepTimer.IsTimeOut())
	{
		m_IsInSleep=false;
		ContinueScript(INTERRUPT_TYPE_SLEEP,0);
	}

	for(UINT i=0;i<m_ESThreadPool.GetCount();i++)
	{
		if(m_ESThreadPool[i].IsInInterrupt()&&m_ESThreadPool[i].IsInterruptTimeOut(SCRIPT_INT_TIMEOUT))
		{
			PrintSystemLog("线程%d的中断%d已超时",
				i,m_ESThreadPool[i].GetInterruptCode());
			m_ESThreadPool[i].SetResult(0);
			int RetCode=m_ScriptExecutor.ContinueScript(m_ESThreadPool[i]);
			if(RetCode)
			{
				PrintLog("执行中断返回出错在行[%u]:%s",
					m_ESThreadPool[i].GetLastLine(),ESGetErrorMsg(m_ESThreadPool[i].GetResultCode()));
			}
			else
			{
				PrintLog("执行中断返回结果:%s",
					BolanToString(m_ESThreadPool[i].GetResult()));
			}
		}
	}

	return ProcessCount;
}

void CClient::PrintLog(LPCTSTR szFormat,...)
{
	if(m_pClientInfoView)
	{
		char szMsg[2048];
		va_list	vl;
		va_start(vl,szFormat);
		CEasyTime CurTime;
		CurTime.FetchLocalTime();
		sprintf_s(szMsg,2048,"[%02d:%02d:%02d]:",CurTime.Hour(),CurTime.Minute(),CurTime.Second());
		vsnprintf_s(szMsg+11,2000,2048,szFormat,vl);
		strcat_s(szMsg,2048,"\r\n");
		m_pClientInfoView->PushLog(szMsg);
		va_end( vl);
	}
}

void CClient::PrintSystemLog(LPCTSTR szFormat,...)
{
	if(m_pMainControlView)
	{
		char szMsg[2048];
		va_list	vl;
		va_start(vl,szFormat);
		CEasyTime CurTime;
		CurTime.FetchLocalTime();
		sprintf_s(szMsg,2048,"[%02d:%02d:%02d][%s]:",
			CurTime.Hour(),CurTime.Minute(),CurTime.Second(),
			(LPCTSTR)m_ClientConfig.AccountName);
		int HeadLen=strlen(szMsg);
		vsnprintf_s(szMsg+HeadLen,2000-HeadLen,2048,szFormat,vl);
		strcat_s(szMsg,2048,"\r\n");
		m_pMainControlView->PushLog(szMsg);
		va_end( vl);
	}
}

void CClient::SetStatus(int Status)
{
	m_Status=Status;
	if(m_pMainControlView)
	{
		m_pMainControlView->PushEvent(m_MainControlViewIndex,m_Status);
	}
}

void CClient::PushCommand(LPCTSTR szCommand)
{
	m_CommandQueue.PushBack(szCommand);
}

CESThread * CClient::AllocEmptyESThread()
{
	for(UINT i=0;i<m_ESThreadPool.GetCount();i++)
	{
		if(!m_ESThreadPool[i].IsInInterrupt())
		{
			m_ESThreadPool[i].SetScript(&m_Script);
			return m_ESThreadPool.GetObject(i);
		}
	}

	PrintSystemLog("无空余脚本线程可用");
	return NULL;
}

CESThread * CClient::AllocEmptyCommandESThread()
{
	for(UINT i=0;i<m_ESThreadPool.GetCount();i++)
	{
		if(!m_ESThreadPool[i].IsInInterrupt())
		{
			m_ESThreadPool[i].SetScript(&m_CommandScript);
			return m_ESThreadPool.GetObject(i);
		}
	}

	PrintSystemLog("无空余脚本线程可用");
	return NULL;
}

bool CClient::CallScriptFunction(CESThread * pESThread,LPCTSTR szFunctionName)
{
	int RetCode=m_ScriptExecutor.CallFunction(*pESThread,szFunctionName);
	if(RetCode)
	{
		PrintLog("执行脚本函数[%s]出错在行[%u]:%s",
			szFunctionName,pESThread->GetLastLine(),ESGetErrorMsg(pESThread->GetResultCode()));
	}
	else
	{
		PrintLog("执行脚本函数[%s]结果:%s",
			szFunctionName,BolanToString(pESThread->GetResult()));
	}
	return true;
}

bool CClient::ContinueScript(int Type,const ES_BOLAN& Result)
{
	for(UINT i=0;i<m_ESThreadPool.GetCount();i++)
	{
		if(m_ESThreadPool[i].IsInInterrupt()&&m_ESThreadPool[i].GetInterruptCode()==Type)
		{
			m_ESThreadPool[i].SetResult(Result);
			int RetCode=m_ScriptExecutor.ContinueScript(m_ESThreadPool[i]);
			if(RetCode)
			{
				PrintLog("执行中断返回出错在行[%u]:%s",
					m_ESThreadPool[i].GetLastLine(),ESGetErrorMsg(m_ESThreadPool[i].GetResultCode()));
			}
			else
			{
				PrintLog("执行中断返回结果:%s",
					BolanToString(m_ESThreadPool[i].GetResult()));
			}
			return true;
		}
	}
	PrintSystemLog("无法找到中断%d对应的线程",Type);
	return false;
}

bool CClient::DoLogin()
{
	CESThread * pESThread=AllocEmptyESThread();
	if(pESThread)
	{		
		pESThread->ClearStack();
		pESThread->PushValueToStack((LPCTSTR)m_ClientConfig.AccountName);
		pESThread->PushValueToStack((LPCTSTR)m_ClientConfig.Password);

		CallScriptFunction(pESThread,"OnLogin");
	}
	return false;
}

void CClient::ClearCharList()
{
	for(UINT i=0;i<m_CharInfoList.GetCount();i++)
	{
		SAFE_DELETE(m_CharInfoList[i]);
	}

	m_CharInfoList.Clear();
}

MAP_CHAR_INFO * CClient::AddChar(UINT64 CharID,CSmartStruct& MoveStatus)
{
	if(m_AroundCharList.Find(CharID))
	{
		return m_pGroup->UpdateCharMoveStatus(CharID,MoveStatus);
	}
	else
	{
		if(!m_AroundCharList.Insert(CharID,CharID))
		{
			PrintLog("添加角色信息失败");
		}
		return m_pGroup->AddChar(CharID,MoveStatus);
	}
}

bool CClient::DeleteChar(UINT64 CharID)
{
	if(m_AroundCharList.Find(CharID))
	{
		m_AroundCharList.Delete(CharID);
		return m_pGroup->DeleteChar(CharID);
	}
	return false;
}

MAP_CHAR_INFO * CClient::GetChar(UINT64 CharID)
{
	if(m_AroundCharList.Find(CharID))
	{
		return m_pGroup->GetCharInfo(CharID);
	}
	return NULL;
}

LPVOID CClient::GetFirstCharPos()
{
	return m_AroundCharList.GetFirstObjectPos();
}

MAP_CHAR_INFO * CClient::GetNextChar(LPVOID& Pos)
{
	UINT64 Key;
	if(m_AroundCharList.GetNextObject(Pos,Key))
	{
		return m_pGroup->GetCharInfo(Key);
	}
	return NULL;
}

int CClient::PlayerLoginAck(int Result ,UINT64 AccountID )
{
	PrintLog("请求登陆，结果%d,AccountID=%llX",Result,AccountID);
	ContinueScript(INTERRUPT_TYPE_LOGIN,Result);
	return COMMON_RESULT_SUCCEED;
}

int CClient::GetCharListAck(int Result ,CSmartStruct& CharData )
{
	PrintLog("请求获取角色列表，结果%d",Result);
	ClearCharList();
	void * Pos=CharData.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct CharInfo=CharData.GetNextMember(Pos,MemberID);
		if(MemberID==SST_CIL_CHAR_INFO)
		{
			CBaseCharInfo * pCharInfo=new CBaseCharInfo();
			if(pCharInfo->ParsePacket(CharInfo,MF_BASE_CHAR_INFO))
			{
				PrintLog("角色信息：ID=0x%llX,Name=%s,Race=%d,Gender=%d",
					pCharInfo->GetCharID(),pCharInfo->GetCharName(),pCharInfo->GetRace(),pCharInfo->GetGender());

				m_CharInfoList.Add(pCharInfo);
			}
			else
			{
				SAFE_DELETE(pCharInfo);
			}
		}
	}

	ContinueScript(INTERRUPT_TYPE_GET_CHAR_LIST,m_CharInfoList.GetCount());
	return COMMON_RESULT_SUCCEED;
}

int CClient::CreateCharAck(int Result ,CSmartStruct& CharData )
{
	PrintLog("请求创建角色，结果%d",Result);

	if(Result==PCCRT_OK)
	{
		CBaseCharInfo * pCharInfo=new CBaseCharInfo();
		if(pCharInfo->ParsePacket(CharData,MF_BASE_CHAR_INFO))
		{
			m_CharInfoList.Add(pCharInfo);
		}
		else
		{
			SAFE_DELETE(pCharInfo);
		}		
	}
	else if(Result==PCCRT_CHAR_NAME_EXIST)
	{
		PrintLog("角色名已存在");
	}
	else
	{
		PrintLog("创建角色失败",Result);
	}

	ContinueScript(INTERRUPT_TYPE_CREATE_CHAR,Result);
	return COMMON_RESULT_SUCCEED;
}

int CClient::DeleteCharAck(int Result ,UINT64 CharID )
{
	PrintLog("请求删除角色，结果%d",Result);
	if(Result==CMRT_OK)
	{
		for(UINT i=0;i<m_CharInfoList.GetCount();i++)
		{
			if(m_CharInfoList[i]->GetCharID()==CharID)
			{
				SAFE_DELETE(m_CharInfoList[i]);
				m_CharInfoList.Delete(i);
				break;
			}
		}
	}

	ContinueScript(INTERRUPT_TYPE_CREATE_CHAR,Result);
	return COMMON_RESULT_SUCCEED;
}

int CClient::SelectCharAck(int Result ,UINT64 CharID ,CSmartStruct& CharData )
{
	PrintLog("请求选择角色，结果%d",Result);
	ContinueScript(INTERRUPT_TYPE_SELECT_CHAR,Result);


	if(Result==CMRT_OK)
	{
		MAP_CHAR_INFO * pCharInfo=AddChar(CharID,CharData);
		if(pCharInfo)
		{
			m_SelfCharID=CharID;

			pCharInfo->BaseCharInfo.ParsePacket(CharData,MF_BASE_CHAR_INFO);

			CCharacterMsgCaller MsgCaller(this,0,0);

			MsgCaller.MapChangeConfirm(CMRT_OK);

			CESThread * pESThread=AllocEmptyESThread();
			if(pESThread)
			{		
				pESThread->ClearStack();
				pESThread->PushValueToStack((LPCTSTR)m_ClientConfig.AccountName);
				pESThread->PushValueToStack((LPCTSTR)m_ClientConfig.Password);

				CallScriptFunction(pESThread,"OnGame");
			}


		}
	}


	return COMMON_RESULT_SUCCEED;
}

int CClient::QueryCharInfoAck(int Result ,CSmartStruct& CharData )
{
	if(Result==CMRT_OK)
	{
		UINT64 CharID=CharData.GetMember(SST_BCI_CHAR_ID);
		m_pGroup->UpdateCharInfo(CharID,CharData);
	}
	
	return COMMON_RESULT_SUCCEED;
}
int CClient::QueryMoveAck(int Result )
{
	return COMMON_RESULT_SUCCEED;
}
int CClient::QueryFlyAck(int Result )
{
	PrintLog("请求传送，结果%d",Result);
	ContinueScript(INTERRUPT_TYPE_FLY,Result);
	return COMMON_RESULT_SUCCEED;
}
int CClient::MapChange(UINT MapID ,float PosX ,float PosY ,float Direction )
{
	PrintLog("场景变换到(%u,%g,%g,%g)",
		MapID,PosX,PosY,Direction);
	CCharacterMsgCaller MsgCaller(this,0,0);

	MsgCaller.MapChangeConfirm(CMRT_OK);
	return COMMON_RESULT_SUCCEED;
}
int CClient::QueryJumpAck(UINT64 ObjectID ,int Result )
{
	return COMMON_RESULT_SUCCEED;
}
int CClient::NewMapObject(UINT64 ObjectID ,CSmartStruct& MoveStatus )
{
	PrintLog("角色%llu出现",ObjectID);
	MAP_CHAR_INFO * pCharInfo=AddChar(ObjectID,MoveStatus);
	if(pCharInfo)
	{
		CCharQueryCenterMsgCaller MsgCaller(this,0,0);
		MsgCaller.QueryCharInfo(ObjectID);
	}
	else
	{
		PrintSystemLog("无法新建角色");
	}
	return COMMON_RESULT_SUCCEED;
}
int CClient::DeleteMapObject(UINT64 ObjectID )
{
	//PrintLog("角色%llu消失",ObjectID);
	DeleteChar(ObjectID);
	return COMMON_RESULT_SUCCEED;
}
int CClient::MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& MoveStatus )
{
	MAP_CHAR_INFO * pCharInfo=m_pGroup->UpdateCharMoveStatus(ObjectID,MoveStatus);
	if(pCharInfo&&ObjectID==m_SelfCharID)
	{
		m_QueryMoveStatus=pCharInfo->MoveStatus.GetMoveStatus();
	}
	return COMMON_RESULT_SUCCEED;
}
int CClient::EventMsgPacket(CSmartStruct& EventMsg )
{
	void * Pos=EventMsg.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct Value=EventMsg.GetNextMember(Pos,MemberID);
		if(MemberID==SST_GDMP_MSG)
		{
			UINT MsgID=Value.GetMember(SST_GDM_MSG_ID);
			CSmartStruct MsgPacket=Value.GetMember(SST_GDM_MSG_DATA);

			HandleMsg(MsgID,MsgPacket);
		}
	}
	return COMMON_RESULT_SUCCEED;
}

BOOL CClient::SendMessagePacket(CDOSMessagePacket * pPacket)
{
	if(m_pMainControlView)
		m_pMainControlView->AddSendBytes(pPacket->GetPacketLength());
	return CDOSClient::SendMessagePacket(pPacket);
}
void CClient::OnRecvData(const BYTE * pData, UINT DataSize)
{
	if(m_pMainControlView)
		m_pMainControlView->AddRecvBytes(DataSize);
	CDOSClient::OnRecvData(pData, DataSize);
}

int CClient::SFLogin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	CPlayerManagerMsgCaller MsgCaller(this,0,0);

	PrintLog("以[%s][%s]进行登陆",
		(LPCTSTR)pParams[0],
		(LPCTSTR)pParams[1]);

	MsgCaller.PlayerLogin(pParams[0],pParams[1]);
	return INTERRUPT_TYPE_LOGIN;
}

int CClient::SFGetCharList(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	CPlayerCharOperatorMsgCaller MsgCaller(this,0,0);

	MsgCaller.GetCharList();

	return INTERRUPT_TYPE_GET_CHAR_LIST;
}
int CClient::SFCreateChar(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	CPlayerCharOperatorMsgCaller MsgCaller(this,0,0);
	CSmartStruct CreateInfo(MAX_PLAYER_CHAR_INFO_SIZE);

	CreateInfo.AddMember(SST_BCI_CHAR_NAME,(LPCTSTR)pParams[0]);
	CreateInfo.AddMember(SST_BCI_RACE,(BYTE)pParams[1]);
	CreateInfo.AddMember(SST_BCI_GENDER,(BYTE)pParams[2]);

	MsgCaller.CreateChar(CreateInfo);

	return INTERRUPT_TYPE_CREATE_CHAR;
}
int CClient::SFDeleteChar(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	CPlayerCharOperatorMsgCaller MsgCaller(this,0,0);

	UINT Index=pParams[0];
	if(Index<m_CharInfoList.GetCount())
	{
		MsgCaller.DeleteChar(m_CharInfoList[Index]->GetCharID());
		return INTERRUPT_TYPE_DELETE_CHAR;
	}
	else
	{
		*pResult=-1;
	}

	return 0;
}
int CClient::SFSelectChar(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	CPlayerCharOperatorMsgCaller MsgCaller(this,0,0);

	UINT Index=pParams[0];
	if(Index<m_CharInfoList.GetCount())
	{
		MsgCaller.SelectChar(m_CharInfoList[Index]->GetCharID());
		return INTERRUPT_TYPE_SELECT_CHAR;
	}
	else
	{
		*pResult=-1;
	}

	return 0;
}

int CClient::SFFly(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	float PosX=pParams[0];
	float PosY=pParams[1];
	PrintLog("请求瞬移到(%g,%g)",
		PosX,PosY);

	CCharacterMsgCaller MsgCaller(this,0,0);

	MsgCaller.QueryFly(0,PosX,PosY,0,0);

	return INTERRUPT_TYPE_FLY;
}
int CClient::SFMove(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	MAP_CHAR_INFO * pCharInfo=m_pGroup->GetCharInfo(m_SelfCharID);
	if(pCharInfo)
	{
		float PosX=pParams[0];
		float PosY=pParams[1];
		PrintLog("请求从(%g,%g)移动到(%g,%g)",
			pCharInfo->MoveStatus.GetPosX(),pCharInfo->MoveStatus.GetPosY(),
			PosX,PosY);
		CCharacterMsgCaller MsgCaller(this,0,0);

		float DX=PosX-pCharInfo->MoveStatus.GetPosX();
		float DY=PosY-pCharInfo->MoveStatus.GetPosY();
		float Dis=sqrt(DX*DX+DY*DY);
		if(Dis>0)
		{
			float Dir=acos(DX/Dis);
			if(DY<0)
			{
				Dir=TWO_PI-Dir;
			}			
			MsgCaller.QueryMove(MS_FORWARD,pCharInfo->MoveStatus.GetPosX(),pCharInfo->MoveStatus.GetPosY(),0,PosX,PosY,0,RS_STOP,Dir);
			return INTERRUPT_TYPE_MOVE;
		}
	}

	return 0;
}

int CClient::SFGetCurPosX(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	MAP_CHAR_INFO * pCharInfo=m_pGroup->GetCharInfo(m_SelfCharID);
	if(pCharInfo)
	{
		*pResult=pCharInfo->MoveStatus.GetPosX();
	}

	return 0;
}
int CClient::SFGetCurPosY(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	MAP_CHAR_INFO * pCharInfo=m_pGroup->GetCharInfo(m_SelfCharID);
	if(pCharInfo)
	{
		*pResult=pCharInfo->MoveStatus.GetPosY();
	}

	return 0;
}

int CClient::SFSleep(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	m_IsInSleep=true;
	m_SleepTimer.SetTimeOut(pParams[0]);
	return INTERRUPT_TYPE_SLEEP;
}