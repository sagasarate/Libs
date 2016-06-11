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


IMPLEMENT_CLASS_INFO_STATIC(CEasyBuffer,CNameObject);

CEasyBuffer::CEasyBuffer(void)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
}

CEasyBuffer::CEasyBuffer(UINT Size)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	Create(Size);
}

CEasyBuffer::CEasyBuffer(LPVOID pBuff,UINT Size)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	Create(pBuff,Size);
}

CEasyBuffer::CEasyBuffer(const CEasyBuffer& Buffer)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
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
		m_pBuffer=new BYTE[Size];
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
}

