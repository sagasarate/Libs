/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyBuffer.cpp                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


CEasyBuffer::CEasyBuffer(LPCTSTR Tag)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	m_Tag = Tag;
}

CEasyBuffer::CEasyBuffer(UINT Size, LPCTSTR Tag)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	m_Tag = Tag;
	Create(Size);
}

CEasyBuffer::CEasyBuffer(LPVOID pBuff,UINT Size, LPCTSTR Tag)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	m_Tag = Tag;
	Create(pBuff,Size);
}

CEasyBuffer::CEasyBuffer(const CEasyBuffer& Buffer, LPCTSTR Tag)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	m_Tag = Tag;
	Create(Buffer.GetBufferSize());
	PushBack(Buffer.GetBuffer(),Buffer.GetUsedSize());
}

CEasyBuffer::~CEasyBuffer(void)
{
	Destory();
}

BOOL CEasyBuffer::Create(UINT Size)
{
	Destory();
	if(Size)
	{
		m_pBuffer = MONITORED_NEW_ARRAY(GetTag(), BYTE, Size);
		m_BufferSize=Size;
		m_UsedSize=0;
		m_IsSelfBuffer=true;
		return TRUE;
	}
	return FALSE;
}

BOOL CEasyBuffer::Create(LPVOID pBuff,UINT Size)
{
	Destory();
	if(pBuff&&Size)
	{
		m_pBuffer=(BYTE *)pBuff;
		m_BufferSize=Size;
		m_UsedSize=0;
		m_IsSelfBuffer=false;
		return TRUE;
	}
	return FALSE;
}

void CEasyBuffer::Destory()
{
	if(m_IsSelfBuffer)
		SAFE_DELETE_ARRAY(m_pBuffer);
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_UsedSize = 0;
	m_IsSelfBuffer = true;
}

