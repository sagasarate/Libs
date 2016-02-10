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
	T* operator=(const T* Pointer)
	{
		SAFE_RELEASE(m_Pointer);
		m_Pointer=(T *)Pointer;		
		return m_Pointer;
	}
	void Detach()
	{
		m_Pointer=NULL;
	}
	
};

template < class T>
class CSmartPtrForDelete
{
protected:
	T * m_Pointer;
public:
	CSmartPtrForDelete()
	{
		m_Pointer=NULL;
	}
	CSmartPtrForDelete(const T * Pointer)
	{
		m_Pointer=(T *)Pointer;
	}
	~CSmartPtrForDelete()
	{
		SAFE_DELETE(m_Pointer);
	}
	operator T*()
	{
		return m_Pointer;
	}
	T& operator *()
	{
		return *m_Pointer;
	}
	T** operator &()
	{
		return &m_Pointer;
	}
	T* operator->()
	{		
		return m_Pointer;
	}	
	bool operator==(const T* Pointer)
	{
		return m_Pointer == Pointer;
	}
	bool operator!=(const T* Pointer)
	{
		return m_Pointer != Pointer;
	}
	T* operator=(const T* Pointer)
	{
		SAFE_DELETE(m_Pointer);
		m_Pointer=(T *)Pointer;		
		return m_Pointer;
	}

};

template < class T>
class CSmartPtrForArray
{
protected:
	T * m_Pointer;
public:
	CSmartPtrForArray()
	{
		m_Pointer=NULL;
	}
	CSmartPtrForArray(const T * Pointer)
	{
		m_Pointer=(T *)Pointer;
	}
	~CSmartPtrForArray()
	{
		SAFE_DELETE_ARRAY(m_Pointer);
	}
	operator T*()
	{
		return m_Pointer;
	}
	T& operator *()
	{
		return *m_Pointer;
	}
	T** operator &()
	{
		return &m_Pointer;
	}
	T* operator->()
	{		
		return m_Pointer;
	}	
	bool operator==(const T* Pointer)
	{
		return m_Pointer == Pointer;
	}
	bool operator!=(const T* Pointer)
	{
		return m_Pointer != Pointer;
	}
	T* operator=(const T* Pointer)
	{
		SAFE_DELETE_ARRAY(m_Pointer);
		m_Pointer=(T *)Pointer;		
		return m_Pointer;
	}
	

};