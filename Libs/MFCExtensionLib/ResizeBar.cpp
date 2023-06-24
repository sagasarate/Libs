// ResizeBar.cpp : 实现文件
//

#include "stdafx.h"
#include "ResizeBar.h"





// CResizeBar

IMPLEMENT_DYNAMIC(CResizeBar, CWnd)

CResizeBar::CResizeBar()
{
	m_pParent=NULL;
	m_Cursor=NULL;
	m_ItemID=-1;
	m_IsInResizing=false;
}

CResizeBar::~CResizeBar()
{
	DestroyCursor(m_Cursor);
}

BOOL CResizeBar::Create(CDialogItemSorter * pParent,UINT ItemID,bool IsVertical)
{
	//if(!CWnd::Create(NULL,"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),pParent,342124))
	//	return FALSE;
	m_pParent=pParent;
	if(IsVertical)
		m_Cursor=LoadCursor(NULL,IDC_SIZENS);
	else
		m_Cursor=LoadCursor(NULL,IDC_SIZEWE);
	m_ItemID=ItemID;
	if(!CWnd::Create(
		AfxRegisterWndClass(0,m_Cursor,(HBRUSH)COLOR_BACKGROUND,NULL),
		_T(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),
		m_pParent->GetParent(),676756))
	{
		return FALSE;
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CResizeBar, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CResizeBar 消息处理程序



void CResizeBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonDown(nFlags, point);

	SetCapture();
	m_IsInResizing=true;	
	m_pParent->GetResizeRange(m_ItemID,m_ResizeRange);
	GetWindowRect(m_ResizeStartRect);
	GetParent()->ScreenToClient(&m_ResizeStartRect);
	GetCursorPos(&m_ResizeStartPos);
	Invalidate();
}

void CResizeBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonUp(nFlags, point);

	ReleaseCapture();
	m_IsInResizing=false;
	CPoint CurPos;
	GetCursorPos(&CurPos);
	CSize Size=CurPos-m_ResizeStartPos;
	m_pParent->OnResize(m_ItemID,Size);
	Invalidate();
}

void CResizeBar::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnMouseMove(nFlags, point);

	if(m_IsInResizing)
	{
		CPoint CurPos;
		GetCursorPos(&CurPos);
		CSize Size=CurPos-m_ResizeStartPos;
		if(Size.cx<0)
		{
			if(-Size.cx>m_ResizeRange.left)
				Size.cx=-m_ResizeRange.left;
		}
		if(Size.cx>0)
		{
			if(Size.cx>m_ResizeRange.right)
				Size.cx=m_ResizeRange.right;
		}
		if(Size.cy<0)
		{
			if(-Size.cy>m_ResizeRange.top)
				Size.cy=-m_ResizeRange.top;
		}
		if(Size.cy>0)
		{
			if(Size.cy>m_ResizeRange.bottom)
				Size.cy=m_ResizeRange.bottom;
		}
		CRect Rect=m_ResizeStartRect;
		Rect.left+=Size.cx;
		Rect.right+=Size.cx;
		Rect.top+=Size.cy;
		Rect.bottom+=Size.cy;
		MoveWindow(&Rect);
		Invalidate();
		
	}
}

void CResizeBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

	if(m_IsInResizing)
	{
		CRect Rect;
		GetClientRect(&Rect);

		dc.SetMapMode(MM_TEXT);
		dc.FillRect(&Rect,CDC::GetHalftoneBrush());
	}
}


