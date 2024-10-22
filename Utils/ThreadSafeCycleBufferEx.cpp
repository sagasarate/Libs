﻿/****************************************************************************/
/*                                                                          */
/*      文件名:    ThreadSafeCycleBufferEx.cpp                                        */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CThreadSafeCycleBufferEx,CCycleBufferEx);

CThreadSafeCycleBufferEx::CThreadSafeCycleBufferEx(LPCTSTR Tag):CCycleBufferEx(Tag)
{
	m_IsLockFront=true;
	m_IsLockBack=true;
}

CThreadSafeCycleBufferEx::CThreadSafeCycleBufferEx(UINT Size,UINT SmoothSize, LPCTSTR Tag):CCycleBufferEx(Tag)
{
	m_IsLockFront = true;
	m_IsLockBack = true;
	Create(Size,SmoothSize);
}

CThreadSafeCycleBufferEx::CThreadSafeCycleBufferEx(LPVOID pBuff,UINT Size,UINT SmoothSize, LPCTSTR Tag):CCycleBufferEx(Tag)
{
	m_IsLockFront = true;
	m_IsLockBack = true;
	Create(pBuff,Size,SmoothSize);
}

CThreadSafeCycleBufferEx::~CThreadSafeCycleBufferEx(void)
{
	Destory();
}

bool CThreadSafeCycleBufferEx::Create(UINT Size,UINT SmoothSize)
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

	if(Size<=SmoothSize*2)
	{
		return false;
	}
	m_pBuffer = MONITORED_NEW_ARRAY(GetTag(), BYTE, Size);
	m_BufferSize=Size-SmoothSize;
	m_SmoothSize=SmoothSize;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	return true;
}

bool CThreadSafeCycleBufferEx::Create(LPVOID pBuff,UINT Size,UINT SmoothSize)
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

	if(Size<=SmoothSize*2)
	{
		return false;
	}

	m_pBuffer=(BYTE *)pBuff;
	m_BufferSize=Size-SmoothSize;
	m_SmoothSize=SmoothSize;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=false;
	return true;
}

void CThreadSafeCycleBufferEx::Destory()
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
	m_SmoothSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
}
