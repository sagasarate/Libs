/****************************************************************************/
/*                                                                          */
/*      文件名:    IDStorage.h                                              */
/*      创建日期:  2009年10月15日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

template < class T ,int StorageMode=EDSM_STATIC>
class CIDStorage
{
protected:
	template < int Mode,class OBJECT_TYPE>
	struct _StorageNode
	{
		UINT ID;
		_StorageNode * pPrev;
		_StorageNode * pNext;
		bool IsFree;
		bool IsUsed;
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
		void NewObject(LPCTSTR Tag)
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
	struct _StorageNode<EDSM_NEW_ONCE,OBJECT_TYPE>
	{
		UINT ID;
		_StorageNode * pPrev;
		_StorageNode * pNext;
		bool IsFree;
		bool IsUsed;
		void InitObject()
		{
			pObject=NULL;
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
		void NewObject(LPCTSTR Tag)
		{
			if (pObject == NULL)
				pObject = MONITORED_NEW(Tag, OBJECT_TYPE);
		}
		void DeleteObject()
		{
			
		}
		void FinalReleaseObject()
		{
			SAFE_DELETE(pObject);
		}
	protected:
		OBJECT_TYPE * pObject;
	};


	template < class OBJECT_TYPE>
	struct _StorageNode<EDSM_NEW_EVERY_TIME,OBJECT_TYPE>
	{
		UINT ID;
		_StorageNode * pPrev;
		_StorageNode * pNext;
		bool IsFree;
		bool IsUsed;
		void InitObject()
		{
			pObject=NULL;
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
		void NewObject(LPCTSTR Tag)
		{
			if (pObject == NULL)
				pObject = MONITORED_NEW(Tag, OBJECT_TYPE);
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
		OBJECT_TYPE * pObject;
	};

	typedef _StorageNode<StorageMode,T> StorageNode;

	struct OBJECT_BUFF_PAGE_INFO
	{
		StorageNode *	pObjectBuffer;
		UINT			BufferSize;
	};

	CEasyArray<OBJECT_BUFF_PAGE_INFO>	m_ObjectBuffPages;
	StorageNode *						m_pFreeListHead;
	StorageNode *						m_pFreeListTail;
	StorageNode *						m_pObjectListHead;
	StorageNode *						m_pObjectListTail;
	UINT								m_ObjectCount;
	UINT								m_UsedObjectCount;
	UINT								m_GrowSize;
	UINT								m_GrowLimit;
	LPCTSTR								m_Tag;
public:
	typedef list_iterator<CIDStorage, T> iterator;
	typedef const_list_iterator<CIDStorage, T> const_iterator;
public:
	CIDStorage(LPCTSTR Tag = _T("CIDStorage"))
	{
		m_pFreeListHead=NULL;
		m_pFreeListTail=NULL;
		m_pObjectListHead=NULL;
		m_pObjectListTail=NULL;
		m_ObjectCount=0;
		m_UsedObjectCount = 0;
		m_GrowSize=0;
		m_GrowLimit=0;
		m_Tag = Tag;
	}
	CIDStorage(UINT Size, UINT GrowSize = 0, UINT GrowLimit = 0, LPCTSTR Tag = _T("CIDStorage"))
	{
		m_pFreeListHead = NULL;
		m_pFreeListTail = NULL;
		m_pObjectListHead = NULL;
		m_pObjectListTail = NULL;
		m_ObjectCount = 0;
		m_UsedObjectCount = 0;
		m_GrowSize = 0;
		m_GrowLimit = 0;
		m_Tag = Tag;
		Create(Size, GrowSize, GrowLimit);
	}
	~CIDStorage()
	{
		Destory();
	}
	LPCTSTR GetTag()
	{
		return m_Tag;
	}
	void SetTag(LPCTSTR Tag)
	{
		m_Tag = Tag;
	}
	bool Create(UINT Size,UINT GrowSize=0,UINT GrowLimit=0)
	{		
		m_ObjectBuffPages.SetTag(GetTag());
		Destory();
		if(Size)
		{
			m_GrowSize=GrowSize;
			m_GrowLimit=GrowLimit;
			return CreateBufferPage(Size);
		}
		return true;
	}
	bool Create(const STORAGE_POOL_SETTING& PoolSetting)
	{
		m_ObjectBuffPages.SetTag(GetTag());
		return Create(PoolSetting.StartSize, PoolSetting.GrowSize, PoolSetting.GrowLimit);
	}
	UINT GetBufferSize() const
	{
		UINT BufferSize=0;
		for(UINT i=0;i<m_ObjectBuffPages.GetCount();i++)
		{
			BufferSize+=m_ObjectBuffPages[i].BufferSize;
		}
		return BufferSize;
	}
	bool IsCreated() const
	{
		return m_ObjectBuffPages.GetCount()!=0;
	}
	void Destory()
	{
		for(UINT i=0;i<m_ObjectBuffPages.GetCount();i++)
		{
			for(UINT j=0;j<m_ObjectBuffPages[i].BufferSize;j++)
			{			
				m_ObjectBuffPages[i].pObjectBuffer[j].FinalReleaseObject();
			}
			SAFE_DELETE_ARRAY(m_ObjectBuffPages[i].pObjectBuffer);
		}
		m_ObjectBuffPages.Clear();
		m_pFreeListHead=NULL;
		m_pFreeListTail=NULL;
		m_pObjectListHead=NULL;
		m_pObjectListTail=NULL;
		m_ObjectCount=0;
		m_GrowSize=0;
		m_GrowLimit=0;
	}
	void Clear()
	{
		m_pFreeListHead=NULL;
		m_pFreeListTail=NULL;
		m_pObjectListHead=NULL;
		m_pObjectListTail=NULL;
		m_ObjectCount=0;

		UINT IDStart=1;
		for(UINT i=0;i<m_ObjectBuffPages.GetCount();i++)
		{
			ClearBuffer(m_ObjectBuffPages[i],IDStart,false);
			IDStart+=m_ObjectBuffPages[i].BufferSize;
		}					
	}
	void ClearToDefault()
	{
		m_pFreeListHead=NULL;
		m_pFreeListTail=NULL;
		m_pObjectListHead=NULL;
		m_pObjectListTail=NULL;
		m_ObjectCount=0;

		UINT IDStart=1;
		if(m_ObjectBuffPages.GetCount()>1)
		{
			for(UINT i=1;i<m_ObjectBuffPages.GetCount();i++)
			{
				for(UINT j=0;j<m_ObjectBuffPages[i].BufferSize;j++)
				{			
					m_ObjectBuffPages[i].pObjectBuffer[j].FinalReleaseObject();
				}
				SAFE_DELETE_ARRAY(m_ObjectBuffPages[i].pObjectBuffer);
			}
			m_ObjectBuffPages.Resize(1);
		}
		for(UINT i=0;i<m_ObjectBuffPages.GetCount();i++)
		{
			ClearBuffer(m_ObjectBuffPages[i],IDStart,false);
			IDStart+=m_ObjectBuffPages[i].BufferSize;
		}
	}
	bool Grow()
	{
		if(m_GrowSize)
		{
			CreateBufferPage(m_GrowSize);
			return true;
		}
		return false;
	}
	UINT NewObject(T** ppObject)
	{
		StorageNode * pNode=NewNode();
		if(pNode)
		{				
			InsertNodeAfter(pNode,m_pObjectListTail);			
			*ppObject=pNode->GetObjectPointer();
			return pNode->ID;
		}
		*ppObject=NULL;
		return 0;
	}

	T* NewObject()
	{
		StorageNode * pNode=NewNode();
		if(pNode)
		{				
			InsertNodeAfter(pNode,m_pObjectListTail);			
			pNode->GetObjectRef().SetID(pNode->ID);
			return pNode->GetObjectPointer();
		}

		return NULL;
	}	

	UINT AddObject(const T& Object)
	{
		UINT ID;
		T * pObject;
		ID=NewObject(&pObject);
		if(ID)
		{
			*pObject=Object;
			return ID;
		}
		return 0;
	}
	LPVOID InsertAfter(LPVOID Pos=NULL)
	{
		StorageNode * pNode=NewNode();
		if(pNode)
		{				
			InsertNodeAfter(pNode,(StorageNode *)Pos);			
			return pNode;
		}
		return NULL;
	}
	LPVOID InsertAfter(const T& Object,LPVOID Pos=NULL)
	{
		StorageNode * pNode=(StorageNode *)InsertAfter(Pos);
		if(pNode)
		{		
			pNode->GetObjectRef()=Object;
			return pNode;
		}
		return NULL;
	}
	LPVOID InsertBefore(LPVOID Pos=NULL)
	{
		StorageNode * pNode=NewNode();
		if(pNode)
		{				
			InsertNodeBefore(pNode,(StorageNode *)Pos);			
			return pNode;
		}
		return NULL;
	}
	LPVOID InsertBefore(const T& Object,LPVOID Pos=NULL)
	{
		StorageNode * pNode=(StorageNode *)InsertBefore(Pos);
		if(pNode)
		{		
			pNode->GetObjectRef()=Object;
			return pNode;
		}
		return NULL;
	}
	LPVOID InsertSorted(const T& Object)
	{
		StorageNode * pNode=NewNode();
		if(pNode)
		{	
			pNode->GetObjectRef()=Object;
			StorageNode * pHead=m_pObjectListHead;
			while(pHead&&pHead->GetObjectRef()<Object)
			{
				pHead=pHead->pNext;
			}
			if(pHead)
			{
				InsertNodeBefore(pNode,pHead);
			}
			else
			{
				InsertNodeAfter(pNode,pHead);
			}
			return pNode;
		}
		return NULL;
	}
	LPVOID GetObjectPosByID(UINT ID) const
	{
		if(ID==0)
			return NULL;
		if (m_ObjectBuffPages.GetCount())
		{
			ID--;
			const OBJECT_BUFF_PAGE_INFO& FirstPage = m_ObjectBuffPages[0];
			if (ID < FirstPage.BufferSize)
			{
				if (!FirstPage.pObjectBuffer[ID].IsFree)
					return &(FirstPage.pObjectBuffer[ID]);
			}
			else
			{
				ID -= FirstPage.BufferSize;
				UINT PageIndex = 1 + ID / m_GrowSize;
				UINT Index = ID % m_GrowSize;
				if (PageIndex < m_ObjectBuffPages.GetCount())
				{
					const OBJECT_BUFF_PAGE_INFO& Page = m_ObjectBuffPages[PageIndex];
					if (Index < Page.BufferSize)
					{
						if (!Page.pObjectBuffer[Index].IsFree)
							return &(Page.pObjectBuffer[Index]);
					}
				}
			}
		}
		return NULL;
	}
	T* GetObject(LPVOID Pos)
	{
		StorageNode * pNode = (StorageNode *)Pos;
		if (pNode)
		{
			if (!pNode->IsFree)
				return pNode->GetObjectPointer();
		}
		
		return NULL;
	}	
	const T* GetObject(LPVOID Pos) const
	{
		const StorageNode * pNode = (const StorageNode *)Pos;
		if (pNode)
		{
			if (!pNode->IsFree)
				return pNode->GetObjectPointer();
		}

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
	UINT GetObjectIDByPos(LPVOID Pos) const
	{
		const StorageNode * pNode = (const StorageNode *)Pos;
		if (pNode)
			return pNode->ID;
		else
			return 0;
	}
	
	bool DeleteObject(UINT ID)
	{
		return DeleteObjectByPos(GetObjectPosByID(ID));
	}	
	bool DeleteObjectByPos(LPVOID Pos)
	{
		StorageNode * pNode=(StorageNode *)Pos;
		if(pNode)
		{
			if (!pNode->IsFree)
			{
				DeleteNode(pNode);
				return true;
			}
		}
		return false;;
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

	T* GetNextObject(LPVOID& Pos)
	{
		if(Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			Pos=pNode->pNext;
			return pNode->GetObjectPointer();
		}
		return NULL;
	}

	T* GetPrevObject(LPVOID& Pos)
	{
		if(Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			Pos=pNode->pPrev;
			return pNode->GetObjectPointer();
		}
		return NULL;
	}
	const T* GetNextObject(LPVOID& Pos) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			Pos = pNode->pNext;
			return pNode->GetObjectPointer();
		}
		return NULL;
	}

	const T* GetPrevObject(LPVOID& Pos) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			Pos = pNode->pPrev;
			return pNode->GetObjectPointer();
		}
		return NULL;
	}
	bool MoveToBefore(LPVOID Pos,LPVOID Target)
	{
		if(Pos&&Pos!=Target&&m_ObjectCount>1)
		{
			StorageNode * pNode=(StorageNode *)Pos;
			StorageNode * pBefore=(StorageNode *)Target;
			PickNode(pNode);
			InsertNodeBefore(pNode,pBefore);
			return true;
		}
		return false;
	}
	bool MoveToAfter(LPVOID Pos,LPVOID Target)
	{
		if(Pos&&Pos!=Target&&m_ObjectCount>1)
		{
			StorageNode * pNode=(StorageNode *)Pos;
			StorageNode * pAfter=(StorageNode *)Target;
			PickNode(pNode);
			InsertNodeAfter(pNode,pAfter);
			return true;
		}
		return false;
	}
	bool MoveSorted(LPVOID Pos)
	{
		StorageNode * pNode=(StorageNode *)Pos;
		if(pNode)
		{	
			PickNode(pNode);
			StorageNode * pHead=m_pObjectListHead;
			while(pHead&&pHead->GetObjectRef()<pNode->GetObjectRef())
			{
				pHead=pHead->pNext;
			}
			if(pHead)
			{
				InsertNodeBefore(pNode,pHead);
			}
			else
			{
				InsertNodeAfter(pNode,pHead);
			}
			return true;
		}
		return false;
	}

	LPVOID PushFront()
	{
		StorageNode * pNode=NewNode();
		if(pNode)
		{				
			InsertNodeBefore(pNode,m_pObjectListHead);			
			return pNode;
		}
		return NULL;
	}

	LPVOID PushFront(const T& Object)
	{
		StorageNode * pNode=(StorageNode *)PushFront();
		if(pNode)
		{			
			pNode->GetObjectRef()=Object;
			return pNode;
		}
		return NULL;
	}

	LPVOID PushBack()
	{
		StorageNode * pNode=NewNode();
		if(pNode)
		{				
			InsertNodeAfter(pNode,m_pObjectListTail);			
			return pNode;
		}
		return NULL;
	}

	LPVOID PushBack(const T& Object)
	{
		StorageNode * pNode=(StorageNode *)PushBack();
		if(pNode)
		{			
			pNode->GetObjectRef()=Object;
			return pNode;
		}
		return NULL;
	}

	bool PopFront(T& Object)
	{
		if(m_pObjectListHead)
		{
			Object=m_pObjectListHead->GetObjectRef();
			DeleteNode(m_pObjectListHead);
			return true;
		}
		return false;
	}	

	bool PopBack(T& Object)
	{
		if(m_pObjectListTail)
		{
			Object=m_pObjectListTail->GetObjectRef();
			DeleteNode(m_pObjectListTail);
			return true;
		}
		return false;
	}
	LPVOID Find(const T& Object) const
	{
		const StorageNode * pNode = m_pObjectListHead;
		while(pNode)
		{
			if(pNode->Object==Object)
				return pNode;
			pNode=pNode->pNext;
		}
		return NULL;
	}
	void Verfy(UINT& UsedCount, UINT& FreeCount) const
	{
		UsedCount = 0;
		FreeCount = 0;
		UINT ObjectBuffSize = GetBufferSize();

		const StorageNode * pNode = m_pObjectListHead;
		while (pNode&&UsedCount < ObjectBuffSize)
		{
			pNode = pNode->pNext;
			UsedCount++;
		}

		pNode = m_pFreeListHead;
		while (pNode&&FreeCount < ObjectBuffSize)
		{
			pNode = pNode->pNext;
			FreeCount++;
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
	LPVOID GetFreeObjectPosByID(UINT ID)
	{
		if (ID == 0)
			return NULL;
		if (m_ObjectBuffPages.GetCount())
		{
			ID--;
			const OBJECT_BUFF_PAGE_INFO& FirstPage = m_ObjectBuffPages[0];
			if (ID < FirstPage.BufferSize)
			{
				if (FirstPage.pObjectBuffer[ID].IsFree)
					return &(FirstPage.pObjectBuffer[ID]);
			}
			else
			{
				ID -= FirstPage.BufferSize;
				UINT PageIndex = 1 + ID / m_GrowSize;
				UINT Index = ID % m_GrowSize;
				if (PageIndex < m_ObjectBuffPages.GetCount())
				{
					const OBJECT_BUFF_PAGE_INFO& Page = m_ObjectBuffPages[PageIndex];
					if (Index < Page.BufferSize)
					{
						if (Page.pObjectBuffer[Index].IsFree)
							return &(Page.pObjectBuffer[Index]);
					}
				}
			}
		}
		return NULL;
	}
	T* GetFreeObject(LPVOID Pos)
	{
		StorageNode * pNode = (StorageNode *)Pos;
		if (pNode)
		{
			if (pNode->IsFree)
				return pNode->GetObjectPointer();
		}

		return NULL;
	}
	void ReleaseFreeObject(LPVOID Pos)
	{
		StorageNode * pNode = (StorageNode *)Pos;
		if (pNode)
		{
			if (pNode->IsFree)
				return pNode->FinalReleaseObject();
		}
	}
protected:
	bool CreateBufferPage(UINT Size)
	{
		if(m_GrowLimit)
		{
			if(m_ObjectBuffPages.GetCount()>=m_GrowLimit)
				return false;
		}		
		OBJECT_BUFF_PAGE_INFO PageInfo;
		PageInfo.BufferSize=Size;		
		PageInfo.pObjectBuffer = MONITORED_NEW_ARRAY(GetTag(), StorageNode, Size);
		for (UINT i = 0; i < Size; i++)
		{
			PageInfo.pObjectBuffer[i].IsUsed = false;
		}
#ifdef LOG_POOL_CREATE		
		PrintImportantLog("Create %u Total %u", Size, GetBufferSize() + Size);
#endif
		UINT IDStart=GetBufferSize()+1;
		ClearBuffer(PageInfo,IDStart,true);
		m_ObjectBuffPages.Add(PageInfo);
		return true;
	}
	void ClearBuffer(OBJECT_BUFF_PAGE_INFO& PageInfo,UINT IDStart,bool IsInit)
	{
		StorageNode * pFreeListHead=NULL;
		StorageNode * pFreeListTail=NULL;

		assert(PageInfo.BufferSize!=0);
		if(PageInfo.BufferSize==0)
			return;
		
		for(UINT i=0;i<PageInfo.BufferSize;i++)
		{
			PageInfo.pObjectBuffer[i].ID=i+IDStart;			
			if(i==0)
			{
				pFreeListHead=PageInfo.pObjectBuffer;
				PageInfo.pObjectBuffer[i].pPrev=NULL;
			}
			else
			{
				PageInfo.pObjectBuffer[i].pPrev=&(PageInfo.pObjectBuffer[i-1]);
			}
			if(i==PageInfo.BufferSize-1)
			{
				pFreeListTail=PageInfo.pObjectBuffer+i;
				PageInfo.pObjectBuffer[i].pNext=NULL;
			}
			else
			{
				PageInfo.pObjectBuffer[i].pNext=&(PageInfo.pObjectBuffer[i+1]);
			}				
			PageInfo.pObjectBuffer[i].IsFree=true;
			if(IsInit)
				PageInfo.pObjectBuffer[i].InitObject();
			else
				PageInfo.pObjectBuffer[i].DeleteObject();
		}
		if(m_pFreeListTail)
		{
			m_pFreeListTail->pNext=pFreeListHead;
			pFreeListHead->pPrev=m_pFreeListTail;
			m_pFreeListTail=pFreeListTail;
		}
		else
		{
			m_pFreeListHead=pFreeListHead;
			m_pFreeListTail=pFreeListTail;
		}
	}
	StorageNode * NewNode()
	{
		if(m_pFreeListHead==NULL&&m_GrowSize)
		{
			CreateBufferPage(m_GrowSize);
		}
		if(m_pFreeListHead)
		{			
			StorageNode * pNode;			

			pNode=m_pFreeListHead;
			if(m_pFreeListHead==m_pFreeListTail)
			{
				m_pFreeListHead=NULL;
				m_pFreeListTail=NULL;
			}
			else
			{
				m_pFreeListHead=pNode->pNext;
				if(m_pFreeListHead)
					m_pFreeListHead->pPrev=NULL;	
			}


			pNode->pPrev=NULL;
			pNode->pNext=NULL;			
			pNode->IsFree = false;
			if (!pNode->IsUsed)
			{
				pNode->IsUsed = true;
				m_UsedObjectCount++;
			}			
			pNode->NewObject(GetTag());
			m_ObjectCount++;
			return pNode;
		}
		return NULL;
	}
	void InsertNodeBefore(StorageNode * pNode,StorageNode * pBefore)
	{
		if(pBefore==NULL)
			pBefore=m_pObjectListHead;
		if(pBefore)
		{
			pNode->pPrev=pBefore->pPrev;
			pNode->pNext=pBefore;
			if(pBefore->pPrev)
				pBefore->pPrev->pNext=pNode;
			pBefore->pPrev=pNode;
		}
		else
		{
			m_pObjectListHead=pNode;
			m_pObjectListTail=pNode;
		}
		if(pBefore==m_pObjectListHead)
		{
			m_pObjectListHead=pNode;
		}
	}
	void InsertNodeAfter(StorageNode * pNode,StorageNode * pAfter)
	{
		if(pAfter==NULL)
			pAfter=m_pObjectListTail;
		if(pAfter)
		{
			pNode->pPrev=pAfter;
			pNode->pNext=pAfter->pNext;
			if(pAfter->pNext)
				pAfter->pNext->pPrev=pNode;
			pAfter->pNext=pNode;
		}
		else
		{
			m_pObjectListHead=pNode;
			m_pObjectListTail=pNode;
		}
		if(pAfter==m_pObjectListTail)
		{
			m_pObjectListTail=pNode;
		}
	}
	void PickNode(StorageNode * pNode)
	{
		if(pNode->pPrev)
			pNode->pPrev->pNext=pNode->pNext;
		if(pNode->pNext)
			pNode->pNext->pPrev=pNode->pPrev;
		if(pNode==m_pObjectListHead)
			m_pObjectListHead=pNode->pNext;
		if(pNode==m_pObjectListTail)
			m_pObjectListTail=pNode->pPrev;
		pNode->pPrev=NULL;
		pNode->pNext=NULL; 
	}
	void DeleteNode(StorageNode * pNode)
	{	
		PickNode(pNode);
		pNode->IsFree=true;		
		pNode->DeleteObject();
		pNode->pPrev=m_pFreeListTail;
		pNode->pNext=NULL;
		if(m_pFreeListTail)
		{
			m_pFreeListTail->pNext=pNode;
			m_pFreeListTail=pNode;
		}
		else
		{			
			m_pFreeListHead=pNode;
			m_pFreeListTail=pNode;
		}
		m_ObjectCount--;
	}

	
};
