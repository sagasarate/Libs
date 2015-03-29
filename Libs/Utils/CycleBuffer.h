/****************************************************************************/
/*                                                                          */
/*      文件名:    CycleBuffer.h                                            */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CCycleBuffer :
	public CNameObject
{
protected:
	BYTE *		m_pBuffer;
	UINT		m_BufferSize;	
	UINT		m_BufferHead;
	UINT		m_BufferTail;	
	bool		m_IsSelfBuffer;

	DECLARE_CLASS_INFO_STATIC(CCycleBuffer);
public:
	CCycleBuffer(void);
	CCycleBuffer(UINT Size);
	CCycleBuffer(LPVOID pBuff,UINT Size);
	virtual ~CCycleBuffer(void);

	BOOL Create(UINT Size);
	BOOL Create(LPVOID pBuff,UINT Size);
	void Destory();

	UINT GetBufferSize();
	UINT GetUsedSize();	

	BOOL PushFront(LPCVOID pData,UINT Size);
	BOOL PushFront(UINT Data,UINT Size);
	BOOL PushBack(LPCVOID pData,UINT Size);
	BOOL PushBack(UINT Data,UINT Size);

	BOOL PopFront(LPVOID pData,UINT Size);
	BOOL PopFront(UINT Data,UINT Size);
	BOOL PopBack(LPVOID pData,UINT Size);
	BOOL PopBack(UINT Data,UINT Size);

	LPVOID GetUsedBuffer();
	int GetSmoothUsedSize();
};

inline UINT CCycleBuffer::GetBufferSize()
{
	//Buffer的可使用大小比创建大小小1，防止首尾指针重叠
	return m_BufferSize-1;
}
inline UINT CCycleBuffer::GetUsedSize()
{
	if(m_BufferTail>=m_BufferHead)
		return m_BufferTail-m_BufferHead;
	else
		return (m_BufferSize-m_BufferHead)+m_BufferTail;

}


inline BOOL CCycleBuffer::PushFront(LPCVOID pData,UINT Size)
{
	if(GetUsedSize()+Size<=GetBufferSize())
	{
		if(m_BufferHead>=Size)
		{			
			if(pData)
				memcpy(m_pBuffer+m_BufferHead-Size,pData,Size);
			m_BufferHead-=Size;
		}
		else
		{
			UINT CutSize=Size-m_BufferHead;
			if(pData)
			{
				memcpy(m_pBuffer,(BYTE *)pData+CutSize,m_BufferHead);						
				memcpy(m_pBuffer+m_BufferSize-CutSize,pData,CutSize);
			}
			m_BufferHead=m_BufferSize-CutSize;
		}	
		return TRUE;
	}
	return FALSE;
}
inline BOOL CCycleBuffer::PushFront(UINT Data,UINT Size)
{
	return PushFront(&Data,Size);
}
inline BOOL CCycleBuffer::PushBack(LPCVOID pData,UINT Size)
{
	if(GetUsedSize()+Size<=GetBufferSize())
	{
		if(m_BufferSize-m_BufferTail>Size)
		{
			if(pData)
				memcpy(m_pBuffer+m_BufferTail,pData,Size);
			m_BufferTail+=Size;
		}
		else
		{
			UINT CutSize=m_BufferSize-m_BufferTail;
			if(pData)
			{
				memcpy(m_pBuffer+m_BufferTail,pData,CutSize);
				memcpy(m_pBuffer,(BYTE *)pData+CutSize,Size-CutSize);
			}
			m_BufferTail=Size-CutSize;
		}		
		return TRUE;
	}
	return FALSE;
}
inline BOOL CCycleBuffer::PushBack(UINT Data,UINT Size)
{
	return PushBack(&Data,Size);
}

inline BOOL CCycleBuffer::PopFront(LPVOID pData,UINT Size)
{
	if(Size<=GetUsedSize())
	{
		if(m_BufferSize-m_BufferHead>Size)
		{
			if(pData)
				memcpy(pData,m_pBuffer+m_BufferHead,Size);
			m_BufferHead+=Size;
		}
		else
		{
			UINT CutSize=m_BufferSize-m_BufferHead;
			if(pData)
			{
				memcpy(pData,m_pBuffer+m_BufferHead,CutSize);
				memcpy((BYTE *)pData+CutSize,m_pBuffer,Size-CutSize);				
			}
			m_BufferHead=Size-CutSize;
		}
		return TRUE;
	}
	return FALSE;
}
inline BOOL CCycleBuffer::PopFront(UINT Data,UINT Size)
{
	return PopFront(&Data,Size);
}
inline BOOL CCycleBuffer::PopBack(LPVOID pData,UINT Size)
{
	if(Size<=GetUsedSize())
	{
		if(m_BufferTail>=Size)
		{
			if(pData)
				memcpy(pData,m_pBuffer+m_BufferTail-Size,Size);
			m_BufferTail-=Size;
		}
		else
		{
			UINT CutSize=Size-m_BufferTail;
			if(pData)
			{
				memcpy((BYTE *)pData+CutSize,m_pBuffer,m_BufferTail);
				memcpy(pData,m_pBuffer+m_BufferSize-CutSize,CutSize);
			}
			m_BufferTail=m_BufferSize-CutSize;
		}		
		return TRUE;
	}
	return FALSE;
}
inline BOOL CCycleBuffer::PopBack(UINT Data,UINT Size)
{
	return PopBack(&Data,Size);
}

inline LPVOID CCycleBuffer::GetUsedBuffer()
{
	return m_pBuffer+m_BufferHead;
}

inline int CCycleBuffer::GetSmoothUsedSize()
{		
	if(m_BufferTail>=m_BufferHead)
		return m_BufferTail-m_BufferHead;
	else
		return m_BufferSize-m_BufferHead;
}