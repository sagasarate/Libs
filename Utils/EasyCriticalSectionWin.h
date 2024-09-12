/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyCriticalSectionWin.h                                 */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CEasyCriticalSection
{
protected:
	CRITICAL_SECTION	m_critical_sec;
public:
	CEasyCriticalSection(void)
	{
		InitializeCriticalSection( &m_critical_sec );
	}
	~CEasyCriticalSection(void)
	{
		DeleteCriticalSection( &m_critical_sec );
	}
	void SetSpinCount(DWORD SpinCount)
	{
		SetCriticalSectionSpinCount(&m_critical_sec,SpinCount);
	}
	void Lock()
	{
		EnterCriticalSection( &m_critical_sec );
	}
	void Unlock()
	{
		LeaveCriticalSection( &m_critical_sec );
	}
#if(_WIN32_WINNT >= 0x0400)
	BOOL TryLock()
	{
		return TryEnterCriticalSection(&m_critical_sec );
	}
#else
	BOOL TryLock()
	{
		Lock();
		return TRUE;
	}
#endif
};
