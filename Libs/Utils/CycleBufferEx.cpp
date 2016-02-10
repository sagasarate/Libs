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

CCycleBufferEx::CCycleBufferEx(void):CNameObject()
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_SmoothSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
}

CCycleBufferEx::CCycleBufferEx(UINT Size,UINT SmoothSize):CNameObject()
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_SmoothSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	Create(Size,SmoothSize);
}

CCycleBufferEx::CCycleBufferEx(LPVOID pBuff,UINT Size,UINT SmoothSize):CNameObject()
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_SmoothSize=0;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	Create(pBuff,Size,SmoothSize);
}

CCycleBufferEx::~CCycleBufferEx(void)
{
	Destory();
}

BOOL CCycleBufferEx::Create(UINT Size,UINT SmoothSize)
{
	Destory();
	if(Size<=SmoothSize*2)
	{
		return FALSE;
	}
	m_pBuffer=new BYTE[Size];
	m_BufferSize=Size-SmoothSize;
	m_SmoothSize=SmoothSize;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=true;
	return TRUE;
}

BOOL CCycleBufferEx::Create(LPVOID pBuff,UINT Size,UINT SmoothSize)
{
	Destory();
	if(Size<=SmoothSize*2)
	{
		return FALSE;
	}

	m_pBuffer=(BYTE *)pBuff;
	m_BufferSize=Size-SmoothSize;
	m_SmoothSize=SmoothSize;
	m_BufferHead=0;
	m_BufferTail=0;
	m_IsSelfBuffer=false;
	return TRUE;
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
