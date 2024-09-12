/****************************************************************************/
/*                                                                          */
/*      文件名:    ThreadSafeCycleBufferEx.h                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CThreadSafeCycleBufferEx :
	public CCycleBufferEx
{
protected:
	CEasyCriticalSection		m_FrontLock;
	CEasyCriticalSection		m_BackLock;
	bool						m_IsLockFront;
	bool						m_IsLockBack;

	DECLARE_CLASS_INFO_STATIC(CThreadSafeCycleBufferEx);
public:
	CThreadSafeCycleBufferEx(LPCTSTR Tag = _T("CThreadSafeCycleBufferEx"));
	CThreadSafeCycleBufferEx(UINT Size, UINT SmoothSize, LPCTSTR Tag = _T("CThreadSafeCycleBufferEx"));
	CThreadSafeCycleBufferEx(LPVOID pBuff, UINT Size, UINT SmoothSize, LPCTSTR Tag = _T("CThreadSafeCycleBufferEx"));
	virtual ~CThreadSafeCycleBufferEx(void);

	BOOL Create(UINT Size,UINT SmoothSize);
	BOOL Create(LPVOID pBuff,UINT Size,UINT SmoothSize);
	void Destory();

	void SetLockMode(bool IsLockFront,bool IsLockBack);

	//BOOL PushFront(LPVOID pData,UINT Size);
	//BOOL PushFront(UINT Data,UINT Size);
	BOOL PushBack(LPCVOID pData,UINT Size);
	BOOL PushBack(UINT Data,UINT Size);

	BOOL PopFront(LPVOID pData,UINT Size);
	BOOL PopFront(UINT Data,UINT Size);
	//BOOL PopBack(LPVOID pData,UINT Size);
	//BOOL PopBack(UINT Data,UINT Size);

	void Clear();

};

inline void CThreadSafeCycleBufferEx::SetLockMode(bool IsLockFront,bool IsLockBack)
{
	m_IsLockFront=IsLockFront;
	m_IsLockBack=IsLockBack;
}

inline BOOL CThreadSafeCycleBufferEx::PushBack(LPCVOID pData,UINT Size)
{
	CAutoLockEx Lock;
	if(m_IsLockBack)
	{
		Lock.Lock(m_BackLock);
	}	
	return CCycleBufferEx::PushBack(pData,Size);
}
inline BOOL CThreadSafeCycleBufferEx::PushBack(UINT Data,UINT Size)
{
	return PushBack(&Data,Size);
}

inline BOOL CThreadSafeCycleBufferEx::PopFront(LPVOID pData,UINT Size)
{
	CAutoLockEx Lock;
	if(m_IsLockFront)
	{
		Lock.Lock(m_FrontLock);
	}	
	return CCycleBufferEx::PopFront(pData,Size);
}
inline BOOL CThreadSafeCycleBufferEx::PopFront(UINT Data,UINT Size)
{
	return PopFront(&Data,Size);
}


inline void CThreadSafeCycleBufferEx::Clear()
{
	CAutoLockEx FrontLock;
	if(m_IsLockFront)
	{
		FrontLock.Lock(m_FrontLock);
	}
	CAutoLockEx BackLock;
	if(m_IsLockBack)
	{
		BackLock.Lock(m_BackLock);
	}

	CCycleBufferEx::Clear();
}