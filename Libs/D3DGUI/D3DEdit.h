/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DEdit.h                                                */
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

struct EDIT_TEXT_LINE_INFO
{
	int StartPos;
	int Len;
	std::vector<int> CharSizes;
	EDIT_TEXT_LINE_INFO()
	{
		StartPos=0;
		Len=0;
	}
	EDIT_TEXT_LINE_INFO(const EDIT_TEXT_LINE_INFO& Info)
	{
		StartPos=Info.StartPos;
		Len=Info.Len;		
		CharSizes.assign(Info.CharSizes.begin(),Info.CharSizes.end());		
	}
	EDIT_TEXT_LINE_INFO& operator=(const EDIT_TEXT_LINE_INFO& Info)
	{
		StartPos=Info.StartPos;
		Len=Info.Len;
		CharSizes.assign(Info.CharSizes.begin(),Info.CharSizes.end());		
		return *this;
	}
};

typedef std::vector<EDIT_TEXT_LINE_INFO> CEditLineList;

class CD3DSimpleList;

class CD3DEdit :
	public CD3DWnd
{
protected:		
	CHAR				m_InputBuff[3];
	bool				m_IsMultiLine;
	bool				m_IsShowCaret;
	bool				m_IsCaretValidate;
	bool				m_AutoWrap;
	bool				m_IsReadyOnly;
	bool				m_IsEncryption;
	
	int					m_LineHeight;
	CEditLineList		m_LineInfos;
	int					m_FirstVisibleLine;
	int					m_FirstVisibleCol;
	int					m_CurVisibleLineCount;
	
	
	DWORD				m_CaretColor;
	int					m_CaretRectIndex;
	int					m_CurCaretRow;
	int					m_CurCaretCol;
	EDIT_TEXT_LINE_INFO m_CurLineInfo;

	bool				m_IsSelecting;
	int					m_SelectingStartRow;
	int					m_SelectingStartCol;
	int					m_SelectStartRow;
	int					m_SelectStartCol;
	int					m_SelectEndRow;
	int					m_SelectEndCol;

	int					m_TextRectIndex;	
	int					m_SelectRectStart;
	int					m_SelectRectCount;

	int					m_ClientCurWidth;
	int					m_ClientCurHeight;

	bool				m_IsEnabledScrollBar;
	int					m_ScrollBarWidth;
	CD3DScrollBar*		m_pScrollBar;

	int *				m_pCharSizes;

	bool				m_EnableIME;
	CD3DWnd *			m_pIMEReadingWnd;
	CD3DWnd *			m_pIMECandidateWnd;

	//bool				m_IsSupportTans;
	
	DECLARE_CLASS_INFO(CD3DEdit)
	
public:
	CD3DEdit();
	CD3DEdit(CD3DGUI * pGUI);
	virtual ~CD3DEdit(void);

	virtual void InitWnd(CD3DGUI *  pGUI);
	virtual void Destory();

	virtual BOOL OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam);

	virtual CEasyRect GetCenterRect();

	virtual void ActiveWnd(bool bActive,bool SendNotify);

	virtual void SetFont(LOGFONT * pLogFont);
	virtual void SetFontAlign(DWORD Align);
	virtual void SetFontSahdowWidth(DWORD ShadowWidth);
	virtual void SetFontScale(FLOAT Scale);

	virtual void SetVisible(bool IsVisible);

	virtual void SetText(LPCTSTR Text);	
	virtual void GetText(CEasyString& Text)
	{
		Text=m_WndText;	
		Text.Replace(_T("\n"),_T("\r\n"));
	}

	virtual void UpdateRects();
	virtual void UpdateText();
	virtual void UpdateCaret();
	virtual void UpdateSelect();

	virtual void Update();

	CEasyStringW GetLineText(int Row);

	virtual void SetMultiLine(bool IsMultiLine);
	bool IsMultiLine()
	{
		return m_IsMultiLine;
	}
	

	void EnableCaret(bool IsShow)
	{
		m_IsShowCaret=IsShow;
	}
	bool IsCaretShow()
	{
		return m_IsShowCaret;
	}

	void EnableAutoWrap(bool IsAutoWrap);
	bool IsAutoWrap()
	{
		return m_AutoWrap;
	}

	void SetReadOnly(bool IsReadly)
	{
		m_IsReadyOnly=IsReadly;
	}
	bool IsReadyOnly()
	{
		return m_IsReadyOnly;
	}
	void SetEncryption(bool IsEncryption);
	bool IsEncryption()
	{
		return m_IsEncryption;
	}

	void SetFirstVisibleLine(int LineIndex);
	int GetFirstVisibleLine()
	{
		return m_FirstVisibleLine;
	}

	void SetCaretColor(DWORD Color);
	DWORD GetCaretColor()
	{
		return m_CaretColor;
	}

	void SetCaretPos(int Pos);
	int GetCaretPos();

	void SetSel(int Start,int End);
	void GetSel(int& Start,int& End);

	void SetScrollBarWidth(int Width);
	int GetScrollBarWidth()
	{
		return m_ScrollBarWidth;
	}

	void EnableScrollBar(bool Enable);	
	bool IsScrollBarEnabled()
	{
		return m_pScrollBar!=NULL;
	}

	CD3DScrollBar* GetScrollBar()
	{
		return m_pScrollBar;
	}

	void Copy();
	void Paste();
	void Cut();
	virtual void SaveToXml(xml_node * pXMLNode);
	virtual bool LoadFromXml(xml_node * pXMLNode);

	void SetIMEReadingWnd(CD3DWnd * pWnd);
	void SetIMECandidateWnd(CD3DWnd * pWnd);
	CD3DWnd * GetIMEReadingWnd()
	{
		return m_pIMEReadingWnd;
	}
	CD3DWnd * GetIMECandidateWnd()
	{
		return m_pIMECandidateWnd;
	}
	void EnableIME(bool Enable);
	bool IsIMEEnabled()
	{
		return m_EnableIME;
	}

protected:	
	virtual bool UpdateFont();

	void GrowTextRect(int Grow);
	void FormatText(CEasyStringW& Text);
	void CaculateLineInfo(CEasyStringW& Text,CEditLineList& LineInfos);
	void IndexToRowCol(int Index,int& Row,int& Col);
	void RowColToIndex(int& Index,int Row,int Col,bool IsForward=true);
	void CheckIndex(int& Index,bool IsForward=true);
	EDIT_TEXT_LINE_INFO * GetLineInfo(int Row);
	void UpdateCurLineInfo();
	void UpdateCaretPos(bool IsForward=true);
	int GetEndCol(int Row);
	bool XYToCaretRowCol(int x,int y,int& Row,int& Col);
	bool CaretRowColToXY(int& x,int& y1,int& y2,int Row,int Col);
	bool DeleteTextAtCaret(bool IsBack);
	void ClearSelection();
	bool DeleteSelection();
	void InsertChar(WCHAR Char,bool WantTrans);
	void InsertStr(LPCWSTR Str,bool WantTrans);
	virtual void GetMiniSize(int& Width,int& Height);
	void MakeCaretVisible();
	RECT GetLineRect(int Index);

	bool ProcessIME(UINT msg, WPARAM wParam, LPARAM lParam);

	
	int GetSelectRectCount()
	{
		return m_SelectRectCount;
	}

	LPITEXTRECT& TextRect()
	{
		return ((LPITEXTRECT&)m_pWndRects[m_TextRectIndex]);
	}

	LPIWNDRECT& SelectRect(int Index)
	{
		return (LPIWNDRECT&)m_pWndRects[m_SelectRectStart+Index];
	}

	LPIWNDRECT& CaretWndRect()
	{
		return (LPIWNDRECT&)m_pWndRects[m_CaretRectIndex];
	}

};

}