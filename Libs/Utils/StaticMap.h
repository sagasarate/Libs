/****************************************************************************/
/*                                                                          */
/*      文件名:    StaticMap.h                                              */
/*      创建日期:  2009年10月15日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once





template < class KEY,class T ,int StorageMode=EDSM_STATIC>
class CStaticMap
{
protected:
	enum enNodeColor
	{
		NC_NONE,
		NC_RED,
		NC_BLACK,		
	};
	template < int Mode,class KEY_TYPE,class OBJECT_TYPE>
	struct _StorageNode
	{
		UINT ID;
		_StorageNode		* pParent;
		_StorageNode		* pRightChild;
		_StorageNode		* pLeftChild;
		_StorageNode		* pFront;
		_StorageNode		* pBack;
		_StorageNode		* pPrev;
		_StorageNode		* pNext;
		enNodeColor			Color;
		bool				IsUsed;
		bool				IsFree;
		KEY_TYPE			Key;
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
		OBJECT_TYPE			Object;
	};
	template < class KEY_TYPE,class OBJECT_TYPE>
	struct _StorageNode<EDSM_NEW_ONCE,KEY_TYPE,OBJECT_TYPE>
	{
		UINT ID;
		_StorageNode		* pParent;
		_StorageNode		* pRightChild;
		_StorageNode		* pLeftChild;
		_StorageNode		* pFront;
		_StorageNode		* pBack;
		_StorageNode		* pPrev;
		_StorageNode		* pNext;
		enNodeColor			Color;
		bool				IsUsed;
		bool				IsFree;
		KEY_TYPE			Key;
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
		OBJECT_TYPE *		pObject;
	};
	template < class KEY_TYPE,class OBJECT_TYPE>
	struct _StorageNode<EDSM_NEW_EVERY_TIME,KEY_TYPE,OBJECT_TYPE>
	{
		UINT ID;
		_StorageNode		* pParent;
		_StorageNode		* pRightChild;
		_StorageNode		* pLeftChild;
		_StorageNode		* pFront;
		_StorageNode		* pBack;
		_StorageNode		* pPrev;
		_StorageNode		* pNext;
		enNodeColor			Color;
		bool				IsUsed;
		bool				IsFree;
		KEY_TYPE			Key;
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
		OBJECT_TYPE *		pObject;
	};

	typedef _StorageNode<StorageMode,KEY,T> StorageNode;

	struct OBJECT_BUFF_PAGE_INFO
	{
		StorageNode *	pObjectBuffer;
		UINT			BufferSize;
	};

	CEasyArray<OBJECT_BUFF_PAGE_INFO>	m_ObjectBuffPages;
	StorageNode	*						m_pFreeListHead;
	StorageNode	* 						m_pFreeListTail;
	StorageNode	* 						m_pObjectListHead;
	StorageNode	* 						m_pObjectListTail;
	StorageNode	* 						m_pTreeRoot;
	StorageNode	* 						m_pFront;
	StorageNode	* 						m_pBack;
	UINT								m_ObjectCount;
	UINT								m_UsedObjectCount;
	UINT								m_GrowSize;
	UINT								m_GrowLimit;
	LPCTSTR								m_Tag;

	//int				m_BlackCount;

public:
	typedef list_iterator<CStaticMap, T> iterator;
	typedef const_list_iterator<CStaticMap, T> const_iterator;
public:
	CStaticMap(LPCTSTR Tag = _T("CStaticMap"))
	{
		m_pFreeListHead=NULL;
		m_pFreeListTail=NULL;
		m_pObjectListHead=NULL;
		m_pObjectListTail=NULL;
		m_pTreeRoot=NULL;
		m_pFront=NULL;
		m_pBack=NULL;
		m_ObjectCount=0;
		m_UsedObjectCount = 0;
		m_GrowSize=0;
		m_GrowLimit=0;
		m_Tag = Tag;
	}
	CStaticMap(UINT Size, UINT GrowSize = 0, UINT GrowLimit = 0, LPCTSTR Tag = _T("CStaticMap"))
	{
		m_pFreeListHead = NULL;
		m_pFreeListTail = NULL;
		m_pObjectListHead = NULL;
		m_pObjectListTail = NULL;
		m_pTreeRoot = NULL;
		m_pFront = NULL;
		m_pBack = NULL;
		m_ObjectCount = 0;
		m_UsedObjectCount = 0;
		m_GrowSize = 0;
		m_GrowLimit = 0;
		m_Tag = Tag;
		Create(Size, GrowSize, GrowLimit);
	}
	~CStaticMap()
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
		m_pTreeRoot=NULL;
		m_pFront=NULL;
		m_pBack=NULL;
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
		m_pTreeRoot=NULL;
		m_pFront=NULL;
		m_pBack=NULL;
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
		m_pTreeRoot=NULL;
		m_pFront=NULL;
		m_pBack=NULL;
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
		return NULL;
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
	UINT New(const KEY& Key,T** ppValue)
	{
		StorageNode * pNode=InsertNode(m_pTreeRoot,Key);
		if(pNode)
		{
			*ppValue=pNode->GetObjectPointer();
			if(pNode->Color==NC_NONE)
			{
				pNode->Color=NC_RED;
				BalanceInsert(pNode);
			}
			return pNode->ID;
		}
		return 0;
	}
	UINT Insert(const KEY& Key,const T& Value)
	{
		StorageNode * pNode=InsertNode(m_pTreeRoot,Key);
		if(pNode)
		{
			pNode->GetObjectRef()=Value;
			if(pNode->Color==NC_NONE)
			{
				pNode->Color=NC_RED;
				BalanceInsert(pNode);
			}
			return pNode->ID;
		}
		return 0;
	}
	BOOL DeleteByID(UINT ID)
	{
		StorageNode * pNode=(StorageNode *)GetObjectPosByID(ID);
		if(pNode)
		{
			StorageNode * pNeedBalanceNode=DeleteNode(pNode,true);
			if(pNeedBalanceNode)
			{
				BalanceDelete(pNeedBalanceNode);
			}
			return TRUE;
		}
		return FALSE;
	}
	BOOL DeleteByPos(void * Pos)
	{
		StorageNode * pNode=(StorageNode *)Pos;
		if(pNode)
		{
			StorageNode * pNeedBalanceNode = DeleteNode(pNode, true);
			if(pNeedBalanceNode)
			{
				BalanceDelete(pNeedBalanceNode);
			}
			return TRUE;
		}
		return FALSE;
	}
	BOOL Delete(const KEY& Key)
	{
		StorageNode * pNode=FindNode(m_pTreeRoot,Key);
		if(pNode)
		{
			StorageNode * pNeedBalanceNode = DeleteNode(pNode, true);
			if(pNeedBalanceNode)
			{
				BalanceDelete(pNeedBalanceNode);
			}
			return TRUE;
		}
		return FALSE;
	}
	BOOL ReMap(const KEY& OldKey, const KEY& NewKey)
	{
		StorageNode * pNode = FindNode(m_pTreeRoot, OldKey);
		if (pNode)
		{
			if (FindNode(m_pTreeRoot, NewKey)==NULL)
			{
				StorageNode * pNeedBalanceNode = DeleteNode(pNode, false);
				if (pNeedBalanceNode)
				{
					BalanceDelete(pNeedBalanceNode);
				}
				pNode->Key = NewKey;
				pNeedBalanceNode = InsertNode(m_pTreeRoot, pNode);
				if (pNeedBalanceNode)
				{					
					if (pNeedBalanceNode->Color == NC_NONE)
					{
						pNeedBalanceNode->Color = NC_RED;
						BalanceInsert(pNode);
					}
					return TRUE;
				}
			}			
		}
		return FALSE;
	}
	void * FinPos(const KEY& Key) const
	{
		return FindNode(m_pTreeRoot, Key);
	}
	T * Find(const KEY& Key)
	{
		StorageNode * pNode = FindNode(m_pTreeRoot, Key);
		if(pNode)
		{
			return pNode->GetObjectPointer();
		}
		return NULL;
	}
	const T * Find(const KEY& Key) const
	{
		const StorageNode * pNode = FindNode(m_pTreeRoot, Key);
		if (pNode)
		{
			return pNode->GetObjectPointer();
		}
		return NULL;
	}
	LPVOID FindPos(const KEY& Key) const
	{
		return (LPVOID)FindNode(m_pTreeRoot, Key);
	}
	LPVOID FindNearPos(const KEY& Key) const
	{
		return (LPVOID)FindNodeNear(m_pTreeRoot, Key);
	}
	T * FindNear(const KEY& Key)
	{
		StorageNode * pNode = FindNodeNear(m_pTreeRoot, Key);
		if (pNode)
		{
			return pNode->GetObjectPointer();
		}
		return NULL;
	}
	const T * FindNear(const KEY& Key) const
	{
		const StorageNode * pNode = FindNodeNear(m_pTreeRoot, Key);
		if (pNode)
		{
			return pNode->GetObjectPointer();
		}
		return NULL;
	}
	T* GetObject(LPVOID Pos)
	{
		StorageNode * pNode = (StorageNode *)Pos;
		if(Pos)
			return pNode->GetObjectPointer();
		else
			return NULL;
	}
	const T* GetObject(LPVOID Pos) const
	{
		const StorageNode * pNode = (const StorageNode *)Pos;
		if (pNode)
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
	LPVOID GetObjectPosByID(UINT ID) const
	{
		if (ID == 0)
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
	UINT GetObjectIDByPos(LPVOID Pos) const
	{
		const StorageNode * pNode = (const StorageNode *)Pos;
		if (pNode)
			return pNode->ID;
		else
			return 0;
	}
	T * GetNextObject(LPVOID& Pos, KEY& Key)
	{
		if(Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			if(!pNode->IsFree)
			{
				Pos=pNode->pNext;
				Key=pNode->Key;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}

	T * GetPrevObject(LPVOID& Pos, KEY& Key)
	{
		if(Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			if(!pNode->IsFree)
			{
				Pos=pNode->pPrev;
				Key=pNode->Key;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}
	const T * GetNextObject(LPVOID& Pos, KEY& Key) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			if (!pNode->IsFree)
			{
				Pos = pNode->pNext;
				Key = pNode->Key;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos = NULL;
			}
		}
		return NULL;
	}

	const T * GetPrevObject(LPVOID& Pos, KEY& Key) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			if (!pNode->IsFree)
			{
				Pos = pNode->pPrev;
				Key = pNode->Key;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos = NULL;
			}
		}
		return NULL;
	}
	T * GetNextObject(LPVOID& Pos)
	{
		if (Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			if (!pNode->IsFree)
			{
				Pos = pNode->pNext;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos = NULL;
			}
		}
		return NULL;
	}

	T * GetPrevObject(LPVOID& Pos)
	{
		if (Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			if (!pNode->IsFree)
			{
				Pos = pNode->pPrev;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos = NULL;
			}
		}
		return NULL;
	}
	const T * GetNextObject(LPVOID& Pos) const
	{
		if(Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			if(!pNode->IsFree)
			{
				Pos=pNode->pNext;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}

	const T * GetPrevObject(LPVOID& Pos) const
	{
		if(Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			if(!pNode->IsFree)
			{
				Pos=pNode->pPrev;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}
	
	LPVOID GetSortedFirstObjectPos() const
	{
		return m_pFront;
	}

	LPVOID GetSortedLastObjectPos() const
	{
		return m_pBack;
	}
	T * GetSortedNextObject(LPVOID& Pos, KEY& Key)
	{
		if(Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			if(!pNode->IsFree)
			{
				Pos=pNode->pBack;
				Key=pNode->Key;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}

	T * GetSortedPrevObject(LPVOID& Pos, KEY& Key)
	{
		if(Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			if(!pNode->IsFree)
			{
				Pos=pNode->pFront;
				Key=pNode->Key;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}
	const T * GetSortedNextObject(LPVOID& Pos, KEY& Key) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			if (!pNode->IsFree)
			{
				Pos = pNode->pBack;
				Key = pNode->Key;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos = NULL;
			}
		}
		return NULL;
	}

	const T * GetSortedPrevObject(LPVOID& Pos, KEY& Key) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			if (!pNode->IsFree)
			{
				Pos = pNode->pFront;
				Key = pNode->Key;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos = NULL;
			}
		}
		return NULL;
	}
	T * GetSortedNextObject(LPVOID& Pos)
	{
		if(Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			if(!pNode->IsFree)
			{
				Pos=pNode->pBack;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}

	T * GetSortedPrevObject(LPVOID& Pos)
	{
		if(Pos)
		{
			StorageNode * pNode = (StorageNode *)Pos;
			if(!pNode->IsFree)
			{
				Pos=pNode->pFront;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}
	const T * GetSortedNextObject(LPVOID& Pos) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			if (!pNode->IsFree)
			{
				Pos = pNode->pBack;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos = NULL;
			}
		}
		return NULL;
	}

	const T * GetSortedPrevObject(LPVOID& Pos) const
	{
		if (Pos)
		{
			const StorageNode * pNode = (const StorageNode *)Pos;
			if (!pNode->IsFree)
			{
				Pos = pNode->pFront;
				return pNode->GetObjectPointer();
			}
			else
			{
				Pos = NULL;
			}
		}
		return NULL;
	}
	BOOL MoveToBefore(LPVOID Pos,LPVOID Target)
	{
		if(Pos&&Pos!=Target&&m_ObjectCount>1)
		{
			StorageNode * pNode=(StorageNode *)Pos;
			StorageNode * pBefore=(StorageNode *)Target;
			PickNode(pNode);
			InsertNodeBefore(pNode,pBefore);
			return TRUE;
		}
		return FALSE;
	}
	BOOL MoveToAfter(LPVOID Pos,LPVOID Target)
	{
		if(Pos&&Pos!=Target&&m_ObjectCount>1)
		{
			StorageNode * pNode=(StorageNode *)Pos;
			StorageNode * pAfter=(StorageNode *)Target;
			PickNode(pNode);
			InsertNodeAfter(pNode,pAfter);
			return TRUE;
		}
		return FALSE;
	}
	BOOL MoveSorted(LPVOID Pos)
	{
		StorageNode * pNode = (StorageNode *)Pos;
		if (pNode)
		{
			PickNode(pNode);
			StorageNode * pHead = m_pObjectListHead;
			while (pHead&&pHead->GetObjectRef() < pNode->GetObjectRef())
			{
				pHead = pHead->pNext;
			}
			if (pHead)
			{
				InsertNodeBefore(pNode, pHead);
			}
			else
			{
				InsertNodeAfter(pNode, pHead);
			}
			return TRUE;
		}
		return FALSE;
	}
	iterator begin()
	{
		return iterator(this,GetFirstObjectPos());
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
	//void PrintTree(CDC * pDC,int StartX,int StartY,int Dis)
	//{
	//	DrawTree(pDC,StartX,StartY,Dis,m_pTreeRoot,1);
	//}
	//void CheckNode()
	//{
	//	m_BlackCount=-1;
	//	CheckNodeRule(m_pTreeRoot,0);
	//}
protected:
	//void CheckNodeRule(StorageNode * pNode,int BlackCount)
	//{
	//	if(pNode)
	//	{
	//		if(pNode->Color==NC_RED)
	//		{
	//			if(pNode->pLeftChild)
	//				assert(pNode->pLeftChild->Color==NC_BLACK);
	//			if(pNode->pRightChild)
	//				assert(pNode->pRightChild->Color==NC_BLACK);
	//		}
	//		else
	//		{
	//			BlackCount++;				
	//		}
	//		if(pNode->pLeftChild)
	//			CheckNodeRule(pNode->pLeftChild,BlackCount);
	//		if(pNode->pRightChild)
	//			CheckNodeRule(pNode->pRightChild,BlackCount);
	//	}
	//	else
	//	{
	//		if(m_BlackCount<0)
	//			m_BlackCount=BlackCount;
	//		else
	//			assert(m_BlackCount==BlackCount);
	//	}
	//	
	//}
	//void DrawTree(CDC * pDC,int X,int Y,int Dis,StorageNode * pRoot,int Level)
	//{
	//	if(pRoot)
	//	{
	//		CEasyString Text;
	//		Text.Format("%d",*(pRoot->pObject));


	//		
	//		if(pRoot->Color==NC_BLACK)
	//			pDC->SetTextColor(RGB(0,0,0));
	//		else if(pRoot->Color==NC_RED)
	//			pDC->SetTextColor(RGB(255,0,0));
	//		else
	//			pDC->SetTextColor(RGB(0,0,255));

	//		
	//		pDC->TextOut(X,Y,(LPCTSTR)Text);

	//		int Num=pow(2.0f,Level);

	//		int D=Dis/Num;

	//		if(pRoot->pLeftChild)
	//		{
	//			pDC->MoveTo(X,Y);
	//			pDC->LineTo(X-D,Y+30);
	//			DrawTree(pDC,X-D,Y+30,Dis,pRoot->pLeftChild,Level+1);
	//		}
	//		

	//		if(pRoot->pRightChild)
	//		{
	//			pDC->MoveTo(X,Y);
	//			pDC->LineTo(X+D,Y+30);				
	//			DrawTree(pDC,X+D,Y+30,Dis,pRoot->pRightChild,Level+1);				
	//		}
	//		
	//	}
	//}
	StorageNode * FindNode(StorageNode * pRoot, const KEY& Key)
	{
		if(pRoot)
		{
			if(pRoot->Key>Key)
			{
				return FindNode(pRoot->pLeftChild,Key);
			}
			else if(pRoot->Key<Key)
			{
				return FindNode(pRoot->pRightChild,Key);
			}
			else
			{
				return pRoot;
			}
		}
		return NULL;
	}
	const StorageNode * FindNode(StorageNode * pRoot, const KEY& Key) const
	{
		if (pRoot)
		{
			if (pRoot->Key>Key)
			{
				return FindNode(pRoot->pLeftChild, Key);
			}
			else if (pRoot->Key<Key)
			{
				return FindNode(pRoot->pRightChild, Key);
			}
			else
			{
				return pRoot;
			}
		}
		return NULL;
	}
	StorageNode * FindNodeNear(StorageNode * pRoot, const KEY& Key)
	{
		if (pRoot)
		{
			if (pRoot->Key>Key)
			{
				if (pRoot->pLeftChild)
					return FindNodeNear(pRoot->pLeftChild, Key);
				else
					return pRoot;
			}
			else if (pRoot->Key<Key)
			{
				if (pRoot->pRightChild)
					return FindNodeNear(pRoot->pRightChild, Key);
				else
					return pRoot;
			}
			else
			{
				return pRoot;
			}
		}
		return NULL;
	}
	const StorageNode * FindNodeNear(StorageNode * pRoot, const KEY& Key) const
	{
		if (pRoot)
		{
			if (pRoot->Key>Key)
			{
				if (pRoot->pLeftChild)
					return FindNodeNear(pRoot->pLeftChild, Key);
				else
					return pRoot;
			}
			else if (pRoot->Key<Key)
			{
				if (pRoot->pRightChild)
					return FindNodeNear(pRoot->pRightChild, Key);
				else
					return pRoot;
			}
			else
			{
				return pRoot;
			}
		}
		return NULL;
	}
	StorageNode * InsertNode(StorageNode * pRoot,const KEY& Key)
	{
		if(pRoot)
		{
			if(pRoot->Key>Key)
			{
				if(pRoot->pLeftChild)
				{					
					return InsertNode(pRoot->pLeftChild,Key);					
				}
				else
				{
					StorageNode * pNewNode=NewNode(Key);
					if(pNewNode)
					{
						pNewNode->pParent=pRoot;						
						pRoot->pLeftChild=pNewNode;	

						pNewNode->pBack=pRoot;
						pNewNode->pFront=pRoot->pFront;
						if(pRoot->pFront)
							pRoot->pFront->pBack=pNewNode;
						pRoot->pFront=pNewNode;
						if(pRoot==m_pFront)
							m_pFront=pNewNode;

					}
					return pNewNode;
				}

			}
			else if(pRoot->Key<Key)
			{
				if(pRoot->pRightChild)
				{					
					return InsertNode(pRoot->pRightChild,Key);
				}
				else
				{
					StorageNode * pNewNode=NewNode(Key);
					if(pNewNode)
					{
						pNewNode->pParent=pRoot;	
						pRoot->pRightChild=pNewNode;						

						pNewNode->pBack=pRoot->pBack;
						pNewNode->pFront=pRoot;
						if(pRoot->pBack)
							pRoot->pBack->pFront=pNewNode;
						pRoot->pBack=pNewNode;
						if(pRoot==m_pBack)
							m_pBack=pNewNode;
					}
					return pNewNode;
				}
			}
			else
			{
				return pRoot;
			}
		}
		else
		{
			m_pTreeRoot=NewNode(Key);		
			m_pFront=m_pTreeRoot;
			m_pBack=m_pTreeRoot;
			return m_pTreeRoot;
		}		
	}	
	StorageNode * InsertNode(StorageNode * pRoot, StorageNode * pNewNode)
	{
		if (pRoot)
		{
			if (pRoot->Key > pNewNode->Key)
			{
				if (pRoot->pLeftChild)
				{
					return InsertNode(pRoot->pLeftChild, pNewNode);
				}
				else
				{
					pNewNode->pParent = pRoot;
					pRoot->pLeftChild = pNewNode;

					pNewNode->pBack = pRoot;
					pNewNode->pFront = pRoot->pFront;
					if (pRoot->pFront)
						pRoot->pFront->pBack = pNewNode;
					pRoot->pFront = pNewNode;
					if (pRoot == m_pFront)
						m_pFront = pNewNode;

					return pNewNode;
				}

			}
			else if (pRoot->Key < pNewNode->Key)
			{
				if (pRoot->pRightChild)
				{
					return InsertNode(pRoot->pRightChild, pNewNode);
				}
				else
				{
					
					pNewNode->pParent = pRoot;
					pRoot->pRightChild = pNewNode;

					pNewNode->pBack = pRoot->pBack;
					pNewNode->pFront = pRoot;
					if (pRoot->pBack)
						pRoot->pBack->pFront = pNewNode;
					pRoot->pBack = pNewNode;
					if (pRoot == m_pBack)
						m_pBack = pNewNode;
					
					return pNewNode;
				}
			}
			else
			{
				return pRoot;
			}
		}
		else
		{
			m_pTreeRoot = pNewNode;
			m_pFront = m_pTreeRoot;
			m_pBack = m_pTreeRoot;
			return m_pTreeRoot;
		}
	}
	StorageNode *  DeleteNode(StorageNode * pNode,bool DeleteFromList)
	{		
		if(pNode->pLeftChild&&pNode->pRightChild)
		{
			StorageNode * pBackNode=pNode->pBack;

			//交换父节点
			if(pNode->pParent&&pBackNode->pParent)
			{
				if(pNode->pParent->pLeftChild==pNode)
				{
					pNode->pParent->pLeftChild=pBackNode;
					if(pBackNode->pParent->pLeftChild==pBackNode)
						pBackNode->pParent->pLeftChild=pNode;						
					else
						pBackNode->pParent->pRightChild=pNode;						
				}
				else
				{
					pNode->pParent->pRightChild=pBackNode;
					if(pBackNode->pParent->pLeftChild==pBackNode)
						pBackNode->pParent->pLeftChild=pNode;						
					else
						pBackNode->pParent->pRightChild=pNode;						
				}
				StorageNode * pTemp=pNode->pParent;
				pNode->pParent=pBackNode->pParent;
				pBackNode->pParent=pTemp;
			}
			else if(pNode->pParent)
			{
				if(pNode->pParent->pLeftChild==pNode)
					pNode->pParent->pLeftChild=pBackNode;
				else
					pNode->pParent->pRightChild=pBackNode;

				pBackNode->pParent=pNode->pParent;
				pNode->pParent=NULL;
				m_pTreeRoot=pNode;
			}
			else
			{
				if(pBackNode->pParent->pLeftChild==pBackNode)
					pBackNode->pParent->pLeftChild=pNode;
				else
					pBackNode->pParent->pRightChild=pNode;
				pNode->pParent=pBackNode->pParent;
				pBackNode->pParent=NULL;
				m_pTreeRoot=pBackNode;
			}

			//交换左子节点
			pBackNode->pLeftChild=pNode->pLeftChild;
			pNode->pLeftChild->pParent=pBackNode;
			pNode->pLeftChild=NULL;
			//交换右子节点
			if(pBackNode->pRightChild)
			{
				pNode->pRightChild->pParent=pBackNode;
				pBackNode->pRightChild->pParent=pNode;
				StorageNode * pTemp=pBackNode->pRightChild;
				pBackNode->pRightChild=pNode->pRightChild;
				pNode->pRightChild=pTemp;
			}
			else
			{
				pBackNode->pRightChild=pNode->pRightChild;
				pNode->pRightChild->pParent=pBackNode;
				pNode->pRightChild=NULL;
			}	
			//交换颜色
			enNodeColor Color=pNode->Color;
			pNode->Color=pBackNode->Color;
			pBackNode->Color=Color;
		}

		StorageNode * pNeedBalanceNode=NULL;

		if(pNode->pRightChild)
		{
			if(pNode->Color==NC_BLACK)
				pNeedBalanceNode=pNode->pRightChild;
			if(pNode->pParent)
			{
				pNode->pRightChild->pParent=pNode->pParent;
				if(pNode->pParent->pLeftChild==pNode)
					pNode->pParent->pLeftChild=pNode->pRightChild;
				else
					pNode->pParent->pRightChild=pNode->pRightChild;
				pNode->pParent=NULL;
			}	
			else
			{
				m_pTreeRoot=pNode->pRightChild;
				m_pTreeRoot->pParent=NULL;
			}

			pNode->pRightChild=NULL;
		}
		else if(pNode->pLeftChild)
		{
			if(pNode->Color==NC_BLACK)
				pNeedBalanceNode=pNode->pLeftChild;
			if(pNode==m_pTreeRoot)
			{
				m_pTreeRoot=pNode->pLeftChild;
				m_pTreeRoot->pParent=NULL;
			}
			else
			{
				pNode->pLeftChild->pParent=pNode->pParent;
				if(pNode->pParent->pLeftChild==pNode)
					pNode->pParent->pLeftChild=pNode->pLeftChild;
				else
					pNode->pParent->pRightChild=pNode->pLeftChild;
				pNode->pParent=NULL;
			}			
			pNode->pLeftChild=NULL;
		}
		else
		{
			if(pNode->pParent)
			{
				if(pNode->Color==NC_BLACK)
					pNeedBalanceNode=pNode->pParent;

				if(pNode->pParent->pLeftChild==pNode)
					pNode->pParent->pLeftChild=NULL;
				else
					pNode->pParent->pRightChild=NULL;
				pNode->pParent=NULL;
			}
			else
			{
				m_pTreeRoot=NULL;
			}
		}

		pNode->Color = NC_NONE;

		if (DeleteFromList)
			DeleteNodeFromList(pNode);

		return pNeedBalanceNode;
	}
	void DeleteNodeFromList(StorageNode * pNode)
	{
		if(pNode==m_pObjectListHead&&pNode==m_pObjectListTail)
		{
			m_pObjectListHead=NULL;
			m_pObjectListTail=NULL;
		}
		else if(pNode==m_pObjectListHead)
		{
			m_pObjectListHead=pNode->pNext;				
			m_pObjectListHead->pPrev=NULL;

		}
		else if(pNode==m_pObjectListTail)
		{
			m_pObjectListTail=pNode->pPrev;
			m_pObjectListTail->pNext=NULL;			
		}
		else
		{
			pNode->pPrev->pNext=pNode->pNext;
			pNode->pNext->pPrev=pNode->pPrev;			
		}		

		if(pNode==m_pFront&&pNode==m_pBack)
		{
			m_pFront=NULL;
			m_pBack=NULL;
		}
		else if(pNode==m_pFront)
		{
			m_pFront=pNode->pBack;				
			m_pFront->pFront=NULL;			
		}
		else if(pNode==m_pBack)
		{
			m_pBack=pNode->pFront;
			m_pBack->pBack=NULL;			
		}
		else
		{
			pNode->pFront->pBack=pNode->pBack;
			pNode->pBack->pFront=pNode->pFront;			
		}	

		
		pNode->IsFree=true;		
		pNode->DeleteObject();
		pNode->pFront=NULL;
		pNode->pBack=NULL;
		
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
			PageInfo.pObjectBuffer[i].pFront=NULL;
			PageInfo.pObjectBuffer[i].pBack=NULL;
			PageInfo.pObjectBuffer[i].pParent=NULL;
			PageInfo.pObjectBuffer[i].pRightChild=NULL;
			PageInfo.pObjectBuffer[i].pLeftChild=NULL;
			PageInfo.pObjectBuffer[i].Color=NC_NONE;
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
	StorageNode * NewNode(const KEY& Key)
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
			
			if(m_pObjectListTail)
			{
				m_pObjectListTail->pNext=pNode;
				pNode->pPrev=m_pObjectListTail;
				pNode->pNext=NULL;
				m_pObjectListTail=pNode;
			}
			else
			{
				m_pObjectListHead=pNode;
				m_pObjectListTail=pNode;
				pNode->pPrev=NULL;
				pNode->pNext=NULL;
			}
			pNode->IsFree=false;
			if (!pNode->IsUsed)
			{
				pNode->IsUsed = true;
				m_UsedObjectCount++;
			}
			pNode->NewObject(GetTag());
			pNode->Key=Key;
			pNode->Color = NC_NONE;
			pNode->pFront = NULL;
			pNode->pBack = NULL;
			pNode->pRightChild = NULL;
			pNode->pLeftChild = NULL;
			
			m_ObjectCount++;
			return pNode;
		}
		return NULL;
	}
	StorageNode * RotateRight(StorageNode * pNode)
	{
		//右旋必须保证有左子树
		StorageNode * pLeftChild=pNode->pLeftChild;
		if(pLeftChild==NULL)
		{
			return NULL;
		}

		if(pNode->pParent)
		{
			if(pNode->pParent->pLeftChild==pNode)
				pNode->pParent->pLeftChild=pLeftChild;
			else
				pNode->pParent->pRightChild=pLeftChild;
			pLeftChild->pParent=pNode->pParent;
		}
		else
		{
			pLeftChild->pParent=NULL;
		}

		pNode->pParent=pLeftChild;	

		pNode->pLeftChild=pLeftChild->pRightChild;
		if(pLeftChild->pRightChild)
			pLeftChild->pRightChild->pParent=pNode;
		pLeftChild->pRightChild=pNode;

		if(pLeftChild->pParent==NULL)
			m_pTreeRoot=pLeftChild;
		return pLeftChild;
	}
	StorageNode * RotateLeft(StorageNode * pNode)
	{
		//左旋必须保证有右子树
		StorageNode * pRightChild=pNode->pRightChild;
		if(pRightChild==NULL)
		{
			return NULL;
		}
		if(pNode->pParent)
		{
			if(pNode->pParent->pLeftChild==pNode)
				pNode->pParent->pLeftChild=pRightChild;
			else
				pNode->pParent->pRightChild=pRightChild;
			pRightChild->pParent=pNode->pParent;
		}
		else
		{
			pRightChild->pParent=NULL;
		}

		pNode->pParent=pRightChild;
		pNode->pRightChild=pRightChild->pLeftChild;
		if(pRightChild->pLeftChild)
			pRightChild->pLeftChild->pParent=pNode;
		pRightChild->pLeftChild=pNode;
		if(pRightChild->pParent==NULL)
			m_pTreeRoot=pRightChild;
		return pRightChild;
	}
	//void BalanceInsert(StorageNode * pNode)
	//{
	//	assert(pNode->Color==NC_RED);
	//	if(pNode==m_pTreeRoot)
	//	{
	//		pNode->Color=NC_BLACK;
	//	}
	//	else
	//	{			
	//		if(pNode->pParent->Color==NC_RED)
	//		{
	//			StorageNode * pParentNode;
	//			if(pNode->pParent==pNode->pParent->pParent->pLeftChild)
	//			{
	//				if(pNode==pNode->pParent->pRightChild)
	//				{
	//					pParentNode=pNode;
	//					pNode->pParent->Color=NC_BLACK;
	//					RotateLeft(pNode->pParent);		
	//					RotateRight(pNode->pParent);
	//				}
	//				else
	//				{
	//					pParentNode=pNode->pParent;
	//					pNode->Color=NC_BLACK;
	//					RotateRight(pNode->pParent->pParent);
	//				}
	//				
	//			}
	//			else
	//			{
	//				if(pNode==pNode->pParent->pLeftChild)
	//				{
	//					pParentNode=pNode;
	//					pNode->pParent->Color=NC_BLACK;
	//					RotateRight(pNode->pParent);
	//					RotateLeft(pNode->pParent);
	//				}
	//				else
	//				{
	//					pParentNode=pNode->pParent;
	//					pNode->Color=NC_BLACK;
	//					RotateLeft(pNode->pParent->pParent);
	//				}
	//				
	//			}
	//			BalanceInsert(pParentNode);
	//		}
	//	}
	//}
	void BalanceInsert(StorageNode * pNode)
	{
		//assert(pNode->Color==NC_RED);		
		if(pNode==m_pTreeRoot)
		{
			pNode->Color=NC_BLACK;
		}
		else
		{			
			assert(pNode->pParent != NULL);
			if(pNode->pParent->Color==NC_RED)
			{
				StorageNode * pParentParent=pNode->pParent->pParent;
				enNodeColor Color1;
				enNodeColor Color2;

				if(pParentParent->pLeftChild)
					Color1=pParentParent->pLeftChild->Color;
				else
					Color1=NC_BLACK;
				if(pParentParent->pRightChild)
					Color2=pParentParent->pRightChild->Color;
				else
					Color2=NC_BLACK;

				if(Color1==NC_RED&&Color2==NC_RED)
				{
					pParentParent->Color=NC_RED;
					pParentParent->pLeftChild->Color=NC_BLACK;
					pParentParent->pRightChild->Color=NC_BLACK;
					BalanceInsert(pParentParent);
				}
				else
				{
					pNode->pParent->pParent->Color=NC_RED;
					StorageNode * pParentNode;
					if(pNode->pParent==pNode->pParent->pParent->pLeftChild)
					{
						if(pNode==pNode->pParent->pRightChild)
						{
							pParentNode=pNode;
							RotateLeft(pNode->pParent);		
							RotateRight(pNode->pParent);
						}
						else
						{
							pParentNode=pNode->pParent;						
							RotateRight(pNode->pParent->pParent);
						}

					}
					else
					{
						if(pNode==pNode->pParent->pLeftChild)
						{
							pParentNode=pNode;
							RotateRight(pNode->pParent);
							RotateLeft(pNode->pParent);
						}
						else
						{
							pParentNode=pNode->pParent;						
							RotateLeft(pNode->pParent->pParent);
						}

					}
					pParentNode->Color=NC_BLACK;
				}
			}
		}
	}
	void BalanceDelete(StorageNode * pNode)
	{
		if(pNode->Color==NC_RED)
		{
			pNode->Color=NC_BLACK;
		}
		else if(pNode!=m_pTreeRoot)
		{
			StorageNode * pBrother;
			if(pNode->pParent->pLeftChild==pNode)
				pBrother=pNode->pParent->pRightChild;
			else
				pBrother=pNode->pParent->pLeftChild;

			if(pBrother)
			{
				if(pBrother->Color==NC_RED)
				{
					pBrother->Color=NC_BLACK;
					pBrother->pParent->Color=NC_RED;
					if(pNode->pParent->pLeftChild==pNode)
						RotateLeft(pNode->pParent);
					else
						RotateRight(pNode->pParent);
					BalanceDelete(pNode);
				}
				else if(pBrother->pLeftChild&&pBrother->pRightChild)
				{
					if(pBrother->pLeftChild->Color==NC_BLACK&&pBrother->pRightChild->Color==NC_BLACK)
					{
						pBrother->Color=NC_RED;
						BalanceDelete(pBrother->pParent);
					}
					else 
					{
						if(pNode->pParent->pLeftChild==pNode)
						{					
							if(pBrother->pRightChild->Color==NC_BLACK)
							{
								pBrother->pLeftChild->Color=NC_BLACK;
								RotateRight(pBrother);
							}
							else
							{
								pBrother->pRightChild->Color=NC_BLACK;
							}
							RotateLeft(pNode->pParent);
						}
						else
						{
							if(pBrother->pLeftChild->Color==NC_BLACK)
							{
								pBrother->pRightChild->Color=NC_BLACK;
								RotateLeft(pBrother);
							}
							else
							{
								pBrother->pLeftChild->Color=NC_BLACK;
							}
							RotateRight(pNode->pParent);
						}
					}
				}
				else if(pBrother->pLeftChild)
				{
					if(pBrother->pLeftChild->Color==NC_BLACK)
					{
						pBrother->Color=NC_RED;
						BalanceDelete(pBrother->pParent);
					}					
					else
					{
						if(pNode->pParent->pLeftChild==pNode)
						{	
							pBrother->pLeftChild->Color=NC_BLACK;
							RotateRight(pBrother);							
							RotateLeft(pNode->pParent);
						}
						else
						{
							if(pBrother->pLeftChild->Color==NC_BLACK)
							{
								RotateLeft(pBrother);
							}
							else
							{
								pBrother->pLeftChild->Color=NC_BLACK;
							}
							RotateRight(pNode->pParent);
						}
					}
				}
				else if(pBrother->pRightChild)
				{
					if(pBrother->pRightChild->Color==NC_BLACK)
					{
						pBrother->Color=NC_RED;
						BalanceDelete(pBrother->pParent);
					}					
					else
					{
						if(pNode->pParent->pLeftChild==pNode)
						{					
							if(pBrother->pRightChild->Color==NC_BLACK)
							{
								RotateRight(pBrother);
							}
							else
							{
								pBrother->pRightChild->Color=NC_BLACK;
							}
							RotateLeft(pNode->pParent);
						}
						else
						{							
							pBrother->pRightChild->Color=NC_BLACK;
							RotateLeft(pBrother);							
							RotateRight(pNode->pParent);
						}
					}
				}
				else
				{
					pBrother->Color=NC_RED;
					BalanceDelete(pBrother->pParent);
				}
			}
			else
			{
				BalanceDelete(pNode->pParent);
			}			
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

};