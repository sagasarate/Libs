/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DCombo.cpp                                             */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\d3dcombo.h"

namespace D3DGUI{


enum COMBO_INTER_CONTROL
{
	CIC_EB=4729,
	CIC_LIST,
};

IMPLEMENT_CLASS_INFO(CD3DCombo,CD3DEdit);

CD3DCombo::CD3DCombo():CD3DEdit()
{
	m_pExpandButton=NULL;
	m_pComboList=NULL;
	m_ComboListHeight=0;
	InitWnd(NULL);

}

CD3DCombo::CD3DCombo(CD3DGUI * pGUI):CD3DEdit(pGUI)
{
	m_pExpandButton=NULL;
	m_pComboList=NULL;
	m_ComboListHeight=0;
	InitWnd(pGUI);	
}

CD3DCombo::~CD3DCombo(void)
{
	
}

void CD3DCombo::InitWnd(CD3DGUI *  pGUI)
{
	m_IsReadyOnly=true;	
	m_IsListShow=false;
	m_CanDrag=false;
	if(pGUI)
	{
		m_pExpandButton=pGUI->CreateButton(CEasyRect(0,0,0,0));
		m_pExpandButton->SetParent(this);
		m_pExpandButton->SetName(_T("ComboEB"));
		m_pExpandButton->SetID(CIC_EB);
		m_pExpandButton->SetInternal(true);
		//m_pExpandButton->EnableFocus(false);

		m_pComboList=pGUI->CreateSimpleList(CEasyRect(0,0,0,0));	
		m_pComboList->SetParent(this);
		m_pComboList->SetName(_T("ComboList"));
		m_pComboList->SetID(CIC_LIST);
		m_pComboList->SetVisible(false);
		m_pComboList->SetInternal(true);
		//m_pComboList->EnableFocus(false);
	}

	
}


BOOL CD3DCombo::OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(CD3DEdit::OnMessage(pWnd,msg,wParam,lParam))	
		return true;


	switch(msg)
	{
	case WM_D3DGUI_BUTTON_CLICK:
		if(wParam==CIC_EB)
		{
			m_IsListShow=!m_IsListShow;
			m_pComboList->SetVisible(m_IsListShow);
			if(m_pComboList->IsVisible())
			{
				CEasyRect Rect=GetWndRect();
				ScreenToClient(&Rect);

				int Height=m_pComboList->GetItemCount()*m_pComboList->GetItemHeight();
				int MiniHeight=m_pComboList->GetItemHeight()+m_pComboList->GetBorder(RECT_TOP)+m_pComboList->GetBorder(RECT_BOTTOM);

				if(m_ComboListHeight<MiniHeight)
					m_ComboListHeight=MiniHeight;

				Height+=m_pComboList->GetBorder(RECT_TOP)+m_pComboList->GetBorder(RECT_BOTTOM);

				if(Height<MiniHeight)
					Height=MiniHeight;

				if(Height>m_ComboListHeight)
					Height=m_ComboListHeight;

				Rect.top=Rect.bottom;
				Rect.bottom=Rect.top+Height;
				m_pComboList->SetRect(Rect);
				m_pComboList->SetFocus();				
			}
			return true;
		}
		break;
	case WM_D3DGUI_LOST_FOCUS:
		if(wParam==CIC_LIST)
		{
			CEasyRect Rect=m_pExpandButton->GetWndRect();
			if(!Rect.PtInRect(CEasyPoint(m_pGUI->GetCurMouseX(),m_pGUI->GetCurMouseY())))
			{
				m_IsListShow=false;
				m_pComboList->SetVisible(m_IsListShow);				
			}
			return true;
		}
		break;
	case WM_D3DGUI_SIMPLE_LIST_SELCHANGE:
		if(wParam==CIC_LIST)
		{
			int Index=m_pComboList->GetFirstSelectedItem();
			if(Index>=0)
			{
				CEasyString Text;
				m_pComboList->GetItemText(Index,Text);
				SetText(Text);
				HandleMessage(this,WM_D3DGUI_COMBO_SELCHANGE,(WPARAM)GetID(),Index);
			}
			return true;
		}
		break;
	case WM_D3DGUI_SIMPLE_LIST_LDBCLICK:
		if(wParam==CIC_LIST)
		{
			m_IsListShow=false;
			m_pComboList->SetVisible(m_IsListShow);		
		}
		break;
	}
	return false;
}

void CD3DCombo::SetVisible(bool IsVisible)
{
	CD3DWnd::SetVisible(IsVisible);
	if(IsVisible)
		m_pComboList->SetVisible(m_IsListShow);
	else
		m_pComboList->SetVisible(false);
}

CEasyRect CD3DCombo::GetCenterRect()
{
	CEasyRect rect=m_WndRect;

	rect.top+=m_Borders[RECT_TOP];
	rect.bottom-=m_Borders[RECT_BOTTOM];
	rect.left+=m_Borders[RECT_LEFT];
	rect.right-=m_Borders[RECT_RIGHT];

	if(m_pScrollBar&&m_IsMultiLine)
		rect.right-=m_ScrollBarWidth;

	rect.right-=rect.Height();
	return rect;
}


void CD3DCombo::UpdateRects()
{
	if(!m_IsVisible)
		return;

	CD3DEdit::UpdateRects();

	
	CEasyRect Rect=GetClientRect();		

	Rect.left=Rect.right;	

	if(m_pScrollBar&&m_IsMultiLine)
		Rect.left+=m_ScrollBarWidth;

	Rect.right=Rect.left+Rect.Height();

	m_pExpandButton->SetRect(Rect);
}


void CD3DCombo::SelectItem(int Index)
{
	m_pComboList->ClearAllSelect();
	m_pComboList->SelectItem(Index);
	Index=m_pComboList->GetFirstSelectedItem();
	if(Index>=0)
	{
		CEasyString Text;
		m_pComboList->GetItemText(Index,Text);
		SetText(Text);
	}
}

int CD3DCombo::AddItem(LPCTSTR ItemText)
{
	return m_pComboList->AddItem(ItemText);
}

bool CD3DCombo::InsertItem(int Index,LPCTSTR ItemText)
{
	return m_pComboList->InsertItem(Index,ItemText);
}

bool CD3DCombo::DeleteItem(int Index)
{
	return m_pComboList->DeleteItem(Index);
}

void CD3DCombo::DeleteAllItem()
{
	m_pComboList->DeleteAllItem();
}

int CD3DCombo::GetItemCount()
{
	return m_pComboList->GetItemCount();
}
bool CD3DCombo::SetItemText(int Index,LPCTSTR Text)
{
	return m_pComboList->SetItemText(Index,Text);
}
bool CD3DCombo::GetItemText(int Index,CEasyString& Text)
{
	return m_pComboList->GetItemText(Index,Text);
}
bool CD3DCombo::SetItemData(int Index,LPVOID pData)
{
	return m_pComboList->SetItemData(Index,pData);
}
LPVOID CD3DCombo::GetItemData(int Index)
{
	return m_pComboList->GetItemData(Index);
}

int CD3DCombo::GetFirstSelectedItem()
{
	return m_pComboList->GetFirstSelectedItem();
}
int CD3DCombo::GetNextSelectedItem(int Index)
{
	return m_pComboList->GetNextSelectedItem(Index);
}

void CD3DCombo::SaveToXml(xml_node * pXMLNode)
{
	xml_node Wnd=pXMLNode->append_child(node_element,_T("Combo"));
	Wnd.append_attribute(_T("Name"),(LPCTSTR)GetName());
	Wnd.append_attribute(_T("ID"),(long)GetID());
	Wnd.append_attribute(_T("IsInternal"),IsInternal());

	xml_node Behavior=Wnd.append_child(node_element,_T("Behavior"));
	SaveBehaviorToXML(Behavior);
	Behavior.append_attribute(_T("IsMultiLine"),m_IsMultiLine);
	Behavior.append_attribute(_T("ShowCaret"),m_IsShowCaret);
	Behavior.append_attribute(_T("CaretColor"),(long)m_CaretColor);
	Behavior.append_attribute(_T("AutoWrap"),m_AutoWrap);
	Behavior.append_attribute(_T("ReadOnly"),m_IsReadyOnly);
	Behavior.append_attribute(_T("Encryption"),m_IsEncryption);
	Behavior.append_attribute(_T("EnableScrollBar"),m_IsEnabledScrollBar);
	Behavior.append_attribute(_T("ScrollBarWidth"),(long)m_ScrollBarWidth);

	xml_node Frame=Wnd.append_child(node_element,_T("Frame"));
	SaveFrameToXML(Frame);
	xml_node Borders=Wnd.append_child(node_element,_T("Borders"));
	SaveBorderToXML(Borders);
	xml_node Text=Wnd.append_child(node_element,_T("Text"));
	SaveTextToXML(Text);
	xml_node Font=Wnd.append_child(node_element,_T("Font"));
	SaveFontToXML(Font);
	if(m_pTexture)
	{	
		xml_node Texture=Wnd.append_child(node_element,_T("Texture"));
		SaveTextureToXML(Texture);
	}

	if(m_ChildWndList.GetCount())
	{
		xml_node Childs=Wnd.append_child(node_element,_T("Childs"));
		SaveChildsToXml(Childs);
	}
}

bool CD3DCombo::LoadFromXml(xml_node * pXMLNode)
{
	if(_tcsnicmp(pXMLNode->name(),_T("Combo"),7)!=0)
		return false;
	if(pXMLNode->has_attribute(_T("Name")))
		SetName(pXMLNode->attribute(_T("Name")).getvalue());

	if(pXMLNode->has_attribute(_T("ID")))
		SetID(pXMLNode->attribute(_T("ID")));

	if(pXMLNode->has_attribute(_T("IsInternal")))
		SetInternal(pXMLNode->attribute(_T("IsInternal")));

	for(int i=0;i<(int)pXMLNode->children();i++)
	{
		if(_tcsnicmp(pXMLNode->child(i).name(),_T("Behavior"),9)==0)
		{
			LoadBehaviorFromXML(pXMLNode->child(i));
			if(pXMLNode->child(i).has_attribute(_T("IsMultiLine")))
				SetMultiLine(pXMLNode->child(i).attribute(_T("IsMultiLine")));
			if(pXMLNode->child(i).has_attribute(_T("ShowCaret")))
				EnableCaret(pXMLNode->child(i).attribute(_T("ShowCaret")));
			if(pXMLNode->child(i).has_attribute(_T("CaretColor")))
				SetCaretColor((UINT)pXMLNode->child(i).attribute(_T("CaretColor")));
			if(pXMLNode->child(i).has_attribute(_T("AutoWrap")))
				EnableAutoWrap(pXMLNode->child(i).attribute(_T("AutoWrap")));
			if(pXMLNode->child(i).has_attribute(_T("ReadOnly")))
				SetReadOnly(pXMLNode->child(i).attribute(_T("ReadOnly")));
			if(pXMLNode->child(i).has_attribute(_T("Encryption")))
				SetEncryption(pXMLNode->child(i).attribute(_T("Encryption")));
			if(pXMLNode->child(i).has_attribute(_T("EnableScrollBar")))
				EnableScrollBar(pXMLNode->child(i).attribute(_T("EnableScrollBar")));
			if(pXMLNode->child(i).has_attribute(_T("ScrollBarWidth")))
				SetScrollBarWidth(pXMLNode->child(i).attribute(_T("ScrollBarWidth")));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Frame"),6)==0)
		{
			LoadFrameFromXML(pXMLNode->child(i));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Borders"),8)==0)
		{
			LoadBorderFromXML(pXMLNode->child(i));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Text"),5)==0)
		{
			LoadTextFromXML(pXMLNode->child(i));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Font"),5)==0)
		{
			LoadFontFromXML(pXMLNode->child(i));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Texture"),8)==0)
		{
			LoadTextureFromXML(pXMLNode->child(i));
		}


	}
	HandleMessage(this,WM_D3DGUI_WND_LOADED,GetID(),(LPARAM)this);

	//装载子窗口
	for(int i=(int)pXMLNode->children()-1;i>=0;i--)
	{
		if(_tcsnicmp(pXMLNode->child(i).name(),_T("Childs"),7)==0)
		{
			LoadChildsFromXml(pXMLNode->child(i));
			break;
		}
	}

	for(int i=(int)m_ChildWndList.GetCount()-1;i>=0;i--)
	{
		if(m_ChildWndList[i]->IsInternal()&&
			m_ChildWndList[i]->IsKindOf(GET_CLASS_INFO(CD3DScrollBar))&&
			(_tcscmp(m_ChildWndList[i]->GetName(),_T("ED_ScrollBar"))==0)&&
			m_ChildWndList[i]!=m_pScrollBar)
		{
			CD3DScrollBar * pScrollBar=(CD3DScrollBar *)m_ChildWndList[i];
			SAFE_RELEASE(m_pScrollBar);
			m_pScrollBar=pScrollBar;
		}
		else if(m_ChildWndList[i]->IsInternal()&&
			m_ChildWndList[i]->IsKindOf(GET_CLASS_INFO(CD3DButton))&&
			(_tcscmp(m_ChildWndList[i]->GetName(),_T("ComboEB"))==0)&&
			m_ChildWndList[i]!=m_pExpandButton)
		{
			CD3DButton * pButton=(CD3DButton *)m_ChildWndList[i];
			SAFE_RELEASE(m_pExpandButton);
			m_pExpandButton=pButton;
		}
		else if(m_ChildWndList[i]->IsInternal()&&
			m_ChildWndList[i]->IsKindOf(GET_CLASS_INFO(CD3DSimpleList))&&
			(_tcscmp(m_ChildWndList[i]->GetName(),_T("ComboList"))==0)&&
			m_ChildWndList[i]!=m_pComboList)
		{
			CD3DSimpleList * pWnd=(CD3DSimpleList *)m_ChildWndList[i];
			SAFE_RELEASE(m_pComboList);
			m_pComboList=pWnd;
			m_pComboList->SetVisible(false);
		}
		
	}	
	TopChild(true);
	HandleMessage(this,WM_D3DGUI_CHILD_LOADED,GetID(),(LPARAM)this);
	
	
	return true;
}



/////////////////////////////////////////

void CD3DCombo::SaveFrameToXML(xml_node& Frame)
{
	CD3DWnd::SaveFrameToXML(Frame);

	Frame.append_attribute(_T("ComboListHeight"),(long)GetComboListHeight());
}

void CD3DCombo::LoadFrameFromXML(xml_node& Frame)
{
	CD3DWnd::LoadFrameFromXML(Frame);
	if(Frame.has_attribute(_T("ComboListHeight")))
		SetComboListHeight(Frame.attribute(_T("ComboListHeight")));
}


void CD3DCombo::GetMiniSize(int& Width,int& Height)
{
	//检测窗口大小
	int LMinWidth,RMinWidth;

	LMinWidth=m_Borders[RECT_LEFT];
	if(LMinWidth<m_Borders[RECT_TOP_LEFT])
		LMinWidth=m_Borders[RECT_TOP_LEFT];
	if(LMinWidth<m_Borders[RECT_BOTTOM_LEFT])
		LMinWidth=m_Borders[RECT_BOTTOM_LEFT];

	RMinWidth=m_Borders[RECT_RIGHT];
	if(RMinWidth<m_Borders[RECT_TOP_RIGHT])
		RMinWidth=m_Borders[RECT_TOP_RIGHT];
	if(RMinWidth<m_Borders[RECT_BOTTOM_RIGHT])
		RMinWidth=m_Borders[RECT_BOTTOM_RIGHT];

	CEasyRect Rect=GetCenterRect();
	
	int MinWidth,MinHeight;
	m_pExpandButton->GetMiniSize(MinWidth,MinHeight);

	if(m_pScrollBar&&m_IsMultiLine)
		Width=LMinWidth+RMinWidth+MinWidth+Rect.Height()+m_ScrollBarWidth;
	else
		Width=LMinWidth+RMinWidth+MinWidth+Rect.Height();
	Height=m_Borders[RECT_TOP]+m_Borders[RECT_BOTTOM]+MinHeight;
	

}

}