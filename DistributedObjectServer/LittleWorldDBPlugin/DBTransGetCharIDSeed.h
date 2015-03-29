#pragma once

class CDBTransGetCharIDSeed :
	public CDBTransaction
{
protected:
	UINT64		m_CharIDSeed;
public:
	CDBTransGetCharIDSeed(void);
	~CDBTransGetCharIDSeed(void);

	virtual bool OnExecute(IDBConnection * pConnection);
	virtual void OnFinish();
};
