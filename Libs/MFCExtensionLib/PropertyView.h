/****************************************************************************/
/*                                                                          */
/*      文件名:    PropertyView.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


// CPropertyView 视图
namespace PropertyGrid
{

class CPropertyView : public CView
{
	DECLARE_DYNCREATE(CPropertyView)

protected:
	CPropertyView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPropertyView();

	CPropertyGrid m_PropertyGrid;

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制此视图
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

	virtual afx_msg LRESULT OnPropertyChanged( WPARAM wParam, LPARAM lParam);
public:
	virtual afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();

	CPropertyGrid& GetPropertyCtrl()
	{
		return m_PropertyGrid;
	}
};


}