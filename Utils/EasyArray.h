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


template<typename T, typename COMPARE_FN>
class COMPRE_FN_ADAPTER
{
public:
	static COMPARE_FN* CompareFN;
	static int Compare(const void* p1, const void* p2);
};

template<typename T, typename COMPARE_FN>
COMPARE_FN* COMPRE_FN_ADAPTER<T, COMPARE_FN>::CompareFN = NULL;

template<typename T, typename COMPARE_FN>
int COMPRE_FN_ADAPTER<T, COMPARE_FN>::Compare(const void* p1, const void* p2)
{
	return (*CompareFN)(*((const T*)p1), *((const T*)p2));
}

template < class T >
class CEasyArray
{
protected:
	T* m_pBuffer;
	size_t	m_BufferSize;
	size_t	m_ArrayLength;
	size_t	m_GrowSize;
	LPCTSTR	m_Tag;
	bool	m_IsDummy;
public:
	explicit CEasyArray(LPCTSTR Tag = _T("CEasyArray"))
	{
		m_pBuffer = NULL;
		m_BufferSize = 0;
		m_ArrayLength = 0;
		m_GrowSize = 1;
		m_Tag = Tag;
		m_IsDummy = false;
	}
	CEasyArray(const CEasyArray& Array, LPCTSTR Tag = _T("CEasyArray"))
	{
		m_pBuffer = NULL;
		m_BufferSize = 0;
		m_ArrayLength = 0;
		m_GrowSize = 1;
		m_Tag = Tag;
		*this = Array;
		m_IsDummy = false;
	}
	CEasyArray(CEasyArray&& Array) noexcept
	{
		m_pBuffer = Array.m_pBuffer;
		m_BufferSize = Array.m_BufferSize;
		m_ArrayLength = Array.m_ArrayLength;
		m_GrowSize = Array.m_GrowSize;
		m_Tag = Array.m_Tag;
		m_IsDummy = Array.m_IsDummy;

		Array.m_pBuffer = NULL;
		Array.m_BufferSize = 0;
		Array.m_ArrayLength = 0;
		Array.m_GrowSize = 1;
		Array.m_IsDummy = false;
	}
	explicit CEasyArray(size_t Size, size_t GrowSize = 1, LPCTSTR Tag = _T("CEasyArray"))
	{
		ASSERT_AND_THROW(GrowSize > 0);
		m_pBuffer = NULL;
		m_BufferSize = 0;
		m_ArrayLength = 0;
		m_Tag = Tag;
		m_IsDummy = false;
		if (GrowSize > 1)
			m_GrowSize = GrowSize;
		else
			m_GrowSize = 1;
		ExpandBuffer(Size);
	}
	~CEasyArray()
	{
		if (m_pBuffer && (!m_IsDummy))
		{
			if (m_ArrayLength)
				DestructObjects(m_pBuffer, m_ArrayLength);
			MONITORED_FREE(m_pBuffer);
			m_pBuffer = NULL;
		}
	}
	LPCTSTR GetTag()
	{
		return m_Tag;
	}
	void SetTag(LPCTSTR Tag)
	{
		m_Tag = Tag;
	}
	void SetGrowSize(size_t	GrowSize)
	{
		if (GrowSize > 1)
			m_GrowSize = GrowSize;
		else
			m_GrowSize = 1;
	}
	void Clear()
	{
		if (m_pBuffer)
		{
			if (m_ArrayLength)
			{
				DestructObjects(m_pBuffer, m_ArrayLength);
				m_ArrayLength = 0;
			}
			ShrinkBuffer();
		}
		else
		{
			m_BufferSize = 0;
			m_ArrayLength = 0;
		}
	}
	void Empty()
	{
		if (m_pBuffer && m_ArrayLength)
		{
			DestructObjects(m_pBuffer, m_ArrayLength);
			m_ArrayLength = 0;
		}
	}
	void Create(size_t Size, size_t GrowSize = 1)
	{
		ASSERT_AND_THROW(GrowSize > 0);
		Clear();
		if (m_GrowSize > 1)
			m_GrowSize = GrowSize;
		else
			m_GrowSize = 1;
		ExpandBuffer(Size);
	}

	T* Add(const T& Value)
	{
		if (m_ArrayLength >= m_BufferSize)
		{
			ExpandBuffer(m_GrowSize);
		}
		ConstructObjects(m_pBuffer + m_ArrayLength, 1);
		m_pBuffer[m_ArrayLength] = Value;
		m_ArrayLength++;
		return m_pBuffer + m_ArrayLength - 1;
	}
	T* AddEmpty()
	{
		if (m_ArrayLength >= m_BufferSize)
		{
			ExpandBuffer(m_GrowSize);
		}
		ConstructObjects(m_pBuffer + m_ArrayLength, 1);
		m_ArrayLength++;
		return m_pBuffer + m_ArrayLength - 1;
	}
	void AddArray(const CEasyArray<T>& Array)
	{
		if (m_ArrayLength + Array.GetCount() >= m_BufferSize)
		{
			ExpandBuffer(Array.GetCount());
		}
		ConstructObjects(m_pBuffer + m_ArrayLength, Array.GetCount());
		for (size_t i = 0; i < Array.GetCount(); i++)
		{
			m_pBuffer[m_ArrayLength] = Array.GetObjectConst(i);
			m_ArrayLength++;
		}
	}
	void AddArray(const T* pArray, size_t Count)
	{
		if (m_ArrayLength + Count >= m_BufferSize)
		{
			ExpandBuffer(Count);
		}
		ConstructObjects(m_pBuffer + m_ArrayLength, Count);
		for (size_t i = 0; i < Count; i++)
		{
			m_pBuffer[m_ArrayLength] = pArray[i];
			m_ArrayLength++;
		}
	}
	T* Insert(size_t BeforeIndex, const T& Value)
	{
		if (BeforeIndex <= m_ArrayLength)
		{
			if (m_ArrayLength >= m_BufferSize)
			{
				ExpandBuffer(m_GrowSize);
			}
			if (m_ArrayLength - BeforeIndex)
				memmove(m_pBuffer + BeforeIndex + 1, m_pBuffer + BeforeIndex, sizeof(T) * (m_ArrayLength - BeforeIndex));
			ConstructObjects(m_pBuffer + BeforeIndex, 1);
			m_pBuffer[BeforeIndex] = Value;
			m_ArrayLength++;
			return m_pBuffer + BeforeIndex;
		}
		else
		{
			return Add(Value);
		}
	}
	T* InsertEmpty(size_t BeforeIndex)
	{
		if (BeforeIndex <= m_ArrayLength)
		{
			if (m_ArrayLength >= m_BufferSize)
			{
				ExpandBuffer(m_GrowSize);
			}
			if (m_ArrayLength - BeforeIndex)
				memmove(m_pBuffer + BeforeIndex + 1, m_pBuffer + BeforeIndex, sizeof(T) * (m_ArrayLength - BeforeIndex));
			ConstructObjects(m_pBuffer + BeforeIndex, 1);
			m_ArrayLength++;
			return m_pBuffer + BeforeIndex;
		}
		else
		{
			return AddEmpty();
		}
	}
	bool Insert(size_t BeforeIndex, const CEasyArray<T>& Array)
	{
		if (BeforeIndex <= m_ArrayLength && Array.GetCount())
		{
			if (m_ArrayLength + Array.GetCount() > m_BufferSize)
			{
				ExpandBuffer(Array.GetCount());
			}
			if (m_ArrayLength - BeforeIndex)
				memmove(m_pBuffer + BeforeIndex + Array.GetCount(), m_pBuffer + BeforeIndex, sizeof(T) * (m_ArrayLength - BeforeIndex));
			ConstructObjects(m_pBuffer + BeforeIndex, Array.GetCount());
			for (size_t i = 0; i < Array.GetCount(); i++)
			{
				m_pBuffer[BeforeIndex + i] = Array.GetObjectConst(i);
			}
			m_ArrayLength += Array.GetCount();
		}
		else
		{
			AddArray(Array);
		}
	}
	bool Delete(size_t Index, size_t Count = 1)
	{
		if (Index < m_ArrayLength)
		{
			if (Count > m_ArrayLength - Index)
				Count = m_ArrayLength - Index;
			DestructObjects(m_pBuffer + Index, Count);
			if (m_ArrayLength - Index - Count)
				memmove(m_pBuffer + Index, m_pBuffer + Index + Count, sizeof(T) * (m_ArrayLength - Index - Count));
			m_ArrayLength -= Count;

			ShrinkBuffer();

			return true;
		}
		return false;
	}
	bool MoveBefore(size_t Index, size_t Before)
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
				memmove(m_pBuffer + Index, m_pBuffer + Index + 1, sizeof(T) * (Before - Index - 1));
				memcpy(m_pBuffer + Before - 1, Temp, sizeof(T));
			}
			else
			{
				memmove(m_pBuffer + Before + 1, m_pBuffer + Before, sizeof(T) * (Index - Before));
				memcpy(m_pBuffer + Before, Temp, sizeof(T));
			}
			return true;
		}
		return false;
	}
	bool MoveAfter(size_t Index, size_t After)
	{
		if (Index < m_ArrayLength)
		{
			if (After > m_ArrayLength - 1)
				After = m_ArrayLength - 1;
			if (Index == After || Index - 1 == After)
				return true;
			BYTE Temp[sizeof(T)];
			memcpy(Temp, m_pBuffer + Index, sizeof(T));
			if (Index < After)
			{
				memmove(m_pBuffer + Index, m_pBuffer + Index + 1, sizeof(T) * (After - Index + 1));
				memcpy(m_pBuffer + After, Temp, sizeof(T));
			}
			else
			{
				memmove(m_pBuffer + After + 1, m_pBuffer + After + 2, sizeof(T) * (Index - After));
				memcpy(m_pBuffer + After + 1, Temp, sizeof(T));
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
		if (NewLength <= m_BufferSize)
		{
			m_ArrayLength = NewLength;
			return true;
		}
		return false;
	}
	size_t GetBufferLength() const
	{
		return m_BufferSize;
	}
	T* GetBuffer()
	{
		return m_pBuffer;
	}
	const T* GetBuffer() const
	{
		return m_pBuffer;
	}
	T* GetObject(size_t Index)
	{
		if (Index < m_ArrayLength)
		{
			return m_pBuffer + Index;
		}
		return NULL;
	}
	const T* GetObject(size_t Index) const
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
		ASSERT_AND_THROW(Index < m_ArrayLength);
#endif
		return m_pBuffer[Index];
	}
	void Resize(size_t NewSize)
	{
		if (NewSize)
		{
			if (NewSize < m_ArrayLength)
			{
				DestructObjects(m_pBuffer + NewSize, m_ArrayLength - NewSize);
				m_ArrayLength = NewSize;
				ShrinkBuffer();
			}
			else if (NewSize > m_ArrayLength)
			{
				ExpandBuffer(NewSize - m_ArrayLength);
				ConstructObjects(m_pBuffer + m_ArrayLength, NewSize - m_ArrayLength);
				m_ArrayLength = NewSize;
			}
		}
		else
		{
			Clear();
		}
	}
	T* GetFirst()
	{
		if (m_ArrayLength)
			return m_pBuffer;
		return NULL;
	}
	const T* GetFirst() const
	{
		if (m_ArrayLength)
			return m_pBuffer;
		return NULL;
	}
	T* GetLast()
	{
		if (m_ArrayLength)
			return m_pBuffer + m_ArrayLength - 1;
		return NULL;
	}
	const T* GetLast() const
	{
		if (m_ArrayLength)
			return m_pBuffer + m_ArrayLength - 1;
		return NULL;
	}
	void Reserve(size_t NewSize)
	{
		if (NewSize > m_BufferSize)
			ExpandBuffer(NewSize - m_BufferSize);
	}
	const CEasyArray& operator=(const CEasyArray& Array)
	{
		Empty();
		if (m_ArrayLength < Array.m_ArrayLength)
			Resize(Array.m_ArrayLength);
		m_ArrayLength = Array.m_ArrayLength;
		for (size_t i = 0; i < m_ArrayLength; i++)
		{
			m_pBuffer[i] = Array.m_pBuffer[i];
		}
		return *this;
	}
	T& operator[](size_t Index)
	{
		ASSERT_AND_THROW(Index < m_ArrayLength);
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
			ExpandBuffer(m_GrowSize);
		}
		T* pSwapBuff = m_pBuffer + GetCount();
		for (UINT i = 0; i < GetCount() / 2; i++)
		{
			memcpy(pSwapBuff, m_pBuffer + i, sizeof(T));
			memcpy(m_pBuffer + i, m_pBuffer + GetCount() - i - 1, sizeof(T));
			memcpy(m_pBuffer + GetCount() - i - 1, pSwapBuff, sizeof(T));
		}
	}

	const CEasyArray<T> SubDummy(size_t Start, size_t Length = -1)
	{
		CEasyArray Dummy;
		if (Start + 1 < m_ArrayLength)
		{
			if (Length > m_ArrayLength - Start)
				Length = m_ArrayLength - Start;
			Dummy.m_pBuffer = m_pBuffer + Start;
			Dummy.m_BufferSize = Length;
			Dummy.m_ArrayLength = Length;
			Dummy.m_Tag = m_Tag;
			Dummy.m_IsDummy = true;
		}
		return Dummy;
	}

	T* begin()
	{
		return m_pBuffer;
	}

	T* end()
	{
		return m_pBuffer + m_ArrayLength;
	}

	const T* begin() const
	{
		return m_pBuffer;
	}

	const T* end() const
	{
		return m_pBuffer + m_ArrayLength;
	}

	bool AddUnique(const T& Value)
	{
		for (T& Data : *this)
		{
			if (Data == Value)
				return false;
		}
		Add(Value);
		return true;
	}

	template<typename EQUAL_FN>
	bool AddUnique(const T& Value, EQUAL_FN EqualFN)
	{
		for (T& Data : *this)
		{
			if (EqualFN(Data, Value))
				return false;
		}
		Add(Value);
		return true;
	}

	inline void AddSorted(const T& Value)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			if (m_pBuffer[Mid] < Value)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		Insert(Start, Value);
	}

	template<typename COMPARE_FN>
	inline void AddSorted(const T& Value, COMPARE_FN CompareFN)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid], Value);
			if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		Insert(Start, Value);
	}

	inline bool AddSortedUniqued(const T& Value)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			if (m_pBuffer[Mid] == Value)
			{
				return false;
			}
			else if (m_pBuffer[Mid] < Value)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		Insert(Start, Value);
		return true;
	}

	template<typename COMPARE_FN>
	inline bool AddSortedUniqued(const T& Value, COMPARE_FN CompareFN)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid], Value);
			if (Result == 0)
			{
				return false;
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		Insert(Start, Value);
	}

	inline size_t Replace(const T& OldValue, const T& NewValue)
	{
		size_t Count = 0;
		for (T& Data : (*this))
		{
			if (Data == OldValue)
			{
				Data = NewValue;
				Count++;
			}
		}
		return Count;
	}

	template<typename EQUAL_FN>
	inline size_t Replace(const T& OldValue, const T& NewValue, EQUAL_FN EqualFN)
	{
		size_t Count = 0;
		for (T& Data : (*this))
		{
			if (EqualFN(Data, OldValue))
			{
				Data = NewValue;
				Count++;
			}
		}
		return Count;
	}

	template<typename EQUAL_FN>
	inline size_t Replace(const T& NewValue, EQUAL_FN EqualFN)
	{
		size_t Count = 0;
		for (T& Data : (*this))
		{
			if (EqualFN(Data))
			{
				Data = NewValue;
				Count++;
			}
		}
		return Count;
	}

	template<typename COMPARE_FN>
	inline void QuickSort(COMPARE_FN CompareFN)
	{
		COMPRE_FN_ADAPTER<T, COMPARE_FN>::CompareFN = &CompareFN;
		qsort(GetBuffer(), GetCount(), sizeof(T), COMPRE_FN_ADAPTER<T, COMPARE_FN>::Compare);
	}

	inline bool Remove(const T& Value)
	{
		for (size_t i = 0; i < m_ArrayLength; i++)
		{
			if (m_pBuffer[i] == Value)
			{
				Delete(i);
				return true;
			}

		}
		return false;
	}

	template<typename EQUAL_FN>
	inline bool Remove(const T& Value, EQUAL_FN EqualFN)
	{
		for (UINT i = 0; i < m_ArrayLength; i++)
		{
			if (EqualFN(m_pBuffer[i], Value))
			{
				Delete(i);
				return true;
			}

		}
		return false;
	}

	template<typename EQUAL_FN>
	inline bool Remove(EQUAL_FN EqualFN)
	{
		for (UINT i = 0; i < m_ArrayLength; i++)
		{
			if (EqualFN(m_pBuffer[i]))
			{
				Delete(i);
				return true;
			}

		}
		return false;
	}

	inline bool RemoveBinary(const T& Value)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			if (m_pBuffer[Mid] == Value)
			{
				Delete(Mid);
				return true;
			}
			else if (m_pBuffer[Mid] < Value)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return false;
	}

	template<typename COMPARE_FN>
	inline bool RemoveBinary(const T& Value, COMPARE_FN CompareFN)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid], Value);
			if (Result == 0)
			{
				Delete(Mid);
				return true;
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return false;
	}

	template<typename COMPARE_FN>
	inline bool RemoveBinary(COMPARE_FN CompareFN)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid]);
			if (Result == 0)
			{
				Delete(Mid);
				return true;
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return false;
	}

	inline size_t RemoveAll(const T& Value)
	{
		size_t Count = 0;
		for (int i = (int)m_ArrayLength - 1; i >= 0; i--)
		{
			if (m_pBuffer[i] == Value)
			{
				Delete(i);
				Count++;
			}
		}
		return Count;
	}

	template<typename EQUAL_FN>
	inline size_t RemoveAll(const T& Value, EQUAL_FN EqualFN)
	{
		size_t Count = 0;
		for (int i = (int)m_ArrayLength - 1; i >= 0; i--)
		{
			if (EqualFN(m_pBuffer[i], Value))
			{
				Delete(i);
				Count++;
			}
		}
		return Count;
	}

	template<typename EQUAL_FN>
	inline size_t RemoveAll(EQUAL_FN EqualFN)
	{
		size_t Count = 0;
		for (int i = (int)m_ArrayLength - 1; i >= 0; i--)
		{
			if (EqualFN(m_pBuffer[i]))
			{
				Delete(i);
				Count++;
			}
		}
		return Count;
	}

	inline bool RemoveAllBinary(const T& Value)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			if (m_pBuffer[Mid] == Value)
			{
				int Head = Mid;
				while (Head > 0 && m_pBuffer[Head - 1] == Value);
				Head--;
				int Tail = Mid + 1;
				while (Tail < (int)m_ArrayLength && m_pBuffer[Tail] == Value)
					Tail++;
				Delete(Head, Tail - Head);
				return true;
			}
			else if (m_pBuffer[Mid] < Value)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return false;
	}

	template<typename COMPARE_FN>
	inline bool RemoveAllBinary(const T& Value, COMPARE_FN CompareFN)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid], Value);
			if (Result == 0)
			{
				int Head = Mid;
				while (Head > 0 && (CompareFN(m_pBuffer[Head - 1], Value) == 0));
				Head--;
				int Tail = Mid + 1;
				while (Tail < (int)m_ArrayLength && (CompareFN(m_pBuffer[Tail], Value) == 0))
					Tail++;
				Delete(Head, Tail - Head);
				return true;
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return false;
	}

	template<typename COMPARE_FN>
	inline bool RemoveAllBinary(COMPARE_FN CompareFN)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid]);
			if (Result == 0)
			{
				int Head = Mid;
				while (Head > 0 && (CompareFN(m_pBuffer[Head - 1]) == 0));
				Head--;
				int Tail = Mid + 1;
				while (Tail < (int)m_ArrayLength && (CompareFN(m_pBuffer[Tail]) == 0))
					Tail++;
				Delete(Head, Tail - Head);
				return true;
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return false;
	}

	inline bool Exist(const T& Value) const
	{
		for (const T& Item : (*this))
		{
			if (Item == Value)
			{
				return true;
			}

		}
		return false;
	}

	template<typename EQUAL_FN>
	inline bool Exist(const T& Value, EQUAL_FN EqualFN) const
	{
		for (const T& Item : (*this))
		{
			if (EqualFN(Item, Value))
			{
				return true;
			}

		}
		return false;
	}

	template<typename EQUAL_FN>
	inline bool Exist(EQUAL_FN EqualFN) const
	{
		for (const T& Item : (*this))
		{
			if (EqualFN(Item))
			{
				return true;
			}

		}
		return false;
	}

	inline bool ExistBinary(const T& Value) const
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			if (m_pBuffer[Mid] == Value)
			{
				return true;
			}
			else if (m_pBuffer[Mid] < Value)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return false;
	}

	template<typename COMPARE_FN>
	inline bool ExistBinary(const T& Value, COMPARE_FN CompareFN) const
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid], Value);
			if (Result == 0)
			{
				return true;
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return false;
	}

	template<typename COMPARE_FN>
	inline bool ExistBinary(COMPARE_FN CompareFN) const
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid]);
			if (Result == 0)
			{
				return true;
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return false;
	}

	inline T* Find(const T& Value)
	{
		for (T& Item : (*this))
		{
			if (Item == Value)
			{
				return &Item;
			}

		}
		return NULL;
	}

	template<typename EQUAL_FN>
	inline T* Find(const T& Value, EQUAL_FN EqualFN)
	{
		for (T& Item : (*this))
		{
			if (EqualFN(Item, Value))
			{
				return &Item;
			}

		}
		return NULL;
	}

	template<typename EQUAL_FN>
	inline T* Find(EQUAL_FN EqualFN)
	{
		for (T& Item : (*this))
		{
			if (EqualFN(Item))
			{
				return &Item;
			}

		}
		return NULL;
	}

	inline const T* Find(const T& Value) const
	{
		for (const T& Item : (*this))
		{
			if (Item == Value)
			{
				return &Item;
			}

		}
		return NULL;
	}

	template<typename EQUAL_FN>
	inline const T* Find(const T& Value, EQUAL_FN EqualFN) const
	{
		for (const T& Item : (*this))
		{
			if (EqualFN(Item, Value))
			{
				return &Item;
			}

		}
		return NULL;
	}

	template<typename EQUAL_FN>
	inline const T* Find(EQUAL_FN EqualFN) const
	{
		for (const T& Item : (*this))
		{
			if (EqualFN(Item))
			{
				return &Item;
			}

		}
		return NULL;
	}

	inline T* FindBinary(const T& Value)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			if (m_pBuffer[Mid] == Value)
			{
				return m_pBuffer[Mid];
			}
			else if (m_pBuffer[Mid] < Value)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return NULL;
	}

	template<typename COMPARE_FN>
	inline T* FindBinary(const T& Value, COMPARE_FN CompareFN)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid], Value);
			if (Result == 0)
			{
				return m_pBuffer[Mid];
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return NULL;
	}

	template<typename COMPARE_FN>
	inline T* FindBinary(COMPARE_FN CompareFN)
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid]);
			if (Result == 0)
			{
				return m_pBuffer[Mid];
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return NULL;
	}

	inline const T* FindBinary(const T& Value) const
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			if (m_pBuffer[Mid] == Value)
			{
				return m_pBuffer[Mid];
			}
			else if (m_pBuffer[Mid] < Value)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return NULL;
	}

	template<typename COMPARE_FN>
	inline const T* FindBinary(const T& Value, COMPARE_FN CompareFN) const
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid], Value);
			if (Result == 0)
			{
				return m_pBuffer[Mid];
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return NULL;
	}

	template<typename COMPARE_FN>
	inline const T* FindBinary(COMPARE_FN CompareFN) const
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid]);
			if (Result == 0)
			{
				return m_pBuffer[Mid];
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return NULL;
	}

	inline int FindIndex(const T& Value) const
	{
		for (size_t i = 0; i < m_ArrayLength; i++)
		{
			if (m_pBuffer[i] == Value)
			{
				return (int)i;
			}

		}
		return -1;
	}

	template<typename EQUAL_FN>
	inline int FindIndex(const T& Value, EQUAL_FN EqualFN) const
	{
		for (size_t i = 0; i < m_ArrayLength; i++)
		{
			if (EqualFN(m_pBuffer[i], Value))
			{
				return (int)i;
			}

		}
		return -1;
	}

	template<typename EQUAL_FN>
	inline int FindIndex(EQUAL_FN EqualFN) const
	{
		for (size_t i = 0; i < m_ArrayLength; i++)
		{
			if (EqualFN(m_pBuffer[i]))
			{
				return (int)i;
			}

		}
		return -1;
	}

	inline int FindIndexBinary(const T& Value) const
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			if (m_pBuffer[Mid] == Value)
			{
				return Mid;
			}
			else if (m_pBuffer[Mid] < Value)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return -1;
	}

	template<typename COMPARE_FN>
	inline int FindIndexBinary(const T& Value, COMPARE_FN CompareFN) const
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid], Value);
			if (Result == 0)
			{
				return Mid;
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return -1;
	}

	template<typename COMPARE_FN>
	inline int FindIndexBinary(COMPARE_FN CompareFN) const
	{
		int Start = 0;
		int End = (int)m_ArrayLength - 1;
		while (Start <= End)
		{
			int Mid = (Start + End) / 2;
			int Result = CompareFN(m_pBuffer[Mid]);
			if (Result == 0)
			{
				return Mid;
			}
			else if (Result < 0)
			{
				Start = Mid + 1;
			}
			else
			{
				End = Mid - 1;
			}
		}
		return NULL;
	}

	template<typename TargetType, typename MAP_FN>
	inline void Map(CEasyArray<TargetType>& TargetList, MAP_FN MapFN) const
	{
		for (const T& Item : (*this))
		{
			TargetList.Add(MapFN(Item));
		}
	}

	template<typename TargetType, typename MAP_FN>
	inline CEasyArray<TargetType> Map(MAP_FN MapFN) const
	{
		CEasyArray<TargetType> TargetList;
		for (const T& Item : (*this))
		{
			TargetList.Add(MapFN(Item));
		}
		return TargetList;
	}

protected:
	void ConstructObjects(T* pObjects, size_t ObjectCount)
	{
		for (size_t i = 0; i < ObjectCount; i++)
		{
			void* pPoint = pObjects + i;

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif

			::new(pPoint) T();

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
		}
	}
	void DestructObjects(T* pObjects, size_t ObjectCount)
	{
		for (size_t i = 0; i < ObjectCount; i++)
		{
			pObjects[i].~T();
		}
	}
	void ExpandBuffer(size_t ExpandSize)
	{
		if (ExpandSize)
		{
			size_t NewSize = (ExpandSize / m_GrowSize) * m_GrowSize;
			if (NewSize < ExpandSize)
				NewSize += m_GrowSize;
			NewSize += m_BufferSize;
			T* pNewBuffer = (T*)MONITORED_MALLOC(GetTag(), sizeof(T) * NewSize);
			if (pNewBuffer)
			{
				size_t CopySize;
				if (NewSize > m_ArrayLength)
					CopySize = m_ArrayLength;
				else
					CopySize = NewSize;
				if (m_pBuffer)
				{
					memcpy(pNewBuffer, m_pBuffer, sizeof(T) * CopySize);
					MONITORED_FREE(m_pBuffer);
				}
				m_BufferSize = NewSize;
				m_pBuffer = pNewBuffer;
			}
		}
	}
	void ShrinkBuffer()
	{
		if (m_BufferSize)
		{
			if (m_BufferSize - m_ArrayLength > m_GrowSize)
			{
				size_t NewSize = (m_ArrayLength / m_GrowSize) * m_GrowSize;
				if (m_GrowSize > 1 && NewSize < m_GrowSize)
					NewSize = m_GrowSize;
				if (NewSize < m_ArrayLength)
					NewSize += m_GrowSize;
				if (NewSize)
				{
					T* pNewBuffer = (T*)MONITORED_MALLOC(GetTag(), sizeof(T) * NewSize);
					if (pNewBuffer)
					{
						memcpy(pNewBuffer, m_pBuffer, sizeof(T) * m_ArrayLength);
						m_BufferSize = NewSize;
						MONITORED_FREE(m_pBuffer);
						m_pBuffer = pNewBuffer;
					}
				}
				else
				{
					MONITORED_FREE(m_pBuffer);
					m_pBuffer = NULL;
					m_BufferSize = 0;
					m_ArrayLength = 0;
				}
			}
		}
	}
};



















#pragma warning (pop)
