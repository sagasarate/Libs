/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyArray.h                                              */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#pragma warning (push)
#pragma warning (disable : 4345)



template < class T >
class CEasyArray
{
protected:
	T *		m_pBuffer;
	size_t	m_BufferSize;
	size_t	m_ArrayLength;
	size_t	m_GrowSize;
public:
	CEasyArray()
	{
		m_pBuffer=NULL;
		m_BufferSize=0;
		m_ArrayLength=0;
		m_GrowSize=1;
	}
	CEasyArray(const CEasyArray& Array)
	{
		m_pBuffer=NULL;
		m_BufferSize=0;
		m_ArrayLength=0;
		m_GrowSize=1;
		*this=Array;		
	}
	CEasyArray(size_t Size,size_t GrowSize=1)
	{
		ASSERT_AND_THROW(GrowSize>0);
		m_pBuffer=NULL;
		m_BufferSize=Size;
		m_ArrayLength=0;
		m_GrowSize=GrowSize;
		if(m_BufferSize)
		{
			m_pBuffer=(T *)malloc(sizeof(T)*m_BufferSize);
			//ConstructObjects(m_pBuffer,m_BufferSize);
		}
	}
	~CEasyArray()
	{
		Clear();
		m_GrowSize=1;
	}

	void Clear()
	{
		if(m_pBuffer)
		{			
			DestructObjects(m_pBuffer,m_ArrayLength);
			free(m_pBuffer);
			m_pBuffer=NULL;
		}
		m_BufferSize=0;
		m_ArrayLength=0;
	}
	void Empty()
	{
		if(m_pBuffer)
		{			
			DestructObjects(m_pBuffer,m_ArrayLength);			
			m_ArrayLength=0;
		}		
	}
	void Create(size_t Size,size_t GrowSize=1)
	{
		ASSERT_AND_THROW(GrowSize>0);
		Clear();		
		m_BufferSize=Size;
		m_ArrayLength=0;
		m_GrowSize=GrowSize;
		if(m_BufferSize)
		{
			m_pBuffer=(T *)malloc(sizeof(T)*m_BufferSize);
			//ConstructObjects(m_pBuffer,m_BufferSize);
		}
	}

	void Add(const T& Value)
	{
		if(m_ArrayLength>=m_BufferSize)
		{
			ResizeBuffer(m_BufferSize+m_GrowSize);
		}
		ConstructObjects(m_pBuffer+m_ArrayLength,1);
		m_pBuffer[m_ArrayLength]=Value;
		m_ArrayLength++;
	}
	T * AddEmpty()
	{
		if(m_ArrayLength>=m_BufferSize)
		{
			ResizeBuffer(m_BufferSize+m_GrowSize);
		}
		ConstructObjects(m_pBuffer+m_ArrayLength,1);
		m_ArrayLength++;
		return m_pBuffer+m_ArrayLength-1;
	}
	void AddArray(const CEasyArray<T>& Array)
	{
		if(m_ArrayLength+Array.GetCount()>=m_BufferSize)
		{
			ResizeBuffer(m_BufferSize+Array.GetCount()+m_GrowSize);
		}
		ConstructObjects(m_pBuffer+m_ArrayLength,Array.GetCount());
		for(size_t i=0;i<Array.GetCount();i++)
		{
			m_pBuffer[m_ArrayLength]=Array.GetObjectConst(i);
			m_ArrayLength++;
		}
	}
	bool Insert(size_t BeforeIndex,const T& Value)
	{
		if(BeforeIndex<=m_ArrayLength)
		{
			if(m_ArrayLength>=m_BufferSize)
			{
				ResizeBuffer(m_BufferSize+m_GrowSize);
			}
			if(m_ArrayLength-BeforeIndex)
				memmove(m_pBuffer+BeforeIndex+1,m_pBuffer+BeforeIndex,sizeof(T)*(m_ArrayLength-BeforeIndex));
			ConstructObjects(m_pBuffer+BeforeIndex,1);
			m_pBuffer[BeforeIndex]=Value;
			m_ArrayLength++;
			return true;
		}
		return false;
	}
	T * InsertEmpty(size_t BeforeIndex)
	{
		if (BeforeIndex <= m_ArrayLength)
		{
			if (m_ArrayLength >= m_BufferSize)
			{
				ResizeBuffer(m_BufferSize + m_GrowSize);
			}
			if (m_ArrayLength - BeforeIndex)
				memmove(m_pBuffer + BeforeIndex + 1, m_pBuffer + BeforeIndex, sizeof(T)*(m_ArrayLength - BeforeIndex));
			ConstructObjects(m_pBuffer + BeforeIndex, 1);
			m_ArrayLength++;
			return m_pBuffer + BeforeIndex;
		}
		return NULL;
	}
	bool Insert(size_t BeforeIndex,const CEasyArray<T>& Array)
	{
		if(BeforeIndex<=m_ArrayLength&&Array.GetCount())
		{
			if(m_ArrayLength+Array.GetCount()>m_BufferSize)
			{
				ResizeBuffer(m_ArrayLength+Array.GetCount()+m_GrowSize);
			}
			if(m_ArrayLength-BeforeIndex)
				memmove(m_pBuffer+BeforeIndex+Array.GetCount(),m_pBuffer+BeforeIndex,sizeof(T)*(m_ArrayLength-BeforeIndex));
			ConstructObjects(m_pBuffer+BeforeIndex,Array.GetCount());
			for(size_t i=0;i<Array.GetCount();i++)
			{
				m_pBuffer[BeforeIndex+i]=Array.GetObjectConst(i);
			}			
			m_ArrayLength+=Array.GetCount();
			return true;
		}
		return false;
	}
	bool Delete(size_t Index)
	{
		if(Index<m_ArrayLength)
		{
			DestructObjects(m_pBuffer+Index,1);
			if(m_ArrayLength-Index-1)
				memmove(m_pBuffer+Index,m_pBuffer+Index+1,sizeof(T)*(m_ArrayLength-Index-1));				
			m_ArrayLength--;

			if(m_BufferSize-m_ArrayLength>=m_GrowSize)
			{
				ShrinkBuffer(m_BufferSize-m_GrowSize);
			}			
			return true;
		}
		return false;
	}
	bool Move(size_t Index, size_t Before)
	{		
		if (Index < m_ArrayLength)
		{
			if (Before > m_ArrayLength)
				Before = m_ArrayLength;
			if (Index == Before || Index + 1 == Before)
				return true;
			BYTE Temp[sizeof(T)];
			memcpy(Temp, m_pBuffer + Index, sizeof(T));
			if (Index < Before)
			{
				memmove(m_pBuffer + Index, m_pBuffer + Index + 1, sizeof(T)*(Before - Index - 1));
				memcpy(m_pBuffer + Before - 1, Temp, sizeof(T));
			}
			else
			{
				memmove(m_pBuffer + Before + 1, m_pBuffer + Before, sizeof(T)*(Index - Before));
				memcpy(m_pBuffer + Before, Temp, sizeof(T));
			}
			return true;
		}
		return false;
	}
	size_t GetCount() const
	{
		return m_ArrayLength;
	}
	bool SetCount(size_t NewLength)
	{
		if(NewLength<=m_BufferSize)
		{
			m_ArrayLength=NewLength;
			return true;
		}
		return false;
	}
	size_t GetBufferLength() const
	{
		return m_BufferSize;
	}
	T * GetBuffer()
	{
		return m_pBuffer;
	}
	const T * GetBuffer() const
	{
		return m_pBuffer;
	}
	T * GetObject(size_t Index)
	{
		if(Index<m_ArrayLength)
		{
			return m_pBuffer+Index;
		}
		return NULL;
	}
	const T * GetObject(size_t Index) const
	{
		if (Index < m_ArrayLength)
		{
			return m_pBuffer + Index;
		}
		return NULL;
	}
	const T& GetObjectConst(size_t Index) const
	{
#ifdef _DEBUG
		ASSERT_AND_THROW(Index<m_ArrayLength);
#endif
		return m_pBuffer[Index];
	}
	void Resize(size_t NewSize)
	{
		if(NewSize)
		{
			if(NewSize<m_ArrayLength)
			{
				DestructObjects(m_pBuffer+NewSize,m_ArrayLength-NewSize);
			}
			ResizeBuffer(NewSize);
			if(NewSize>m_ArrayLength)
			{
				ConstructObjects(m_pBuffer+m_ArrayLength,NewSize-m_ArrayLength);
			}
			
			m_ArrayLength=NewSize;
		}
		else
		{
			Clear();
		}
	}
	void Reserve(size_t NewSize)
	{		
		ResizeBuffer(NewSize);		
	}
	const CEasyArray& operator=(const CEasyArray& Array)
	{
		Empty();
		if(m_ArrayLength<Array.m_ArrayLength)
			Resize(Array.m_ArrayLength);
		m_ArrayLength=Array.m_ArrayLength;
		for(size_t i=0;i<m_ArrayLength;i++)
		{
			m_pBuffer[i]=Array.m_pBuffer[i];
		}
		return *this;
	}
	T& operator[](size_t Index)
	{
		ASSERT_AND_THROW(Index<m_ArrayLength);
		return m_pBuffer[Index];
	}
	const T& operator[](size_t Index) const
	{
		ASSERT_AND_THROW(Index < m_ArrayLength);
		return m_pBuffer[Index];
	}
	void Reverse()
	{
		if (m_ArrayLength >= m_BufferSize)
		{
			ResizeBuffer(m_BufferSize + m_GrowSize);
		}
		T * pSwapBuff = m_pBuffer + sizeof(T)*GetCount();
		for (UINT i = 0; i < GetCount() / 2; i++)
		{
			memcpy(pSwapBuff, m_pBuffer + i, sizeof(T));
			memcpy(m_pBuffer + i, m_pBuffer + GetCount() - i - 1, sizeof(T));
			memcpy(m_pBuffer + GetCount() - i - 1, pSwapBuff, sizeof(T));
		}
	}

protected:
	void ConstructObjects(T * pObjects,size_t ObjectCount)
	{
		for(size_t i=0;i<ObjectCount;i++)
		{
			void * pPoint=pObjects+i;

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif

			::new(pPoint) T();

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new new( __FILE__, __LINE__ )
#endif
		}
	}
	void DestructObjects(T * pObjects,size_t ObjectCount)
	{
		for(size_t i=0;i<ObjectCount;i++)
		{
			pObjects[i].~T();
		}
	}
	void ResizeBuffer(size_t NewSize)
	{
		if(NewSize==0)
		{
			Clear();
			return;
		}	
		if(NewSize<=m_BufferSize)
		{
			return;
		}
		T * pNewBuffer=(T *)malloc(sizeof(T)*NewSize);
		if (pNewBuffer)
		{
			size_t CopySize;
			if (NewSize > m_ArrayLength)
				CopySize = m_ArrayLength;
			else
				CopySize = NewSize;
			memcpy(pNewBuffer, m_pBuffer, sizeof(T)*CopySize);
			m_BufferSize = NewSize;
			free(m_pBuffer);
			m_pBuffer = pNewBuffer;
		}
	}	
	void ShrinkBuffer(size_t NewSize)
	{		
		if(NewSize==0)
		{
			Clear();
			return;
		}
		if(NewSize>=m_BufferSize)
		{
			ASSERT_AND_THROW(NewSize<m_BufferSize);
			return;
		}
		T * pNewBuffer=(T *)malloc(sizeof(T)*NewSize);				
		if (pNewBuffer)
		{
			memcpy(pNewBuffer, m_pBuffer, sizeof(T)*m_ArrayLength);
			m_BufferSize = NewSize;
			free(m_pBuffer);
			m_pBuffer = pNewBuffer;
		}
	}
};

#pragma warning (pop)