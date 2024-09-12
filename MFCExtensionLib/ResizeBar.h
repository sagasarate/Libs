#pragma once


// CResizeBar

class CDialogItemSorter;

class CResizeBar : public CWnd
{
	DECLARE_DYNAMIC(CResizeBar)
protected:
	CDialogItemSorter *		m_pParent;
	HCURSOR					m_Cursor;
	UINT					m_ItemID;
	bool					m_IsInResizing;
	CRect					m_ResizeRange;
	CRect					m_ResizeStartRect;
	CPoint					m_ResizeStartPos;

public:
	CResizeBar();
	virtual ~CResizeBar();

	BOOL Create(CDialogItemSorter * pParent,UINT ItemID,bool IsVertical);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
};


