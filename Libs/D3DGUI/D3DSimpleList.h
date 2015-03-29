/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DSimpleList.h                                          */
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

struct SIMPLE_LIST_LINE_INFO
{
	CEasyStringW	Text;
	bool		IsSelected;
	LPVOID		pItemData;
	SIMPLE_LIST_LINE_INFO()
	{
		IsSelected=false;
		pItemData=NULL;
	}
	SIMPLE_LIST_LINE_INFO(const SIMPLE_LIST_LINE_INFO& Info)
	{
		Text=Info.Text;
		IsSelected=Info.IsSelected;
		pItemData=Info.pItemData;
	}
};

typedef CEasyArray<SIMPLE_LIST_LINE_INFO> CSimpleListLineInfos;

class CD3DSimpleList :
	public CD3DWnd
{
protected:
	int						m_TextRectStart;
	int						m_TextRectCount;
	int						m_SelectRectStart;
	int						m_SelectRectCount;

	int						m_LineSpace;
	int						m_LineHeight;
	int						m_FirstVisibleLine;
	int						m_CurVisibleLineCount;

	int						m_ClientCurWidth;
	int						m_ClientCurHeight;

	bool					m_IsLoopScroll;
	int						m_ScrollBarWidth;
	CD3DScrollBar*			m_pScrollBar;

	CSimpleListLineInfos	m_LineInfos;

	bool					m_IsControlPress;
	bool					m_IsShiftPress;
	int						m_ShiftSelectStart;
	int						m_RecentSelect;
	bool					m_AllowMutliSelect;

	DECLARE_CLASS_INFO(CD3DSimpleList)

public:
	CD3DSimpleList();
	CD3DSimpleList(CD3DGUI * pGUI);
	virtual ~CD3DSimpleList(void);
	virtual void InitWnd(CD3DGUI *  pGUI);
	

	virtual BOOL OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam);

	virtual CEasyRect GetCenterRect();
	virtual void SetFont(LOGFONT * pLogFont);
	virtual void SetFontAlign(DWORD Align);
	virtual void SetFontSahdowWidth(DWORD ShadowWidth);
	virtual void SetFontScale(FLOAT Scale);
	virtual void SetVisible(bool IsVisible);

	virtual void GetMiniSize(int& Width,int& Height);

	virtual void UpdateRects();
	virtual void UpdateText();
	virtual void UpdateSelect();	
	
	
	void SetLineSpace(int Space)
	{
		m_LineSpace=Space;
		UpdateText();
	}
	int GetLineSpace()
	{
		return m_LineSpace;
	}

	void EnableLoopScroll(bool Enable)
	{
		m_IsLoopScroll=Enable;
	}
	bool IsLoopScroll()
	{
		return m_IsLoopScroll;
	}

	void SetScrollBarWidth(int Width);
	int GetScrollBarWidth()
	{
		return m_ScrollBarWidth;
	}

	//void EnableScrollBar(bool Enable);	
	CD3DScrollBar* GetScrollBar()
	{
		return m_pScrollBar;
	}

	void Scroll(int Detal)
	{
		ScrollTo(m_FirstVisibleLine+Detal);
	}
	void ScrollTo(int Line);
	int GetFirstVisibleLine()
	{
		return m_FirstVisibleLine;
	}

	int XYToItemIndex(int x,int y);

	void SetAllowMutliSelect(bool IsAllow)
	{
		m_AllowMutliSelect=IsAllow;
	}
	bool IsAllowMutliSelect()
	{
		return m_AllowMutliSelect;
	}

	void SelectItem(int Index);
	void SelectItemRange(int Start,int End);
	void ClearAllSelect();

	int AddItem(LPCTSTR ItemText);
	bool InsertItem(int Index,LPCTSTR ItemText);
	bool DeleteItem(int Index);
	void DeleteAllItem();

	int GetItemCount();

	int GetFirstSelectedItem();
	int GetNextSelectedItem(int Index);

	bool SetItemText(int Index,LPCTSTR Text);
	bool GetItemText(int Index,CEasyString& Text);
	

	bool SetItemData(int Index,LPVOID pData);
	LPVOID GetItemData(int Index);

	virtual void SaveToXml(xml_node * pXMLNode);
	virtual bool LoadFromXml(xml_node * pXMLNode);

	void MakeItemVisible(int Index);

	UINT GetItemHeight()
	{
		return m_LineHeight+m_LineSpace;
	}

protected:
	virtual bool UpdateFont();

	int GetTextRectCount()
	{
		return m_TextRectCount;
	}
	int GetSelectRectCount()
	{
		return m_SelectRectCount;
	}

	LPITEXTRECT& TextRect(int Index)
	{
		return ((LPITEXTRECT&)m_pWndRects[m_TextRectStart+Index]);
	}

	LPIWNDRECT& SelectRect(int Index)
	{
		return (LPIWNDRECT&)m_pWndRects[m_SelectRectStart+Index];
	}

	void GrowTextRect(int Grow);
	void CheckScroll();

};

}