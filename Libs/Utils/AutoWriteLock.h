/****************************************************************************/
/*                                                                          */
/*      文件名:    AutoWriteLock.h                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CAutoWriteLock
{
protected:
	CEasyReadWriteLock * m_pLock;
public:
	CAutoWriteLock()
	{
		m_pLock=NULL;
	}
	CAutoWriteLock(CEasyReadWriteLock& ReadWriteLock)
	{
		m_pLock=&ReadWriteLock;
		m_pLock->LockWrite();
	}
	void Lock(CEasyReadWriteLock& ReadWriteLock)
	{
		m_pLock=&ReadWriteLock;
		m_pLock->LockWrite();
	}
	BOOL TryLock(CEasyReadWriteLock& ReadWriteLock)
	{
		if(ReadWriteLock.TryLockWrite())
		{
			m_pLock=&ReadWriteLock;
			return TRUE;
		}
		return FALSE;
	}
	~CAutoWriteLock()
	{
		if(m_pLock)
			m_pLock->Unlock();
	}
};