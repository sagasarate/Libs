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
	struct _StorageNode<EDSM_NEW_ONCE,OBJECT_TYPE>
	{
		UINT ID;
		_StorageNode * pPrev;
		_StorageNode * pNext;
		bool IsFree;				
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
		void NewObject()
		{
			if(pObject==NULL)
				pObject=new OBJECT_TYPE;
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
		void NewObject()
		{
			if(pObject==NULL)
				pObject=new OBJECT_TYPE;
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
	UINT								m_GrowSize;
	UINT								m_GrowLimit;
public:
	CIDStorage()
	{
		m_pFreeListHead=NULL;
		m_pFreeListTail=NULL;
		m_pObjectListHead=NULL;
		m_pObjectListTail=NULL;
		m_ObjectCount=0;
		m_GrowSize=0;
		m_GrowLimit=0;
	}
	~CIDStorage()
	{
		Destory();
	}
	bool Create(UINT Size,UINT GrowSize=0,UINT GrowLimit=0)
	{		
		Destory();
		if(Size)
		{
			m_GrowSize=GrowSize;
			m_GrowLimit=GrowLimit;
			return CreateBufferPage(Size);
		}
		return true;
	}
	UINT GetBufferSize()
	{
		UINT BufferSize=0;
		for(UINT i=0;i<m_ObjectBuffPages.GetCount();i++)
		{
			BufferSize+=m_ObjectBuffPages[i].BufferSize;
		}
		return BufferSize;
	}
	BOOL IsCreated()
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
		return NULL;
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
		StorageNode * pNode=(StorageNode *)InsertNodeBefore(Pos);
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
	LPVOID GetObjectPos(UINT ID)
	{
		if(ID==0)
			return NULL;

		ID--;
		for(UINT i=0;i<m_ObjectBuffPages.GetCount();i++)
		{
			if(ID<m_ObjectBuffPages[i].BufferSize)
			{
				if(!m_ObjectBuffPages[i].pObjectBuffer[ID].IsFree)
					return &(m_ObjectBuffPages[i].pObjectBuffer[ID]);
				return NULL;
			}
			ID-=m_ObjectBuffPages[i].BufferSize;
		}		
		return NULL;
	}
	T* GetObject(LPVOID Pos)
	{
		StorageNode * pNode=(StorageNode *)Pos;	
		if (pNode)
		{
			if (!pNode->IsFree)
				return pNode->GetObjectPointer();
		}
		
		return NULL;
	}	
	T* GetObject(UINT ID)
	{		
		return GetObject(GetObjectPos(ID));
	}
	UINT GetObjectID(LPVOID Pos)
	{
		StorageNode * pNode=(StorageNode *)Pos;	
		if (pNode)
			return pNode->ID;
		else
			return 0;
	}
	
	BOOL DeleteObject(UINT ID)
	{
		return DeleteObjectByPos(GetObjectPos(ID));
	}	
	BOOL DeleteObjectByPos(LPVOID Pos)
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

	UINT GetObjectCount()
	{
		return m_ObjectCount;
	}

	LPVOID GetFirstObjectPos()
	{
		return m_pObjectListHead;
	}

	LPVOID GetLastObjectPos()
	{
		return m_pObjectListTail;
	}

	T* GetNext(LPVOID& Pos)
	{
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;
			Pos=pNode->pNext;
			return pNode->GetObjectPointer();
		}
		return NULL;
	}

	T* GetPrev(LPVOID& Pos)
	{
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;
			Pos=pNode->pPrev;
			return pNode->GetObjectPointer();
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
			return TRUE;
		}
		return FALSE;
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

	BOOL PopFront(T& Object)
	{
		if(m_pObjectListHead)
		{
			Object=m_pObjectListHead->GetObjectRef();
			DeleteNode(m_pObjectListHead);
			return TRUE;
		}
		return FALSE;
	}	

	BOOL PopBack(T& Object)
	{
		if(m_pObjectListTail)
		{
			Object=m_pObjectListTail->GetObjectRef();
			DeleteNode(m_pObjectListTail);
			return TRUE;
		}
		return FALSE;
	}
	LPVOID Find(const T& Object)
	{
		StorageNode * pNode=m_pObjectListHead;
		while(pNode)
		{
			if(pNode->Object==Object)
				return pNode;
			pNode=pNode->pNext;
		}
		return NULL;
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
		PageInfo.pObjectBuffer=new StorageNode[Size];
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
			pNode->IsFree=false;				
			pNode->NewObject();
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

	void Verfy(UINT& UsedCount,UINT& FreeCount)
	{
		UsedCount=0;
		FreeCount=0;
		UINT ObjectBuffSize=GetBufferSize();

		StorageNode * pNode=m_pObjectListHead;
		while(pNode&&UsedCount<ObjectBuffSize)
		{
			pNode=pNode->pNext;
			UsedCount++;
		}

		pNode=m_pFreeListHead;
		while(pNode&&FreeCount<ObjectBuffSize)
		{
			pNode=pNode->pNext;
			FreeCount++;
		}
	}

};