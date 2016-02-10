/****************************************************************************/
/*                                                                          */
/*      文件名:    GrowBuffer.cpp                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

#define BUFFER_NODE_INIT_SIZE		32

IMPLEMENT_CLASS_INFO_STATIC(CGrowBuffer,CNameObject);

CGrowBuffer::CGrowBuffer(void)
{
	m_BufferNodes=NULL;
	m_BufferNodeCount=0;
	m_UsedBufferNodeCount=0;
	m_CurBufferNodeIndex=0;
	m_BufferSize=0;
	m_UsedSize=0;
	m_FirstSize=0;
	m_GrowSize=0;
}

CGrowBuffer::CGrowBuffer(UINT InitSize,UINT GrowSize)
{
	m_BufferNodes=NULL;
	m_BufferNodeCount=0;
	m_UsedBufferNodeCount=0;
	m_CurBufferNodeIndex=0;
	m_BufferSize=0;
	m_UsedSize=0;
	m_FirstSize=0;
	m_GrowSize=0;
	Create(InitSize,GrowSize);
}

CGrowBuffer::~CGrowBuffer(void)
{
	Destory();
}

BOOL CGrowBuffer::Create(UINT InitSize,UINT GrowSize)
{
	Destory();
	m_BufferNodes=new BUFFER_NODE[BUFFER_NODE_INIT_SIZE];
	m_BufferNodeCount=BUFFER_NODE_INIT_SIZE;
	m_UsedBufferNodeCount=1;
	m_CurBufferNodeIndex=0;
	m_BufferSize=InitSize;
	m_UsedSize=0;
	m_FirstSize=InitSize;
	m_GrowSize=GrowSize;
	m_BufferNodes[0].pBuffer=new char[m_FirstSize];
	m_BufferNodes[0].BufferSize=m_FirstSize;
	m_BufferNodes[0].UsedSize=0;
	return TRUE;
}

void CGrowBuffer::Destory()
{
	for(UINT i=0;i<m_BufferNodeCount;i++)
	{
		SAFE_DELETE_ARRAY(m_BufferNodes[i].pBuffer);
	}
	SAFE_DELETE_ARRAY(m_BufferNodes);
	m_BufferNodeCount=0;
	m_UsedBufferNodeCount=0;
	m_CurBufferNodeIndex=0;
	m_BufferSize=0;
	m_UsedSize=0;
	m_FirstSize=0;
	m_GrowSize=0;
}

BOOL CGrowBuffer::SetUsedSize(UINT Size)
{
	if(Size>m_BufferSize)
	{
		if(!GrowBuffer(m_UsedSize+Size))
			return FALSE;
	}
	if(Size<m_UsedSize)
	{
		m_UsedSize=Size;
		UINT NodePos=BufferPosToNodePos(Size);
		for(UINT i=m_CurBufferNodeIndex;i>=NodePos&&Size;i--)
		{
			if(Size>m_BufferNodes[i].UsedSize)
			{
				Size-=m_BufferNodes[i].UsedSize;
				m_BufferNodes[i].UsedSize=0;
			}
			else
			{
				m_BufferNodes[i].UsedSize-=Size;
				Size=0;
			}
		}
		m_CurBufferNodeIndex=NodePos;
	}
	else if(Size>m_UsedSize)
	{
		m_UsedSize=Size;
		UINT NodePos=BufferPosToNodePos(Size);
		for(UINT i=m_CurBufferNodeIndex;i<=NodePos&&Size;i++)
		{
			if(Size>m_BufferNodes[i].BufferSize-m_BufferNodes[i].UsedSize)
			{
				Size-=m_BufferNodes[i].BufferSize-m_BufferNodes[i].UsedSize;
				m_BufferNodes[i].UsedSize=m_BufferNodes[i].BufferSize;
			}
			else
			{
				m_BufferNodes[i].UsedSize+=Size;
				Size=0;
			}
		}
		m_CurBufferNodeIndex=NodePos;
	}
	return TRUE;
}

void CGrowBuffer::MakeSmooth()
{
	if(m_CurBufferNodeIndex==0)
		return;

	char * pNewBuffer=new char[m_UsedSize];
	UINT CopyPtr=0;
	for(UINT i=0;i<m_UsedBufferNodeCount;i++)
	{
		memcpy(pNewBuffer+CopyPtr,m_BufferNodes[i].pBuffer,m_BufferNodes[i].UsedSize);
		CopyPtr+=m_BufferNodes[i].UsedSize;
	}
	for(UINT i=0;i<m_UsedBufferNodeCount;i++)
	{
		SAFE_DELETE_ARRAY(m_BufferNodes[i].pBuffer);
		m_BufferNodes[i].BufferSize=0;
		m_BufferNodes[i].UsedSize=0;
	}
	m_UsedBufferNodeCount=1;
	m_CurBufferNodeIndex=1;
	m_BufferSize=m_UsedSize;
	m_FirstSize=m_UsedSize;
	m_BufferNodes[0].pBuffer=pNewBuffer;
	m_BufferNodes[0].BufferSize=m_UsedSize;
	m_BufferNodes[0].UsedSize=m_UsedSize;
}

BOOL CGrowBuffer::GrowBuffer(UINT NewSize)
{
	if(NewSize<=m_BufferSize)
		return TRUE;
	if(m_UsedBufferNodeCount>=m_BufferNodeCount)
	{
		UINT AddNodeCount=(NewSize-m_BufferSize)/m_GrowSize+1;
		BUFFER_NODE * pNewBufferNodes=new BUFFER_NODE[m_BufferNodeCount+AddNodeCount];
		memcpy(pNewBufferNodes,m_BufferNodes,sizeof(BUFFER_NODE)*m_BufferNodeCount);
		SAFE_DELETE_ARRAY(m_BufferNodes);
		m_BufferNodes=pNewBufferNodes;
		m_BufferNodeCount+=AddNodeCount;
	}
	UINT NeedGrowSize=NewSize-m_BufferSize;
	while(NeedGrowSize)
	{
		if(NeedGrowSize>m_GrowSize)
		{
			NeedGrowSize-=m_GrowSize;
		}
		else
		{
			NeedGrowSize=0;
		}
		m_BufferNodes[m_UsedBufferNodeCount].pBuffer=new char[m_GrowSize];
		m_BufferNodes[m_UsedBufferNodeCount].BufferSize=m_GrowSize;
		m_BufferNodes[m_UsedBufferNodeCount].UsedSize=0;
		m_BufferSize+=m_GrowSize;
		m_UsedBufferNodeCount++;
	}
	return TRUE;
}
