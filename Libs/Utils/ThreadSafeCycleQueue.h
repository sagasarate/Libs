#pragma once

template < class T >
class CThreadSafeCycleQueue :
	public CCycleQueue<T>
{
protected:
	CEasyCriticalSection		m_FrontLock;
	CEasyCriticalSection		m_BackLock;
	bool						m_IsLockFront;
	bool						m_IsLockBack;
public:
	CThreadSafeCycleQueue()
		:CCycleQueue<T>()
	{
		m_IsLockFront = true;
		m_IsLockBack = true;
	}
	~CThreadSafeCycleQueue()
	{

	}

	bool Create(UINT Size)
	{
		CAutoLockEx FrontLock;
		if (m_IsLockFront)
		{
			FrontLock.Lock(m_FrontLock);
		}
		CAutoLockEx BackLock;
		if (m_IsLockBack)
		{
			BackLock.Lock(m_BackLock);
		}
		return CCycleQueue<T>::Create(Size);
	}
	void Destory()
	{
		CAutoLockEx FrontLock;
		if (m_IsLockFront)
		{
			FrontLock.Lock(m_FrontLock);
		}
		CAutoLockEx BackLock;
		if (m_IsLockBack)
		{
			BackLock.Lock(m_BackLock);
		}
		CCycleQueue<T>::Destory();
	}
	void Clear()
	{
		CAutoLockEx FrontLock;
		if (m_IsLockFront)
		{
			FrontLock.Lock(m_FrontLock);
		}
		CAutoLockEx BackLock;
		if (m_IsLockBack)
		{
			BackLock.Lock(m_BackLock);
		}
		CCycleQueue<T>::Clear();
	}

	void SetLockMode(bool IsLockFront, bool IsLockBack)
	{
		m_IsLockFront = IsLockFront;
		m_IsLockBack = IsLockBack;
	}

	bool PushBack(T * pValue)
	{
		CAutoLockEx BackLock;
		if (m_IsLockBack)
		{
			BackLock.Lock(m_BackLock);
		}
		return CCycleQueue<T>::PushBack(pValue);
	}

	bool PopFront(T * pValue)
	{
		CAutoLockEx FrontLock;
		if (m_IsLockFront)
		{
			FrontLock.Lock(m_FrontLock);
		}
		return CCycleQueue<T>::PopFront(pValue);
	}
};

