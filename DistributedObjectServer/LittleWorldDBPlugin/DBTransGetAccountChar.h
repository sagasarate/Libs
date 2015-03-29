#pragma once

class CDBTransGetAccountChar :
	public CDBTransaction
{
protected:
	OBJECT_ID						m_CallerID;
	UINT64							m_AccountID;
	CEasyArray<BASE_CHAR_INFO>		m_CharList;
	int								m_Result;
public:
	CDBTransGetAccountChar(OBJECT_ID CallerID,UINT64 AccountID);
	~CDBTransGetAccountChar(void);

	virtual bool OnExecute(IDBConnection * pConnection);
	virtual void OnFinish();
};
