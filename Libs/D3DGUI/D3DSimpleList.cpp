/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DSimpleList.cpp                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\d3dsimplelist.h"

namespace D3DGUI{

#define LINE_GROW	8




IMPLEMENT_CLASS_INFO(CD3DSimpleList,CD3DWnd);

CD3DSimpleList::CD3DSimpleList():CD3DWnd()
{
	m_pScrollBar=NULL;	
	InitWnd(NULL);
}

CD3DSimpleList::CD3DSimpleList(CD3DGUI * pGUI):CD3DWnd(pGUI)
{	
	m_pScrollBar=NULL;	
	InitWnd(pGUI);	
}

CD3DSimpleList::~CD3DSimpleList(void)
{
}

void CD3DSimpleList::InitWnd(CD3DGUI *  pGUI)
{
	m_LineSpace=0;
	m_LineHeight=17;
	m_FirstVisibleLine=0;


	SAFE_DELETE_ARRAY(m_pWndRects);
	m_WndRectCount=RECT_TEXT+LINE_GROW*6;
	m_pWndRects=new LPIBASERECT[m_WndRectCount];
	ZeroMemory(m_pWndRects,sizeof(LPIBASERECT)*m_WndRectCount);
	m_TextRectStart=RECT_TEXT;
	m_TextRectCount=LINE_GROW*3;
	m_SelectRectStart=m_TextRectStart+m_TextRectCount;
	m_SelectRectCount=LINE_GROW*3;



	m_ClientCurWidth=0;
	m_ClientCurHeight=0;

	m_IsLoopScroll=false;
	m_ScrollBarWidth=16;

	

	m_IsControlPress=false;
	m_IsShiftPress=false;
	m_ShiftSelectStart=0;
	m_RecentSelect=-1;
	m_AllowMutliSelect=false;


	m_FontAlign=DT_LEFT|DT_VCENTER;

	m_CanDrag=false;

	SAFE_RELEASE(m_pScrollBar);

	if(m_pGUI)
	{
		m_pScrollBar=m_pGUI->CreateScrollBar(CEasyRect(0,0,0,0));
		m_pScrollBar->SetParent(this);
		m_pScrollBar->SetVisible(false);
		m_pScrollBar->SetName(_T("SL_ScrollBar"));
		m_pScrollBar->SetInternal(true);
		m_pScrollBar->EnableFocus(false);
	}

	
}


BOOL CD3DSimpleList::OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	if(CD3DWnd::OnMessage(pWnd,msg,wParam,lParam))	
		return true;

	switch(msg)
	{
	case WM_KEYDOWN:
		{			
			switch(wParam)
			{
			case VK_CONTROL:
				m_IsControlPress=true;
				return true;
			case VK_SHIFT:
				if(!m_IsShiftPress)
				{
					m_IsShiftPress=true;
					m_ShiftSelectStart=m_RecentSelect;
				}				
				return true;
			case VK_UP:
				{
					int Select=GetFirstSelectedItem();
					Select--;
					if(Select<0&&m_IsLoopScroll)
					{
						Select=GetItemCount()-1;
					}
					if(Select>=0)
					{
						ClearAllSelect();
						SelectItem(Select);
						MakeItemVisible(Select);
						HandleMessage(this,WM_D3DGUI_SIMPLE_LIST_SELCHANGE,(WPARAM)GetID(),Select);
						m_RecentSelect=Select;
					}
				}
				return true;
			case VK_DOWN:
				{
					int Select=GetFirstSelectedItem();
					Select++;
					if(Select>=GetItemCount()&&m_IsLoopScroll)
					{
						Select=0;
					}
					if(Select<GetItemCount())
					{
						ClearAllSelect();
						SelectItem(Select);
						MakeItemVisible(Select);
						HandleMessage(this,WM_D3DGUI_SIMPLE_LIST_SELCHANGE,(WPARAM)GetID(),Select);
						m_RecentSelect=Select;
					}
				}
				return true;
			}
		}
		break;
	case WM_KEYUP:
		{			
			switch(wParam)
			{
			case VK_CONTROL:
				m_IsControlPress=false;
				return true;
			case VK_SHIFT:
				m_IsShiftPress=false;
				m_ShiftSelectStart=0;
				return true;			
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{		
			int xPos=LOWORD(lParam);
			int yPos=HIWORD(lParam);
			int index=XYToItemIndex(xPos,yPos);
			if(index>=0)
			{
				if(m_IsShiftPress)
				{
					ClearAllSelect();
					SelectItemRange(m_ShiftSelectStart,index);
				}
				else
				{
					if(!m_IsControlPress&&!m_AllowMutliSelect)
						ClearAllSelect();
					SelectItem(index);
				}
				if(m_RecentSelect!=index)
				{					
					HandleMessage(this,WM_D3DGUI_SIMPLE_LIST_SELCHANGE,(WPARAM)GetID(),index);
					m_RecentSelect=index;
				}
				
			}
		}
		return true;
	case WM_LBUTTONUP:
		{
			int xPos=LOWORD(lParam);
			int yPos=HIWORD(lParam);
			int index=XYToItemIndex(xPos,yPos);
			if(index>=0)
			{
				HandleMessage(this,WM_D3DGUI_SIMPLE_LIST_LCLICK,(WPARAM)GetID(),index);
			}
		}
		return true;
	case WM_LBUTTONDBLCLK:
		{
			int xPos=LOWORD(lParam);
			int yPos=HIWORD(lParam);
			int index=XYToItemIndex(xPos,yPos);
			if(index>=0)
			{
				HandleMessage(this,WM_D3DGUI_SIMPLE_LIST_LDBCLICK,(WPARAM)GetID(),index);
			}
		}
		return true;
	case WM_MOUSEWHEEL:
		{
			int Delta = -GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
			Scroll(Delta);
		}
		return true;
	case WM_D3DGUI_SCROLL_BAR_SCROLL:
		ScrollTo((int)lParam);		
		return true;
	}	
	return false;
}

CEasyRect CD3DSimpleList::GetCenterRect()
{
	CEasyRect rect=m_WndRect;

	rect.top+=m_Borders[RECT_TOP];
	rect.bottom-=m_Borders[RECT_BOTTOM];
	rect.left+=m_Borders[RECT_LEFT];
	rect.right-=m_Borders[RECT_RIGHT];

	if(m_pScrollBar->IsVisible())
		rect.right-=m_ScrollBarWidth;
	return rect;
}

void CD3DSimpleList::SetFont(LOGFONT * pLogFont)
{
	if(pLogFont)
	{
		m_LogFont=*pLogFont;
		m_LineHeight=(m_LogFont.lfHeight+m_FontShadowWidth*2+1)*m_FontScale;
		m_WantUpdateFont=true;	
		UpdateFont();
	}
}

void CD3DSimpleList::SetFontAlign(DWORD Align)
{
	m_FontAlign=Align;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DSimpleList::SetFontSahdowWidth(DWORD ShadowWidth)
{
	m_FontShadowWidth=ShadowWidth;
	m_LineHeight=(m_LogFont.lfHeight+m_FontShadowWidth*2+1)*m_FontScale;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DSimpleList::SetFontScale(FLOAT Scale)
{
	m_FontScale=Scale;
	m_LineHeight=(m_LogFont.lfHeight+m_FontShadowWidth*2+1)*m_FontScale;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DSimpleList::SetVisible(bool IsVisible)
{
	CD3DWnd::SetVisible(IsVisible);	
	CheckScroll();
	if(IsVisible)
		UpdateText();
}

void CD3DSimpleList::GetMiniSize(int& Width,int& Height)
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

	Width=LMinWidth+RMinWidth+m_ScrollBarWidth;
	if(m_pScrollBar->IsVisible())
	{
		int MinWidth,MinHeight;
		m_pScrollBar->GetMiniSize(MinWidth,MinHeight);
		Height=m_Borders[RECT_TOP]+m_Borders[RECT_BOTTOM]+MinHeight;
	}
	else
	{
		Height=m_Borders[RECT_TOP]+m_Borders[RECT_BOTTOM];
	}

}


void CD3DSimpleList::UpdateRects()
{
	if(!m_IsVisible)
		return;

	CEasyRect ClientRect=GetClientRect();	
	ClientToScreen(&ClientRect);


	//计算需要的可视行数
	m_CurVisibleLineCount=(int)floor((FLOAT)ClientRect.Height()/(m_LineHeight+m_LineSpace));

	//根据需要扩充文字矩形数
	if(m_CurVisibleLineCount>GetTextRectCount())
		GrowTextRect(m_CurVisibleLineCount-GetTextRectCount()+LINE_GROW);

	CheckScroll();

	CD3DWnd::UpdateRects();

	if(m_pScrollBar->IsVisible())
	{
		CEasyRect Rect=GetClientRect();		
		Rect.left=Rect.right;
		Rect.right+=m_ScrollBarWidth;
		m_pScrollBar->SetRect(Rect);
	}
}

void CD3DSimpleList::UpdateText()
{
	if(!m_IsVisible)
		return;

	CEasyRect ClientRect=GetClientRect();	
	ClientToScreen(&ClientRect);

	FLOAT_RECT Rect(ClientRect);
	

	
	for(int i=0;i<GetTextRectCount();i++)
	{
		if(TextRect(i))
			TextRect(i)->SetVisible(false);
	}		

	
	//更新文字
	for(int i=0;i<(int)m_LineInfos.GetCount()-m_FirstVisibleLine;i++)
	{
		
		if(TextRect(i)==NULL)
		{
			TextRect(i)=CreateTextRect();
			RebuildOrder();			
		}			
		if(TextRect(i))
		{
			Rect.bottom=Rect.top+m_LineHeight;
			if(Rect.bottom<=(FLOAT)ClientRect.bottom)
			{
				TextRect(i)->EnableUpdate(false);
				TextRect(i)->SetRect(&Rect);
				
				TextRect(i)->SetTextW((LPCWSTR)m_LineInfos[i+m_FirstVisibleLine].Text,
					m_LineInfos[i+m_FirstVisibleLine].Text.GetLength());
				TextRect(i)->EnableUpdate(true);

				Rect.top=Rect.bottom+m_LineSpace;	
				TextRect(i)->SetVisible(true);
			}
			else
				break;

		}				
		
	}
	m_ClientCurWidth=ClientRect.Width();
	m_ClientCurHeight=ClientRect.Height();
	UpdateSelect();	
}

void CD3DSimpleList::UpdateSelect()
{
	if(!m_IsVisible)
		return;	


	for(int i=0;i<GetSelectRectCount();i++)
	{
		if(SelectRect(i))
			SelectRect(i)->SetVisible(false);
	}	

	CEasyRect ClientRect=GetClientRect();	
	ClientToScreen(&ClientRect);

	FLOAT_RECT Rect(ClientRect);

	for(int i=0;i<(int)m_LineInfos.GetCount()-m_FirstVisibleLine;i++)
	{			
		Rect.bottom=Rect.top+m_LineHeight;
		if(Rect.bottom<=(FLOAT)ClientRect.bottom)
		{
			if(m_LineInfos[i+m_FirstVisibleLine].IsSelected)
			{
				if(SelectRect(i)==NULL)
				{
					SelectRect(i)=CreateRect();
					RebuildOrder();			
					SelectRect(i)->SetEffectMode(D3DGUI_EFFECT_INVERSE_COLOR);
				}
				if(SelectRect(i))
				{
					SelectRect(i)->SetRect(&Rect);
					SelectRect(i)->SetVisible(true);
				}
			}
			Rect.top=Rect.bottom+m_LineSpace;	
		}
		else
			break;
	}
}


void CD3DSimpleList::SetScrollBarWidth(int Width)
{
	m_ScrollBarWidth=Width;
	UpdateRects();
}

//void CD3DSimpleList::EnableScrollBar(bool Enable)
//{
//	if(Enable)
//	m_pScrollBar->SetVisible(true);
//	UpdateRects();		
//}


void CD3DSimpleList::ScrollTo(int Line)
{
	m_FirstVisibleLine=Line;

	if(m_FirstVisibleLine>(int)m_LineInfos.GetCount()-m_CurVisibleLineCount)
		m_FirstVisibleLine=(int)m_LineInfos.GetCount()-m_CurVisibleLineCount;
	if(m_FirstVisibleLine<0)
		m_FirstVisibleLine=0;	

	if(m_pScrollBar)
	{
		if(m_pScrollBar->GetScrollPos()!=m_FirstVisibleLine)
			m_pScrollBar->SetScrollPos(m_FirstVisibleLine);
	}
	UpdateText();
}

int CD3DSimpleList::XYToItemIndex(int x,int y)
{
	for(int i=0;i<GetTextRectCount();i++)
	{
		if(TextRect(i))
		{
			if(TextRect(i)->IsVisible())
			{
				FLOAT_RECT Rect=TextRect(i)->GetRect();
				if(Rect.IsIn((FLOAT)x,(FLOAT)y))
				{
					return i+m_FirstVisibleLine;
				}
			}
		}
	}
	return -1;
}

void CD3DSimpleList::SelectItem(int Index)
{
	if(!m_AllowMutliSelect)
		ClearAllSelect();
	if(Index>=0&&Index<(int)m_LineInfos.GetCount())
	{
		m_LineInfos[Index].IsSelected=true;		
		UpdateSelect();
	}	
}

void CD3DSimpleList::SelectItemRange(int Start,int End)
{
	if(!m_AllowMutliSelect)
	{
		SelectItem(End);
		return;
	}

	if(Start>End)
	{
		int temp=End;
		End=Start;
		Start=temp;
	}
	if(Start<0)
		Start=0;
	if(End>=(int)m_LineInfos.GetCount())
		End=(int)m_LineInfos.GetCount()-1;
	
	for(int i=Start;i<=End;i++)
	{
		m_LineInfos[i].IsSelected=true;
	}
	UpdateSelect();
}

void CD3DSimpleList::ClearAllSelect()
{
	for(int i=0;i<(int)m_LineInfos.GetCount();i++)
	{
		m_LineInfos[i].IsSelected=false;
	}
	UpdateSelect();
}

int CD3DSimpleList::AddItem(LPCTSTR ItemText)
{
	SIMPLE_LIST_LINE_INFO Info;
	Info.Text=ItemText;
	m_LineInfos.Add(Info);
	CheckScroll();
	UpdateText();
	return (int)m_LineInfos.GetCount()-1;
}

bool CD3DSimpleList::InsertItem(int Index,LPCTSTR ItemText)
{
	SIMPLE_LIST_LINE_INFO Info;
	Info.Text=ItemText;
	
	if(m_LineInfos.Insert(Index,Info))
	{
		CheckScroll();
		UpdateText();
		return true;
	}
	else
	{
		return false;
	}	
}

bool CD3DSimpleList::DeleteItem(int Index)
{
	if(m_LineInfos.Delete(Index))
	{
		CheckScroll();
		UpdateText();
		return true;
	}
	return false;
}

void CD3DSimpleList::DeleteAllItem()
{
	m_LineInfos.Clear();
	CheckScroll();
	UpdateText();
}

int CD3DSimpleList::GetItemCount()
{
	return (int)m_LineInfos.GetCount();
}

int CD3DSimpleList::GetFirstSelectedItem()
{
	for(int i=0;i<(int)m_LineInfos.GetCount();i++)
	{
		if(m_LineInfos[i].IsSelected)
			return i;
	}
	return -1;
}

int CD3DSimpleList::GetNextSelectedItem(int Index)
{
	for(int i=Index+1;i<(int)m_LineInfos.GetCount();i++)
	{
		if(m_LineInfos[i].IsSelected)
			return i;
	}
	return -1;
}

bool CD3DSimpleList::SetItemText(int Index,LPCTSTR Text)
{
	if(Index>=0&&Index<(int)m_LineInfos.GetCount())
	{
		m_LineInfos[Index].Text=Text;
		return true;
	}
	return false;
}

bool CD3DSimpleList::GetItemText(int Index,CEasyString& Text)
{
	if(Index>=0&&Index<(int)m_LineInfos.GetCount())
	{		
		Text=m_LineInfos[Index].Text;
		return true;
	}
	return false;
}

bool CD3DSimpleList::SetItemData(int Index,LPVOID pData)
{
	if(Index>=0&&Index<(int)m_LineInfos.GetCount())
	{		
		m_LineInfos[Index].pItemData=pData;
		return true;
	}
	return false;
}

LPVOID CD3DSimpleList::GetItemData(int Index)
{
	if(Index>=0&&Index<(int)m_LineInfos.GetCount())
	{		
		return m_LineInfos[Index].pItemData;
	}
	return NULL;
}

void CD3DSimpleList::SaveToXml(xml_node * pXMLNode)
{
	xml_node Wnd=pXMLNode->append_child(node_element,_T("SimpleList"));
	Wnd.append_attribute(_T("Name"),(LPCTSTR)GetName());
	Wnd.append_attribute(_T("ID"),(long)GetID());
	Wnd.append_attribute(_T("IsInternal"),IsInternal());

	xml_node Behavior=Wnd.append_child(node_element,_T("Behavior"));
	SaveBehaviorToXML(Behavior);
	Behavior.append_attribute(_T("LineSpace"),(long)m_LineSpace);
	Behavior.append_attribute(_T("IsMultiSelect"),m_AllowMutliSelect);
	Behavior.append_attribute(_T("ScrollBarWidth"),(long)m_ScrollBarWidth);	
	Behavior.append_attribute(_T("IsLoopScroll"),m_IsLoopScroll);	

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

bool CD3DSimpleList::LoadFromXml(xml_node * pXMLNode)
{
	if(_tcsnicmp(pXMLNode->name(),_T("SimpleList"),11)!=0)
		return false;
	if(pXMLNode->has_attribute(_T("Name")))
		SetName(pXMLNode->attribute(_T("Name")).getvalue());

	if(pXMLNode->has_attribute(_T("ID")))
		SetID((UINT)pXMLNode->attribute(_T("ID")));

	if(pXMLNode->has_attribute(_T("IsInternal")))
		SetInternal((bool)pXMLNode->attribute(_T("IsInternal")));


	for(int i=0;i<(int)pXMLNode->children();i++)
	{
		if(_tcsnicmp(pXMLNode->child(i).name(),_T("Behavior"),9)==0)
		{
			LoadBehaviorFromXML(pXMLNode->child(i));
			if(pXMLNode->child(i).has_attribute(_T("LineSpace")))
				SetLineSpace((int)pXMLNode->child(i).attribute(_T("LineSpace")));
			if(pXMLNode->child(i).has_attribute(_T("IsMultiSelect")))
				SetAllowMutliSelect((bool)pXMLNode->child(i).attribute(_T("IsMultiSelect")));
			if(pXMLNode->child(i).has_attribute(_T("ScrollBarWidth")))
				SetScrollBarWidth((int)pXMLNode->child(i).attribute(_T("ScrollBarWidth")));	
			if(pXMLNode->child(i).has_attribute(_T("IsLoopScroll")))
				EnableLoopScroll(pXMLNode->child(i).attribute(_T("IsLoopScroll")));
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
	//识别内部对象
	for(int i=(int)m_ChildWndList.GetCount()-1;i>=0;i--)
	{
		if(m_ChildWndList[i]->IsInternal()&&
			m_ChildWndList[i]->IsKindOf(GET_CLASS_INFO(CD3DScrollBar))&&
			(_tcscmp(m_ChildWndList[i]->GetName(),_T("SL_ScrollBar"))==0)&&
			m_ChildWndList[i]!=m_pScrollBar)
		{
			CD3DScrollBar * pScrollBar=(CD3DScrollBar *)m_ChildWndList[i];
			SAFE_RELEASE(m_pScrollBar);
			m_pScrollBar=pScrollBar;
		}
	}
	TopChild(true);
	HandleMessage(this,WM_D3DGUI_CHILD_LOADED,GetID(),(LPARAM)this);
	return true;
}

void CD3DSimpleList::MakeItemVisible(int Index)
{
	if(Index<0||Index>=GetItemCount())
		return;

	CEasyRect ClientRect=GetClientRect();	
	ClientToScreen(&ClientRect);
	int TotalVisibleLine=ClientRect.Height()/m_LineHeight;
	if(Index<m_FirstVisibleLine)
	{
		ScrollTo(Index);
	}
	else if(Index>m_FirstVisibleLine+TotalVisibleLine-1)
	{
		ScrollTo(Index-TotalVisibleLine+1);
	}
}
////////////////////////////////////////////////////////
bool CD3DSimpleList::UpdateFont()
{
	if(m_WantUpdateFont)
	{
		for(int i=0;i<GetTextRectCount();i++)
		{			
			if(TextRect(i))
			{
				TextRect(i)->EnableUpdate(false);
				TextRect(i)->SetFont(&m_LogFont);
				TextRect(i)->SetColor(m_FontColor);
				TextRect(i)->SetAlign(m_FontAlign);
				TextRect(i)->SetShadowMode(m_FontShadowMode);
				TextRect(i)->SetShadowColor(m_FontShadowColor);
				TextRect(i)->SetShadowWidth(m_FontShadowWidth);	
				TextRect(i)->SetCharSpace(m_FontCharSpace);	
				TextRect(i)->SetLineSpace(m_FontLineSpace);
				TextRect(i)->SetScale(m_FontScale);
				m_FontCharSpace=TextRect(i)->GetCharSpace();
				m_FontLineSpace=TextRect(i)->GetLineSpace();
				TextRect(i)->EnableUpdate(true);				
				UpdateRects();
			}
		}
		m_WantUpdateFont=false;
		return true;
	}
	return false;
}

void CD3DSimpleList::GrowTextRect(int Grow)
{

	int NewCount=m_WndRectCount+Grow*2;
	

	LPIBASERECT *pNewRects=new LPIBASERECT[NewCount];
	ZeroMemory(pNewRects,sizeof(LPIBASERECT)*NewCount);
	//memcpy(pNewRects,m_pWndRects,sizeof(LPIBASERECT)*(m_WndRectCount-1));
	memcpy(pNewRects,m_pWndRects,sizeof(LPIBASERECT)*m_SelectRectStart);
	memcpy(pNewRects+m_SelectRectStart+Grow,m_pWndRects+m_SelectRectStart,sizeof(LPIBASERECT)*m_SelectRectCount);
	

	SAFE_DELETE_ARRAY(m_pWndRects);
	m_pWndRects=pNewRects;
	m_WndRectCount=NewCount;	

	m_TextRectStart=RECT_TEXT;
	m_TextRectCount+=Grow;
	m_SelectRectStart=m_TextRectStart+m_TextRectCount;
	m_SelectRectCount+=Grow;

}

void CD3DSimpleList::CheckScroll()
{
	if(!m_IsVisible)
		return;

	if((int)m_LineInfos.GetCount()>m_CurVisibleLineCount)
	{
		if(!m_pScrollBar->IsVisible())
		{
			m_pScrollBar->SetVisible(true);
			CEasyRect Rect=GetClientRect();		
			Rect.left=Rect.right;
			Rect.right+=m_ScrollBarWidth;
			m_pScrollBar->SetRect(Rect);
		}
		m_pScrollBar->SetMaxScrollPos((int)m_LineInfos.GetCount()-m_CurVisibleLineCount);
		ScrollTo(m_FirstVisibleLine);
		
	}
	else
	{
		m_FirstVisibleLine=0;
		m_pScrollBar->SetVisible(false);
	}
}

}