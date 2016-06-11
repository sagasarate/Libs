/****************************************************************************/
/*                                                                          */
/*      文件名:    GrowBuffer.h                                             */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CGrowBuffer :
	public CNameObject
{
protected:
	struct BUFFER_NODE
	{
		char *				pBuffer;
		UINT				BufferSize;
		UINT				UsedSize;
		BUFFER_NODE()
		{
			pBuffer=NULL;
			BufferSize=0;
			UsedSize=0;
		}
	};
	BUFFER_NODE *	m_BufferNodes;
	UINT			m_BufferNodeCount;
	UINT			m_UsedBufferNodeCount;
	UINT			m_CurBufferNodeIndex;
	UINT			m_BufferSize;
	UINT			m_UsedSize;
	UINT			m_FirstSize;
	UINT			m_GrowSize;
	DECLARE_CLASS_INFO_STATIC(CGrowBuffer);
public:	

	CGrowBuffer(void);
	CGrowBuffer(UINT InitSize,UINT GrowSize);
	
	virtual ~CGrowBuffer(void);

	BOOL Create(UINT InitSize,UINT GrowSize);	
	void Destory();

	UINT GetBufferSize() const;
	UINT GetUsedSize() const;
	BOOL SetUsedSize(UINT Size);
	UINT GetFreeSize() const;
	LPVOID GetBuffer() const;
	LPVOID GetFreeBuffer() const;
	
	BOOL PushBack(LPCVOID pData,UINT Size);
	BOOL PushConstBack(UINT Data,UINT Size);	

	BOOL Peek(UINT& Pos,LPVOID pData,UINT Size) const;

	void MakeSmooth();
protected:
	UINT BufferPosToNodePos(UINT Pos,UINT * pOffset=NULL) const;
	BOOL GrowBuffer(UINT NewSize);

};

inline UINT CGrowBuffer::GetBufferSize() const
{
	return m_BufferSize;
}
inline UINT CGrowBuffer::GetUsedSize() const
{
	return m_UsedSize;
}

inline UINT CGrowBuffer::GetFreeSize() const
{
	return m_BufferSize-m_UsedSize;
}
inline LPVOID CGrowBuffer::GetBuffer() const
{
	return m_BufferNodes[0].pBuffer;
}
inline LPVOID CGrowBuffer::GetFreeBuffer() const
{
	return m_BufferNodes[0].pBuffer+m_BufferNodes[0].UsedSize;
}


inline BOOL CGrowBuffer::PushBack(LPCVOID pData,UINT Size)
{
	if(GetFreeSize()<Size)
	{
		if(!GrowBuffer(m_UsedSize+Size))
			return FALSE;
	}

	m_UsedSize+=Size;
	char * pSrcData=(char *)pData;
	while(Size)
	{
		UINT NodeFreeSize=m_BufferNodes[m_CurBufferNodeIndex].BufferSize-m_BufferNodes[m_CurBufferNodeIndex].UsedSize;
		if(Size>=NodeFreeSize)
		{				
			if(pSrcData)
			{
				memcpy(m_BufferNodes[m_CurBufferNodeIndex].pBuffer+m_BufferNodes[m_CurBufferNodeIndex].UsedSize,
					pSrcData,NodeFreeSize);
				pSrcData+=NodeFreeSize;
			}				
			m_BufferNodes[m_CurBufferNodeIndex].UsedSize+=NodeFreeSize;
			Size-=NodeFreeSize;		
			m_CurBufferNodeIndex++;
		}
		else
		{				
			if(pSrcData)
			{
				memcpy(m_BufferNodes[m_CurBufferNodeIndex].pBuffer+m_BufferNodes[m_CurBufferNodeIndex].UsedSize,
					pSrcData,Size);
				pSrcData+=Size;
			}				
			m_BufferNodes[m_CurBufferNodeIndex].UsedSize+=Size;
			Size=0;
		}
		
	}

	return TRUE;
	
}

inline BOOL CGrowBuffer::PushConstBack(UINT Data,UINT Size)
{
	return PushBack(&Data,Size);
}



inline BOOL CGrowBuffer::Peek(UINT& Pos,LPVOID pData,UINT Size) const
{
	if(Pos+Size<=m_UsedSize)
	{
		char * pDestData=(char *)pData;
		UINT NodeOffset;
		UINT NodePtr=BufferPosToNodePos(Pos,&NodeOffset);		
		while(Size)
		{
			UINT NodeDataSize=m_BufferNodes[NodePtr].UsedSize-NodeOffset;
			if(Size>=NodeDataSize)
			{
				if(pDestData)
				{
					memcpy(pDestData,m_BufferNodes[NodePtr].pBuffer+NodeOffset,NodeDataSize);
					pDestData+=NodeDataSize;
				}
				Pos+=NodeDataSize;
				Size-=NodeDataSize;
				NodePtr++;
			}
			else
			{
				if(pDestData)
				{
					memcpy(pDestData,m_BufferNodes[NodePtr].pBuffer+NodeOffset,Size);
					pDestData+=Size;
				}
				Pos+=Size;
				Size=0;
			}			
			NodeOffset=0;
		}
		
		return TRUE;
	}
	return FALSE;
}

inline UINT CGrowBuffer::BufferPosToNodePos(UINT Pos,UINT * pOffset) const
{	
	if(Pos<m_FirstSize)
	{
		if(pOffset)
			*pOffset=Pos;
		return 0;
	}
	else if(Pos==m_FirstSize)
	{
		if(pOffset)
			*pOffset=0;
		return 1;
	}
	else
	{
		if(pOffset)
			*pOffset=(Pos-m_FirstSize)%m_GrowSize;
		return (Pos-m_FirstSize)/m_GrowSize+1;
	}
}