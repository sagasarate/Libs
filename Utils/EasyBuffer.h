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
	BYTE*		m_pBuffer;
	size_t		m_BufferSize;
	size_t		m_UsedSize;
	bool		m_IsSelfBuffer;
	LPCTSTR		m_Tag;
public:
	CEasyBuffer(LPCTSTR Tag = _T("CEasyBuffer"));
	CEasyBuffer(size_t Size, LPCTSTR Tag = _T("CEasyBuffer"));
	CEasyBuffer(LPVOID pBuff, size_t Size, LPCTSTR Tag = _T("CEasyBuffer"));
	CEasyBuffer(LPCVOID pBuff, size_t Size, LPCTSTR Tag = _T("CEasyBuffer"));
	CEasyBuffer(const CEasyBuffer& Buffer, LPCTSTR Tag = _T("CEasyBuffer"));
	CEasyBuffer(CEasyBuffer&& Buffer) noexcept;
	virtual ~CEasyBuffer(void);

	LPCTSTR GetTag()
	{
		return m_Tag;
	}
	void SetTag(LPCTSTR Tag)
	{
		m_Tag = Tag;
	}

	bool Create(size_t Size);
	bool Create(LPVOID pBuff, size_t Size);
	void Destory();

	void Clear();

	size_t GetBufferSize() const;
	size_t GetUsedSize() const;
	bool SetUsedSize(size_t Size);
	size_t GetFreeSize() const;
	LPVOID GetBuffer() const;
	LPVOID GetFreeBuffer() const;

	bool PushFront(LPCVOID pData, size_t Size);
	bool PushConstFront(UINT Data, size_t Size);
	bool PushBack(LPCVOID pData, size_t Size);
	bool PushConstBack(UINT Data, size_t Size);

	bool PopFront(LPVOID pData, size_t Size);
	bool PopBack(LPVOID pData, size_t Size);

	bool Peek(size_t& Pos, LPVOID pData, size_t Size) const;

	bool PickData(CEasyBuffer& Buffer);

	CEasyBuffer& operator=(const CEasyBuffer& Buffer);


};

inline CEasyBuffer::CEasyBuffer(LPCTSTR Tag)
{
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_UsedSize = 0;
	m_IsSelfBuffer = true;
	m_Tag = Tag;
}

inline CEasyBuffer::CEasyBuffer(size_t Size, LPCTSTR Tag)
{
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_UsedSize = 0;
	m_IsSelfBuffer = true;
	m_Tag = Tag;
	Create(Size);
}

inline CEasyBuffer::CEasyBuffer(LPVOID pBuff, size_t Size, LPCTSTR Tag)
{
	m_pBuffer = (BYTE*)pBuff;
	m_BufferSize = Size;
	m_UsedSize = 0;
	m_IsSelfBuffer = false;
	m_Tag = Tag;
}
inline CEasyBuffer::CEasyBuffer(LPCVOID pBuff, size_t Size, LPCTSTR Tag)
{
	m_pBuffer = (BYTE*)pBuff;
	m_BufferSize = Size;
	m_UsedSize = Size;
	m_IsSelfBuffer = false;
	m_Tag = Tag;
}
inline CEasyBuffer::CEasyBuffer(CEasyBuffer&& Buffer) noexcept
{
	m_pBuffer = Buffer.m_pBuffer;
	m_BufferSize = Buffer.m_BufferSize;
	m_UsedSize = Buffer.m_UsedSize;
	m_IsSelfBuffer = Buffer.m_IsSelfBuffer;
	m_Tag = Buffer.m_Tag;

	Buffer.m_pBuffer = NULL;
	Buffer.m_BufferSize = 0;
	Buffer.m_UsedSize = 0;
	Buffer.m_IsSelfBuffer = true;
}
inline CEasyBuffer::CEasyBuffer(const CEasyBuffer& Buffer, LPCTSTR Tag)
{
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_UsedSize = 0;
	m_IsSelfBuffer = true;
	m_Tag = Tag;
	Create(Buffer.GetBufferSize());
	PushBack(Buffer.GetBuffer(), Buffer.GetUsedSize());
}

inline CEasyBuffer::~CEasyBuffer(void)
{
	Destory();
}

inline bool CEasyBuffer::Create(size_t Size)
{
	Destory();
	if (Size)
	{
		m_pBuffer = MONITORED_NEW_ARRAY(GetTag(), BYTE, Size);
		m_BufferSize = Size;
		m_UsedSize = 0;
		m_IsSelfBuffer = true;
		return true;
	}
	return false;
}

inline bool CEasyBuffer::Create(LPVOID pBuff, size_t Size)
{
	Destory();
	if (pBuff && Size)
	{
		m_pBuffer = (BYTE*)pBuff;
		m_BufferSize = Size;
		m_UsedSize = 0;
		m_IsSelfBuffer = false;
		return true;
	}
	return false;
}

inline void CEasyBuffer::Destory()
{
	if (m_IsSelfBuffer)
		SAFE_DELETE_ARRAY(m_pBuffer);
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_UsedSize = 0;
	m_IsSelfBuffer = true;
}

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
inline bool CEasyBuffer::SetUsedSize(size_t Size)
{
	if (Size <= m_BufferSize)
	{
		m_UsedSize = Size;
		return true;
	}
	return false;
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

inline bool CEasyBuffer::PushFront(LPCVOID pData, size_t Size)
{
	if (m_pBuffer)
	{
		if (m_UsedSize + Size <= m_BufferSize)
		{
			memmove(m_pBuffer + Size, m_pBuffer, m_UsedSize);
			if (pData)
				memcpy(m_pBuffer, pData, Size);
			m_UsedSize += Size;
			return true;
		}
	}
	return false;
}
inline bool CEasyBuffer::PushConstFront(UINT Data, size_t Size)
{
	return PushFront(&Data, Size);
}
inline bool CEasyBuffer::PushBack(LPCVOID pData, size_t Size)
{
	if (m_pBuffer)
	{
		if (m_UsedSize + Size <= m_BufferSize)
		{
			if (pData)
				memcpy(m_pBuffer + m_UsedSize, pData, Size);
			m_UsedSize += Size;
			return true;
		}
	}
	return false;
}
inline bool CEasyBuffer::PushConstBack(UINT Data, size_t Size)
{
	return PushBack(&Data, Size);
}

inline bool CEasyBuffer::PopFront(LPVOID pData, size_t Size)
{
	if (m_pBuffer)
	{
		if (Size <= m_UsedSize)
		{
			if (pData)
				memcpy(pData, m_pBuffer, Size);
			m_UsedSize -= Size;
			memmove(m_pBuffer, m_pBuffer + Size, m_UsedSize);
			return true;
		}
	}
	return false;
}

inline bool CEasyBuffer::PopBack(LPVOID pData, size_t Size)
{
	if (m_pBuffer)
	{
		if (Size <= m_UsedSize)
		{
			m_UsedSize -= Size;
			if (pData)
				memcpy(pData, m_pBuffer + m_UsedSize, Size);
			return true;
		}
	}
	return false;
}

inline bool CEasyBuffer::Peek(size_t& Pos, LPVOID pData, size_t Size) const
{
	if (m_pBuffer)
	{
		if (Pos + Size <= m_UsedSize)
		{
			if (pData)
				memcpy(pData, m_pBuffer + Pos, Size);
			Pos += Size;
			return true;
		}
	}
	return false;
}

inline bool CEasyBuffer::PickData(CEasyBuffer& Buffer)
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
		return true;
	}
	return false;
}


inline CEasyBuffer& CEasyBuffer::operator=(const CEasyBuffer& Buffer)
{
	Create(Buffer.GetUsedSize());
	PushBack(Buffer.GetBuffer(), Buffer.GetUsedSize());
	return *this;
}
