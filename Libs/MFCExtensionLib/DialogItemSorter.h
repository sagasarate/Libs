/****************************************************************************/
/*                                                                          */
/*      文件名:    DialogItemSorter.h                                       */
/*      创建日期:  2009年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#include "ResizeBar.h"

class CDialogItemSorter
{
public:
	enum DIALOG_ITEM_SORT_TYPE
	{
		SORT_MASK=0x01,
		SORT_HORIZONTAL=0x00,				//水平排列
		SORT_VERTICAL=0x01,					//垂直排列
		RESIZE_MASK=0x06,
		RESIZE_FIT_PARENT_WIDTH=0x02,
		RESIZE_FIT_PARENT_HEIGHT=0x04,
		ALIGN_MASK=0x18,
		ALIGN_LEFT=0x08,
		ALIGN_TOP=0x08,
		ALIGN_RIGHT=0x10,
		ALIGN_BOTTOM=0x10,
		ALIGN_CENTER=0x00,
		PARAM_FIT_RATE=0x20,
		PARAM_ALIGN_SPACE=0x40,
		PARAM_ALIGN_PRE_SPACE=0x80,
		ENABLE_RESIZE_BAR=0x100,
	};
protected:
	struct SORT_TREE_ITEM
	{
		UINT										ID;
		int											SortType;		
		float										SortParam1;
		float										SortParam2;
		SORT_TREE_ITEM *							pParent;
		CArray<SORT_TREE_ITEM *,SORT_TREE_ITEM *>	ChildList;
		CRect										Rect; 
		int											DialogItemID;
		CResizeBar *								pResizeBar;

		SORT_TREE_ITEM()
		{
			ID=0;
			SortType=0;
			SortParam1=0;
			SortParam2=0;
			pParent=NULL;
			DialogItemID=0;
			pResizeBar=NULL;
		}
		~SORT_TREE_ITEM()
		{
			for(size_t i=0;i<ChildList.GetCount();i++)
			{
				SAFE_DELETE(ChildList[i]);
			}
			ChildList.RemoveAll();
			SAFE_DELETE(pResizeBar);
		}
	};
	CWnd *				m_pParent;
	SORT_TREE_ITEM *	m_pItemTree;
public:
	CDialogItemSorter(void);
	~CDialogItemSorter(void);

	void SetParent(CWnd * pParent)
	{
		m_pParent=pParent;
	}
	CWnd * GetParent()
	{
		return m_pParent;
	}
	void SetSortType(UINT ID,int SortType,float SortParam1,float SortParam2);

	bool AddDialogItem(UINT ParentID,UINT ID,int SortType,int DialogItemID,CRect Rect,float SortParam1,float SortParam2);

	void DoSort();

	void GetResizeRange(UINT ItemID,RECT& Range);
	void OnResize(UINT ItemID,CSize& Size);
protected:
	CDialogItemSorter::SORT_TREE_ITEM * FindTreeNode(SORT_TREE_ITEM * pRoot,UINT ID);
	void ResizeItems(SORT_TREE_ITEM * pRoot);
	void AlignItems(SORT_TREE_ITEM * pRoot);
	void GetDialogItemSize(SORT_TREE_ITEM * pRoot,bool GetRoot);
	void SetDialogItemSize(SORT_TREE_ITEM * pRoot);
};
