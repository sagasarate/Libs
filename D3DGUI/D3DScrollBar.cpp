/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DScrollBar.cpp                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\d3dscrollbar.h"

namespace D3DGUI{

#define SCROLL_READY_TIME	500
#define SCROLL_TIME			50

enum SCROLL_BAR_BUTTON_ID
{
	SBB_DEC=3242,
	SBB_INC,
	SBB_TRACK,
};

enum SCROLL_BAR_SCROLLING_STATUS
{
	SBSS_NONE,
	SBSS_READY,
	SBSS_SCROLLING,
};


IMPLEMENT_CLASS_INFO(CD3DScrollBar,CD3DWnd);

CD3DScrollBar::CD3DScrollBar():CD3DWnd()
{
	m_pDecButton=NULL;
	m_pIncButton=NULL;
	m_pTrackButton=NULL;
	InitWnd(NULL);
}

CD3DScrollBar::CD3DScrollBar(CD3DGUI * pGUI):CD3DWnd(pGUI)
{
	m_pDecButton=NULL;
	m_pIncButton=NULL;
	m_pTrackButton=NULL;
	InitWnd(pGUI);	
}

CD3DScrollBar::~CD3DScrollBar(void)
{
	//SAFE_RELEASE(m_pDecButton);
	//SAFE_RELEASE(m_pIncButton);
	//SAFE_RELEASE(m_pTrackButton);
}

void CD3DScrollBar::InitWnd(CD3DGUI *  pGUI)
{
	SAFE_RELEASE(m_pDecButton);
	SAFE_RELEASE(m_pIncButton);
	SAFE_RELEASE(m_pTrackButton);
	if(pGUI)
	{
		m_pDecButton=pGUI->CreateButton(CEasyRect(0,0,0,0));
		m_pDecButton->SetParent(this);
		m_pDecButton->SetName(_T("SBDec"));
		m_pDecButton->SetID(SBB_DEC);	
		m_pDecButton->SetInternal(true);
		m_pDecButton->EnableFocus(false);

		m_pIncButton=pGUI->CreateButton(CEasyRect(0,0,0,0));
		m_pIncButton->SetParent(this);
		m_pIncButton->SetName(_T("SBInc"));
		m_pIncButton->SetID(SBB_INC);
		m_pIncButton->SetInternal(true);
		m_pIncButton->EnableFocus(false);

		m_pTrackButton=pGUI->CreateButton(CEasyRect(0,0,0,0));
		m_pTrackButton->SetParent(this);
		m_pTrackButton->SetName(_T("SBTrack"));
		m_pTrackButton->SetID(SBB_TRACK);
		m_pTrackButton->SetInternal(true);
		m_pTrackButton->EnableFocus(false);
	}

	m_MaxScrollPos=0;
	m_CurScrollPos=0;

	m_ScrollStyle=SCROLL_BAR_VERTICAL;

	m_IsTracking=false;
	m_Scrolling=0;
	m_ScrollStatus=SBSS_NONE;
	m_TrackingStartPos=0;
	m_TrackingStartScrollPos=0;

	m_CanDrag=false;
	m_CanResize=false;

	m_Borders[RECT_TOP]=10;
	m_Borders[RECT_BOTTOM]=10;
}



BOOL CD3DScrollBar::OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(CD3DWnd::OnMessage(pWnd,msg,wParam,lParam))	
		return true;

	switch(msg)
	{
	case WM_D3DGUI_BUTTON_CLICK:
		if(m_ScrollStatus!=SBSS_SCROLLING)
		{
			if(wParam==SBB_DEC)
			{
				if(m_CurScrollPos>0)
					Scroll(-1,true);
			}
			else if(wParam==SBB_INC)
			{
				if(m_CurScrollPos<m_MaxScrollPos)
					Scroll(1,true);
			}	
			return true;
		}
		break;
	case WM_D3DGUI_BUTTON_DOWN:
		if(wParam==SBB_TRACK)
		{
			if(m_ScrollStyle==SCROLL_BAR_VERTICAL)
			{
				m_TrackingStartPos=HIWORD(lParam);
				m_TrackingStartBarPos=m_pTrackButton->GetWndRect().top;
			}
			else
			{
				m_TrackingStartPos=LOWORD(lParam);	
				m_TrackingStartBarPos=m_pTrackButton->GetWndRect().left;
			}
			m_TrackingStartScrollPos=m_CurScrollPos;
			m_IsTracking=true;
			SetCaptureAll(true);
			return true;
		}
		else if(wParam==SBB_DEC)
		{
			m_Scrolling=-1;
			m_ScrollStatus=SBSS_NONE;
			return true;
		}
		else if(wParam==SBB_INC)
		{
			m_Scrolling=1;
			m_ScrollStatus=SBSS_NONE;
			return true;
		}
		break;
	case WM_D3DGUI_BUTTON_UP:		
		if(wParam==SBB_DEC||wParam==SBB_INC)
		{
			m_Scrolling=0;			
			return true;
		}
		break;
	case WM_LBUTTONDOWN:
		if(m_IsTracking)
		{
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if(m_IsTracking)
		{
			m_IsTracking=false;
			SetCaptureAll(false);
			m_pTrackButton->SetButtonStatus(BUTTON_STATUS_NORMAL);
			UpdateTrack();
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if(m_IsTracking)
		{
			int Len;
			if(m_ScrollStyle==SCROLL_BAR_VERTICAL)
				Len=HIWORD(lParam)-m_TrackingStartPos;
			else
				Len=LOWORD(lParam)-m_TrackingStartPos;
		
			CEasyRect ClientRect=GetClientRect();
			ClientToScreen(&ClientRect);
			int TraceSize,BarSize;

			if(m_ScrollStyle==SCROLL_BAR_VERTICAL)
			{				
				BarSize=ClientRect.Height()/(m_MaxScrollPos+1);
				int MinWidth,MinHeight;
				m_pTrackButton->GetMiniSize(MinWidth,MinHeight);
				if(BarSize<MinHeight)
					BarSize=MinHeight;	
				TraceSize=ClientRect.Height()-BarSize;

				CEasyRect Rect=ClientRect;	
				Rect.top=m_TrackingStartBarPos+Len;

				if(Rect.top<ClientRect.top)
					Rect.top=ClientRect.top;			
				if(Rect.top>ClientRect.top+TraceSize)
					Rect.top=ClientRect.top+TraceSize;	
				Rect.bottom=Rect.top+BarSize;
				ScreenToClient(&Rect);
				m_pTrackButton->SetRect(Rect);
			}
			else
			{
				BarSize=ClientRect.Width()/(m_MaxScrollPos+1);
				int MinWidth,MinHeight;
				m_pTrackButton->GetMiniSize(MinWidth,MinHeight);
				if(BarSize<MinWidth)
					BarSize=MinWidth;		
				TraceSize=ClientRect.Width()-BarSize;

				CEasyRect Rect=ClientRect;	
				Rect.left=m_TrackingStartBarPos+Len;

				if(Rect.left<ClientRect.left)
					Rect.left=ClientRect.left;			
				if(Rect.left>ClientRect.left+TraceSize)
					Rect.left=ClientRect.left+TraceSize;	
				Rect.right=Rect.left+BarSize;
				ScreenToClient(&Rect);
				m_pTrackButton->SetRect(Rect);
			}
			if(TraceSize&&m_CurScrollPos!=m_TrackingStartScrollPos+Len*m_MaxScrollPos/TraceSize)
			{
				m_CurScrollPos=m_TrackingStartScrollPos+Len*m_MaxScrollPos/TraceSize;
				HandleMessage(this,WM_D3DGUI_SCROLL_BAR_SCROLL,(WPARAM)GetID(),m_CurScrollPos);
			}
			return true;			
		}
		break;
		
	}
	return false;
}

void CD3DScrollBar::SetScrollStyle(int ScrollStyle)
{
	m_ScrollStyle=ScrollStyle;	
	UpdateRects();
}
void CD3DScrollBar::SetMaxScrollPos(int MaxScrollPos)
{
	m_MaxScrollPos=MaxScrollPos;	
	UpdateTrack();
}

void CD3DScrollBar::Scroll(int Delta,bool SendMsg)
{
	m_CurScrollPos+=Delta;
	UpdateTrack();
	if(SendMsg)
		HandleMessage(this,WM_D3DGUI_SCROLL_BAR_SCROLL,(WPARAM)GetID(),m_CurScrollPos);

}

void CD3DScrollBar::SetScrollPos(int ScrollPos,bool SendMsg)
{
	m_CurScrollPos=ScrollPos;
	UpdateTrack();
	if(SendMsg)
		HandleMessage(this,WM_D3DGUI_SCROLL_BAR_SCROLL,(WPARAM)GetID(),m_CurScrollPos);
}

void CD3DScrollBar::SetVisible(bool IsVisible)
{
	m_pDecButton->SetVisible(IsVisible);
	m_pIncButton->SetVisible(IsVisible);
	m_pTrackButton->SetVisible(IsVisible);
	CD3DWnd::SetVisible(IsVisible);
}

CEasyRect CD3DScrollBar::GetCenterRect()
{
	CEasyRect rect=m_WndRect;

	rect.top+=m_Borders[RECT_TOP];
	rect.bottom-=m_Borders[RECT_BOTTOM];
	rect.left+=m_Borders[RECT_LEFT];
	rect.right-=m_Borders[RECT_RIGHT];
	if(m_ScrollStyle==SCROLL_BAR_VERTICAL)
	{
		rect.top+=rect.Width();
		rect.bottom-=rect.Width();
	}
	else
	{
		rect.left+=rect.Height();
		rect.right-=rect.Height();
	}
	return rect;
}

void CD3DScrollBar::UpdateRects()
{
	if(!m_IsVisible)
		return;

	CEasyRect Rect;
	CD3DWnd::UpdateRects();
	CEasyRect ClientRect=GetClientRect();
	
	

	if(m_ScrollStyle==SCROLL_BAR_VERTICAL)
	{
		Rect=ClientRect;
		Rect.top-=Rect.Width();
		Rect.bottom=Rect.top+Rect.Width();
		m_pDecButton->SetRect(Rect);

		Rect=ClientRect;
		Rect.top=Rect.bottom;
		Rect.bottom=Rect.top+Rect.Width();
		m_pIncButton->SetRect(Rect);		
	}
	else
	{
		Rect=ClientRect;
		Rect.left-=Rect.Height();
		Rect.right=Rect.left+Rect.Height();
		m_pDecButton->SetRect(Rect);

		Rect=ClientRect;
		Rect.left=Rect.right;
		Rect.right=Rect.left+Rect.Height();
		m_pIncButton->SetRect(Rect);		
	}
	UpdateTrack();

}

void CD3DScrollBar::UpdateTrack()
{
	if(!m_IsVisible)
		return;

	CEasyRect Rect;
	CEasyRect ClientRect=GetClientRect();
	ClientToScreen(&ClientRect);

	if(m_MaxScrollPos<0)
		m_MaxScrollPos=0;

	if(m_CurScrollPos<0)
		m_CurScrollPos=0;
	if(m_CurScrollPos>m_MaxScrollPos)
		m_CurScrollPos=m_MaxScrollPos;

	if(m_ScrollStyle==SCROLL_BAR_VERTICAL)
	{
		Rect=ClientRect;
		int Len=ClientRect.Height()/(m_MaxScrollPos+1);
		int MinWidth,MinHeight;
		m_pTrackButton->GetMiniSize(MinWidth,MinHeight);
		if(Len<MinHeight)
			Len=MinHeight;
		if(m_MaxScrollPos)
			Rect.top=Rect.top+(ClientRect.Height()-Len)*m_CurScrollPos/m_MaxScrollPos;			
		Rect.bottom=Rect.top+Len;
		ScreenToClient(&Rect);
		m_pTrackButton->SetRect(Rect);
	}
	else
	{
		Rect=ClientRect;
		int Len=ClientRect.Width()/(m_MaxScrollPos+1);
		int MinWidth,MinHeight;
		m_pTrackButton->GetMiniSize(MinWidth,MinHeight);
		if(Len<MinWidth)
			Len=MinWidth;
		if(m_MaxScrollPos)
			Rect.left=Rect.left+(ClientRect.Width()-Len)*m_CurScrollPos/m_MaxScrollPos;		
		Rect.right=Rect.left+Len;
		ScreenToClient(&Rect);
		m_pTrackButton->SetRect(Rect);
	}
}

void CD3DScrollBar::Update()
{
	if(m_Scrolling)
	{
		switch(m_ScrollStatus)
		{
		case SBSS_NONE:
			m_ScrollStartTime=CEasyTimer::GetTime();
			m_ScrollStatus=SBSS_READY;
			break;
		case SBSS_READY:
			if(CEasyTimer::GetTime()-m_ScrollStartTime>SCROLL_READY_TIME)
			{
				m_ScrollStatus=SBSS_SCROLLING;
				
			}
			break;
		case SBSS_SCROLLING:
			{
				DWORD time=CEasyTimer::GetTime();
				if(time-m_ScrollStartTime>SCROLL_TIME)
				{
					m_ScrollStartTime=time;
					Scroll(m_Scrolling,true);
				}
			}
			break;
		}
	}
}

void CD3DScrollBar::GetMiniSize(int& Width,int& Height)
{
	CD3DWnd::GetMiniSize(Width,Height);
	int Len=0;
	int MinWidth,MinHeight;

	CEasyRect Rect=GetCenterRect();

	if(m_ScrollStyle==SCROLL_BAR_VERTICAL)
	{
		m_pDecButton->GetMiniSize(MinWidth,MinHeight);		
		Width+=MinWidth;		
		m_pTrackButton->GetMiniSize(MinWidth,MinHeight);
		Height+=MinHeight;		
		Height+=Rect.Width()*2;
	}
	else
	{
		m_pDecButton->GetMiniSize(MinWidth,MinHeight);		
		Height+=MinHeight;		
		m_pTrackButton->GetMiniSize(MinWidth,MinHeight);
		Width+=MinWidth;
		Width+=Rect.Height()*2;
		
	}

}

void CD3DScrollBar::SaveToXml(xml_node * pXMLNode)
{
	xml_node Wnd=pXMLNode->append_child(node_element,_T("ScrollBar"));
	Wnd.append_attribute(_T("Name"),(LPCTSTR)GetName());
	Wnd.append_attribute(_T("ID"),(long)GetID());
	Wnd.append_attribute(_T("IsInternal"),IsInternal());

	xml_node Behavior=Wnd.append_child(node_element,_T("Behavior"));
	SaveBehaviorToXML(Behavior);
	Behavior.append_attribute(_T("ScrollRange"),(long)m_MaxScrollPos);
	Behavior.append_attribute(_T("Style"),(long)m_ScrollStyle);

	xml_node Frame=Wnd.append_child(node_element,_T("Frame"));
	SaveFrameToXML(Frame);	
	xml_node Borders=Wnd.append_child(node_element,_T("Borders"));
	SaveBorderToXML(Borders);	
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

bool CD3DScrollBar::LoadFromXml(xml_node * pXMLNode)
{
	if(_tcsnicmp(pXMLNode->name(),_T("ScrollBar"),10)!=0)
		return false;
	if(pXMLNode->has_attribute(_T("Name")))
		SetName(pXMLNode->attribute(_T("Name")).getvalue());

	if(pXMLNode->has_attribute(_T("ID")))
		SetID((UINT)pXMLNode->attribute(_T("ID")));

	if(pXMLNode->has_attribute(_T("IsInternal")))
		SetInternal(pXMLNode->attribute(_T("IsInternal")));


	for(int i=0;i<(int)pXMLNode->children();i++)
	{
		if(_tcsnicmp(pXMLNode->child(i).name(),_T("Behavior"),9)==0)
		{
			LoadBehaviorFromXML(pXMLNode->child(i));
			if(pXMLNode->child(i).has_attribute(_T("ScrollRange")))
				SetMaxScrollPos(pXMLNode->child(i).attribute(_T("ScrollRange")));
			if(pXMLNode->child(i).has_attribute(_T("Style")))
				SetScrollStyle(pXMLNode->child(i).attribute(_T("Style")));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Frame"),6)==0)
		{
			LoadFrameFromXML(pXMLNode->child(i));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Borders"),8)==0)
		{
			LoadBorderFromXML(pXMLNode->child(i));
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
			m_ChildWndList[i]->IsKindOf(GET_CLASS_INFO(CD3DButton))&&
			(_tcscmp(m_ChildWndList[i]->GetName(),_T("SBDec"))==0)&&
			m_ChildWndList[i]!=m_pDecButton)
		{
			CD3DButton * pButton=(CD3DButton *)m_ChildWndList[i];
			SAFE_RELEASE(m_pDecButton);
			m_pDecButton=pButton;
		}
		else if(m_ChildWndList[i]->IsInternal()&&
			m_ChildWndList[i]->IsKindOf(GET_CLASS_INFO(CD3DButton))&&
			(_tcscmp(m_ChildWndList[i]->GetName(),_T("SBInc"))==0)&&
			m_ChildWndList[i]!=m_pIncButton)
		{
			CD3DButton * pButton=(CD3DButton *)m_ChildWndList[i];
			SAFE_RELEASE(m_pIncButton);
			m_pIncButton=pButton;
		}
		else if(m_ChildWndList[i]->IsInternal()&&
			m_ChildWndList[i]->IsKindOf(GET_CLASS_INFO(CD3DButton))&&
			(_tcscmp(m_ChildWndList[i]->GetName(),_T("SBTrack"))==0)&&
			m_ChildWndList[i]!=m_pTrackButton)
		{
			CD3DButton * pButton=(CD3DButton *)m_ChildWndList[i];
			SAFE_RELEASE(m_pTrackButton);
			m_pTrackButton=pButton;
		}
	}
	TopChild(true);
	HandleMessage(this,WM_D3DGUI_CHILD_LOADED,GetID(),(LPARAM)this);
	return true;
}

}