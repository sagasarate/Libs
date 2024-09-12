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


class CEasyBuffer
{
protected:
	BYTE* m_pBuffer;
	size_t		m_BufferSize;
	size_t		m_UsedSize;
	bool		m_IsSelfBuffer;
	LPCTSTR		m_Tag;
public:
	CEasyBuffer(LPCTSTR Tag = _T("CEasyBuffer"));
	CEasyBuffer(size_t Size, LPCTSTR Tag = _T("CEasyBuffer"));
	CEasyBuffer(LPVOID pBuff, size_t Size, LPCTSTR Tag = _T("CEasyBuffer"));
	CEasyBuffer(const CEasyBuffer& Buffer, LPCTSTR Tag = _T("CEasyBuffer"));
	virtual ~CEasyBuffer(void);

	LPCTSTR GetTag()
	{
		return m_Tag;
	}
	void SetTag(LPCTSTR Tag)
	{
		m_Tag = Tag;
	}

	BOOL Create(size_t Size);
	BOOL Create(LPVOID pBuff, size_t Size);
	void Destory();

	void Clear();

	size_t GetBufferSize() const;
	size_t GetUsedSize() const;
	BOOL SetUsedSize(size_t Size);
	size_t GetFreeSize() const;
	LPVOID GetBuffer() const;
	LPVOID GetFreeBuffer() const;

	BOOL PushFront(LPCVOID pData, size_t Size);
	BOOL PushConstFront(UINT Data, size_t Size);
	BOOL PushBack(LPCVOID pData, size_t Size);
	BOOL PushConstBack(UINT Data, size_t Size);

	BOOL PopFront(LPVOID pData, size_t Size);
	BOOL PopBack(LPVOID pData, size_t Size);

	BOOL Peek(size_t& Pos, LPVOID pData, size_t Size) const;

	BOOL PickData(CEasyBuffer& Buffer);

	CEasyBuffer& operator=(const CEasyBuffer& Buffer);


};
inline void CEasyBuffer::Clear()
{
	m_UsedSize = 0;
}
inline size_t CEasyBuffer::GetBufferSize() const
{
	return m_BufferSize;
}
inline size_t CEasyBuffer::GetUsedSize() const
{
	return m_UsedSize;
}
inline BOOL CEasyBuffer::SetUsedSize(size_t Size)
{
	if (Size <= m_BufferSize)
	{
		m_UsedSize = Size;
		return TRUE;
	}
	return FALSE;
}
inline size_t CEasyBuffer::GetFreeSize() const
{
	return m_BufferSize - m_UsedSize;
}
inline LPVOID CEasyBuffer::GetBuffer() const
{
	return m_pBuffer;
}

inline LPVOID CEasyBuffer::GetFreeBuffer() const
{
	return m_pBuffer + m_UsedSize;
}

inline BOOL CEasyBuffer::PushFront(LPCVOID pData, size_t Size)
{
	if (m_pBuffer)
	{
		if (m_UsedSize + Size <= m_BufferSize)
		{
			memmove(m_pBuffer + Size, m_pBuffer, m_UsedSize);
			if (pData)
				memcpy(m_pBuffer, pData, Size);
			m_UsedSize += Size;
			return TRUE;
		}
	}
	return FALSE;
}
inline BOOL CEasyBuffer::PushConstFront(UINT Data, size_t Size)
{
	return PushFront(&Data, Size);
}
inline BOOL CEasyBuffer::PushBack(LPCVOID pData, size_t Size)
{
	if (m_pBuffer)
	{
		if (m_UsedSize + Size <= m_BufferSize)
		{
			if (pData)
				memcpy(m_pBuffer + m_UsedSize, pData, Size);
			m_UsedSize += Size;
			return TRUE;
		}
	}
	return FALSE;
}
inline BOOL CEasyBuffer::PushConstBack(UINT Data, size_t Size)
{
	return PushBack(&Data, Size);
}

inline BOOL CEasyBuffer::PopFront(LPVOID pData, size_t Size)
{
	if (m_pBuffer)
	{
		if (Size <= m_UsedSize)
		{
			if (pData)
				memcpy(pData, m_pBuffer, Size);
			m_UsedSize -= Size;
			memmove(m_pBuffer, m_pBuffer + Size, m_UsedSize);
			return TRUE;
		}
	}
	return FALSE;
}

inline BOOL CEasyBuffer::PopBack(LPVOID pData, size_t Size)
{
	if (m_pBuffer)
	{
		if (Size <= m_UsedSize)
		{
			m_UsedSize -= Size;
			if (pData)
				memcpy(pData, m_pBuffer + m_UsedSize, Size);
			return TRUE;
		}
	}
	return FALSE;
}

inline BOOL CEasyBuffer::Peek(size_t& Pos, LPVOID pData, size_t Size) const
{
	if (m_pBuffer)
	{
		if (Pos + Size <= m_UsedSize)
		{
			if (pData)
				memcpy(pData, m_pBuffer + Pos, Size);
			Pos += Size;
			return TRUE;
		}
	}
	return FALSE;
}

inline BOOL CEasyBuffer::PickData(CEasyBuffer& Buffer)
{
	if (m_pBuffer && Buffer.m_pBuffer)
	{
		Destory();
		m_pBuffer = Buffer.m_pBuffer;
		m_BufferSize = Buffer.m_BufferSize;
		m_UsedSize = Buffer.m_UsedSize;
		m_IsSelfBuffer = Buffer.m_IsSelfBuffer;
		Buffer.m_pBuffer = NULL;
		Buffer.m_BufferSize = 0;
		Buffer.m_UsedSize = 0;
		Buffer.m_IsSelfBuffer = true;
		return TRUE;
	}
	return FALSE;
}


inline CEasyBuffer& CEasyBuffer::operator=(const CEasyBuffer& Buffer)
{
	Create(Buffer.GetUsedSize());
	PushBack(Buffer.GetBuffer(), Buffer.GetUsedSize());
	return *this;
}
