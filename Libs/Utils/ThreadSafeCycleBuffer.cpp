/****************************************************************************/
/*                                                                          */
/*      文件名:    ThreadSafeCycleBuffer.cpp                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CThreadSafeCycleBuffer,CNameObject);

CThreadSafeCycleBuffer::CThreadSafeCycleBuffer(void):CNameObject()
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	m_IsLockFront=true;
	m_IsLockBack=true;
}

CThreadSafeCycleBuffer::CThreadSafeCycleBuffer(UINT Size):CNameObject()
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	m_IsLockFront=true;
	m_IsLockBack=true;
	Create(Size);
}

CThreadSafeCycleBuffer::CThreadSafeCycleBuffer(LPVOID pBuff,UINT Size):CNameObject()
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	m_IsLockFront=true;
	m_IsLockBack=true;
	Create(pBuff,Size);
}

CThreadSafeCycleBuffer::~CThreadSafeCycleBuffer(void)
{
	Destory();
}

BOOL CThreadSafeCycleBuffer::Create(UINT Size)
{
	Destory();
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
	m_pBuffer=new BYTE[Size];
	m_BufferSize=Size;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	m_IsLockFront=true;
	m_IsLockBack=true;
	return TRUE;
}

BOOL CThreadSafeCycleBuffer::Create(LPVOID pBuff,UINT Size)
{
	Destory();
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
	m_pBuffer=(BYTE *)pBuff;
	m_BufferSize=Size;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=false;
	m_IsLockFront=true;
	m_IsLockBack=true;
	return TRUE;
}

void CThreadSafeCycleBuffer::Destory()
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
	if(m_IsSelfBuffer)
		SAFE_DELETE_ARRAY(m_pBuffer);
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	m_IsLockFront=true;
	m_IsLockBack=true;
}
