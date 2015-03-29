/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DScrollBar.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "d3dwnd.h"

namespace D3DGUI{

enum D3D_SCROLL_BAR_STYLE
{
	SCROLL_BAR_VERTICAL,
	SCROLL_BAR_HORIZONTAL,
};

class CD3DScrollBar :
	public CD3DWnd
{
protected:
	CD3DButton *	m_pDecButton;
	CD3DButton *	m_pIncButton;
	CD3DButton *	m_pTrackButton;

	int				m_MaxScrollPos;
	int				m_CurScrollPos;

	DWORD			m_ScrollStyle;

	int				m_Scrolling;
	int				m_ScrollStatus;
	int				m_ScrollStartTime;
	bool			m_IsTracking;
	int				m_TrackingStartPos;
	int				m_TrackingStartBarPos;
	int				m_TrackingStartScrollPos;

	DECLARE_CLASS_INFO(CD3DScrollBar)

public:
	CD3DScrollBar();
	CD3DScrollBar(CD3DGUI * pGUI);
	virtual ~CD3DScrollBar(void);
	virtual void InitWnd(CD3DGUI *  pGUI);
	

	virtual BOOL OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam);

	CD3DButton * GetDecButton()
	{
		return m_pDecButton;
	}
	CD3DButton * GetIncButton()
	{
		return m_pIncButton;
	}
	CD3DButton * GetTrackButton()
	{
		return m_pTrackButton;
	}

	void SetScrollStyle(int ScrollStyle);
	DWORD GetScrollStyle()
	{
		return m_ScrollStyle;
	}

	void SetMaxScrollPos(int MaxScrollPos);
	int GetMaxScrollPos()
	{
		return m_MaxScrollPos;
	}

	void Scroll(int Delta,bool SendMsg=false);
	void SetScrollPos(int ScrollPos,bool SendMsg=false);
	int GetScrollPos()
	{
		return m_CurScrollPos;
	}

	virtual void SetVisible(bool IsVisible);

	virtual CEasyRect GetCenterRect();

	virtual void UpdateRects();
	virtual void UpdateTrack();

	virtual void Update();

	virtual void GetMiniSize(int& Width,int& Height);

	virtual void SaveToXml(xml_node * pXMLNode);
	virtual bool LoadFromXml(xml_node * pXMLNode);
	
};

}