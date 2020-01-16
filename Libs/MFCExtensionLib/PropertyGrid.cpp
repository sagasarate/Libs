// PropertyGrid.cpp : 实现文件
//

#include "stdafx.h"
#include "PropertyGrid.h"



// CPropertyGrid

namespace PropertyGrid
{

IMPLEMENT_DYNAMIC(CPropertyGrid, CListCtrl)

CPropertyGrid::CPropertyGrid()
	:m_PropertyList(_T("CPropertyGrid"))
{
	m_CurEditItem=-1;
}

CPropertyGrid::~CPropertyGrid()
{
}


BEGIN_MESSAGE_MAP(CPropertyGrid, CListCtrl)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, &CPropertyGrid::OnNMClick)
END_MESSAGE_MAP()

BOOL CPropertyGrid::Create(const RECT& Rect,CWnd * pParentWnd)
{	
	if(CListCtrl::Create(WS_CHILD|LVS_REPORT,Rect,pParentWnd,123324))
	{
		SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		InsertColumn(0,_T("属性"),LVCFMT_LEFT,80);
		InsertColumn(1,_T("值"),LVCFMT_LEFT,100);



		CFont * pFont=GetFont();

		LOGFONT LogFont;
		pFont->GetLogFont(&LogFont);

		HFONT hFont=CreateFontIndirect(&LogFont);


	

		m_Inputer.Create(this,CFont::FromHandle(hFont));

		return TRUE;
	}
	return FALSE;
}

void CPropertyGrid::Clear()
{
	m_PropertyList.Clear();
	DeleteAllItems();	
	m_CurEditItem=-1;
}

void CPropertyGrid::AddProperty(PROPERTY_INFO * pInfo)
{	
	int Format=0;
	switch(pInfo->PropertyType)
	{
	case PROT_NUMBER:
	case PROT_SPIN_NUMBER:
		Format=LVCFMT_RIGHT;
		break;
	default:
		Format=LVCFMT_LEFT;
		break;
	}
	
	int Item=InsertItem(GetItemCount(),pInfo->Name);
	SetItemText(Item,1,pInfo->Value);
	SetItemData(Item,m_PropertyList.GetCount());
	pInfo->BindRow=Item;
	m_PropertyList.Add(*pInfo);	
}

void CPropertyGrid::SetProperty(PROPERTY_INFO * pInfo,bool AddWhileNotExist)
{
	PROPERTY_INFO * pPropertyInfo=GetPropertyByID(pInfo->ID);
	if(pPropertyInfo)
	{
		int Item=pPropertyInfo->BindRow;
		*pPropertyInfo=*pInfo;
		pPropertyInfo->BindRow=Item;
		SetItemText(Item,1,pPropertyInfo->Value);
	}
	else if(AddWhileNotExist)
	{
		AddProperty(pInfo);
	}
}

void CPropertyGrid::SetPropertyValue(int Index,CPropertyValue Value)
{
	if(Index>=0&&Index<m_PropertyList.GetCount())
	{
		m_PropertyList[Index].Value=Value;
		int Item=m_PropertyList[Index].BindRow;
		SetItemText(Item,1,m_PropertyList[Index].Value);
	}
}

void CPropertyGrid::SetPropertyValueByID(int ID,CPropertyValue Value)
{
	for(int i=0;i<m_PropertyList.GetCount();i++)
	{
		if(m_PropertyList[i].ID==ID)
		{
			m_PropertyList[i].Value=Value;
			int Item=m_PropertyList[i].BindRow;
			SetItemText(Item,1,m_PropertyList[i].Value);			
		}
	}
}

void CPropertyGrid::SetPropertyValue(LPCTSTR Name,CPropertyValue Value)
{
	for(int i=0;i<m_PropertyList.GetCount();i++)
	{
		if(m_PropertyList[i].Name==Name)
		{
			m_PropertyList[i].Value=Value;
			int Item=m_PropertyList[i].BindRow;
			SetItemText(Item,1,m_PropertyList[i].Value);
		}
	}
}

PROPERTY_INFO * CPropertyGrid::GetPropertyByName(LPCTSTR Name)
{
	for(int i=0;i<m_PropertyList.GetCount();i++)
	{
		if(m_PropertyList[i].Name==Name)
			return &(m_PropertyList[i]);
	}
	return NULL;
}

PROPERTY_INFO * CPropertyGrid::GetPropertyByID(int ID)
{
	for(int i=0;i<m_PropertyList.GetCount();i++)
	{
		if(m_PropertyList[i].ID==ID)
			return &(m_PropertyList[i]);
	}
	return NULL;
}


void CPropertyGrid::OnFinishEdit(bool IsChanged)
{
	if(m_CurEditItem<0||m_CurEditItem>=m_PropertyList.GetCount())
		return;
	PROPERTY_INFO * pInfo=&(m_PropertyList[m_CurEditItem]);

	PROPERTY_CHANGE_INFO ChangeInfo;

	ChangeInfo.pInfo=pInfo;

	GetParent()->SendMessage(WM_PROPERTY_GRID_EDIT_END,m_CurEditItem,(LPARAM)&pInfo);

	if(pInfo->PropertyType==PROT_CUSTOM)
	{
		if(IsChanged)
			GetParent()->SendMessage(WM_PROPERTY_GRID_VALUE_CHANGED,m_CurEditItem,(LPARAM)&ChangeInfo);
	}
	else
	{
		if((CString)pInfo->Value!=m_Inputer.GetInputText())
		{
			ChangeInfo.NewValue=m_Inputer.GetInputText();
			ChangeInfo.OldValue=pInfo->Value;
			pInfo->Value=m_Inputer.GetInputText();			

			SetItemText(pInfo->BindRow,1,(LPCTSTR)pInfo->Value);
			if(GetParent())
			{
				GetParent()->SendMessage(WM_PROPERTY_GRID_VALUE_CHANGED,m_CurEditItem,(LPARAM)&ChangeInfo);
			}
		}	
	}
	m_CurEditItem=-1;
}
// CPropertyGrid 消息处理程序

BOOL CPropertyGrid::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	LVHITTESTINFO HitTestInfo;

	GetCursorPos(&HitTestInfo.pt);
	ScreenToClient(&HitTestInfo.pt);

	SubItemHitTest(&HitTestInfo);
	if((HitTestInfo.flags&LVHT_ONITEM)&&HitTestInfo.iSubItem==1)
	{
		CRect ItemRect;
		m_CurEditItem=GetItemData(HitTestInfo.iItem);
		GetSubItemRect(HitTestInfo.iItem,1,LVIR_BOUNDS,ItemRect);
		m_Inputer.MoveWindow(&ItemRect);
		PROPERTY_INFO * pInfo=&(m_PropertyList[m_CurEditItem]);
		m_Inputer.StartInput(pInfo->PropertyType,pInfo->Define,pInfo->Value.GetString(),pInfo->pConfigDialog);	
		GetParent()->SendMessage(WM_PROPERTY_GRID_EDIT_START,m_CurEditItem,(LPARAM)&pInfo);
	}
	return TRUE;
}


}



