#pragma once

class CDBTransSaveCharData :
	public CDBTransaction
{
protected:
	OBJECT_ID						m_CallerID;
	PLAYER_CHAR_INFO				m_CharInfo;
	int								m_Result;
public:
	CDBTransSaveCharData(OBJECT_ID CallerID,PLAYER_CHAR_INFO& CharInfo);
	~CDBTransSaveCharData(void);

	virtual bool OnExecute(IDBConnection * pConnection);
	virtual void OnFinish();
};
