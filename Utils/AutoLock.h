/****************************************************************************/
/*                                                                          */
/*      文件名:    AutoLock.h                                               */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CAutoLock	
{
protected:
	CEasyCriticalSection * m_pSection;
public:
	CAutoLock(CEasyCriticalSection& Section)
	{
		m_pSection=&Section;
		m_pSection->Lock();
	}
	~CAutoLock()
	{
		m_pSection->Unlock();
	}
};

class CAutoLockEx
{
protected:
	CEasyCriticalSection * m_pSection;
public:
	CAutoLockEx()
	{
		m_pSection=NULL;
	}
	CAutoLockEx(CEasyCriticalSection& Section)
	{
		m_pSection=&Section;
		m_pSection->Lock();
	}
	void Lock(CEasyCriticalSection& Section)
	{
		m_pSection=&Section;
		m_pSection->Lock();
	}	
	BOOL TryLock(CEasyCriticalSection& Section)
	{
		if(Section.TryLock())
		{
			m_pSection=&Section;
			return TRUE;
		}
		return FALSE;
	}
	~CAutoLockEx()
	{
		if(m_pSection)
			m_pSection->Unlock();
	}
};