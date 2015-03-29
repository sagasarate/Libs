#pragma once


// CMapBox

class CDlgMapView;
class CMapBox : public CStatic
{
	DECLARE_DYNAMIC(CMapBox)
protected:
	CDlgMapView * m_pParent;
public:
	CMapBox();
	virtual ~CMapBox();

protected:
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	void SetParent(CDlgMapView * pParent)
	{
		m_pParent=pParent;
	}
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


