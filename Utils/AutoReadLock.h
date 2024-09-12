/****************************************************************************/
/*                                                                          */
/*      文件名:    AutoReadLock.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CAutoReadLock
{
protected:
	CEasyReadWriteLock * m_pLock;
public:
	CAutoReadLock()
	{
		m_pLock=NULL;
	}
	CAutoReadLock(CEasyReadWriteLock& ReadWriteLock)
	{
		m_pLock=&ReadWriteLock;
		m_pLock->LockRead();
	}
	void Lock(CEasyReadWriteLock& ReadWriteLock)
	{
		m_pLock=&ReadWriteLock;
		m_pLock->LockRead();
	}
	BOOL TryLock(CEasyReadWriteLock& ReadWriteLock)
	{
		if(ReadWriteLock.TryLockRead())
		{
			m_pLock=&ReadWriteLock;
			return TRUE;
		}
		return FALSE;
	}
	~CAutoReadLock()
	{
		if(m_pLock)
			m_pLock->Unlock();
	}
};
