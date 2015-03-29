// MapBox.cpp : 实现文件
//

#include "stdafx.h"
#include "MapBox.h"



// CMapBox

IMPLEMENT_DYNAMIC(CMapBox, CStatic)

CMapBox::CMapBox()
{
	m_pParent=NULL;
}

CMapBox::~CMapBox()
{
}


BEGIN_MESSAGE_MAP(CMapBox, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CMapBox 消息处理程序



BOOL CMapBox::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CRect Rect;
	GetClientRect(&Rect);
	pDC->FillSolidRect(&Rect,RGB(0,0,0));
	return FALSE;
}

void CMapBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	if(m_pParent)
	{
		m_pParent->DrawMap(&dc);
	}
}

void CMapBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_pParent)
	{
		m_pParent->OnClickMap(point);
	}
	CStatic::OnLButtonUp(nFlags, point);
}
