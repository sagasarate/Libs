#pragma once

class CClientGroup;
class CMainControlView;
class CClientInfoView;

class CClient :
	public CDOSClient,
	public CPlayerManagerAckMsgHandler,
	public CPlayerCharOperatorAckMsgHandler,
	public CCharacterAckMsgHandler,
	public CMapAreaAckMsgHandler
{
public:
	enum STATUS
	{
		STATUS_NONE,
		STATUS_DISCONNECT,
		STATUS_CONNECTING,
		STATUS_LOGIN,
		STATUS_GAMING,
	};
protected:
	enum INTERRUPT_TYPE
	{
		INTERRUPT_TYPE_SLEEP=1,
		INTERRUPT_TYPE_LOGIN,		
		INTERRUPT_TYPE_GET_CHAR_LIST,
		INTERRUPT_TYPE_CREATE_CHAR,
		INTERRUPT_TYPE_DELETE_CHAR,
		INTERRUPT_TYPE_SELECT_CHAR,
		INTERRUPT_TYPE_MOVE,
		INTERRUPT_TYPE_FLY,
	};
	
	UINT											m_ID;
	int												m_Status;
	CEasyTimer										m_StatusTimer;
	CClientGroup *									m_pGroup;
	CNetServer *									m_pServer;
	CMainControlView *								m_pMainControlView;
	CClientInfoView *								m_pClientInfoView;
	UINT											m_MainControlViewIndex;
	CLIENT_CONFIG									m_ClientConfig;

	CESVariableList									m_VarList;
	CESFunctionList									m_FunList;
	CESBolanStack									m_Script;
	CESBolanStack									m_CommandScript;
	CEasyScriptExecutor								m_ScriptExecutor;
	CEasyArray<CESThread>							m_ESThreadPool;

	CEasyTimer										m_SleepTimer;
	bool											m_IsInSleep;

	CThreadSafeIDStorage<CEasyString>				m_CommandQueue;

	UINT64											m_AccountID;
	CEasyArray<CBaseCharInfo *>						m_CharInfoList;
	UINT64											m_SelfCharID;


	CThreadSafeStaticMap<UINT64,UINT64>				m_AroundCharList;
	CEasyTimer										m_CharMoveStatusUpdateTimer;
	BYTE											m_QueryMoveStatus;



	DECLARE_DOS_MSG_MAP_CLIENT(CClient)
public:
	CClient(void);
	~CClient(void);

	void SetID(UINT ID)
	{
		m_ID=ID;
	}
	UINT GetID()
	{
		return m_ID;
	}

	bool Init(CClientGroup * pGroup,CNetServer * pServer,CMainControlView * pMainControlView,CLIENT_CONFIG& ClientConfig);


	void StartRun();
	virtual void OnConnection(BOOL IsSucceed);
	virtual void OnDisconnection();
	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	


	void SetMainControlViewIndex(UINT Index)
	{
		m_MainControlViewIndex=Index;
	}
	LPCTSTR GetClientName()
	{
		return m_ClientConfig.AccountName;
	}
	CClientGroup * GetClientGroup()
	{
		return m_pGroup;
	}
	void SetClientInfoView(CClientInfoView * pClientInfoView)
	{
		m_pClientInfoView=pClientInfoView;
	}

	void PrintLog(LPCTSTR szFormat,...);
	void PrintSystemLog(LPCTSTR szFormat,...);

	void SetStatus(int Status);

	UINT64 GetSelfCharID()
	{
		return m_SelfCharID;
	}	

	void PushCommand(LPCTSTR szCommand);

	MAP_CHAR_INFO * GetChar(UINT64 CharID);
	LPVOID GetFirstCharPos();
	MAP_CHAR_INFO * GetNextChar(LPVOID& Pos);

protected:
	CESThread * AllocEmptyESThread();
	CESThread * AllocEmptyCommandESThread();
	bool CallScriptFunction(CESThread * pESThread,LPCTSTR szFunctionName);
	bool ContinueScript(int Type,const ES_BOLAN& Result);
	bool DoLogin();
	void ClearCharList();

	MAP_CHAR_INFO * AddChar(UINT64 CharID,CSmartStruct& MoveStatus);
	bool DeleteChar(UINT64 CharID);
	

protected:
	virtual int PlayerLoginAck(int Result ,UINT64 AccountID );
	virtual int GetCharListAck(int Result ,CSmartStruct& CharData );
	virtual int CreateCharAck(int Result ,CSmartStruct& CharData );
	virtual int DeleteCharAck(int Result ,UINT64 CharID );
	virtual int SelectCharAck(int Result ,UINT64 CharID ,CSmartStruct& CharData );

	
	


	virtual int QueryCharInfoAck(int Result ,CSmartStruct& CharData );
	virtual int QueryMoveAck(int Result );	
	virtual int QueryFlyAck(int Result );
	virtual int MapChange(UINT MapID ,float PosX ,float PosY ,float Direction );
	virtual int QueryJumpAck(UINT64 ObjectID ,int Result );

	virtual int NewMapObject(UINT64 ObjectID ,CSmartStruct& MoveStatus );
	virtual int DeleteMapObject(UINT64 ObjectID );
	virtual int MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& MoveStatus );
	virtual int EventMsgPacket(CSmartStruct& EventMsg );
	


	virtual BOOL SendMessagePacket(CDOSMessagePacket * pPacket);
	virtual void OnRecvData(const BYTE * pData, UINT DataSize);

protected:
	int SFLogin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFGetCharList(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFCreateChar(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFDeleteChar(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFSelectChar(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFFly(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFMove(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFGetCurPosX(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFGetCurPosY(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int SFSleep(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
};
