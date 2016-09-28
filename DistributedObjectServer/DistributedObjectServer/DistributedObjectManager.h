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

	BOOL Init(CDOSObjectManager * pDOSObjectManager, const STORAGE_POOL_SETTING& PoolConfig);
	virtual void Destory();
	

	virtual BOOL RegisterObject(DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo);
	BOOL RegisterObject(DOS_OBJECT_REGISTER_INFO_FOR_CS& ObjectRegisterInfo, MONO_DOMAIN_INFO& MonoDomainInfo);

	BOOL UnregisterObject(CDistributedObjectOperator * pObjectOperator);

	CDistributedObjectOperator * CreateObjectOperator(IDistributedObject * pObject,OBJECT_ID ObjectID);
	CDistributedObjectOperator * CreateObjectOperator(MONO_DOMAIN_INFO& MonoDomainInfo, MonoObject * pObject, OBJECT_ID ObjectID);
	BOOL DeleteObjectOperator(CDistributedObjectOperator * pObjectOperator);

	void PrintObjectCount();
	
};

