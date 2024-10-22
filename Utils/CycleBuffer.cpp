﻿/****************************************************************************/
/*                                                                          */
/*      文件名:    CycleBuffer.cpp                                          */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CCycleBuffer,CNameObject);

CCycleBuffer::CCycleBuffer(LPCTSTR Tag) :CNameObject()
{
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_BufferHead = 0;
	m_BufferTail = 0;
	m_IsSelfBuffer = true;
	m_Tag = Tag;
}

CCycleBuffer::CCycleBuffer(UINT Size, LPCTSTR Tag) :CNameObject()
{
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_BufferHead = 0;
	m_BufferTail = 0;
	m_IsSelfBuffer = true;
	m_Tag = Tag;
	Create(Size);
}

CCycleBuffer::CCycleBuffer(LPVOID pBuff, UINT Size, LPCTSTR Tag) :CNameObject()
{
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_BufferHead = 0;
	m_BufferTail = 0;
	m_IsSelfBuffer = true;
	m_Tag = Tag;
	Create(pBuff, Size);
}

CCycleBuffer::~CCycleBuffer(void)
{
	Destory();
}

bool CCycleBuffer::Create(UINT Size)
{
	Destory();	
	m_BufferSize = Size + 1;
	m_pBuffer = MONITORED_NEW_ARRAY(GetTag(), BYTE, m_BufferSize);
	m_BufferHead = 0;
	m_BufferTail = 0;
	m_IsSelfBuffer = true;
	return true;
}

bool CCycleBuffer::Create(LPVOID pBuff, UINT Size)
{
	Destory();	
	m_pBuffer = (BYTE *)pBuff;
	m_BufferSize = Size;
	m_BufferHead = 0;
	m_BufferTail = 0;
	m_IsSelfBuffer = false;
	return true;
}

void CCycleBuffer::Destory()
{
	if (m_IsSelfBuffer)
		SAFE_DELETE_ARRAY(m_pBuffer);
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_BufferHead = 0;
	m_BufferTail = 0;
	m_IsSelfBuffer = true;
}

void CCycleBuffer::Clear()
{	
	m_BufferHead = 0;
	m_BufferTail = 0;
}


void CCycleBuffer::CloneFrom(const CCycleBuffer& TargetBuffer)
{
	if (m_IsSelfBuffer)
		SAFE_DELETE_ARRAY(m_pBuffer);
	m_BufferSize = 0;
	if (TargetBuffer.m_pBuffer&&TargetBuffer.m_BufferSize)
	{
		m_BufferSize = TargetBuffer.m_BufferSize;
		m_pBuffer = MONITORED_NEW_ARRAY(GetTag(), BYTE, m_BufferSize);
		m_IsSelfBuffer = true;
		memcpy(m_pBuffer, TargetBuffer.m_pBuffer, m_BufferSize);
	}
	m_BufferHead = TargetBuffer.m_BufferHead;
	m_BufferTail = TargetBuffer.m_BufferTail;

}