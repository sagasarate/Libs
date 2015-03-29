#pragma once

class CMainControlView;
class CClientGroup :
	public CEasyThread
{
protected:
	UINT							m_Index;
	CNetServer *					m_pServer;
	CMainControlView *				m_pMainControlView;
	CLIENT_GROUP_CONFIG				m_GroupConfig;
	CIDStorage<CClient>				m_ClientList;

	CThreadSafeStaticMap<UINT64,MAP_CHAR_INFO>		m_AroundCharList;
	CEasyTimer										m_CharMoveStatusUpdateTimer;
public:
	CClientGroup(void);
	~CClientGroup(void);

	void SetIndex(UINT Index)
	{
		m_Index=Index;
	}
	UINT GetIndex()
	{
		return m_Index;
	}


	bool Init(CNetServer * pServer,CMainControlView * pMainControlView,CLIENT_GROUP_CONFIG& GroupConfig);

	virtual BOOL OnStart();	
	virtual BOOL OnRun();
	virtual void OnTerminate();

	CClient * GetClient(UINT ID)
	{
		return m_ClientList.GetObject(ID);
	}

	MAP_CHAR_INFO * AddChar(UINT64 CharID,CSmartStruct& MoveStatus);
	bool DeleteChar(UINT64 CharID);
	MAP_CHAR_INFO * UpdateCharMoveStatus(UINT64 CharID,CSmartStruct& MoveStatus);
	bool UpdateCharInfo(UINT64 CharID,CSmartStruct& CharData);
	MAP_CHAR_INFO * GetCharInfo(UINT64 CharID);
};
