#pragma once

class CDBTransCreateChar :
	public CDBTransaction
{
protected:
	OBJECT_ID			m_CallerID;
	CHAR_CREATE_INFO	m_CreateInfo;
	PLAYER_CHAR_INFO	m_PlayerCharInfo;
	int					m_Result;
public:
	CDBTransCreateChar(OBJECT_ID CallerID,CHAR_CREATE_INFO& CreateInfo);
	~CDBTransCreateChar(void);

	virtual bool OnExecute(IDBConnection * pConnection);
	virtual void OnFinish();
};
