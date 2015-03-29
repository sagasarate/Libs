/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWnd.cpp                                               */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include "d3dwnd.h"

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif

#include <sstream>

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new new( __FILE__, __LINE__ )
#endif

namespace D3DGUI{


IMPLEMENT_CLASS_INFO(CD3DWnd,CNameObject);

CD3DWnd::CD3DWnd():CNameObject()
{
	m_pWndRects=NULL;
	m_pTexture=NULL;
	m_pParentWnd=NULL;
	m_pGUI=NULL;
	InitWnd(NULL);
}

CD3DWnd::CD3DWnd(CD3DGUI *  pGUI):CNameObject()
{
	m_pWndRects=NULL;
	m_pTexture=NULL;
	m_pParentWnd=NULL;
	m_pGUI=NULL;
	InitWnd(pGUI);
}

void CD3DWnd::InitWnd(CD3DGUI *  pGUI)
{
	Destory();

	m_pGUI=pGUI; 

	if(m_pGUI)
	{
		m_pGUI->AddWnd(this);
		m_pGUI->AddRootWnd(this);		
	}

	m_pParentWnd=NULL;

	m_IsDragging=false;	


	m_WndRectCount=RECT_MAX;

	
	m_pWndRects=new LPIBASERECT[m_WndRectCount];
	ZeroMemory(m_pWndRects,sizeof(LPIBASERECT)*m_WndRectCount);

	m_pTexture=NULL;


	m_WndRect=CEasyRect(0,0,100,100);

	ZeroMemory(&m_Borders,sizeof(m_Borders));	

	ZeroMemory(&m_LogFont,sizeof(m_LogFont));

	_tcscpy_s(m_LogFont.lfFaceName,LF_FACESIZE,_T("SimSun"));
	m_LogFont.lfHeight=16;
	m_LogFont.lfWidth=0;
	m_LogFont.lfEscapement=0;
	m_LogFont.lfOrientation=0;
	m_LogFont.lfWeight=400;
	m_LogFont.lfItalic=false;
	m_LogFont.lfUnderline=false;
	m_LogFont.lfStrikeOut=false;
	m_LogFont.lfCharSet=DEFAULT_CHARSET;
	m_LogFont.lfOutPrecision=OUT_DEFAULT_PRECIS;
	m_LogFont.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	m_LogFont.lfQuality=DEFAULT_QUALITY;
	m_LogFont.lfPitchAndFamily=DEFAULT_PITCH | FF_DONTCARE;

	m_FontColor=0xffffffff;
	m_FontAlign=DT_WORDBREAK;
	m_FontShadowMode=0;
	m_FontShadowColor=0xff000000;
	m_FontShadowWidth=0;
	m_FontCharSpace=0;
	m_FontLineSpace=0;
	m_FontScale=1.0f;


	m_CanDrag=true;
	m_CanResize=false;
	m_Resizing=0;	
	m_TabIndex=0;
	m_IsTabContainer=false;

	m_IsCaptureAll=false;
	m_IsVisible=true;
	m_IsActive=false;

	m_IsEnable=true;
	m_CanGetFocus=true;
	m_IsTopmost=false;

	m_BKColor=0xffffffff;

	m_WantUpdateFont=false;
	m_IsInternal=false;	

	m_WndData=0;
}

void CD3DWnd::Destory()
{
	//删除子窗口
	for(int i=m_ChildWndList.GetCount()-1;i>=0;i--)	
	{			
		CD3DWnd * pWnd=m_ChildWndList[i];
		SAFE_RELEASE(pWnd);
	}
	m_ChildWndList.Clear();

	//从父窗口那里删除自己

	if(m_pParentWnd)
	{
		m_pParentWnd->DelChildWnd(this);
	}
	else
	{
		if(m_pGUI)
			m_pGUI->DelRootWnd(this);		
	}
	
	if(m_pWndRects)
	{
		for(int i=0;i<m_WndRectCount;i++)
		{
			SAFE_RELEASE(m_pWndRects[i]);
		}
		m_WndRectCount=0;
	}
	SAFE_DELETE_ARRAY(m_pWndRects);
	SAFE_RELEASE(m_pTexture);

	if(m_pGUI)
		m_pGUI->DelWnd(this);

	CNameObject::Destory();
}



CD3DWnd::~CD3DWnd(void)
{	
	Destory();
}


bool CD3DWnd::SetTexture(LPCTSTR TextureFileName,RECT& TextureRect)
{
	SAFE_RELEASE(m_pTexture);
	m_pTexture=m_pGUI->GetObjectCreator()->CreateTextureFromFile(TextureFileName);
	if(m_pTexture)
	{	
		m_TextureRect=TextureRect;
		return UpdateTextureRect();
	}

	return false;
	
}

void CD3DWnd::GetTexture(CEasyString& TextureFileName,RECT& TextureRect)
{
	if(m_pTexture)
		TextureFileName=m_pTexture->GetName();
	TextureRect=m_TextureRect;
}

void CD3DWnd::Move(int x, int y)
{	
	CEasyRect OldRect=m_WndRect;	
	if(m_WndRect.left!=x||m_WndRect.top!=y)
	{
		m_WndRect.MoveToXY(x,y);
		UpdateRects();		
	}
}

void CD3DWnd::Resize(int width, int height)
{
	m_WndRect.right=m_WndRect.left+width;
	m_WndRect.bottom=m_WndRect.top+height;
	UpdateRects();
}

void CD3DWnd::SetRect(RECT& rect)
{
	m_WndRect=rect;	
	UpdateRects();
}

CEasyRect CD3DWnd::GetRect()
{
	return m_WndRect;
}

CEasyRect CD3DWnd::GetCenterRect()
{
	CEasyRect rect=m_WndRect;

	rect.top+=m_Borders[RECT_TOP];
	rect.bottom-=m_Borders[RECT_BOTTOM];
	rect.left+=m_Borders[RECT_LEFT];
	rect.right-=m_Borders[RECT_RIGHT];
	
	return rect;
}

CEasyRect CD3DWnd::GetWndRect()
{
	CEasyRect Rect=GetRect();
	if(m_pParentWnd)
		m_pParentWnd->ClientToScreen(&Rect);
	return Rect;
}

CEasyRect CD3DWnd::GetClientRect()
{
	CEasyRect Rect=GetCenterRect();	
	Rect.right-=Rect.left;
	Rect.bottom-=Rect.top;
	Rect.left=0;
	Rect.top=0;
	return Rect;
}

void CD3DWnd::SetBorders(WIN_BORDERS& Borders)
{
	memcpy(m_Borders,Borders,sizeof(WIN_BORDERS));
	UpdateRects();
	UpdateTextureRect();
}

void CD3DWnd::GetBorders(WIN_BORDERS& Borders)
{
	memcpy(Borders,m_Borders,sizeof(WIN_BORDERS));
}

int CD3DWnd::GetBorder(UINT Index)
{
	if(Index<RECT_BORDER_COUNT)
	{
		return m_Borders[Index];
	}
	return -1;
}

bool CD3DWnd::SetEffectMode(UINT Index,int EffectMode)
{
	if(Index<m_WndRectCount)
	{
		if(m_pWndRects[Index])
		{
			return m_pWndRects[Index]->SetEffectMode(EffectMode);
		}
	}
	return false;
}

BOOL CD3DWnd::OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{	
	CEasyRect rect;
	int xPos,yPos;	


	switch(msg)
	{
	case WM_LBUTTONDOWN:
		xPos=LOWORD(lParam);
		yPos=HIWORD(lParam);			

		if(m_CanDrag)
		{
			if(m_pWndRects[RECT_TOP])
			{
				rect=m_pWndRects[RECT_TOP]->GetRect();			
				if(rect.PtInRect(CEasyPoint(xPos,yPos)))
				{
					CEasyRect WndRect=GetRect();				
					m_IsDragging=true;
					m_DraggingStartMousePoint.x=xPos;
					m_DraggingStartMousePoint.y=yPos;
					m_DraggingStartPoint.x=WndRect.left;
					m_DraggingStartPoint.y=WndRect.top;			
					SetCaptureAll(true);
					return true;
				}
			}
		}
		if(m_CanResize)
		{
			if(m_pWndRects[RECT_RIGHT])
			{
				rect=m_pWndRects[RECT_RIGHT]->GetRect();			
				if(rect.PtInRect(CEasyPoint(xPos,yPos)))
				{
					CEasyRect WndRect=GetRect();			
					m_Resizing=2;
					m_DraggingStartMousePoint.x=xPos;
					m_DraggingStartMousePoint.y=yPos;
					m_DraggingStartPoint.x=WndRect.left;
					m_DraggingStartPoint.y=WndRect.top;	
					m_DraggingStartSize.cx=WndRect.Width();
					m_DraggingStartSize.cy=WndRect.Height();
					SetCaptureAll(true);
					return true;
				}
			}
			if(m_pWndRects[RECT_BOTTOM])
			{
				rect=m_pWndRects[RECT_BOTTOM]->GetRect();		
				if(rect.PtInRect(CEasyPoint(xPos,yPos)))
				{
					CEasyRect WndRect=GetRect();			
					m_Resizing=4;
					m_DraggingStartMousePoint.x=xPos;
					m_DraggingStartMousePoint.y=yPos;
					m_DraggingStartPoint.x=WndRect.left;
					m_DraggingStartPoint.y=WndRect.top;	
					m_DraggingStartSize.cx=WndRect.Width();
					m_DraggingStartSize.cy=WndRect.Height();
					SetCaptureAll(true);
					return true;
				}
			}
			if(m_pWndRects[RECT_LEFT])
			{
				rect=m_pWndRects[RECT_LEFT]->GetRect();
				if(rect.PtInRect(CEasyPoint(xPos,yPos)))
				{
					CEasyRect WndRect=GetRect();			
					m_Resizing=8;
					m_DraggingStartMousePoint.x=xPos;
					m_DraggingStartMousePoint.y=yPos;
					m_DraggingStartPoint.x=WndRect.left;
					m_DraggingStartPoint.y=WndRect.top;	
					m_DraggingStartSize.cx=WndRect.Width();
					m_DraggingStartSize.cy=WndRect.Height();
					SetCaptureAll(true);
					return true;
				}
			}
			if(m_pWndRects[RECT_TOP_LEFT])
			{
				rect=m_pWndRects[RECT_TOP_LEFT]->GetRect();
				if(rect.PtInRect(CEasyPoint(xPos,yPos)))
				{
					CEasyRect WndRect=GetRect();			
					m_Resizing=1|8;
					m_DraggingStartMousePoint.x=xPos;
					m_DraggingStartMousePoint.y=yPos;
					m_DraggingStartPoint.x=WndRect.left;
					m_DraggingStartPoint.y=WndRect.top;	
					m_DraggingStartSize.cx=WndRect.Width();
					m_DraggingStartSize.cy=WndRect.Height();
					SetCaptureAll(true);
					return true;
				}
			}
			if(m_pWndRects[RECT_TOP_RIGHT])
			{
				rect=m_pWndRects[RECT_TOP_RIGHT]->GetRect();
				if(rect.PtInRect(CEasyPoint(xPos,yPos)))
				{
					CEasyRect WndRect=GetRect();			
					m_Resizing=1|2;
					m_DraggingStartMousePoint.x=xPos;
					m_DraggingStartMousePoint.y=yPos;
					m_DraggingStartPoint.x=WndRect.left;
					m_DraggingStartPoint.y=WndRect.top;	
					m_DraggingStartSize.cx=WndRect.Width();
					m_DraggingStartSize.cy=WndRect.Height();
					SetCaptureAll(true);
					return true;
				}
			}
			if(m_pWndRects[RECT_BOTTOM_LEFT])
			{
				rect=m_pWndRects[RECT_BOTTOM_LEFT]->GetRect();
				if(rect.PtInRect(CEasyPoint(xPos,yPos)))
				{
					CEasyRect WndRect=GetRect();			
					m_Resizing=4|8;
					m_DraggingStartMousePoint.x=xPos;
					m_DraggingStartMousePoint.y=yPos;
					m_DraggingStartPoint.x=WndRect.left;
					m_DraggingStartPoint.y=WndRect.top;	
					m_DraggingStartSize.cx=WndRect.Width();
					m_DraggingStartSize.cy=WndRect.Height();
					SetCaptureAll(true);
					return true;
				}
			}
			if(m_pWndRects[RECT_BOTTOM_RIGHT])
			{
				rect=m_pWndRects[RECT_BOTTOM_RIGHT]->GetRect();
				if(rect.PtInRect(CEasyPoint(xPos,yPos)))
				{
					CEasyRect WndRect=GetRect();			
					m_Resizing=4|2;
					m_DraggingStartMousePoint.x=xPos;
					m_DraggingStartMousePoint.y=yPos;
					m_DraggingStartPoint.x=WndRect.left;
					m_DraggingStartPoint.y=WndRect.top;	
					m_DraggingStartSize.cx=WndRect.Width();
					m_DraggingStartSize.cy=WndRect.Height();
					SetCaptureAll(true);
					return true;
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		if(m_IsDragging||m_Resizing)
		{
			m_IsDragging=false;				
			m_Resizing=0;
			SetCaptureAll(false);
			HandleMessage(this,WM_D3DGUI_RESIZED,GetID(),(LPARAM)this);
			return true;		
		}
		break;
	case WM_MOUSEMOVE:
		xPos=(short)(LOWORD(lParam));
		yPos=(short)(HIWORD(lParam));
		if(m_IsDragging)
		{				
			POINT Pos;
			Pos.x = (xPos-m_DraggingStartMousePoint.x)+m_DraggingStartPoint.x; 
			Pos.y = (yPos-m_DraggingStartMousePoint.y)+m_DraggingStartPoint.y;			//if(m_pParentWnd)
			//	m_pParentWnd->ScreenToClient(&Pos);
			Move(Pos.x,Pos.y);			
			HandleMessage(this,WM_D3DGUI_RESIZING,GetID(),(LPARAM)this);
			return true;
		}
		if(m_Resizing>0)
		{
			CEasyRect WndRect=GetRect();
			int dx,dy;
			dx=xPos-m_DraggingStartMousePoint.x;
			dy=yPos-m_DraggingStartMousePoint.y;
			if(dx||dy)
			{				
				if(m_Resizing&1)
				{
					WndRect.top=m_DraggingStartPoint.y+dy;
					WndRect.bottom=WndRect.top+m_DraggingStartSize.cy-dy;
				}
				if(m_Resizing&2)
				{
					WndRect.right=WndRect.left+m_DraggingStartSize.cx+dx;
				}
				if(m_Resizing&4)
				{
					WndRect.bottom=WndRect.top+m_DraggingStartSize.cy+dy;
				}
				if(m_Resizing&8)
				{
					WndRect.left=m_DraggingStartPoint.x+dx;
					WndRect.right=WndRect.left+m_DraggingStartSize.cx-dx;			
				}			
				SetRect(WndRect);		
				HandleMessage(this,WM_D3DGUI_RESIZING,GetID(),(LPARAM)this);
			}			
			return true;
		}
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_TAB:
			if(GetParent())
			{
				GetParent()->ActiveNextChildWnd(this);
				return true;
			}			
		}
		break;
	}
	return false;
}

BOOL CD3DWnd::HandleMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool CanProcess=false;
	bool Processed=false;
	if(msg>WM_D3DGUI_START&&msg<WM_D3DGUI_SYSTEM_MSG_END)
	{
		CanProcess=true;	
	}
	else
	{
		if(m_IsVisible&&m_IsEnable)
			CanProcess=true;
	}
	if(CanProcess)
		Processed=OnMessage(pWnd,msg,wParam,lParam);

	if(!Processed)
	{
		if(m_pParentWnd!=NULL) 
			return m_pParentWnd->HandleMessage(pWnd,msg,wParam,lParam);
		else if(msg>WM_D3DGUI_START)
			return m_pGUI->SendMsgToParent(msg,wParam,lParam);
		else
			return false;
	}
	else 
		return true;

}


CD3DWnd * CD3DWnd::GetParent(void)
{
	return m_pParentWnd;
}

void CD3DWnd::SetParent(CD3DWnd * parent,bool IsUpdateParent)
{
	if(parent)
	{	
		if(IsUpdateParent)
		{
			if(IsUpdateParent)

			if(m_pParentWnd)
				m_pParentWnd->DelChildWnd(this);
			else
			{			
				m_pGUI->DelRootWnd(this);
			}
			parent->AddChildWnd(this);
		}
		m_pParentWnd=parent;
		TopTo(m_pParentWnd);
	}
	else
	{
		if(m_pParentWnd)
		{
			if(IsUpdateParent)
			{
				m_pParentWnd->DelChildWnd(this);			
				m_pGUI->AddRootWnd(this);
			}
			m_pParentWnd=NULL;			
		}		
	}
	UpdateRects();
}

void CD3DWnd::SetFont(LOGFONT * pLogFont)
{
	if(pLogFont)
	{
		m_LogFont=*pLogFont;
		UpdateFont();
	}
}
void CD3DWnd::SetFontColor(DWORD Color)
{
	m_FontColor=Color;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DWnd::SetFontAlign(DWORD Align)
{
	m_FontAlign=Align;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DWnd::SetFontShadowMode(DWORD ShadowMode)
{
	m_FontShadowMode=ShadowMode;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DWnd::SetFontShadowColor(DWORD ShadowColor)
{
	m_FontShadowColor=ShadowColor;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DWnd::SetFontShadowWidth(DWORD ShadowWidth)
{
	m_FontShadowWidth=ShadowWidth;
	m_WantUpdateFont=true;
	UpdateFont();
}
void CD3DWnd::SetFontCharSpace(int Space)
{
	m_FontCharSpace=Space;
	m_WantUpdateFont=true;
	UpdateFont();
}
void CD3DWnd::SetFontLineSpace(int Space)
{
	m_FontLineSpace=Space;
	m_WantUpdateFont=true;
	UpdateFont();
}

void CD3DWnd::SetFontScale(FLOAT Scale)
{
	m_FontScale=Scale;
	m_WantUpdateFont=true;
	UpdateFont();
}

LOGFONT * CD3DWnd::GetFont()
{
	return &m_LogFont;
}

DWORD CD3DWnd::GetFontColor()
{
	return m_FontColor;
}

DWORD CD3DWnd::GetFontAlign()
{
	return m_FontAlign;
}

DWORD CD3DWnd::GetFontShadowMode()
{
	return m_FontShadowMode;
}

DWORD CD3DWnd::GetFontShadowColor()
{
	return m_FontShadowColor;
}

DWORD CD3DWnd::GetFontShadowWidth()
{
	return m_FontShadowWidth;
}

int CD3DWnd::GetFontCharSpace()
{
	return m_FontCharSpace;
}
int CD3DWnd::GetFontLineSpace()
{
	return m_FontLineSpace;
}

FLOAT CD3DWnd::GetFontScale()
{
	return m_FontScale;
}

void CD3DWnd::SetText(LPCTSTR Text)
{
	m_WndText=Text;
	UpdateText();
	IUITextRect * pTextRect=GetTextRect();
	if(pTextRect)
	{		
		pTextRect->SetTextW((LPCWSTR)m_WndText,m_WndText.GetLength());		
	}
}

void CD3DWnd::SetTextW(LPCWSTR Text)
{
	m_WndText=Text;
	UpdateText();
	IUITextRect * pTextRect=GetTextRect();
	if(pTextRect)
	{		
		pTextRect->SetTextW((LPCWSTR)m_WndText,m_WndText.GetLength());		
	}
}

void CD3DWnd::SetBKColor(DWORD BKColor)
{
	m_BKColor=BKColor;
	for(int i=RECT_TOP;i<RECT_CENTER;i++)
	{
		if(m_pWndRects[i])
			((IUIWndRect *)m_pWndRects[i])->SetColor(m_BKColor);
	}	
}


void CD3DWnd::ClientToScreen(RECT * rect)
{
	CEasyRect WndRect=GetCenterRect();

	rect->left+=WndRect.left;
	rect->right+=WndRect.left;
	rect->top+=WndRect.top;
	rect->bottom+=WndRect.top;
	if(m_pParentWnd!=NULL) 
		m_pParentWnd->ClientToScreen(rect);
}

void CD3DWnd::ScreenToClient(RECT * rect)
{
	CEasyRect WndRect=GetCenterRect();

	rect->left-=WndRect.left;
	rect->right-=WndRect.left;
	rect->top-=WndRect.top;
	rect->bottom-=WndRect.top;
	if(m_pParentWnd!=NULL) 
		m_pParentWnd->ScreenToClient(rect);
}

void CD3DWnd::ClientToScreen(POINT * point)
{
	CEasyRect WndRect=GetCenterRect();

	point->x+=WndRect.left;
	point->y+=WndRect.top;
	if(m_pParentWnd!=NULL) 
		m_pParentWnd->ClientToScreen(point);
}

void CD3DWnd::ScreenToClient(POINT * point)
{
	CEasyRect WndRect=GetCenterRect();

	point->x-=WndRect.left;
	point->y-=WndRect.top;
	if(m_pParentWnd!=NULL) 
		m_pParentWnd->ScreenToClient(point);
}


void CD3DWnd::ActiveWnd(bool bActive,bool SendNotify)
{
	//m_pGUI->ActiveWnd(this,bActive);
	m_IsActive=bActive;
	if(m_IsActive)
	{
		TopParent();
		TopTo(true);
		if(SendNotify)
			HandleMessage(this,WM_D3DGUI_GET_FOCUS,GetID(),(LPARAM)this);
	}
	else
	{
		if(SendNotify)
			HandleMessage(this,WM_D3DGUI_LOST_FOCUS,GetID(),(LPARAM)this);
	}
}

void CD3DWnd::ActiveNextChildWnd(CD3DWnd * pCurWnd)
{
	if(m_IsTabContainer)
	{
		for(UINT i=0;i<m_TabWndList.GetCount();i++)
		{
			if(m_TabWndList[i]==pCurWnd)
			{
				if(i<m_TabWndList.GetCount()-1)
				{
					m_TabWndList[i+1]->SetFocus();
				}
				else
				{
					m_TabWndList[0]->SetFocus();
				}
				return;
			}
		}
		if(m_TabWndList.GetCount())
		{
			m_TabWndList[0]->SetFocus();
		}
	}
	else
	{
		if(GetParent())
			GetParent()->ActiveNextChildWnd(pCurWnd);
	}

}

void CD3DWnd::UpdateRects()
{
	CEasyRect rect;

	if(!m_IsVisible)
		return;
	

	int MinWidth,MinHeight;

	GetMiniSize(MinWidth,MinHeight);
	if(m_WndRect.Width()<MinWidth)
		m_WndRect.right=m_WndRect.left+MinWidth;

	if(m_WndRect.Height()<MinHeight)
		m_WndRect.bottom=m_WndRect.top+MinHeight;
	
	rect=GetCenterRect();

	if(m_pParentWnd)
		m_pParentWnd->ClientToScreen(&rect);

	if(rect.Width()||rect.Height())
	{
		if(m_pWndRects[RECT_CENTER]==NULL)
		{
			m_pWndRects[RECT_CENTER]=CreateRect();		
			RebuildOrder();
		}
		m_pWndRects[RECT_CENTER]->SetRect(&FLOAT_RECT(rect));			
	}	
	
		
	
	rect=m_WndRect;
	if(m_pParentWnd)
		m_pParentWnd->ClientToScreen(&rect);		
	for(int i=RECT_TOP;i<RECT_CENTER;i++)
	{
		if(m_Borders[i])
		{
			if(m_pWndRects[i]==NULL)
			{
				m_pWndRects[i]=CreateRect();
				RebuildOrder();		
			}
			FLOAT_RECT FRect;
			switch(i)
			{
			case RECT_TOP:				
				FRect.left		=(FLOAT)rect.left+m_Borders[RECT_TOP_LEFT];
				FRect.top		=(FLOAT)rect.top;
				FRect.right		=(FLOAT)rect.right-m_Borders[RECT_TOP_RIGHT];
				FRect.bottom	=(FLOAT)rect.top+m_Borders[RECT_TOP];
				break;
			case RECT_BOTTOM:
				FRect.left		=(FLOAT)rect.left+m_Borders[RECT_TOP_LEFT];
				FRect.top		=(FLOAT)rect.bottom-m_Borders[RECT_BOTTOM];
				FRect.right		=(FLOAT)rect.right-m_Borders[RECT_TOP_RIGHT];
				FRect.bottom	=(FLOAT)rect.bottom;
				break;
			case RECT_LEFT:
				FRect.left		=(FLOAT)rect.left;
				FRect.top		=(FLOAT)rect.top+m_Borders[RECT_TOP];
				FRect.right		=(FLOAT)rect.left+m_Borders[RECT_LEFT];
				FRect.bottom	=(FLOAT)rect.bottom-m_Borders[RECT_BOTTOM];
				break;
			case RECT_RIGHT:
				FRect.left		=(FLOAT)rect.right-m_Borders[RECT_RIGHT];
				FRect.top		=(FLOAT)rect.top+m_Borders[RECT_TOP];
				FRect.right		=(FLOAT)rect.right;
				FRect.bottom	=(FLOAT)rect.bottom-m_Borders[RECT_BOTTOM];
				break;
			case RECT_TOP_LEFT:
				FRect.left		=(FLOAT)rect.left;
				FRect.top		=(FLOAT)rect.top;
				FRect.right		=(FLOAT)rect.left+m_Borders[RECT_TOP_LEFT];
				FRect.bottom	=(FLOAT)rect.top+m_Borders[RECT_TOP];
				break;
			case RECT_TOP_RIGHT:
				FRect.left		=(FLOAT)rect.right-m_Borders[RECT_TOP_RIGHT];
				FRect.top		=(FLOAT)rect.top;
				FRect.right		=(FLOAT)rect.right;
				FRect.bottom	=(FLOAT)rect.top+m_Borders[RECT_TOP];
				break;
			case RECT_BOTTOM_LEFT:
				FRect.left		=(FLOAT)rect.left;
				FRect.top		=(FLOAT)rect.bottom-m_Borders[RECT_BOTTOM];
				FRect.right		=(FLOAT)rect.left+m_Borders[RECT_BOTTOM_LEFT];
				FRect.bottom	=(FLOAT)rect.bottom;
				break;
			case RECT_BOTTOM_RIGHT:
				FRect.left		=(FLOAT)rect.right-m_Borders[RECT_BOTTOM_RIGHT];
				FRect.top		=(FLOAT)rect.bottom-m_Borders[RECT_BOTTOM];
				FRect.right		=(FLOAT)rect.right;
				FRect.bottom	=(FLOAT)rect.bottom;
				break;			
			}
			m_pWndRects[i]->SetRect(&FRect);			
		}
		else
		{
			if(m_pWndRects[i])
			{
				SAFE_RELEASE(m_pWndRects[i]);
			}
		}
	}
	UpdateText();
	UpdateChildRects();
}

bool CD3DWnd::UpdateTextureRect()
{		
	if(m_pTexture==NULL)
		return false;	
	for(int i=0;i<RECT_TEXT;i++)
	{
		if(m_Borders[i])
		{
			FLOAT_RECT UVRect;
			CEasyRect TextureRect;
			if(m_pWndRects[i]==NULL)
			{
				m_pWndRects[i]=CreateRect();
				RebuildOrder();
			}
			((IUIWndRect *)m_pWndRects[i])->SetTexture(m_pTexture);
			switch(i)
			{
			case RECT_CENTER:
				TextureRect.left	=m_TextureRect.left+m_Borders[RECT_LEFT];
				TextureRect.top		=m_TextureRect.top+m_Borders[RECT_TOP];
				TextureRect.right	=m_TextureRect.right-m_Borders[RECT_RIGHT]-1;
				TextureRect.bottom	=m_TextureRect.bottom-m_Borders[RECT_BOTTOM]-1;
				break;
			case RECT_TOP:
				TextureRect.left	=m_TextureRect.left+m_Borders[RECT_TOP_LEFT];
				TextureRect.top		=m_TextureRect.top;
				TextureRect.right	=m_TextureRect.right-m_Borders[RECT_TOP_RIGHT]-1;
				TextureRect.bottom	=m_TextureRect.top+m_Borders[RECT_TOP];
				break;
			case RECT_BOTTOM:
				TextureRect.left	=m_TextureRect.left+m_Borders[RECT_TOP_LEFT];
				TextureRect.top		=m_TextureRect.bottom-m_Borders[RECT_BOTTOM];
				TextureRect.right	=m_TextureRect.right-m_Borders[RECT_TOP_RIGHT]-1;
				TextureRect.bottom	=m_TextureRect.bottom;
				break;
			case RECT_LEFT:
				TextureRect.left	=m_TextureRect.left;
				TextureRect.top		=m_TextureRect.top+m_Borders[RECT_TOP];
				TextureRect.right	=m_TextureRect.left+m_Borders[RECT_LEFT];
				TextureRect.bottom	=m_TextureRect.bottom-m_Borders[RECT_BOTTOM]-1;
				break;
			case RECT_RIGHT:
				TextureRect.left	=m_TextureRect.right-m_Borders[RECT_RIGHT];
				TextureRect.top		=m_TextureRect.top+m_Borders[RECT_TOP];
				TextureRect.right	=m_TextureRect.right;
				TextureRect.bottom	=m_TextureRect.bottom-m_Borders[RECT_BOTTOM]-1;
				break;
			case RECT_TOP_LEFT:
				TextureRect.left	=m_TextureRect.left;
				TextureRect.top		=m_TextureRect.top;
				TextureRect.right	=m_TextureRect.left+m_Borders[RECT_TOP_LEFT];
				TextureRect.bottom	=m_TextureRect.top+m_Borders[RECT_TOP];
				break;
			case RECT_TOP_RIGHT:
				TextureRect.left		=m_TextureRect.right-m_Borders[RECT_TOP_RIGHT];
				TextureRect.top		=m_TextureRect.top;
				TextureRect.right	=m_TextureRect.right;
				TextureRect.bottom	=m_TextureRect.top+m_Borders[RECT_TOP];
				break;
			case RECT_BOTTOM_LEFT:
				TextureRect.left	=m_TextureRect.left;
				TextureRect.top		=m_TextureRect.bottom-m_Borders[RECT_BOTTOM];
				TextureRect.right	=m_TextureRect.left+m_Borders[RECT_BOTTOM_LEFT];
				TextureRect.bottom	=m_TextureRect.bottom;
				break;
			case RECT_BOTTOM_RIGHT:
				TextureRect.left	=m_TextureRect.right-m_Borders[RECT_BOTTOM_RIGHT];
				TextureRect.top		=m_TextureRect.bottom-m_Borders[RECT_BOTTOM];
				TextureRect.right	=m_TextureRect.right;
				TextureRect.bottom	=m_TextureRect.bottom;
				break;			
			}
			UVRect.left=m_pTexture->X2U(TextureRect.left);
			UVRect.right=m_pTexture->X2U(TextureRect.right);
			UVRect.top=m_pTexture->Y2V(TextureRect.top);
			UVRect.bottom=m_pTexture->Y2V(TextureRect.bottom);
			((IUIWndRect *)m_pWndRects[i])->SetUV(&UVRect);
		}
	}
	return true;
}

void CD3DWnd::UpdateText()
{
	if(!m_IsVisible)
		return;

	CEasyRect Rect=GetClientRect();
	ClientToScreen(&Rect);

	if(!m_WndText.IsEmpty())
	{
		if(m_pWndRects[RECT_TEXT]==NULL)		
		{
			m_pWndRects[RECT_TEXT]=CreateTextRect();
			RebuildOrder();
		}
		if(m_pWndRects[RECT_TEXT])
			m_pWndRects[RECT_TEXT]->SetRect(&FLOAT_RECT(Rect));
	}
	
	
}

void CD3DWnd::Update()
{
	//UpdateFont();
}

void CD3DWnd::SaveToXml(xml_node * pXMLNode)
{
	xml_node Wnd=pXMLNode->append_child(node_element,_T("Window"));
	Wnd.append_attribute(_T("Name"),(LPCTSTR)GetName());
	Wnd.append_attribute(_T("ID"),(long)GetID());
	Wnd.append_attribute(_T("IsInternal"),IsInternal());	

	xml_node Behavior=Wnd.append_child(node_element,_T("Behavior"));
	SaveBehaviorToXML(Behavior);

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

bool CD3DWnd::LoadFromXml(xml_node * pXMLNode)
{
	if(_tcsnicmp(pXMLNode->name(),_T("Window"),7)!=0)
		return false;
	if(pXMLNode->has_attribute(_T("Name")))
		SetName(pXMLNode->attribute(_T("Name")).getvalue());

	if(pXMLNode->has_attribute(_T("ID")))
		SetID((long)pXMLNode->attribute(_T("ID")));

	if(pXMLNode->has_attribute(_T("IsInternal")))
		SetInternal((bool)pXMLNode->attribute(_T("IsInternal")));


	for(int i=0;i<(int)pXMLNode->children();i++)
	{
		if(_tcsnicmp(pXMLNode->child(i).name(),_T("Behavior"),9)==0)
		{
			LoadBehaviorFromXML(pXMLNode->child(i));
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
	TopChild(true);
	HandleMessage(this,WM_D3DGUI_CHILD_LOADED,GetID(),(LPARAM)this);
	
	return true;
}

bool CD3DWnd::LoadFromFile(LPCTSTR FileName,LPCTSTR WndName)
{
	pug::xml_parser Xml;

	Xml.parse_file(FileName,pug::parse_trim_attribute);
	xml_node GUI=Xml.document();
	if(!GUI.moveto_child(_T("D3DGUI")))
		return false;	

	for(int i=0;i<(int)GUI.children();i++)
	{
		if(WndName==NULL)
			return LoadFromXml(&GUI.child(i));
		if(GUI.child(i).attribute(_T("Name")).getvalue()==WndName)
			return LoadFromXml(&GUI.child(i));
	}
	return false;
}



//bool CD3DWnd::ToUSOFile(CUSOFile * pResourceManager,UINT Param)
//{	
//
//	if(pResourceManager==NULL)
//		return false;	
//
//	IFileAccessor * pFile=pResourceManager->GetFile();
//	if(pFile==NULL)
//		return false;
//
//	pug::xml_parser Xml;
//
//	Xml.create();
//	xml_node Doc;
//	Doc = Xml.document();	
//	
//	SaveToXml(&Doc);	
//
//	std::strstream os;		
//
//	os << Xml.document();
//	os<<'\0';
//	os.freeze();
//
//	CEasyString Script=os.str();
//
//	Script.Replace("\n","\r\n");
//
//	STORAGE_STRUCT Data;
//
//	strncpy_0(Data.ObjectHead.Type,USO_FILE_MAX_TYPE_LEN,GetClassInfo().ClassName,USO_FILE_MAX_TYPE_LEN);	
//	strncpy_0(Data.ObjectHead.Name,USO_FILE_MAX_OBJECT_NAME,GetName(),USO_FILE_MAX_OBJECT_NAME);	
//	Data.ScriptStrLen=Script.GetLength()+1;
//	Data.ObjectHead.Size=sizeof(STORAGE_STRUCT)+Data.ScriptStrLen;	
//	
//
//	pFile->Write(&Data,sizeof(STORAGE_STRUCT));
//	pFile->Write((LPSTR)((LPCTSTR)Script),Data.ScriptStrLen);	
//
//	return true;
//}
//
//bool CD3DWnd::FromUSOFile(CUSOFile * pResourceManager,UINT Param)
//{
//	if(pResourceManager==NULL)
//		return false;	
//
//	IFileAccessor * pFile=pResourceManager->GetFile();
//	if(pFile==NULL)
//		return false;
//
//	STORAGE_STRUCT * pData;
//	BYTE * pBuff;
//	char * pScript;
//	UINT Size;
//
//	pFile->Read(&Size,sizeof(UINT));
//	pBuff=new BYTE[Size];
//	pFile->Read(pBuff+sizeof(UINT),Size-sizeof(UINT));
//	pData=(STORAGE_STRUCT *)pBuff;
//	pScript=(char *)pBuff+sizeof(STORAGE_STRUCT);
//	pData->ObjectHead.Size=Size;
//
//
//	if((!GetClassInfo().IsKindOf(pData->ObjectHead.Type))||
//		pData->ObjectHead.Size<sizeof(STORAGE_STRUCT))
//	{	
//		delete[] pBuff;
//		return false;
//	}
//
//	
//
//	pData->ObjectHead.Name[USO_FILE_MAX_OBJECT_NAME-1]=0;
//	SetName(pData->ObjectHead.Name);
//
//	pug::xml_parser Xml;
//	Xml.parse(pScript,pug::parse_trim_attribute);	
//	xml_node Doc=Xml.document();
//
//	if(Doc.children()>0)
//	{
//		if(LoadFromXml(&Doc.child(0)))
//		{	
//			delete[] pBuff;
//			return true;
//		}
//	}
//	delete[] pBuff;
//	return false;
//	
//}

void CD3DWnd::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	if(m_pTexture)
	{
		m_pTexture->PickResource(pResourceManager,Param);
	}

	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{
		m_ChildWndList[i]->PickResource(pResourceManager,Param);
	}	
}

void CD3DWnd::OnLoaded()
{
}

bool CD3DWnd::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::ToSmartStruct(Packet,pResourceManager,Param))
		return false;

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_GUI_SCRIPT,(LPCTSTR)m_TempScript,m_TempScript.GetLength()));

	return true;
}
bool CD3DWnd::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	LPCTSTR szScript=Packet.GetMember(SST_GUI_SCRIPT);
	if(szScript)
	{
		pug::xml_parser Xml;
		Xml.parse((TCHAR *)szScript,pug::parse_trim_attribute);	
		xml_node Doc=Xml.document();

		if(Doc.children()>0)
		{
			if(!LoadFromXml(&Doc.child(0)))
			{	
				return false;
			}
		}
	}
	return true;
}

UINT CD3DWnd::GetSmartStructSize(UINT Param)
{
	UINT Size=CNameObject::GetSmartStructSize(Param);

	pug::xml_parser Xml;

	Xml.create();
	xml_node Doc;
	Doc = Xml.document();	
	
	SaveToXml(&Doc);	
#ifdef UNICODE
	std::wstringstream os;
#else
	std::stringstream os;		
#endif

	os << Xml.document();
	os<<'\0';

	m_TempScript=os.str().c_str();

	m_TempScript.Replace(_T("\n"),_T("\r\n"));

	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(m_TempScript.GetLength());

	return Size;
}

//////////////////////////////////////////////////////



void CD3DWnd::SaveChildsToXml(xml_node& Childs)
{
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{
		m_ChildWndList[i]->SaveToXml(&Childs);
	}
}

void CD3DWnd::SaveFrameToXML(xml_node& Frame)
{
	CEasyRect Rect=GetRect();
	Frame.append_attribute(_T("X"),(long)Rect.left);
	Frame.append_attribute(_T("Y"),(long)Rect.top);
	Frame.append_attribute(_T("Width"),(long)Rect.Width());
	Frame.append_attribute(_T("Height"),(long)Rect.Height());
	Frame.append_attribute(_T("BKColor"),(long)GetBKColor());
}
void CD3DWnd::SaveBorderToXML(xml_node& Borders)
{
	Borders.append_attribute(_T("Top"),(long)m_Borders[RECT_TOP]);
	Borders.append_attribute(_T("Bottom"),(long)m_Borders[RECT_BOTTOM]);
	Borders.append_attribute(_T("Left"),(long)m_Borders[RECT_LEFT]);
	Borders.append_attribute(_T("Right"),(long)m_Borders[RECT_RIGHT]);
	Borders.append_attribute(_T("TopLeft"),(long)m_Borders[RECT_TOP_LEFT]);
	Borders.append_attribute(_T("TopRight"),(long)m_Borders[RECT_TOP_RIGHT]);
	Borders.append_attribute(_T("BottomLeft"),(long)m_Borders[RECT_BOTTOM_LEFT]);
	Borders.append_attribute(_T("BottomRight"),(long)m_Borders[RECT_BOTTOM_RIGHT]);
}
void CD3DWnd::SaveTextToXML(xml_node& Text)
{
	CEasyString WndText;
	GetText(WndText);
	WndText.Replace(_T("\r\n"),_T("[&RetuRn]"));
	WndText.Replace(_T("\""),_T("[&InveRtedComMa]"));
	Text.append_attribute(_T("Text"),(LPCTSTR)WndText);
}
void CD3DWnd::SaveFontToXML(xml_node& Font)
{
	Font.append_attribute(_T("FaceName"),m_LogFont.lfFaceName);
	Font.append_attribute(_T("Height"),(long)m_LogFont.lfHeight);
	Font.append_attribute(_T("Width"),(long)m_LogFont.lfWidth);
	Font.append_attribute(_T("Weight"),(long)m_LogFont.lfWeight);
	Font.append_attribute(_T("CharSet"),(long)m_LogFont.lfCharSet);
	Font.append_attribute(_T("Italic"),(bool)m_LogFont.lfItalic);
	Font.append_attribute(_T("OutPrecision"),(long)m_LogFont.lfOutPrecision);
	Font.append_attribute(_T("PitchAndFamily"),(long)m_LogFont.lfPitchAndFamily);
	Font.append_attribute(_T("Quality"),(long)m_LogFont.lfQuality);
	Font.append_attribute(_T("FontColor"),(long)m_FontColor);
	Font.append_attribute(_T("Align"),(long)m_FontAlign);
	Font.append_attribute(_T("ShadowMode"),(long)m_FontShadowMode);
	Font.append_attribute(_T("ShadowColor"),(long)m_FontShadowColor);
	Font.append_attribute(_T("ShadowWidth"),(long)m_FontShadowWidth);
	Font.append_attribute(_T("CharSpace"),(long)m_FontCharSpace);
	Font.append_attribute(_T("LineSpace"),(long)m_FontLineSpace);
}
void CD3DWnd::SaveTextureToXML(xml_node& Texture)
{
	Texture.append_attribute(_T("TextureFile"),(LPCTSTR)m_pTexture->GetName());
	Texture.append_attribute(_T("RectLeft"),m_TextureRect.left);
	Texture.append_attribute(_T("RectTop"),m_TextureRect.top);
	Texture.append_attribute(_T("RectRight"),m_TextureRect.right);
	Texture.append_attribute(_T("RectBottom"),m_TextureRect.bottom);
}

void CD3DWnd::SaveBehaviorToXML(xml_node& Behavior)
{
	Behavior.append_attribute(_T("Visible"),m_IsVisible);
	Behavior.append_attribute(_T("Enable"),m_IsEnable);
	Behavior.append_attribute(_T("CanDrag"),m_CanDrag);
	Behavior.append_attribute(_T("CanResize"),m_CanResize);
	Behavior.append_attribute(_T("CanGetFocus"),m_CanGetFocus);
	Behavior.append_attribute(_T("IsTopmost"),m_IsTopmost);
	Behavior.append_attribute(_T("TabIndex"),m_TabIndex);
	Behavior.append_attribute(_T("IsTabContainer"),m_IsTabContainer);
	
}

void CD3DWnd::LoadChildsFromXml(xml_node& Childs)
{
	m_pGUI->LoadFromXML(&Childs,this);
}

void CD3DWnd::LoadFrameFromXML(xml_node& Frame)
{
	if(Frame.has_attribute(_T("X")))
	{
		CEasyRect Rect(0,0,0,0);
		Rect.left=(long)Frame.attribute(_T("X"));
		Rect.top=(long)Frame.attribute(_T("Y"));
		Rect.right=Rect.left+(long)Frame.attribute(_T("Width"));
		Rect.bottom=Rect.top+(long)Frame.attribute(_T("Height"));
		SetRect(Rect);
	}
	if(Frame.has_attribute(_T("BKColor")))
		SetBKColor((long)Frame.attribute(_T("BKColor")));
}

void CD3DWnd::LoadBorderFromXML(xml_node& Borders)
{
	WIN_BORDERS WndBorders;

	WndBorders[RECT_TOP]=(long)Borders.attribute(_T("Top"));
	WndBorders[RECT_BOTTOM]=(long)Borders.attribute(_T("Bottom"));
	WndBorders[RECT_LEFT]=(long)Borders.attribute(_T("Left"));
	WndBorders[RECT_RIGHT]=(long)Borders.attribute(_T("Right"));
	WndBorders[RECT_TOP_LEFT]=(long)Borders.attribute(_T("TopLeft"));
	WndBorders[RECT_TOP_RIGHT]=(long)Borders.attribute(_T("TopRight"));
	WndBorders[RECT_BOTTOM_LEFT]=(long)Borders.attribute(_T("BottomLeft"));
	WndBorders[RECT_BOTTOM_RIGHT]=(long)Borders.attribute(_T("BottomRight"));

	SetBorders(WndBorders);
}

void CD3DWnd::LoadTextFromXML(xml_node& Text)
{
	CEasyString WndText=Text.attribute(_T("Text")).getvalue();
	WndText.Replace(_T("[&RetuRn]"),_T("\r\n"));
	WndText.Replace(_T("[&InveRtedComMa]"),_T("\""));
	SetText(WndText);
}

void CD3DWnd::LoadFontFromXML(xml_node& Font)
{
	LOGFONT LogFont;
	ZeroMemory(&LogFont,sizeof(LogFont));

	if(Font.has_attribute(_T("FaceName")))
	{
		strncpy_0(LogFont.lfFaceName,LF_FACESIZE,Font.attribute(_T("FaceName")).getvalue(),LF_FACESIZE);
		LogFont.lfHeight=(long)Font.attribute(_T("Height"));
		LogFont.lfWidth=(long)Font.attribute(_T("Width"));
		LogFont.lfWeight=(long)Font.attribute(_T("Weight"));
		LogFont.lfCharSet=(long)Font.attribute(_T("CharSet"));
		LogFont.lfItalic=(bool)Font.attribute(_T("Italic"));
		LogFont.lfOutPrecision=(long)Font.attribute(_T("OutPrecision"));
		LogFont.lfPitchAndFamily=(long)Font.attribute(_T("PitchAndFamily"));
		LogFont.lfQuality=(long)Font.attribute(_T("Quality"));
		SetFont(&LogFont);
	}
	if(Font.has_attribute(_T("FontColor")))
		SetFontColor((long)Font.attribute(_T("FontColor")));
	if(Font.has_attribute(_T("Align")))
		SetFontAlign((long)Font.attribute(_T("Align")));
	if(Font.has_attribute(_T("ShadowMode")))
		SetFontShadowMode((long)Font.attribute(_T("ShadowMode")));
	if(Font.has_attribute(_T("ShadowColor")))
		SetFontShadowColor((long)Font.attribute(_T("ShadowColor")));
	if(Font.has_attribute(_T("ShadowWidth")))
		SetFontShadowWidth((long)Font.attribute(_T("ShadowWidth")));
	if(Font.has_attribute(_T("CharSpace")))
		SetFontCharSpace((long)Font.attribute(_T("CharSpace")));
	if(Font.has_attribute(_T("LineSpace")))
		SetFontLineSpace((long)Font.attribute(_T("LineSpace")));
}

void CD3DWnd::LoadTextureFromXML(xml_node& Texture)
{	
	CEasyString TexFile;
	CEasyRect Rect;
	TexFile=Texture.attribute(_T("TextureFile")).getvalue();
	Rect.left=Texture.attribute(_T("RectLeft"));
	Rect.top=Texture.attribute(_T("RectTop"));
	Rect.right=Texture.attribute(_T("RectRight"));
	Rect.bottom=Texture.attribute(_T("RectBottom"));
	SetTexture(TexFile,Rect);
}

void CD3DWnd::LoadBehaviorFromXML(xml_node& Behavior)
{
	if(Behavior.has_attribute(_T("Visible")))
		SetVisible((bool)Behavior.attribute(_T("Visible")));
	if(Behavior.has_attribute(_T("Enable")))
		EnableWindow((bool)Behavior.attribute(_T("Enable")));
	if(Behavior.has_attribute(_T("CanDrag")))
		EnableDrag((bool)Behavior.attribute(_T("CanDrag")));
	if(Behavior.has_attribute(_T("CanResize")))
		EnableResize((bool)Behavior.attribute(_T("CanResize")));
	if(Behavior.has_attribute(_T("CanGetFocus")))
		EnableFocus((bool)Behavior.attribute(_T("CanGetFocus")));
	if(Behavior.has_attribute(_T("IsTopmost")))
		SetTopmost((bool)Behavior.attribute(_T("IsTopmost")));
	if(Behavior.has_attribute(_T("TabIndex")))
		SetTabIndex((long)Behavior.attribute(_T("TabIndex")));
	if(Behavior.has_attribute(_T("IsTabContainer")))
		EnableTabContainer((bool)Behavior.attribute(_T("IsTabContainer")));

	
}
 
bool CD3DWnd::AddChildWnd(CD3DWnd * child)
{	
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{
		if(m_ChildWndList[i]==child)
			return false;
	}
	m_ChildWndList.Add(child);
	AddTabWnd(child);
	
	child->UpdateRects();
	child->TopTo(this);
	return true;
}

bool CD3DWnd::DelChildWnd(CD3DWnd * child)
{
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{		
		if(m_ChildWndList[i]==child)
		{
			m_ChildWndList.Delete(i);
			DelTabWnd(child);			
			return true;
		}
	}
	return false;
}

bool CD3DWnd::AddTabWnd(CD3DWnd * child)
{
	if(m_IsTabContainer)
	{
		if(child->GetTabIndex()==0)
			return false;
		for(UINT i=0;i<m_TabWndList.GetCount();i++)
		{
			if(m_TabWndList[i]->GetTabIndex()>child->GetTabIndex())
			{
				m_TabWndList.Insert(i,child);
				return true;
			}
		}
		m_TabWndList.Add(child);
		return true;
	}
	else
	{
		if(GetParent())
			return GetParent()->AddTabWnd(child);
	}
	return false;
}

bool CD3DWnd::DelTabWnd(CD3DWnd * child)
{
	if(m_IsTabContainer)
	{
		if(child->GetTabIndex())
		{
			for(UINT j=0;j<m_TabWndList.GetCount();j++)
			{
				if(m_TabWndList[j]==child)
				{
					m_TabWndList.Delete(j);
					return true;
				}
			}
		}
	}
	else
	{
		if(GetParent())
			return GetParent()->DelTabWnd(child);
	}
	return false;
}

bool CD3DWnd::UpdateFont()
{
	if(m_WantUpdateFont)
	{		
		IUITextRect * pTextRect=GetTextRect();
		if(pTextRect)		
		{
			pTextRect->EnableUpdate(false);
			pTextRect->SetFont(&m_LogFont);
			pTextRect->SetColor(m_FontColor);
			pTextRect->SetAlign(m_FontAlign);
			pTextRect->SetShadowMode(m_FontShadowMode);
			pTextRect->SetShadowColor(m_FontShadowColor);
			pTextRect->SetShadowWidth(m_FontShadowWidth);		
			pTextRect->SetCharSpace(m_FontCharSpace);
			pTextRect->SetLineSpace(m_FontLineSpace);
			pTextRect->SetScale(m_FontScale);
			m_FontCharSpace=pTextRect->GetCharSpace();
			m_FontLineSpace=pTextRect->GetLineSpace();
			pTextRect->EnableUpdate(true);
		}
		m_WantUpdateFont=false;
		return true;
	}
	return false;
}

void CD3DWnd::UpdateChildRects()
{
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{		
		m_ChildWndList[i]->UpdateRects();
	}
}

void CD3DWnd::TopTo(bool bRedraw,CD3DWnd * Before)
{
	m_pGUI->LeftWndToTop(this,Before);

	if(bRedraw)
	{
		IUIBaseRect * pBottomRect;
		if(Before)
			pBottomRect=Before->GetTopRect();
		else
			pBottomRect=NULL;

		CEasyArray<IUIBaseRect *> RectList(64,64);
		GetRectIncludeChild(RectList);

		if(RectList.GetCount())	
		{
			RectList[0]->TopTo(RectList.GetBuffer(),RectList.GetCount(),pBottomRect);
		}
	}

	TopChild(false);
}

IUIBaseRect * CD3DWnd::GetTopRect()
{
	for(int i=m_WndRectCount-1;i>=0;i--)
	{
		if(m_pWndRects[i])
			return m_pWndRects[i];
	}
	return NULL;
}

void CD3DWnd::GetRectIncludeChild(CEasyArray<IUIBaseRect *>& RectList)
{
	for(UINT i=0;i<m_WndRectCount;i++)
	{
		if(m_pWndRects[i])
			RectList.Add(m_pWndRects[i]);
	}
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{		
		m_ChildWndList[i]->GetRectIncludeChild(RectList);
	}
}

void CD3DWnd::TopChild(bool Redraw)
{
	CEasyArray<IUIBaseRect *> RectList(64,64);
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{		
		if(Redraw)
			m_ChildWndList[i]->GetRectIncludeChild(RectList);
		m_pGUI->LeftWndToTop(m_ChildWndList[i],this);
	}

	if(Redraw)
	{
		IUIBaseRect * pTopRect=GetTopRect();
		if(RectList.GetCount())
		{
			RectList[0]->TopTo(RectList.GetBuffer(),RectList.GetCount(),pTopRect);
		}	
	}

	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{		
		m_ChildWndList[i]->TopChild(false);
	}
}


void CD3DWnd::SetVisible(bool IsVisible)
{
	if(m_IsVisible!=IsVisible)
	{	
		m_IsVisible=IsVisible;
		for(int i=0;i<m_WndRectCount;i++)
		{
			if(m_pWndRects[i])
				m_pWndRects[i]->SetVisible(IsVisible);
		}
		if(m_IsVisible)
		{
			UpdateRects();
			UpdateText();
			UpdateFont();
		}
		HandleMessage(this,WM_D3DGUI_VISIBLE_CHANGE,(WPARAM)m_IsVisible,0);
		for(UINT i=0;i<m_ChildWndList.GetCount();i++)
		{
			m_ChildWndList[i]->SetVisible(IsVisible);
		}
	}
}

void CD3DWnd::SetTabIndex(UINT Index)
{
	if(GetParent())
	{
		GetParent()->DelTabWnd(this);
		m_TabIndex=Index;
		GetParent()->AddTabWnd(this);
	}
}

void CD3DWnd::EnableTabContainer(bool Enbale)
{
	m_IsTabContainer=Enbale;
	if(!Enbale)
	{
		m_TabWndList.Clear();
	}	
}

void CD3DWnd::SetCaptureAll(bool IsCaptureAll)
{
	m_IsCaptureAll=IsCaptureAll;
	if(IsCaptureAll)
		m_pGUI->SetCaptureAllWnd(this);
	else
		m_pGUI->SetCaptureAllWnd(NULL);
}

void CD3DWnd::SetFocus()
{
	m_pGUI->ActiveWnd(this,true);
}

IUIWndRect * CD3DWnd::CreateRect()
{
	IUIWndRect * pWndRect=m_pGUI->GetObjectCreator()->CreateWndRect(NULL);
	if(pWndRect)
	{
		pWndRect->SetColor(m_BKColor);	
		pWndRect->SetVisible(m_IsVisible);
	}
	return pWndRect;
}

IUITextRect * CD3DWnd::CreateTextRect()
{
	IUITextRect * pText=m_pGUI->GetObjectCreator()->CreateTextRect(NULL,NULL);
	
	if(pText)
	{
		pText->EnableUpdate(false);
		pText->SetFont(&m_LogFont);
		pText->SetColor(m_FontColor);
		pText->SetAlign(m_FontAlign);
		pText->SetShadowMode(m_FontShadowMode);
		pText->SetShadowColor(m_FontShadowColor);
		pText->SetShadowWidth(m_FontShadowWidth);	
		pText->SetCharSpace(m_FontCharSpace);
		pText->SetLineSpace(m_FontLineSpace);
		pText->EnableUpdate(true);
		pText->SetVisible(m_IsVisible);
	}	
	return pText;
}

void CD3DWnd::RebuildOrder()
{

	CEasyArray<IUIBaseRect *> RectList(64,64);
	GetRectIncludeChild(RectList);

	if(RectList.GetCount()>1)	
	{
		RectList[1]->TopTo(RectList.GetBuffer()+1,RectList.GetCount()-1,RectList[0]);
	}
}

void CD3DWnd::TopParent()
{
	if(m_pParentWnd)
	{
		m_pParentWnd->TopParent();	
		TopTo(false);
	}
	else
	{
		TopTo(true);
	}
	
}

void CD3DWnd::GetMiniSize(int& Width,int& Height)
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

	Width=LMinWidth+RMinWidth;
	Height=m_Borders[RECT_TOP]+m_Borders[RECT_BOTTOM];


	
}

CD3DWnd * CD3DWnd::GetChildWndByID(int ID)
{
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{
		if(m_ChildWndList[i]->GetID()==ID)
			return m_ChildWndList[i];
	}
	return NULL;
}
CD3DWnd * CD3DWnd::GetChildWndByName(LPCTSTR Name)
{
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{
		if(_tcscmp(m_ChildWndList[i]->GetName(),Name)==0)
			return m_ChildWndList[i];
	}
	return NULL;
}

bool CD3DWnd::IsChild(CD3DWnd * pWnd)
{
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{
		if(m_ChildWndList[i]==pWnd)
			return true;
	}
	return false;
}

bool CD3DWnd::IsChildRecursion(CD3DWnd * pWnd)
{
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{
		if(m_ChildWndList[i]==pWnd)
			return true;
	}
	for(UINT i=0;i<m_ChildWndList.GetCount();i++)
	{
		if(m_ChildWndList[i]->IsChildRecursion(pWnd))
			return true;
	}
	return false;
}

IUITextRect * CD3DWnd::GetTextRect()
{
	if(m_pWndRects[RECT_TEXT]==NULL)
	{
		UpdateText();		
	}
	return (IUITextRect *)m_pWndRects[RECT_TEXT];
}

//CNameObject::STORAGE_STRUCT * CD3DWnd::USOCreateHead(UINT Param)
//{
//	STORAGE_STRUCT * pHead=new STORAGE_STRUCT;
//	ZeroMemory(pHead,sizeof(STORAGE_STRUCT));
//	pHead->Size=sizeof(STORAGE_STRUCT);
//	return pHead;
//}
//
//int CD3DWnd::USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param)
//{
//	int HeadSize=CNameObject::USOWriteHead(pHead,pResourceManager,Param);
//	if(HeadSize<0)
//		return -1;
//
//	STORAGE_STRUCT * pLocalHead=(STORAGE_STRUCT *)pHead;
//
//	
//	pug::xml_parser Xml;
//
//	Xml.create();
//	xml_node Doc;
//	Doc = Xml.document();	
//	
//	SaveToXml(&Doc);	
//
//	std::strstream os;		
//
//	os << Xml.document();
//	os<<'\0';
//	os.freeze();
//
//	m_TempScript=os.str();
//
//	m_TempScript.Replace("\n","\r\n");
//
//	
//	pLocalHead->ScriptStrLen=m_TempScript.GetLength()+1;
//	pLocalHead->Size+=pLocalHead->ScriptStrLen;	
//
//	return sizeof(STORAGE_STRUCT);
//}
//
//bool CD3DWnd::USOWriteData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param)
//{
//	if(!CNameObject::USOWriteData(pHead,pResourceManager,Param))
//		return false;
//
//	if(pResourceManager==NULL)
//		return false;	
//
//	IFileAccessor * pFile=pResourceManager->GetFile();
//	if(pFile==NULL)
//		return false;
//
//	STORAGE_STRUCT * pLocalHead=(STORAGE_STRUCT *)pHead;
//
//	if(!m_TempScript.IsEmpty())
//	{
//		UINT WriteSize=(UINT)pFile->Write((LPSTR)((LPCTSTR)m_TempScript),pLocalHead->ScriptStrLen);
//		m_TempScript.Clear();
//		if(WriteSize<pLocalHead->ScriptStrLen)
//			return false;
//	}
//	return true;
//}
//
//int CD3DWnd::USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param)
//{	
//	int ReadSize=CNameObject::USOReadHead(pHead,pResourceManager,Param);
//	if(ReadSize<0)
//		return -1;	
//
//	return sizeof(STORAGE_STRUCT);
//}
//
//int CD3DWnd::USOReadData(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,BYTE * pData,int DataSize,UINT Param)
//{
//	int ReadSize=CNameObject::USOReadData(pHead,pResourceManager,pData,DataSize,Param);
//
//	pData+=ReadSize;
//	DataSize-=ReadSize;
//
//	STORAGE_STRUCT * pLocalHead=(STORAGE_STRUCT *)pHead;
//
//	if(pLocalHead->ScriptStrLen)
//	{
//		pug::xml_parser Xml;
//		Xml.parse((TCHAR *)pData,pug::parse_trim_attribute);	
//		xml_node Doc=Xml.document();
//
//		if(Doc.children()>0)
//		{
//			if(LoadFromXml(&Doc.child(0)))
//			{	
//				return -1;
//			}
//		}
//
//		ReadSize+=pLocalHead->ScriptStrLen;
//	}
//	return ReadSize;
//}

}