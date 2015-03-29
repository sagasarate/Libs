#pragma once

class CDistributedObjectManager :
	public IDistributedObjectManager
{
protected:
	CDOSObjectManager			*m_pDOSObjectManager;

	CThreadSafeIDStorage<CDistributedObjectOperator>	m_ObjectPool;
public:
	CDistributedObjectManager(void);
	~CDistributedObjectManager(void);

	BOOL Init(CDOSObjectManager * pDOSObjectManager,UINT MaxObjectCount);
	virtual void Destory();
	

	virtual BOOL RegisterObject(DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo);

	BOOL UnregisterObject(CDistributedObjectOperator * pObjectOperator);

	CDistributedObjectOperator * CreateObjectOperator(IDistributedObject * pObject,OBJECT_ID ObjectID);
	BOOL DeleteObjectOperator(CDistributedObjectOperator * pObjectOperator);

	void PrintObjectCount();
	
};

