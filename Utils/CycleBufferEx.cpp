/****************************************************************************/
/*                                                                          */
/*      文件名:    CycleBufferEx.cpp                                        */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CCycleBufferEx,CNameObject);

CCycleBufferEx::CCycleBufferEx(LPCTSTR Tag):CNameObject()
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_SmoothSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	m_Tag = Tag;
}

CCycleBufferEx::CCycleBufferEx(UINT Size,UINT SmoothSize, LPCTSTR Tag):CNameObject()
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_SmoothSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	m_Tag = Tag;
	Create(Size,SmoothSize);
}

CCycleBufferEx::CCycleBufferEx(LPVOID pBuff,UINT Size,UINT SmoothSize, LPCTSTR Tag):CNameObject()
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_SmoothSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	m_Tag = Tag;
	Create(pBuff,Size,SmoothSize);
}

CCycleBufferEx::~CCycleBufferEx(void)
{
	Destory();
}

bool CCycleBufferEx::Create(UINT Size,UINT SmoothSize)
{
	Destory();
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

bool CCycleBufferEx::Create(LPVOID pBuff,UINT Size,UINT SmoothSize)
{
	Destory();
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

void CCycleBufferEx::Destory()
{
	if(m_IsSelfBuffer)
		SAFE_DELETE_ARRAY(m_pBuffer);

	m_pBuffer=NULL;
	m_BufferSize=0;
	m_SmoothSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
}
