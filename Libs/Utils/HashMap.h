#pragma once

class CHashCodeMaker
{
public:
	template < typename T>
	static UINT GetHashCode(const T& Value);
};

template <>
inline UINT CHashCodeMaker::GetHashCode<char>(const char& Value)
{
	return (UINT)Value;
}

template <>
inline UINT CHashCodeMaker::GetHashCode<unsigned char>(const unsigned char& Value)
{
	return (UINT)Value;
}

template <>
inline UINT CHashCodeMaker::GetHashCode<short>(const short& Value)
{
	return (UINT)Value;
}

template <>
inline UINT CHashCodeMaker::GetHashCode<unsigned short>(const unsigned short& Value)
{
	return (UINT)Value;
}

template <>
inline UINT CHashCodeMaker::GetHashCode<int>(const int& Value)
{
	return (UINT)Value;
}

template <>
inline UINT CHashCodeMaker::GetHashCode<unsigned int>(const unsigned int& Value)
{
	return (UINT)Value;
}

template <>
inline UINT CHashCodeMaker::GetHashCode<__int64>(const __int64& Value)
{
	return ((UINT)((unsigned __int64)Value & 0xFFFFFFFF)) ^ ((UINT)((unsigned __int64)Value >> 32));
}

template <>
inline UINT CHashCodeMaker::GetHashCode<unsigned __int64>(const unsigned __int64& Value)
{
	return ((UINT)(Value & 0xFFFFFFFF)) ^ ((UINT)(Value >> 32));
}

template <>
inline UINT CHashCodeMaker::GetHashCode<float>(const float& Value)
{
	return *((UINT *)(&Value));
}

template <>
inline UINT CHashCodeMaker::GetHashCode<double>(const double& Value)
{
	const unsigned __int64 * pValue = (const unsigned __int64 *)(&Value);
	return ((UINT)((*pValue) & 0xFFFFFFFF)) ^ ((UINT)((*pValue) >> 32));
}

template <typename T>
inline UINT CHashCodeMaker::GetHashCode(const T& Value)
{
	return Value.GetHashCode();
}

template < class KEY, class T, int StorageMode = EDSM_STATIC>
class CHashMap
{
protected:
	template < int Mode, class OBJECT_TYPE>
	struct _StorageNode
	{
		UINT			ID;
		_StorageNode *	pPrev;
		_StorageNode *	pNext;
		_StorageNode *	pFront;
		_StorageNode *	pBack;
		KEY				Key;
		bool			IsUsed;
		bool			IsFree;
		void InitObject()
		{
		}
		OBJECT_TYPE& GetObjectRef()
		{
			return Object;
		}
		OBJECT_TYPE * GetObjectPointer()
		{
			return &Object;
		}
		const OBJECT_TYPE& GetObjectRef() const
		{
			return Object;
		}
		const OBJECT_TYPE * GetObjectPointer() const
		{
			return &Object;
		}
		void NewObject()
		{

		}
		void DeleteObject()
		{

		}
		void FinalReleaseObject()
		{

		}
	protected:
		OBJECT_TYPE Object;
	};

	template < class OBJECT_TYPE>
	struct _StorageNode < EDSM_NEW_ONCE, OBJECT_TYPE >
	{
		UINT			ID;
		_StorageNode *	pPrev;
		_StorageNode *	pNext;
		_StorageNode *	pFront;
		_StorageNode *	pBack;
		KEY				Key;
		bool			IsUsed;
		bool			IsFree;
		void InitObject()
		{
			pObject = NULL;
		}
		OBJECT_TYPE& GetObjectRef()
		{
			return *pObject;
		}
		OBJECT_TYPE * GetObjectPointer()
		{
			return pObject;
		}
		const OBJECT_TYPE& GetObjectRef() const
		{
			return *pObject;
		}
		const OBJECT_TYPE * GetObjectPointer() const
		{
			return pObject;
		}
		void NewObject()
		{
			if (pObject == NULL)
				pObject = new OBJECT_TYPE;
		}
		void DeleteObject()
		{

		}
		void FinalReleaseObject()
		{
			SAFE_DELETE(pObject);
		}
	protected:
		OBJECT_TYPE *	pObject;
	};


	template < class OBJECT_TYPE>
	struct _StorageNode < EDSM_NEW_EVERY_TIME, OBJECT_TYPE >
	{
		UINT			ID;
		_StorageNode *	pPrev;
		_StorageNode *	pNext;
		_StorageNode *	pFront;
		_StorageNode *	pBack;
		KEY				Key;
		bool			IsUsed;
		bool			IsFree;
		void InitObject()
		{
			pObject = NULL;
		}
		OBJECT_TYPE& GetObjectRef()
		{
			return *pObject;
		}
		OBJECT_TYPE * GetObjectPointer()
		{
			return pObject;
		}
		const OBJECT_TYPE& GetObjectRef() const
		{
			return *pObject;
		}
		const OBJECT_TYPE * GetObjectPointer() const
		{
			return pObject;
		}
		void NewObject()
		{
			if (pObject == NULL)
				pObject = new OBJECT_TYPE;
		}
		void DeleteObject()
		{
			SAFE_DELETE(pObject);
		}
		void FinalReleaseObject()
		{
			SAFE_DELETE(pObject);
		}
	protected:
		OBJECT_TYPE *	pObject;
	};

	typedef _StorageNode<StorageMode, T> StorageNode;
	typedef StorageNode * LPSTORAGENODE;

	struct OBJECT_BUFF_PAGE_INFO
	{
		StorageNode *	pObjectBuffer;
		UINT			BufferSize;
	};

	const float BUFFER_GROW_FACTOR = 0.7f;

	CEasyArray<OBJECT_BUFF_PAGE_INFO>		m_ObjectBuffPages;
	UINT									m_BufferSize;
	StorageNode *							m_pFreeListHead;
	StorageNode *							m_pFreeListTail;
	StorageNode *							m_pObjectListHead;
	StorageNode *							m_pObjectListTail;
	UINT									m_ObjectCount;
	UINT									m_UsedObjectCount;
	UINT									m_GrowLimit;

	LPSTORAGENODE *							m_HashTable;
	UINT									m_HashTableLen;
	UINT									m_HashMask;
public:
	typedef list_iterator<CHashMap, T> iterator;
	typedef const_list_iterator<CHashMap, T> const_iterator;
public:
	CHashMap()
	{
		m_BufferSize = 0;
		m_pFreeListHead = NULL;
		m_pFreeListTail = NULL;
		m_pObjectListHead = NULL;
		m_pObjectListTail = NULL;
		m_ObjectCount = 0;
		m_UsedObjectCount = 0;
		m_GrowLimit = 0;
		m_HashTable = NULL;
		m_HashTableLen = 0;
		m_HashMask = 0;
	}
	CHashMap(UINT Size, UINT GrowLimit = 0)
	{
		m_BufferSize = 0;
		m_pFreeListHead = NULL;
		m_pFreeListTail = NULL;
		m_pObjectListHead = NULL;
		m_pObjectListTail = NULL;
		m_ObjectCount = 0;
		m_UsedObjectCount = 0;
		m_GrowLimit = 0;
		m_HashTable = NULL;
		m_HashTableLen = 0;
		m_HashMask = 0;
		Create(Size, GrowLimit);
	}
	~CHashMap()
	{
		Destory();
	}
	bool Create(UINT Size, UINT GrowLimit = 0)
	{
		Destory();
		if (Size > 1)
		{
			UINT RealSize = 1;
			while (RealSize < Size)
			{
				RealSize = RealSize << 1;
			}
			m_GrowLimit = GrowLimit;
			if (CreateBufferPage(RealSize))
			{
				RebuildHashTable();
				return true;
			}
		}
		return true;
	}
	bool Create(const STORAGE_POOL_SETTING& PoolSetting)
	{
		return Create(PoolSetting.StartSize, PoolSetting.StartSize + PoolSetting.GrowSize*PoolSetting.GrowLimit);
	}
	UINT GetBufferSize() const
	{
		return m_BufferSize;
	}
	bool IsCreated()
	{
		return m_ObjectBuffPages.GetCount() != 0;
	}
	void Destory()
	{
		for (UINT i = 0; i < m_ObjectBuffPages.GetCount(); i++)
		{
			for (UINT j = 0; j < m_ObjectBuffPages[i].BufferSize; j++)
			{
				m_ObjectBuffPages[i].pObjectBuffer[j].FinalReleaseObject();
			}
			SAFE_DELETE_ARRAY(m_ObjectBuffPages[i].pObjectBuffer);
		}
		m_ObjectBuffPages.Clear();
		m_BufferSize = 0;
		m_pFreeListHead = NULL;
		m_pFreeListTail = NULL;
		m_pObjectListHead = NULL;
		m_pObjectListTail = NULL;
		m_ObjectCount = 0;
		m_GrowLimit = 0;
		SAFE_DELETE_ARRAY(m_HashTable);
		m_HashTableLen = 0;
		m_HashMask = 0;
	}
	void Clear()
	{
		m_pFreeListHead = NULL;
		m_pFreeListTail = NULL;
		m_pObjectListHead = NULL;
		m_pObjectListTail = NULL;
		m_ObjectCount = 0;

		UINT IDStart = 1;
		for (UINT i = 0; i < m_ObjectBuffPages.GetCount(); i++)
		{
			ClearBuffer(m_ObjectBuffPages[i], IDStart, false);
			IDStart += m_ObjectBuffPages[i].BufferSize;
		}
		ZeroMemory(m_HashTable, sizeof(LPSTORAGENODE)*m_HashTableLen);		
	}
	void ClearToDefault()
	{
		m_pFreeListHead = NULL;
		m_pFreeListTail = NULL;
		m_pObjectListHead = NULL;
		m_pObjectListTail = NULL;
		m_ObjectCount = 0;

		UINT IDStart = 1;
		if (m_ObjectBuffPages.GetCount()>1)
		{
			for (UINT i = 1; i < m_ObjectBuffPages.GetCount(); i++)
			{
				for (UINT j = 0; j < m_ObjectBuffPages[i].BufferSize; j++)
				{
					m_ObjectBuffPages[i].pObjectBuffer[j].FinalReleaseObject();
				}
				SAFE_DELETE_ARRAY(m_ObjectBuffPages[i].pObjectBuffer);
			}
			m_ObjectBuffPages.Resize(1);
		}
		for (UINT i = 0; i < m_ObjectBuffPages.GetCount(); i++)
		{
			ClearBuffer(m_ObjectBuffPages[i], IDStart, false);
			IDStart += m_ObjectBuffPages[i].BufferSize;
		}
		CaculateBufferSize();
		RebuildHashTable();
	}
	bool CheckGrow()
	{
		if ((float)m_ObjectCount / (float)m_BufferSize> BUFFER_GROW_FACTOR)
		{
			if ((GetBufferSize() << 1) < m_GrowLimit)
			{
				if (CreateBufferPage(GetBufferSize()))
				{
					RebuildHashTable();
					return true;
				}
			}
		}
		return false;
	}
	UINT New(const KEY& Key, T** ppValue)
	{
		StorageNode * pNode = InsertNode(Key, NULL);
		if (pNode)
		{
			*ppValue = pNode->GetObjectPointer();			
			return pNode->ID;
		}
		return 0;
	}
	UINT Insert(const KEY& Key, const T& Value)
	{
		StorageNode * pNode = InsertNode(Key, NULL);
		if (pNode)
		{
			pNode->GetObjectRef() = Value;			
			return pNode->ID;
		}
		return 0;
	}
	bool Delete(const KEY& Key)
	{
		return DeleteNode(Key);
	}	
	T * Find(const KEY& Key)
	{
		StorageNode * pNode = FindNode(Key);
		if (pNode)
		{
			return pNode->GetObjectPointer();
		}
		return NULL;
	}
	const T * Find(const KEY& Key) const
	{
		const StorageNode * pNode = FindNode(Key);
		if (pNode)
		{
			return pNode->GetObjectPointer();
		}
		return NULL;
	}
	void * FinPos(const KEY& Key) const
	{
		return FindNode(Key);
	}
	T* GetObject(LPVOID Pos)
	{
		StorageNode * pNode = (StorageNode *)Pos;
		if (Pos)
			return pNode->GetObjectPointer();
		else
			return NULL;
	}
	const T* GetObject(LPVOID Pos) const
	{
		const StorageNode * pNode = (const StorageNode *)Pos;
		if (Pos)
			return pNode->GetObjectPointer();
		else
			return NULL;
	}
	T* GetObject(UINT ID)
	{
		return GetObject(GetObjectPosByID(ID));
	}
	const T* GetObject(UINT ID) const
	{
		return GetObject(GetObjectPosByID(ID));
	}
	LPVOID GetObjectPosByID(UINT ID) const
	{
		if (ID == 0)
			return NULL;

		ID--;
		for (UINT i = 0; i < m_ObjectBuffPages.GetCount(); i++)
		{
			if (ID < m_ObjectBuffPages[i].BufferSize)
			{
				if (!m_ObjectBuffPages[i].pObjectBuffer[ID].IsFree)
					return &(m_ObjectBuffPages[i].pObjectBuffer[ID]);
				return NULL;
			}
			ID -= m_ObjectBuffPages[i].BufferSize;
		}
		return NULL;
	}
	T& operator[](const KEY& Key)
	{
		StorageNode * pNode = FindNode(Key);
		if (pNode == NULL)
		{
			pNode = InsertNode(Key, NULL);			
		}
		return pNode->Object;
	}

	UINT GetObjectCount() const
	{
		return m_ObjectCount;
	}
	UINT GetUsedObjectCount() const
	{
		return m_UsedObjectCount;
	}
	LPVOID GetFirstObjectPos() const
	{
		return m_pObjectListHead;
	}

	LPVOID GetLastObjectPos() const
	{
		return m_pObjectListTail;
	}

	LPVOID GetNextObjectPos(LPVOID Pos) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			return pNode->pNext;
		}
		return NULL;
	}
	LPVOID GetPrevObjectPos(LPVOID Pos) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			return pNode->pPrev;
		}
		return NULL;
	}

	T * GetNextObject(LPVOID& Pos, KEY& Key) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (StorageNode *)Pos;
			Pos = pNode->pNext;
			Key = pNode->Key;
			return pNode->GetObjectPointer();
		}
		return NULL;
	}

	T * GetPrevObject(LPVOID& Pos, KEY& Key) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (StorageNode *)Pos;
			Pos = pNode->pPrev;
			Key = pNode->Key;
			return pNode->GetObjectPointer();
		}
	}
	T * GetNextObject(LPVOID& Pos)
	{
		if (Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			Pos = pNode->pNext;
			return pNode->GetObjectPointer();
		}
		return NULL;
	}

	T * GetPrevObject(LPVOID& Pos)
	{
		if (Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			Pos = pNode->pPrev;
			return pNode->GetObjectPointer();
		}
		return NULL;
	}	
	
	void Verfy(UINT& ObjectCount, UINT& HashCount) const
	{
		ObjectCount = 0;
		HashCount = 0;

		const StorageNode * pNode = m_pObjectListHead;
		while (pNode)
		{
			ObjectCount++;
			pNode = pNode->pNext;
		}
		for (UINT i = 0; i < m_HashTableLen; i++)
		{
			const StorageNode * pNode = m_HashTable[i];
			while (pNode)
			{
				HashCount++;
				pNode = pNode->pBack;
			}
		}
	}
	iterator begin()
	{
		return iterator(this, GetFirstObjectPos());
	}

	iterator end()
	{
		return iterator(this, NULL);
	}
	const_iterator begin() const
	{
		return const_iterator(this, GetFirstObjectPos());
	}

	const_iterator end() const
	{
		return const_iterator(this, NULL);
	}
protected:
	bool CreateBufferPage(UINT Size)
	{
		if (m_GrowLimit)
		{
			if (m_ObjectBuffPages.GetCount() >= m_GrowLimit)
				return false;
		}
		OBJECT_BUFF_PAGE_INFO PageInfo;
		PageInfo.BufferSize = Size;
		PageInfo.pObjectBuffer = new StorageNode[Size];
		for (UINT i = 0; i < Size; i++)
		{
			PageInfo.pObjectBuffer[i].IsUsed = false;
		}
#ifdef LOG_POOL_CREATE
		PrintImportantLog("Create %u Total %u", Size, GetBufferSize() + Size);
#endif
		UINT IDStart = GetBufferSize() + 1;
		ClearBuffer(PageInfo, IDStart, true);
		m_ObjectBuffPages.Add(PageInfo);
		CaculateBufferSize();
		return true;
	}
	void CaculateBufferSize()
	{
		m_BufferSize = 0;
		for (UINT i = 0; i < m_ObjectBuffPages.GetCount(); i++)
		{
			m_BufferSize += m_ObjectBuffPages[i].BufferSize;
		}		
	}
	void ClearBuffer(OBJECT_BUFF_PAGE_INFO& PageInfo, UINT IDStart, bool IsInit)
	{
		StorageNode * pFreeListHead = NULL;
		StorageNode * pFreeListTail = NULL;

		assert(PageInfo.BufferSize != 0);
		if (PageInfo.BufferSize == 0)
			return;

		for (UINT i = 0; i < PageInfo.BufferSize; i++)
		{
			PageInfo.pObjectBuffer[i].ID = i + IDStart;
			if (i == 0)
			{
				pFreeListHead = PageInfo.pObjectBuffer;
				PageInfo.pObjectBuffer[i].pPrev = NULL;
			}
			else
			{
				PageInfo.pObjectBuffer[i].pPrev = &(PageInfo.pObjectBuffer[i - 1]);
			}
			if (i == PageInfo.BufferSize - 1)
			{
				pFreeListTail = PageInfo.pObjectBuffer + i;
				PageInfo.pObjectBuffer[i].pNext = NULL;
			}
			else
			{
				PageInfo.pObjectBuffer[i].pNext = &(PageInfo.pObjectBuffer[i + 1]);
			}
			PageInfo.pObjectBuffer[i].IsFree = true;
			if (IsInit)
				PageInfo.pObjectBuffer[i].InitObject();
			else
				PageInfo.pObjectBuffer[i].DeleteObject();
		}
		if (m_pFreeListTail)
		{
			m_pFreeListTail->pNext = pFreeListHead;
			pFreeListHead->pPrev = m_pFreeListTail;
			m_pFreeListTail = pFreeListTail;
		}
		else
		{
			m_pFreeListHead = pFreeListHead;
			m_pFreeListTail = pFreeListTail;
		}
	}
	void RebuildHashTable()
	{
		m_HashTableLen = GetBufferSize();
		SAFE_DELETE_ARRAY(m_HashTable);
		m_HashTable = new LPSTORAGENODE[m_HashTableLen];
		ZeroMemory(m_HashTable, sizeof(LPSTORAGENODE)*m_HashTableLen);
		m_HashMask = m_HashTableLen - 1;
		//UINT Len = m_HashTableLen;
		//while (Len)
		//{
		//	m_HashMask = (m_HashMask << 1) + 1;
		//	Len = Len >> 1;
		//}
		StorageNode * pNode = m_pObjectListHead;
		while (pNode)
		{
			pNode->pFront = NULL;
			pNode->pBack = NULL;
			InsertNode(pNode->Key, pNode);
			pNode = pNode->pNext;
		}
	}
	void InsertNodeBefore(StorageNode * pNode, StorageNode * pBefore)
	{
		if (pBefore == NULL)
			pBefore = m_pObjectListHead;
		if (pBefore)
		{
			pNode->pPrev = pBefore->pPrev;
			pNode->pNext = pBefore;
			if (pBefore->pPrev)
				pBefore->pPrev->pNext = pNode;
			pBefore->pPrev = pNode;
		}
		else
		{
			m_pObjectListHead = pNode;
			m_pObjectListTail = pNode;
		}
		if (pBefore == m_pObjectListHead)
		{
			m_pObjectListHead = pNode;
		}
	}
	void InsertNodeAfter(StorageNode * pNode, StorageNode * pAfter)
	{
		if (pAfter == NULL)
			pAfter = m_pObjectListTail;
		if (pAfter)
		{
			pNode->pPrev = pAfter;
			pNode->pNext = pAfter->pNext;
			if (pAfter->pNext)
				pAfter->pNext->pPrev = pNode;
			pAfter->pNext = pNode;
		}
		else
		{
			m_pObjectListHead = pNode;
			m_pObjectListTail = pNode;
		}
		if (pAfter == m_pObjectListTail)
		{
			m_pObjectListTail = pNode;
		}
	}
	void PickNode(StorageNode * pNode)
	{
		if (pNode->pPrev)
			pNode->pPrev->pNext = pNode->pNext;
		if (pNode->pNext)
			pNode->pNext->pPrev = pNode->pPrev;
		if (pNode == m_pObjectListHead)
			m_pObjectListHead = pNode->pNext;
		if (pNode == m_pObjectListTail)
			m_pObjectListTail = pNode->pPrev;
		pNode->pPrev = NULL;
		pNode->pNext = NULL;
	}
	StorageNode * NewNode(const KEY& Key)
	{
		if (m_pFreeListHead)
		{
			StorageNode * pNode;

			pNode = m_pFreeListHead;
			if (m_pFreeListHead == m_pFreeListTail)
			{
				m_pFreeListHead = NULL;
				m_pFreeListTail = NULL;
			}
			else
			{
				m_pFreeListHead = pNode->pNext;
				if (m_pFreeListHead)
					m_pFreeListHead->pPrev = NULL;
			}


			pNode->pPrev = NULL;
			pNode->pNext = NULL;
			pNode->pFront = NULL;
			pNode->pBack = NULL;
			pNode->IsFree = false;
			if (!pNode->IsUsed)
			{
				pNode->IsUsed = true;
				m_UsedObjectCount++;
			}
			pNode->Key = Key;
			pNode->NewObject();
			m_ObjectCount++;
			return pNode;
		}
		return NULL;
	}
	
	void ReleaseNode(StorageNode * pNode)
	{
		PickNode(pNode);
		pNode->IsFree = true;
		pNode->DeleteObject();
		pNode->pPrev = m_pFreeListTail;
		pNode->pNext = NULL;
		if (m_pFreeListTail)
		{
			m_pFreeListTail->pNext = pNode;
			m_pFreeListTail = pNode;
		}
		else
		{
			m_pFreeListHead = pNode;
			m_pFreeListTail = pNode;
		}
		m_ObjectCount--;
	}
	StorageNode * FindNode(const KEY& Key)
	{
		UINT HashCode = CHashCodeMaker::GetHashCode(Key);
		UINT Index = HashCode&m_HashMask;
		if (Index < m_HashTableLen)
		{
			StorageNode * pNode = m_HashTable[Index];
			while (pNode)
			{
				if (pNode->Key == Key)
				{
					return pNode;
				}
				pNode = pNode->pBack;
			}
		}
		return NULL;
	}
	const StorageNode * FindNode(const KEY& Key) const
	{
		UINT HashCode = CHashCodeMaker::GetHashCode(Key);
		UINT Index = HashCode&m_HashMask;
		if (Index < m_HashTableLen)
		{
			const StorageNode * pNode = m_HashTable[Index];
			while (pNode)
			{
				if (pNode->Key == Key)
				{
					return pNode;
				}
				pNode = pNode->pBack;
			}
		}
		return NULL;
	}
	StorageNode * InsertNode(const KEY& Key, StorageNode * pNode)
	{
		CheckGrow();
		UINT HashCode = CHashCodeMaker::GetHashCode(Key);
		UINT Index = HashCode&m_HashMask;
		if (Index < m_HashTableLen)
		{
			StorageNode * pRoot = m_HashTable[Index];
			StorageNode * pSameNode = pRoot;
			while (pSameNode)
			{
				if (pSameNode->Key == Key)
				{
					break;
				}
				pSameNode = pSameNode->pBack;
			}
			if (pSameNode)
			{
				//已有相同Key的元素，替换
				if (pNode)
				{
					pSameNode->GetObjectRef() = pNode->GetObjectRef();
					ReleaseNode(pNode);
				}					
				return pSameNode;
			}
			else
			{
				if (pNode == NULL)
				{
					pNode = NewNode(Key);
					if (pNode)
						InsertNodeAfter(pNode, m_pObjectListTail);
				}					
				if (pNode)
				{
					
					m_HashTable[Index] = pNode;
					if (pRoot)
					{
						pNode->pBack = pRoot;
						pRoot->pFront = pNode;
					}
					return pNode;
				}
			}
		}
		return NULL;
	}
	bool  DeleteNode(const KEY& Key)
	{
		UINT HashCode = CHashCodeMaker::GetHashCode(Key);
		UINT Index = HashCode&m_HashMask;
		if (Index < m_HashTableLen)
		{
			StorageNode * pNode = m_HashTable[Index];
			while (pNode)
			{
				if (pNode->Key == Key)
				{
					if (pNode->pFront)
						pNode->pFront->pBack = pNode->pBack;
					else
						m_HashTable[Index] = pNode->pBack;

					if (pNode->pBack)
						pNode->pBack->pFront = pNode->pFront;
					
					ReleaseNode(pNode);
					return true;
				}
				pNode = pNode->pBack;
			}
		}
		return false;
	}
};

