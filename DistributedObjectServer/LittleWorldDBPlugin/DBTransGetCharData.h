#pragma once

class CDBTransGetCharData :
	public CDBTransaction
{
protected:
	OBJECT_ID						m_CallerID;
	PLAYER_CHAR_INFO				m_CharInfo;
	int								m_Result;
public:
	CDBTransGetCharData(OBJECT_ID CallerID,UINT64 AccountID,UINT64 CharID);
	~CDBTransGetCharData(void);

	virtual bool OnExecute(IDBConnection * pConnection);
	virtual void OnFinish();
};
