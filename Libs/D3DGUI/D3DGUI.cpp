/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DGUI.cpp                                               */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include "D3DGUI.h"



namespace D3DGUI{

IMPLEMENT_FILE_CHANNEL_MANAGER(CD3DGUI)
IMPLEMENT_CLASS_INFO_STATIC(CD3DGUI,CNameObject);


CD3DGUI::CD3DGUI(IUIObjectCreator * pObjectCreator,HWND hParentWnd):CNameObject()
{
	m_pObjectCreator=pObjectCreator;
	m_ReadyToWork=false;
	m_pActiveWnd=NULL;
	m_pRecentMouseWnd=NULL;
	m_pCaptureAllWnd=NULL;
	m_pModalWnd=NULL;
	m_hParentWnd=hParentWnd;
}

CD3DGUI::~CD3DGUI(void)
{
	Clear();
}



void CD3DGUI::AddWnd(CD3DWnd *pWnd)
{
	if(m_D3DWndList.Find(pWnd)==NULL)
		m_D3DWndList.InsertBefore(pWnd);
}

CD3DWnd * CD3DGUI::CreateWnd(CEasyRect rect)
{
	CD3DWnd * wnd;
	wnd=new CD3DWnd(this);
	wnd->SetRect(rect);	
	wnd->TopTo(true);
	return wnd;
}

CD3DButton * CD3DGUI::CreateButton(CEasyRect rect)
{
	CD3DButton * wnd;
	wnd=new CD3DButton(this);
	wnd->SetRect(rect);	
	wnd->TopTo(true);
	return wnd;
}

CD3DEdit * CD3DGUI::CreateEdit(CEasyRect rect)
{
	CD3DEdit * wnd;
	wnd=new CD3DEdit(this);
	wnd->SetRect(rect);	
	wnd->TopTo(true);
	return wnd;
}

CD3DScrollBar * CD3DGUI::CreateScrollBar(CEasyRect rect)
{
	CD3DScrollBar * wnd;
	wnd=new CD3DScrollBar(this);
	wnd->SetRect(rect);	
	wnd->TopTo(true);
	return wnd;
}

CD3DSimpleList * CD3DGUI::CreateSimpleList(CEasyRect rect)
{
	CD3DSimpleList * wnd;
	wnd=new CD3DSimpleList(this);
	wnd->SetRect(rect);	
	wnd->TopTo(true);
	return wnd;
}

CD3DCombo * CD3DGUI::CreateCombo(CEasyRect rect)
{
	CD3DCombo * wnd;
	wnd=new CD3DCombo(this);
	wnd->SetRect(rect);	
	wnd->TopTo(true);
	return wnd;
}

CD3DProgressBar * CD3DGUI::CreateProgressBar(CEasyRect rect)
{
	CD3DProgressBar * wnd;
	wnd=new CD3DProgressBar(this);
	wnd->SetRect(rect);	
	wnd->TopTo(true);
	return wnd;
}

BOOL CD3DGUI::HandleMessage(UINT msg, WPARAM wParam , LPARAM lParam)
{
	CD3DWnd * pWnd;		
	BOOL ActiveMessage=false;

	if(msg>=WM_D3DGUI_START)
		return false;
	if(m_pActiveWnd)
	{
		if(!m_pActiveWnd->IsVisible())
			ActiveWnd(m_pActiveWnd,false);
	}
		
	switch(msg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDBLCLK:	
		ActiveMessage=true;		
	case WM_MOUSEMOVE:		
	case WM_MOUSEWHEEL:	
		{
			//找到窗口
			m_CurMouseX=LOWORD(lParam);
			m_CurMouseY=HIWORD(lParam);	
			pWnd=GetWndAtPos(m_CurMouseX,m_CurMouseY);
			//处理MouseLeaveEnter消息
			if(m_pRecentMouseWnd!=pWnd)
			{
				if(m_pRecentMouseWnd)
					m_pRecentMouseWnd->HandleMessage(m_pRecentMouseWnd,WM_D3DGUI_MOUSE_LEAVE,wParam,lParam);
				m_pRecentMouseWnd=pWnd;
				if(m_pRecentMouseWnd)
					m_pRecentMouseWnd->HandleMessage(m_pRecentMouseWnd,WM_D3DGUI_MOUSE_ENTER,wParam,lParam);					
			}	
			//处理消息
			if(ActiveMessage)
			{
				CD3DWnd* pActiveWnd=pWnd;
				while(pActiveWnd&&(!pActiveWnd->CanGetFocus()))
					pActiveWnd=pActiveWnd->GetParent();
				ActiveWnd(pActiveWnd,true);					
			}
			if(pWnd)
			{				
				return pWnd->HandleMessage(pWnd,msg,wParam,lParam);				
			}
			return false;
		}
		break;
	case WM_KEYUP:
	case WM_KEYDOWN:		
	case WM_CHAR:
	default:
		if(m_pActiveWnd)
			return m_pActiveWnd->HandleMessage(m_pActiveWnd,msg,wParam,lParam);
		break;
	}
	return false;
}

BOOL CD3DGUI::SendMsgToParent(UINT msg, WPARAM wParam , LPARAM lParam)
{
	if(m_hParentWnd)
		return (BOOL)SendMessage(m_hParentWnd,msg,wParam,lParam);
	return false;
}

void CD3DGUI::Update()
{
	CD3DWnd * pWnd;
	LPVOID pos;
	pos=m_D3DWndList.GetFirstObjectPos();
	while(pos)
	{
		pWnd=*m_D3DWndList.GetNext(pos);
		pWnd->Update();
	}
}

bool CD3DGUI::LeftWndToTop(CD3DWnd * pWnd,CD3DWnd * Before)
{
	LPVOID pos;

	pos=m_D3DWndList.Find(pWnd);
	if(pos==NULL)
		return false;
	
	LPVOID WndPos=m_D3DWndList.Find(pWnd);
	if(WndPos==NULL)
		return false;
	LPVOID BeforePos=NULL;
	if(Before)
		LPVOID BeforePos=m_D3DWndList.Find(Before);
	
	m_D3DWndList.MoveToBefore(WndPos,BeforePos);
	
	return true;
}

void CD3DGUI::ActiveWnd(CD3DWnd * pWnd, bool active,bool SendNotify)
{
	
	if(active)
	{
		if(pWnd==m_pActiveWnd)
			return;

		if(m_pActiveWnd)
			m_pActiveWnd->ActiveWnd(false,SendNotify);
		m_pActiveWnd=pWnd;
		if(m_pActiveWnd)
			m_pActiveWnd->ActiveWnd(true,SendNotify);		
	}
	else
	{
		if(pWnd!=m_pActiveWnd)
			return;

		if(m_pActiveWnd)
			m_pActiveWnd->ActiveWnd(false,SendNotify);

		m_pActiveWnd=NULL;

		LPVOID pos;
		
		pos=m_D3DWndList.Find(m_pActiveWnd);
		m_D3DWndList.GetNext(pos);
		if(pos)
		{
			CD3DWnd * m_pActiveWnd=*m_D3DWndList.GetNext(pos);
			if(m_pActiveWnd)
				m_pActiveWnd->ActiveWnd(true,SendNotify);
		}	
		
	}
}

CD3DWnd * CD3DGUI::GetWndAtPos(int x,int y)
{
	LPVOID pos;
	CD3DWnd * pHitWnd=NULL;

	if(m_pCaptureAllWnd)
	{
		return m_pCaptureAllWnd;
	}

	pos=m_D3DWndList.GetFirstObjectPos();
	
	while(pos)
	{			
		CD3DWnd * pWnd=*m_D3DWndList.GetNext(pos);
		if(pWnd->IsVisible())
		{
			CEasyRect rect=pWnd->GetWndRect();							
			if(rect.PtInRect(CEasyPoint(x,y)))
			{
				pHitWnd=pWnd;
				break;
			}
		}
	}
	
	if(m_pModalWnd)
	{
		if(pHitWnd)
		{
			if(!m_pModalWnd->IsChildRecursion(pHitWnd))
				pHitWnd=m_pModalWnd;
		}
		else
		{
			pHitWnd=m_pModalWnd;
		}
	}

	return pHitWnd;
}

LPVOID CD3DGUI::GetFirstWndPosAtPos(int x,int y)
{
	LPVOID pos;
	CD3DWnd * pWnd;

	pos=m_D3DWndList.GetFirstObjectPos();
	while(pos)
	{			
		pWnd=*m_D3DWndList.GetNext(pos);
		if(pWnd->IsVisible())
		{
			CEasyRect rect=pWnd->GetWndRect();							
			if(rect.PtInRect(CEasyPoint(x,y)))
				return pos;
		}

	}
	return NULL;
}

CD3DWnd * CD3DGUI::GetNextWndAtPos(LPVOID & Pos,int x,int y)
{	
	CD3DWnd * pWnd;
	
	while(Pos)
	{			
		pWnd=*m_D3DWndList.GetNext(Pos);
		if(pWnd->IsVisible())
		{
			CEasyRect rect=pWnd->GetWndRect();							
			if(rect.PtInRect(CEasyPoint(x,y)))
				return pWnd;
		}

	}
	return NULL;
}

bool CD3DGUI::AddRootWnd(CD3DWnd * child)
{	
	if(m_RootWndList.Find(child)==NULL)
	{
		m_RootWndList.InsertAfter(child);
		return true;
	}
	return false;
}

bool CD3DGUI::DelRootWnd(CD3DWnd * child)
{

	LPVOID pos=m_RootWndList.Find(child);
	if(pos)
	{
		m_RootWndList.DeleteObject(pos);
		return true;
	}
	return false;
}

bool CD3DGUI::DelWnd(CD3DWnd * pWnd)
{
	LPVOID pos;
		
	pos=m_D3DWndList.Find(pWnd);
	if(pos)
	{
		if(pWnd==m_pActiveWnd)
			ActiveWnd(m_pActiveWnd,false);
		if(pWnd==m_pRecentMouseWnd)
			m_pRecentMouseWnd=NULL;
		m_D3DWndList.DeleteObject(pos);
		return true;
	}
	return false;
}

void CD3DGUI::Clear()
{
	CD3DWnd * pWnd;
	LPVOID pos;
	pos=m_RootWndList.GetFirstObjectPos();
	while(pos)
	{
		pWnd=*m_RootWndList.GetNext(pos);
		SAFE_RELEASE(pWnd);
	}
	m_D3DWndList.Clear();	
}

CD3DWnd * CD3DGUI::GetWndByName(LPCTSTR Name)
{
	LPVOID pos=m_D3DWndList.GetFirstObjectPos();
	
	while(pos)
	{
		CD3DWnd * pWnd=*m_D3DWndList.GetNext(pos);
		if(_tcscmp(pWnd->GetName(),Name)==0)
			return pWnd;
	}
	return NULL;
}

CD3DWnd * CD3DGUI::GetWndByID(UINT ID)
{
	LPVOID pos=m_D3DWndList.GetFirstObjectPos();

	while(pos)
	{
		CD3DWnd * pWnd=*m_D3DWndList.GetNext(pos);
		if(pWnd->GetID()==ID)
			return pWnd;
	}
	return NULL;
}

bool CD3DGUI::SaveToFile(LPCTSTR FileName)
{
	
	pug::xml_parser Xml;

	Xml.create();
	xml_node Doc;
	Doc = Xml.document();
	xml_node pi = Doc.append_child(node_pi);
	pi.name(_T("xml"));
	pi.attribute(_T("version")) = _T("1.0");
#ifdef UNICODE
	pi.attribute(_T("encoding")) = _T("ucs-2");
#else
	pi.attribute(_T("encoding")) = _T("gb2312");
#endif
	xml_node GUI=Doc.append_child(node_element,_T("D3DGUI"));

	SaveToXML(&GUI);


	return Xml.SaveToFile(Xml.document(), FileName, -1, GetFileChannel());
}

void CD3DGUI::SaveToXML(xml_node * pXMLNode)
{
	CD3DWnd * pWnd;
	LPVOID pos;
	pos=m_RootWndList.GetFirstObjectPos();
	while(pos)
	{
		pWnd=*m_RootWndList.GetNext(pos);
		pWnd->SaveToXml(pXMLNode);			
	}
}

bool CD3DGUI::SaveToUSO(CUSOResourceManager * pResourceManager)
{
	CD3DWnd * pWnd;
	LPVOID pos;
	pos=m_RootWndList.GetFirstObjectPos();
	while(pos)
	{
		pWnd=*m_RootWndList.GetNext(pos);
		pResourceManager->AddObject(pWnd);					
	}
	return true;
}

bool CD3DGUI::SaveToUSOFile(LPCTSTR FileName)
{
	CUSOResourceManager USOFile;
	if(SaveToUSO(&USOFile))
	{
		return USOFile.Export(FileName);
	}
	return false;
}

bool CD3DGUI::LoadFromFile(LPCTSTR FileName)
{
	
	pug::xml_parser Xml;

	Xml.parse_file(FileName, pug::parse_trim_attribute, GetFileChannel());
	xml_node GUI=Xml.document();
	if(GUI.moveto_child(_T("D3DGUI")))
	{
		return LoadFromXML(&(GUI));
	}
	return false;
}

bool CD3DGUI::LoadFromXML(xml_node * pXMLNode,CD3DWnd * pParent)
{
	CD3DWnd * pWnd;
	for(int i=0;i<(int)pXMLNode->children();i++)
	{
		pWnd=NULL;
		if(_tcsnicmp(pXMLNode->child(i).name(),_T("Window"),6)==0)
		{
			pWnd=CreateWnd(CEasyRect(0,0,0,0));
			pWnd->SetParent(pParent);
			pWnd->LoadFromXml(&(pXMLNode->child(i)));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Button"),6)==0)
		{
			pWnd=CreateButton(CEasyRect(0,0,0,0));
			pWnd->SetParent(pParent);
			pWnd->LoadFromXml(&(pXMLNode->child(i)));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Edit"),6)==0)
		{
			pWnd=CreateEdit(CEasyRect(0,0,0,0));
			pWnd->SetParent(pParent);
			pWnd->LoadFromXml(&(pXMLNode->child(i)));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("ScrollBar"),6)==0)
		{
			pWnd=CreateScrollBar(CEasyRect(0,0,0,0));
			pWnd->SetParent(pParent);
			pWnd->LoadFromXml(&(pXMLNode->child(i)));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("SimpleList"),6)==0)
		{
			pWnd=CreateSimpleList(CEasyRect(0,0,0,0));
			pWnd->SetParent(pParent);
			pWnd->LoadFromXml(&(pXMLNode->child(i)));
		}
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("Combo"),6)==0)
		{
			pWnd=CreateCombo(CEasyRect(0,0,0,0));
			pWnd->SetParent(pParent);
			pWnd->LoadFromXml(&(pXMLNode->child(i)));
		}	
		else if(_tcsnicmp(pXMLNode->child(i).name(),_T("ProgressBar"),12)==0)
		{
			pWnd=CreateProgressBar(CEasyRect(0,0,0,0));
			pWnd->SetParent(pParent);
			pWnd->LoadFromXml(&(pXMLNode->child(i)));
		}
	}
	return true;
}

}
