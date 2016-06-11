/****************************************************************************/
/*                                                                          */
/*      文件名:    CycleBufferEx.h                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CCycleBufferEx :
	public CNameObject
{
protected:
	BYTE *		m_pRealBuffer;
	BYTE *		m_pBuffer;
	UINT		m_BufferSize;	
	UINT		m_SmoothSize;
	UINT		m_BufferHead;
	UINT		m_BufferTail;	
	bool		m_IsSelfBuffer;

	DECLARE_CLASS_INFO_STATIC(CCycleBufferEx);
public:
	CCycleBufferEx(void);
	CCycleBufferEx(UINT Size,UINT SmoothSize);
	CCycleBufferEx(LPVOID pBuff,UINT Size,UINT SmoothSize);
	virtual ~CCycleBufferEx(void);

	BOOL Create(UINT Size,UINT SmoothSize);
	BOOL Create(LPVOID pBuff,UINT Size,UINT SmoothSize);
	void Destory();

	UINT GetBufferSize();
	UINT GetSmoothSize();
	UINT GetUsedSize();
	UINT GetSmoothUsedSize();
	UINT GetFreeSize();
	UINT GetSmoothFreeSize();

	LPVOID GetBuffer();
	LPVOID GetFreeBuffer();

	//BOOL PushFront(LPCVOID pData,UINT Size);
	//BOOL PushFront(UINT Data,UINT Size);
	BOOL PushBack(LPCVOID pData,UINT Size);
	BOOL PushBack(UINT Data,UINT Size);

	BOOL PopFront(LPVOID pData,UINT Size);
	BOOL PopFront(UINT Data,UINT Size);
	//BOOL PopBack(LPVOID pData,UINT Size);
	//BOOL PopBack(UINT Data,UINT Size);

	void Clear();

};

inline UINT CCycleBufferEx::GetBufferSize()
{
	//Buffer的可使用大小比创建大小小1，防止首尾指针重叠
	return m_BufferSize-1;
}

inline UINT CCycleBufferEx::GetSmoothSize()
{
	return m_SmoothSize;
}

inline UINT CCycleBufferEx::GetUsedSize()
{
	if(m_BufferTail>=m_BufferHead)
		return m_BufferTail-m_BufferHead;
	else
		return (m_BufferSize-m_BufferHead)+m_BufferTail;
}

inline UINT CCycleBufferEx::GetSmoothUsedSize()
{
	if(m_BufferTail>=m_BufferHead)
	{
		return m_BufferTail-m_BufferHead;
	}
	else
	{
		UINT SmoothUseSize=m_BufferSize-m_BufferHead+m_SmoothSize;
		UINT UseSize=GetUsedSize();
		if(SmoothUseSize>UseSize)
			return UseSize;
		else
			return SmoothUseSize;
	}
}

inline UINT CCycleBufferEx::GetFreeSize()
{
	if(m_BufferTail>=m_BufferHead)
		return (m_BufferSize-m_BufferTail)+m_BufferHead-1;
	else
		return m_BufferTail-m_BufferHead-1;
}

inline UINT CCycleBufferEx::GetSmoothFreeSize()
{
	if(m_BufferTail>=m_BufferHead)
	{
		UINT SmoothFreeSize=m_BufferSize-m_BufferTail+m_SmoothSize-1;
		UINT FreeSize=GetFreeSize();
		if(SmoothFreeSize>FreeSize)
			return FreeSize;
		else
			return SmoothFreeSize;

	}
	else
	{
		return m_BufferTail-m_BufferHead-1;
	}
}

inline LPVOID CCycleBufferEx::GetBuffer()
{
	return m_pBuffer+m_BufferHead;
}

inline LPVOID CCycleBufferEx::GetFreeBuffer()
{
	return m_pBuffer+m_BufferTail;
}

inline BOOL CCycleBufferEx::PushBack(LPCVOID pData,UINT Size)
{
	if(GetUsedSize()+Size<=GetBufferSize()&&Size<=GetSmoothSize())
	{
		if(m_BufferSize-m_BufferTail>=Size)
		{
			if(pData)
				memcpy(m_pBuffer+m_BufferTail,pData,Size);
			if(m_BufferTail<GetSmoothSize())
			{
				UINT CutSize=GetSmoothSize()-m_BufferTail;
				if(CutSize>Size)
					CutSize=Size;
				memcpy(m_pBuffer+m_BufferSize+m_BufferTail,m_pBuffer+m_BufferTail,CutSize);
			}
			m_BufferTail+=Size;
		}
		else
		{
			UINT CutSize=m_BufferSize-m_BufferTail;
			if(pData)
				memcpy(m_pBuffer+m_BufferTail,pData,Size);
			memcpy(m_pBuffer,m_pBuffer+m_BufferSize,Size-CutSize);
			m_BufferTail=Size-CutSize;
		}		
		return TRUE;
	}
	return FALSE;
}
inline BOOL CCycleBufferEx::PushBack(UINT Data,UINT Size)
{
	return PushBack(&Data,Size);
}

inline BOOL CCycleBufferEx::PopFront(LPVOID pData,UINT Size)
{
	if(Size<=GetUsedSize())
	{
		if(m_BufferSize-m_BufferHead>=Size)
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
				memcpy(pData,m_pBuffer+m_BufferHead,Size);						
			}
			m_BufferHead=Size-CutSize;
		}
		return TRUE;
	}
	return FALSE;
}
inline BOOL CCycleBufferEx::PopFront(UINT Data,UINT Size)
{
	return PopFront(&Data,Size);
}

inline void CCycleBufferEx::Clear()
{
	m_BufferHead=0;
	m_BufferTail=0;	
}