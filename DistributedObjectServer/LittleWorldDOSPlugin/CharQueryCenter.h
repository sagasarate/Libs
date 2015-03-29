#pragma once

class CCharQueryCenter :
	public IDistributedObject,
	public CCharQueryCenterMsgHandler
{
protected:
	IDistributedObjectOperator *		m_pOperator;
	CStaticMap<UINT64,OBJECT_ID>		m_QueryMap;


	DECLARE_DOS_MSG_MAP(CCharQueryCenter)
public:
	CCharQueryCenter(void);
	~CCharQueryCenter(void);

	virtual BOOL Initialize(IDistributedObjectOperator * pOperator);
	virtual void Destory();

protected:

	virtual int RegisterChar(UINT64 CharID );
	virtual int UnregisterChar(UINT64 CharID );
	virtual int QueryCharInfo(UINT64 CharID );
};
