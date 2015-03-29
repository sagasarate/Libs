/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyBuffer.h                                             */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CEasyBuffer :
	public CNameObject
{
protected:
	BYTE *		m_pBuffer;
	UINT			m_BufferSize;
	UINT			m_UsedSize;
	bool		m_IsSelfBuffer;

	DECLARE_CLASS_INFO_STATIC(CEasyBuffer);
public:
	CEasyBuffer(void);
	CEasyBuffer(UINT Size);
	CEasyBuffer(LPVOID pBuff,UINT Size);
	CEasyBuffer(const CEasyBuffer& Buffer);
	virtual ~CEasyBuffer(void);

	BOOL Create(UINT Size);
	BOOL Create(LPVOID pBuff,UINT Size);
	void Destory();

	UINT GetBufferSize() const;
	UINT GetUsedSize() const;
	BOOL SetUsedSize(UINT Size);
	UINT GetFreeSize() const;
	LPVOID GetBuffer() const;
	LPVOID GetFreeBuffer() const;

	BOOL PushFront(LPCVOID pData,UINT Size);
	BOOL PushConstFront(UINT Data,UINT Size);
	BOOL PushBack(LPCVOID pData,UINT Size);
	BOOL PushConstBack(UINT Data,UINT Size);

	BOOL PopFront(LPVOID pData,UINT Size);
	BOOL PopBack(LPVOID pData,UINT Size);

	BOOL Peek(UINT& Pos,LPVOID pData,UINT Size) const;

	BOOL PickData(CEasyBuffer& Buffer);

	CEasyBuffer& operator=(const CEasyBuffer& Buffer);


};

inline UINT CEasyBuffer::GetBufferSize() const
{
	return m_BufferSize;
}
inline UINT CEasyBuffer::GetUsedSize() const
{
	return m_UsedSize;
}
inline BOOL CEasyBuffer::SetUsedSize(UINT Size)
{
	if(Size<=m_BufferSize)
	{
		m_UsedSize=Size;
		return TRUE;
	}
	return FALSE;
}
inline UINT CEasyBuffer::GetFreeSize() const
{
	return m_BufferSize-m_UsedSize;
}
inline LPVOID CEasyBuffer::GetBuffer() const
{
	return m_pBuffer;
}

inline LPVOID CEasyBuffer::GetFreeBuffer() const
{
	return m_pBuffer+m_UsedSize;
}

inline BOOL CEasyBuffer::PushFront(LPCVOID pData,UINT Size)
{
	if(m_pBuffer)
	{
		if(m_UsedSize+Size<=m_BufferSize)
		{
			memmove(m_pBuffer+Size,m_pBuffer,m_UsedSize);
			if(pData)
				memcpy(m_pBuffer,pData,Size);
			m_UsedSize+=Size;
			return TRUE;
		}
	}
	return FALSE;
}
inline BOOL CEasyBuffer::PushConstFront(UINT Data,UINT Size)
{
	return PushFront(&Data,Size);
}
inline BOOL CEasyBuffer::PushBack(LPCVOID pData,UINT Size)
{
	if(m_pBuffer)
	{
		if(m_UsedSize+Size<=m_BufferSize)
		{
			if(pData)
				memcpy(m_pBuffer+m_UsedSize,pData,Size);
			m_UsedSize+=Size;
			return TRUE;
		}
	}
	return FALSE;
}
inline BOOL CEasyBuffer::PushConstBack(UINT Data,UINT Size)
{
	return PushBack(&Data,Size);
}

inline BOOL CEasyBuffer::PopFront(LPVOID pData,UINT Size)
{
	if(m_pBuffer)
	{
		if(Size<=m_UsedSize)
		{
			if(pData)
				memcpy(pData,m_pBuffer,Size);
			m_UsedSize-=Size;
			memmove(m_pBuffer,m_pBuffer+Size,m_UsedSize);
			return TRUE;
		}
	}
	return FALSE;
}

inline BOOL CEasyBuffer::PopBack(LPVOID pData,UINT Size)
{
	if(m_pBuffer)
	{
		if(Size<=m_UsedSize)
		{
			if(pData)
				memcpy(pData,m_pBuffer+m_UsedSize,Size);
			m_UsedSize-=Size;
			return TRUE;
		}
	}
	return FALSE;
}

inline BOOL CEasyBuffer::Peek(UINT& Pos,LPVOID pData,UINT Size) const
{
	if(m_pBuffer)
	{
		if(Pos+Size<=m_UsedSize)
		{
			if(pData)
				memcpy(pData,m_pBuffer+Pos,Size);
			Pos+=Size;
			return TRUE;
		}
	}
	return FALSE;
}

inline BOOL CEasyBuffer::PickData(CEasyBuffer& Buffer)
{
	if(m_pBuffer&&Buffer.m_pBuffer)
	{
		Destory();
		m_pBuffer=Buffer.m_pBuffer;
		m_BufferSize=Buffer.m_BufferSize;
		m_UsedSize=Buffer.m_UsedSize;
		m_IsSelfBuffer=Buffer.m_IsSelfBuffer;
		Buffer.m_pBuffer=NULL;
		Buffer.m_BufferSize=0;
		Buffer.m_UsedSize=0;
		Buffer.m_IsSelfBuffer=true;
		return TRUE;
	}
	return FALSE;
}


inline CEasyBuffer& CEasyBuffer::operator=(const CEasyBuffer& Buffer)
{
	Create(Buffer.GetUsedSize());
	PushBack(Buffer.GetBuffer(),Buffer.GetUsedSize());
	return *this;
}
