#pragma once

class CDBTransAccountLogin :
	public CDBTransaction
{
protected:
	OBJECT_ID		m_CallerID;
	CEasyString		m_AccountName;
	CEasyString		m_Password;
	UINT64			m_AccountID;
public:
	CDBTransAccountLogin(OBJECT_ID CallerID,LPCTSTR szAccountName,LPCTSTR szPassword);
	~CDBTransAccountLogin(void);

	virtual bool OnExecute(IDBConnection * pConnection);
	virtual void OnFinish();
};
