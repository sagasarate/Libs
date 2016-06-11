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
	m_BufferSize = Size + 1;
	m_pBuffer = new BYTE[m_BufferSize];
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
	m_BufferHead = 0;
	m_BufferTail = 0;
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

void CThreadSafeCycleBuffer::Clear()
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
	m_BufferHead = 0;
	m_BufferTail = 0;
}


void CThreadSafeCycleBuffer::CloneFrom(const CThreadSafeCycleBuffer& TargetBuffer)
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
	CAutoLockEx TargetFrontLock;
	if (TargetBuffer.m_IsLockFront)
	{
		TargetFrontLock.Lock(*((CEasyCriticalSection *)(&TargetBuffer.m_FrontLock)));
	}
	CAutoLockEx TargetBackLock;
	if (TargetBuffer.m_IsLockBack)
	{
		TargetBackLock.Lock(*((CEasyCriticalSection *)(&TargetBuffer.m_BackLock)));
	}
	m_BufferHead = 0;
	m_BufferTail = 0;
	if (m_IsSelfBuffer)
		SAFE_DELETE_ARRAY(m_pBuffer);
	m_BufferSize = 0;
	if (TargetBuffer.m_pBuffer&&TargetBuffer.m_BufferSize)
	{
		m_BufferSize = TargetBuffer.m_BufferSize;
		m_pBuffer = new BYTE[m_BufferSize];
		m_IsSelfBuffer = true;
		memcpy(m_pBuffer, TargetBuffer.m_pBuffer, m_BufferSize);
	}	
	m_BufferHead = TargetBuffer.m_BufferHead;
	m_BufferTail = TargetBuffer.m_BufferTail;
	
}