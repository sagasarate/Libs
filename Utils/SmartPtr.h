/****************************************************************************/
/*                                                                          */
/*      文件名:    SmartPtr.h                                               */
/*      创建日期:  2009年07月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

template < class T>
class CSmartPtr
{
protected:
	T * m_Pointer;
public:
	CSmartPtr()
	{
		m_Pointer=NULL;
	}
	CSmartPtr(const T * Pointer)
	{
		m_Pointer=(T *)Pointer;
		//m_Pointer->AddUseRef();
	}
	CSmartPtr(const CSmartPtr& Pointer)
	{
		m_Pointer = Pointer.m_Pointer;
		m_Pointer->AddUseRef();
	}
	~CSmartPtr()
	{
		SAFE_RELEASE(m_Pointer);
	}
	operator T*() const
	{
		return m_Pointer;
	}
	T& operator *() const
	{
		return *m_Pointer;
	}
	T** operator &() const
	{
		return &m_Pointer;
	}
	T* operator->() const
	{		
		return m_Pointer;
	}	
	bool operator==(const T* Pointer) const
	{
		return m_Pointer == Pointer;
	}
	bool operator!=(const T* Pointer) const
	{
		return m_Pointer != Pointer;
	}
	const CSmartPtr& operator=(const T* Pointer)
	{
		SAFE_RELEASE(m_Pointer);
		m_Pointer=(T *)Pointer;	
		//m_Pointer->AddUseRef();
		return *this;
	}
	const CSmartPtr& operator=(const CSmartPtr& Pointer)
	{
		SAFE_RELEASE(m_Pointer);
		m_Pointer = Pointer.m_Pointer;
		m_Pointer->AddUseRef();
		return *this;
	}
	void Detach()
	{
		m_Pointer=NULL;
	}
	
};

