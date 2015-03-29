/****************************************************************************/
/*                                                                          */
/*      文件名:    DialogItemSorter.cpp                                     */
/*      创建日期:  2009年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


CDialogItemSorter::CDialogItemSorter(void)
{
	m_pParent=NULL;
	m_pItemTree=new SORT_TREE_ITEM;
}

CDialogItemSorter::~CDialogItemSorter(void)
{
	SAFE_DELETE(m_pItemTree);
}

void CDialogItemSorter::SetSortType(UINT ID,int SortType,float SortParam1,float SortParam2)
{
	SORT_TREE_ITEM * pNode=FindTreeNode(m_pItemTree,ID);
	if(pNode)
	{
		pNode->SortType=SortType;
		pNode->SortParam1=SortParam1;
		pNode->SortParam2=SortParam2;
	}
}

bool CDialogItemSorter::AddDialogItem(UINT ParentID,UINT ID,int SortType,int DialogItemID,CRect Rect,float SortParam1,float SortParam2)
{
	SORT_TREE_ITEM * pParent=FindTreeNode(m_pItemTree,ParentID);
	if(pParent)
	{
		SORT_TREE_ITEM * pNewNode=new SORT_TREE_ITEM;
		pNewNode->ID=ID;
		pNewNode->SortType=SortType;
		pNewNode->SortParam1=SortParam1;
		pNewNode->SortParam2=SortParam2;
		pNewNode->DialogItemID=DialogItemID;
		pNewNode->Rect=Rect;

		pNewNode->pParent=pParent;
		pParent->ChildList.Add(pNewNode);
		return true;
	}
	return false;
}

void CDialogItemSorter::DoSort()
{
	if(m_pParent&&::IsWindow(m_pParent->GetSafeHwnd()))
	{
		m_pParent->GetClientRect(&(m_pItemTree->Rect));
		GetDialogItemSize(m_pItemTree,true);
		ResizeItems(m_pItemTree);
		AlignItems(m_pItemTree);
		SetDialogItemSize(m_pItemTree);
		
	}
}

void CDialogItemSorter::GetResizeRange(UINT ItemID,RECT& Range)
{
	SORT_TREE_ITEM * pItem=FindTreeNode(m_pItemTree,ItemID);
	if(pItem)
	{
		SORT_TREE_ITEM * pNextItem=NULL;
		SORT_TREE_ITEM * pParent=pItem->pParent;
		if(pParent)
		{
			for(UINT i=0;i<pParent->ChildList.GetCount();i++)
			{
				if(pParent->ChildList[i]==pItem)
				{
					if(i<pParent->ChildList.GetCount()-1)
					{
						pNextItem=pParent->ChildList[i+1];
					}
				}
			}
		}
		if(pNextItem)
		{
			if(pParent->SortType&SORT_VERTICAL)
			{
				Range.left=0;
				Range.right=0;
				Range.top=pItem->Rect.Height();
				Range.bottom=pNextItem->Rect.Height();
			}
			else
			{
				Range.left=pItem->Rect.Width();
				Range.right=pNextItem->Rect.Width();
				Range.top=0;
				Range.bottom=0;
			}
		}
	}
}

void CDialogItemSorter::OnResize(UINT ItemID,CSize& Size)
{
	SORT_TREE_ITEM * pItem=FindTreeNode(m_pItemTree,ItemID);
	if(pItem)
	{
		SORT_TREE_ITEM * pNextItem=NULL;
		SORT_TREE_ITEM * pParent=pItem->pParent;
		if(pParent)
		{
			for(UINT i=0;i<pParent->ChildList.GetCount();i++)
			{
				if(pParent->ChildList[i]==pItem)
				{
					if(i<pParent->ChildList.GetCount()-1)
					{
						pNextItem=pParent->ChildList[i+1];
					}
				}
			}
		}
		if(pNextItem)
		{
			if(pParent->SortType&SORT_VERTICAL)
			{				
				if(Size.cy<0)
				{
					int MoveSize=-Size.cy;
					if(MoveSize>pItem->Rect.Height())
						MoveSize=pItem->Rect.Height();
					pItem->Rect.bottom-=MoveSize;
					pNextItem->Rect.top-=MoveSize;
				}
				else
				{
					int MoveSize=Size.cy;
					if(MoveSize>pNextItem->Rect.Height())
						MoveSize=pNextItem->Rect.Height();
					pItem->Rect.bottom+=MoveSize;
					pNextItem->Rect.top+=MoveSize;
				}
			}
			else
			{
				if(Size.cx<0)
				{
					int MoveSize=-Size.cx;
					if(MoveSize>pItem->Rect.Width())
						MoveSize=pItem->Rect.Width();
					pItem->Rect.right-=MoveSize;
					pNextItem->Rect.left-=MoveSize;
				}
				else
				{
					int MoveSize=Size.cx;
					if(MoveSize>pNextItem->Rect.Width())
						MoveSize=pNextItem->Rect.Width();
					pItem->Rect.right+=MoveSize;
					pNextItem->Rect.left+=MoveSize;
				}
			}
		}

		GetDialogItemSize(pItem,false);
		ResizeItems(pItem);
		AlignItems(pItem);
		SetDialogItemSize(pItem);

		GetDialogItemSize(pNextItem,false);
		ResizeItems(pNextItem);
		AlignItems(pNextItem);
		SetDialogItemSize(pNextItem);
	}
}

CDialogItemSorter::SORT_TREE_ITEM * CDialogItemSorter::FindTreeNode(SORT_TREE_ITEM * pRoot,UINT ID)
{
	if(pRoot->ID==ID)
		return pRoot;
	for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
	{
		SORT_TREE_ITEM * pTree=FindTreeNode(pRoot->ChildList[i],ID);
		if(pTree)
			return pTree;
	}
	return NULL;
}

void CDialogItemSorter::ResizeItems(SORT_TREE_ITEM * pRoot)
{
	CRect Rect=pRoot->Rect;
	if(pRoot->SortType&SORT_VERTICAL)
	{
		int FitHeight=Rect.Height();
		int FitItemCount=0;
		for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
		{
			SORT_TREE_ITEM * pChild=pRoot->ChildList[i];			

			if(!(pChild->SortType&RESIZE_FIT_PARENT_HEIGHT))
			{
				FitHeight-=pChild->Rect.Height();				
			}
			else
			{
				FitItemCount++;
			}
			if(pChild->SortType&RESIZE_FIT_PARENT_WIDTH)
			{
				pChild->Rect.left=Rect.left;
				pChild->Rect.right=Rect.right;
			}
		}
		if(FitItemCount)
		{		
			if(pRoot->SortType&PARAM_ALIGN_SPACE)
				FitHeight-=(pRoot->ChildList.GetCount()-1)*pRoot->SortParam1;		

			int Height=FitHeight/FitItemCount;
			for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
			{
				SORT_TREE_ITEM * pChild=pRoot->ChildList[i];
				if(pChild->SortType&RESIZE_FIT_PARENT_HEIGHT)
				{
					if(pChild->SortType&PARAM_FIT_RATE)
						pChild->Rect.bottom=pChild->Rect.top+FitHeight*pChild->SortParam1;
					else
						pChild->Rect.bottom=pChild->Rect.top+Height;
				}
			}
		}
	}
	else
	{
		int FitWidth=Rect.Width();
		int FitItemCount=0;
		for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
		{
			SORT_TREE_ITEM * pChild=pRoot->ChildList[i];
			if(!(pChild->SortType&RESIZE_FIT_PARENT_WIDTH))
			{
				FitWidth-=pChild->Rect.Width();				
			}
			else
			{
				FitItemCount++;
			}
			if(pChild->SortType&RESIZE_FIT_PARENT_HEIGHT)
			{
				pChild->Rect.top=Rect.top;
				pChild->Rect.bottom=Rect.bottom;
			}
		}
		if(FitItemCount)
		{		
			if(pRoot->SortType&PARAM_ALIGN_SPACE)
				FitWidth-=(pRoot->ChildList.GetCount()-1)*pRoot->SortParam1;		

			int Width=FitWidth/FitItemCount;
			for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
			{
				SORT_TREE_ITEM * pChild=pRoot->ChildList[i];
				if(pChild->SortType&RESIZE_FIT_PARENT_WIDTH)
				{
					if(pChild->SortType&PARAM_FIT_RATE)
						pChild->Rect.right=pChild->Rect.left+FitWidth*pChild->SortParam1;
					else
						pChild->Rect.right=pChild->Rect.left+Width;
				}
			}
		}
	}
	for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
	{
		ResizeItems(pRoot->ChildList[i]);
	}
}

void CDialogItemSorter::AlignItems(SORT_TREE_ITEM * pRoot)
{
	CRect Rect=pRoot->Rect;
	if(pRoot->SortType&SORT_VERTICAL)
	{
		int Height=0;
		int ItemCount=pRoot->ChildList.GetCount();
		for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
		{
			SORT_TREE_ITEM * pChild=pRoot->ChildList[i];

			Height+=pChild->Rect.Height();
		}
		if(ItemCount)
		{		
			int AlignType=pRoot->SortType&ALIGN_MASK;
			int Pre=0;
			int Space=0;
			if(AlignType==ALIGN_CENTER)
			{
				if(pRoot->SortType&PARAM_ALIGN_SPACE)
				{
					//Pre=(Rect.Height()-Height-pRoot->SortParam1*(ItemCount-1))/2;			
					Space=pRoot->SortParam1;
				}
				else
				{
					Pre=(Rect.Height()-Height)/(ItemCount+1);			
					Space=Pre;
				}				
			}
			else if(AlignType==ALIGN_TOP)
			{
				if(pRoot->SortType&PARAM_ALIGN_SPACE)
				{
					//Pre=pRoot->SortParam1;	
					Space=pRoot->SortParam1;
				}
				else
				{
					//Pre=0;
					Space=0;
				}
			}
			else if(AlignType==ALIGN_BOTTOM)
			{
				if(pRoot->SortType&PARAM_ALIGN_SPACE)
				{
					//Pre=pRoot->SortParam1;	
					Space=pRoot->SortParam1;
				}
				else
				{
					//Pre=Rect.Height()-Height-pRoot->SortParam1*(ItemCount-1);
					Space=0;
				}
			}

			if(pRoot->SortType&ENABLE_RESIZE_BAR&&Space==0)
			{
				Space=5;
			}

			int Top=Rect.top+Pre;
			for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
			{
				SORT_TREE_ITEM * pChild=pRoot->ChildList[i];

				pChild->Rect.bottom=Top+pChild->Rect.Height();
				pChild->Rect.top=Top;
				Top+=pChild->Rect.Height();

				if((pRoot->SortType&ENABLE_RESIZE_BAR)&&i<pRoot->ChildList.GetCount()-1)
				{
					if(pChild->pResizeBar==NULL)
					{
						pChild->pResizeBar=new CResizeBar();
						pChild->pResizeBar->Create(this,pChild->ID,true);
					}
					pChild->pResizeBar->MoveWindow(Rect.left,Top,Rect.right-Rect.left,Space);
				}

				Top+=Space;

				int Left=Rect.left+(Rect.Width()-pChild->Rect.Width())/2;
				pChild->Rect.right=Left+pChild->Rect.Width();
				pChild->Rect.left=Left;
			}
		}
	}
	else
	{
		int Width=0;
		int ItemCount=pRoot->ChildList.GetCount();
		for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
		{
			SORT_TREE_ITEM * pChild=pRoot->ChildList[i];

			Width+=pChild->Rect.Width();
		}
		if(ItemCount)
		{		
			int AlignType=pRoot->SortType&ALIGN_MASK;
			int Pre=0;
			int Space=0;
			if(AlignType==ALIGN_CENTER)
			{
				if(pRoot->SortType&PARAM_ALIGN_SPACE)
				{
					//Pre=(Rect.Width()-Width-pRoot->SortParam1*(ItemCount-1))/2;			
					Space=pRoot->SortParam1;
				}
				else
				{
					//Pre=(Rect.Width()-Width)/(ItemCount+1);			
					Space=Pre;
				}
				
			}
			else if(AlignType==ALIGN_LEFT)
			{
				if(pRoot->SortType&PARAM_ALIGN_SPACE)
				{
					//Pre=pRoot->SortParam1;	
					Space=pRoot->SortParam1;
				}
				else
				{
					//Pre=0;
					Space=0;
				}
				
			}
			else if(AlignType==ALIGN_RIGHT)
			{
				if(pRoot->SortType&PARAM_ALIGN_SPACE)
				{
					//Pre=pRoot->SortParam1;	
					Space=pRoot->SortParam1;
				}
				else
				{
					//Pre=Rect.Width()-Width-pRoot->SortParam1*(ItemCount-1);
					Space=0;
				}
				
			}

			if(pRoot->SortType&ENABLE_RESIZE_BAR&&Space==0)
			{
				Space=5;
			}

			int Left=Rect.left+Pre;
			for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
			{
				SORT_TREE_ITEM * pChild=pRoot->ChildList[i];

				pChild->Rect.right=Left+pChild->Rect.Width();
				pChild->Rect.left=Left;

				Left+=pChild->Rect.Width();

				if((pRoot->SortType&ENABLE_RESIZE_BAR)&&i<pRoot->ChildList.GetCount()-1)
				{
					if(pChild->pResizeBar==NULL)
					{
						pChild->pResizeBar=new CResizeBar();
						pChild->pResizeBar->Create(this,pChild->ID,false);
					}
					pChild->pResizeBar->MoveWindow(Left,Rect.top,Space,Rect.bottom-Rect.top);
				}

				Left+=Space;

				int Top=Rect.top+(Rect.Height()-pChild->Rect.Height())/2;
				pChild->Rect.bottom=Top+pChild->Rect.Height();
				pChild->Rect.top=Top;

				
			}
		}
	}

	for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
	{
		AlignItems(pRoot->ChildList[i]);
	}
}

void CDialogItemSorter::GetDialogItemSize(SORT_TREE_ITEM * pRoot,bool GetRoot)
{
	if(GetRoot)
	{
		CWnd * pDialogItem=m_pParent->GetDlgItem(pRoot->DialogItemID);
		if(pDialogItem)
			pDialogItem->GetWindowRect(&(pRoot->Rect));
	}

	for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
	{
		GetDialogItemSize(pRoot->ChildList[i],true);
	}
}
void CDialogItemSorter::SetDialogItemSize(SORT_TREE_ITEM * pRoot)
{
	CWnd * pDialogItem=m_pParent->GetDlgItem(pRoot->DialogItemID);
	if(pDialogItem)
	{
		pDialogItem->MoveWindow(&(pRoot->Rect));
		pDialogItem->Invalidate();
	}

	for(size_t i=0;i<pRoot->ChildList.GetCount();i++)
	{
		SetDialogItemSize(pRoot->ChildList[i]);
	}
}