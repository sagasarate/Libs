/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyReadWriteLockLinux.h                                 */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CEasyReadWriteLock
{
protected:
	pthread_rwlock_t		m_Lock;
public:
	CEasyReadWriteLock()
	{		
		pthread_rwlock_init(&m_Lock,NULL);
	}
	~CEasyReadWriteLock()
	{
		pthread_rwlock_destroy(&m_Lock);
	}
	void LockRead()
	{
		pthread_rwlock_rdlock(&m_Lock);
	}
	BOOL TryLockRead()
	{
		if(pthread_rwlock_tryrdlock(&m_Lock)==0)
			return TRUE;
		else
			return FALSE;
	}	
	void LockWrite()
	{
		pthread_rwlock_wrlock(&m_Lock);
	}
	BOOL TryLockWrite()
	{
		if(pthread_rwlock_trywrlock(&m_Lock)==0)
			return TRUE;
		else
			return FALSE;
	}
	void Unlock()
	{
		pthread_rwlock_unlock(&m_Lock);
	}
};