/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyMap.h                                                */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


template < class KEY,class T >
class CEasyMap
{
protected:
	enum enNodeColor
	{
		NC_NONE,
		NC_RED,
		NC_BLACK,		
	};
	struct StorageNode
	{		
		StorageNode		* pParent;
		StorageNode		* pRightChild;
		StorageNode		* pLeftChild;
		StorageNode		* pFront;
		StorageNode		* pBack;
		StorageNode		* pPrev;
		StorageNode		* pNext;
		enNodeColor		Color;		
		KEY				Key;
		T				Object;
		StorageNode():Key(),Object()
		{
			pParent=NULL;
			pRightChild=NULL;
			pLeftChild=NULL;
			pFront=NULL;
			pBack=NULL;
			pPrev=NULL;
			pNext=NULL;
			Color=NC_NONE;
		}
	};	
	StorageNode		* m_pObjectListHead;
	StorageNode		* m_pObjectListTail;
	StorageNode		* m_pTreeRoot;
	StorageNode		* m_pFront;
	StorageNode		* m_pBack;
	UINT			m_ObjectCount;	
	LPCTSTR			m_Tag;
public:
	CEasyMap(LPCTSTR Tag = _T("CEasyMap"))
	{
		
		m_pObjectListHead=NULL;
		m_pObjectListTail=NULL;
		m_pTreeRoot=NULL;
		m_pFront=NULL;
		m_pBack=NULL;
		m_ObjectCount=0;	
		m_Tag = Tag;
	}
	~CEasyMap()
	{
		Clear();
	}	
	LPCTSTR GetTag()
	{
		return m_Tag;
	}
	void SetTag(LPCTSTR Tag)
	{
		m_Tag = Tag;
	}
	void Clear()
	{
		while(m_pObjectListHead)
		{
			StorageNode	* pNode=m_pObjectListHead;
			m_pObjectListHead=m_pObjectListHead->pNext;
			SAFE_DELETE(pNode);
		}		
		m_pObjectListHead=NULL;
		m_pObjectListTail=NULL;
		m_pTreeRoot=NULL;
		m_pFront=NULL;
		m_pBack=NULL;
		m_ObjectCount=0;
	}	
	T* New(const KEY& Key)
	{
		StorageNode * pNode=InsertNode(m_pTreeRoot,Key);
		if(pNode)
		{			
			if(pNode->Color==NC_NONE)
			{
				pNode->Color=NC_RED;
				BalanceInsert(pNode);
			}
			return &(pNode->Object);
		}
		return NULL;
	}
	bool Insert(const KEY& Key,const T& Value)
	{
		StorageNode * pNode=InsertNode(m_pTreeRoot,Key);
		if(pNode)
		{
			pNode->Object=Value;
			if(pNode->Color==NC_NONE)
			{
				pNode->Color=NC_RED;
				BalanceInsert(pNode);
			}
			return true;
		}
		return false;
	}	
	bool Delete(const KEY& Key)
	{
		StorageNode * pNode=FindNode(m_pTreeRoot,Key);
		if(pNode)
		{
			StorageNode * pNeedBalanceNode=DeleteNode(pNode);
			if(pNeedBalanceNode)
			{
				BalanceDelete(pNeedBalanceNode);
			}
			return true;
		}
		return false;
	}
	bool DeleteByPos(void * Pos)
	{
		StorageNode * pNode=(StorageNode *)Pos;
		if(pNode)
		{
			StorageNode * pNeedBalanceNode=DeleteNode(pNode);
			if(pNeedBalanceNode)
			{
				BalanceDelete(pNeedBalanceNode);
			}
			return true;
		}
		return false;
	}
	T * Find(const KEY& Key)
	{
		StorageNode * pNode=FindNode(m_pTreeRoot,Key);
		if(pNode)
		{
			return &(pNode->Object);
		}
		return NULL;
	}
	T* GetObject(LPVOID Pos)
	{
		StorageNode * pNode=(StorageNode *)Pos;
		if(Pos)
			return &(pNode->Object);
		else
			return NULL;
	}
	T& operator[](const KEY& Key)
	{
		StorageNode * pNode=FindNode(m_pTreeRoot,Key);
		if(pNode==NULL)
		{
			pNode=InsertNode(m_pTreeRoot,Key);
			if(pNode->Color==NC_NONE)
			{
				pNode->Color=NC_RED;
				BalanceInsert(pNode);
			}				
		}
		return pNode->Object;
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

	T * GetNextObject(LPVOID& Pos,KEY& Key)
	{
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;			
			if(pNode->Color!=NC_NONE)
			{
				Pos=pNode->pNext;
				Key=pNode->Key;
				return &(pNode->Object);
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}

	T * GetPrevObject(LPVOID& Pos,KEY& Key)
	{
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;			
			if(pNode->Color!=NC_NONE)
			{
				Pos=pNode->pPrev;
				Key=pNode->Key;
				return &(pNode->Object);
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}
	T * GetNextObject(LPVOID& Pos)
	{
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;			
			if(pNode->Color!=NC_NONE)
			{
				Pos=pNode->pNext;
				return &(pNode->Object);
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}

	T * GetPrevObject(LPVOID& Pos)
	{
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;			
			if(pNode->Color!=NC_NONE)
			{
				Pos=pNode->pPrev;
				return &(pNode->Object);
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}
	LPVOID GetSortedFirstObjectPos()
	{
		return m_pFront;
	}

	LPVOID GetSortedLastObjectPos()
	{
		return m_pBack;
	}
	T * GetSortedNextObject(LPVOID& Pos,KEY& Key)
	{
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;			
			if(pNode->Color!=NC_NONE)
			{
				Pos=pNode->pBack;
				Key=pNode->Key;
				return &(pNode->Object);
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}

	T * GetSortedPrevObject(LPVOID& Pos,KEY& Key)
	{
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;			
			if(pNode->Color!=NC_NONE)
			{
				Pos=pNode->pFront;
				Key=pNode->Key;
				return (pNode->Object);
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
	}
	T * GetSortedNextObject(LPVOID& Pos)
	{
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;			
			if(pNode->Color!=NC_NONE)
			{
				Pos=pNode->pBack;
				return &(pNode->Object);
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
			StorageNode * pNode=(StorageNode *)Pos;			
			if(pNode->Color!=NC_NONE)
			{
				Pos=pNode->pFront;
				return (pNode->Object);
			}
			else
			{
				Pos=NULL;
			}
		}
		return NULL;
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
	StorageNode * FindNode(StorageNode * pRoot,const KEY& Key)
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
	StorageNode *  DeleteNode(StorageNode * pNode)
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

		MONITORED_DELETE(pNode);
		m_ObjectCount--;
	}
	StorageNode * NewNode(const KEY& Key)
	{
		StorageNode * pNode= MONITORED_NEW(GetTag(), StorageNode);
		pNode->Key=Key;
		
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
		
		m_ObjectCount++;
		return pNode;
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


};