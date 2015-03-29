/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DGUI.h                                                 */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "D3DWnd.h"
#include "D3DButton.h"
#include "D3DScrollBar.h"
#include "D3DEdit.h"
#include "D3DSimpleList.h"
#include "D3DCombo.h"
#include "D3DProgressBar.h"

namespace D3DGUI{

//class CD3DWnd;
class CD3DGUI :
	public CNameObject
{
protected:	
	IUIObjectCreator *				m_pObjectCreator;
	HWND							m_hParentWnd;

	CEasyList<CD3DWnd *>			m_D3DWndList;
	CEasyList<CD3DWnd *>			m_RootWndList;
	bool							m_ReadyToWork;
	CD3DWnd *						m_pActiveWnd;
	CD3DWnd *						m_pRecentMouseWnd;
	CD3DWnd *						m_pCaptureAllWnd;
	CD3DWnd *						m_pModalWnd;

	int								m_CurMouseX;
	int								m_CurMouseY;

	DECLARE_FILE_CHANNEL_MANAGER
	DECLARE_CLASS_INFO_STATIC(CD3DGUI)

public:	
	CD3DGUI(IUIObjectCreator * pObjectCreator,HWND hParentWnd=NULL);
	virtual ~CD3DGUI(void);	
	
	void AddWnd(CD3DWnd *pWnd);
	CD3DWnd * CreateWnd(CEasyRect rect);
	CD3DButton * CreateButton(CEasyRect rect);
	CD3DEdit * CreateEdit(CEasyRect rect);
	CD3DScrollBar * CreateScrollBar(CEasyRect rect);
	CD3DSimpleList * CreateSimpleList(CEasyRect rect);
	CD3DCombo * CreateCombo(CEasyRect rect);
	CD3DProgressBar * CreateProgressBar(CEasyRect rect);

	virtual BOOL HandleMessage(UINT msg, WPARAM wParam , LPARAM lParam);
	BOOL SendMsgToParent(UINT msg, WPARAM wParam , LPARAM lParam);
	void Update();

	bool LeftWndToTop(CD3DWnd * pWnd,CD3DWnd * Before=NULL);
	void ActiveWnd(CD3DWnd * pWnd, bool active,bool SendNotify=true);
	void SetCaptureAllWnd(CD3DWnd * pCaptureAllWnd)
	{
		m_pCaptureAllWnd=pCaptureAllWnd;
	}
	void SetModalWnd(CD3DWnd * pModalWnd)
	{
		m_pModalWnd=pModalWnd;
	}
	CD3DWnd * GetModalWnd()
	{
		return m_pModalWnd;
	}

	IUIObjectCreator * GetObjectCreator()
	{return m_pObjectCreator;}


	int GetCurMouseX()
	{
		return m_CurMouseX;
	}

	int GetCurMouseY()
	{
		return m_CurMouseY;
	}
	CD3DWnd * GetWndAtPos(int x,int y);
	LPVOID GetFirstWndPosAtPos(int x,int y);
	CD3DWnd * GetNextWndAtPos(LPVOID & Pos,int x,int y);

	LPVOID GetFirstWndPos()
	{
		return m_D3DWndList.GetFirstObjectPos();
	}
	CD3DWnd * GetNextWnd(LPVOID& Pos)
	{		
		CD3DWnd ** ppWnd=m_D3DWndList.GetNext(Pos);
		if(ppWnd)
		{
			return *ppWnd;
		}
		return NULL;
	}

	LPVOID GetFirstRootWndPos()
	{
		return m_RootWndList.GetFirstObjectPos();
	}
	CD3DWnd * GetNextRootWnd(LPVOID& Pos)
	{
		CD3DWnd ** ppWnd=m_RootWndList.GetNext(Pos);
		if(ppWnd)
		{
			return *ppWnd;
		}
		return NULL;
	}

	bool AddRootWnd(CD3DWnd * child);
	bool DelRootWnd(CD3DWnd * child);
	bool DelWnd(CD3DWnd * pWnd);

	void Clear();

	CD3DWnd * GetWndByName(LPCTSTR Name);
	CD3DWnd * GetWndByID(UINT ID);

	HWND GetHWND()
	{
		return m_hParentWnd;
	}

	

	void Release()
	{delete this;}


	bool SaveToFile(LPCTSTR FileName);
	void SaveToXML(xml_node * pXMLNode);
	bool SaveToUSO(CUSOResourceManager * pResourceManager);
	bool SaveToUSOFile(LPCTSTR FileName);

	bool LoadFromFile(LPCTSTR FileName);	
	bool LoadFromXML(xml_node * pXMLNode,CD3DWnd * pParent=NULL);
protected:
	
};

}